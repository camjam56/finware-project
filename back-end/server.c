#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "libs/cJSON.h"
#include <sqlite3.h>
#include <regex.h>

const int PORT = 5656;
const int BACKLOG = 12;
const int BUFFER_SIZE = 1024;

int login_check(

	sqlite3* db,
	const char *username,
	const char *password){

	sqlite3_stmt *stmt;
	const char *sql = "SELECT 1 FROM users WHERE username=? AND password=? LIMIT 1;";

	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (rc != SQLITE_OK){
		printf("Error: Failed to prepare statement (lc)");
		return 0;
	}

	sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);

	rc = sqlite3_step(stmt);

	int login_success = 0;
	if (rc == SQLITE_ROW){
		login_success = 1;
	}
	sqlite3_finalize(stmt);
	return login_success;
}

int unique_username_check(sqlite3 *db, cJSON *reg_username){

	sqlite3_stmt *stmt;
	const char *sql = "SELECT 1 FROM users WHERE username=?";

	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (rc != SQLITE_OK){
		printf("Error: Failed to prepare statement (uuc)");
		return 0;
	}

	sqlite3_bind_text(stmt, 1, reg_username->valuestring, -1, SQLITE_STATIC);

	rc = sqlite3_step(stmt);
	if (rc == SQLITE_ROW){
		printf("Username duplicate found, failed to register");
		sqlite3_finalize(stmt);
		return 0;
	}

	sqlite3_finalize(stmt);
	return 1;
}

void login_handler(int new_fd, cJSON *root){

	//Extracting the fields into respective variables
	cJSON *username = cJSON_GetObjectItem(root, "username");
	cJSON *password = cJSON_GetObjectItem(root, "password");

	if (!cJSON_IsString(username) || !cJSON_IsString(password)) {
		printf("Invalid JSON structure, failed to parse\n");
		cJSON_Delete(root);
		close(new_fd);
		return;
	}

	int login_success = 0;

	sqlite3* db;
	if (sqlite3_open("data_be/users.db", &db) != SQLITE_OK){
		fprintf(stderr, "Failed to access database: %s\n", sqlite3_errmsg(db));
		return;
	}

	login_success = login_check(db, username->valuestring, password->valuestring);
	sqlite3_close(db);

	if (login_success == 1){
		printf("Login Successful!\n");
		const char* response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/plain\r\n"
			"Access-Control-Allow-Origin: *\r\n"
			"Content-Length: 1\r\n"
			"\r\n"
			"1";
		send(new_fd, response, strlen(response), 0);
	}
	else {
		printf("Failed to login\n");
		const char* response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: text/plain\r\n"
			"Access-Control-Allow-Origin: *\r\n"
			"Content-Length: 1\r\n"
			"\r\n"
			"0";
		send(new_fd, response, strlen(response), 0);
	}

}

int password_validity_check(sqlite3 *db, cJSON *reg_username, cJSON *reg_password, cJSON *reg_password_confirm){

	regex_t regex;
	int ret;

	const char *pattern = "^[A-Za-z0-9?!]+$";

	ret = regcomp(&regex, pattern, REG_EXTENDED);

	if(ret){
	fprintf(stderr, "Regex failed to compile\n");
	return 0;
	}

	ret = regexec(&regex, reg_password->valuestring, 0, NULL, 0);
	regfree(&regex);

	if(ret != 0 || strcmp(reg_password->valuestring, reg_password_confirm->valuestring) != 0){
		fprintf(stderr, "Failed to regexec or match passwords, invalid input");
		return 0;
	}

	if (sqlite3_open("data_be/users.db", &db) != SQLITE_OK){
		fprintf(stderr, "Failed to access database: %s\n", sqlite3_errmsg(db));
		return 0;
	}

	return 1;
}

void register_handler(int new_fd, cJSON *root){

	cJSON *reg_username = cJSON_GetObjectItem(root, "regUsername");
	cJSON *reg_password = cJSON_GetObjectItem(root, "regPassword");
	cJSON *reg_password_confirm = cJSON_GetObjectItem(root, "passwordConfirm");

	if (!cJSON_IsString(reg_username) || !cJSON_IsString(reg_password) || !cJSON_IsString(reg_password_confirm)) {
		printf("Invalid JSON structure, failed to parse\n");
		cJSON_Delete(root);
		close(new_fd);
		return;
	}

	sqlite3 *db;

	const char* response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/plain\r\n"
		"Access-Control-Allow-Origin: *\r\n"
		"Content-Length: 1\r\n"
		"\r\n"
		"0";


	if (!password_validity_check(db, reg_username, reg_password, reg_password_confirm)){
		fprintf(stderr, "Failed to ensure password validity");
		send(new_fd, response, strlen(response), 0);
		close(new_fd);
		return;
	}

	if (!unique_username_check(db, reg_username)){
		fprintf(stderr, "Username was not unique, failed to register");
		send(new_fd, response, strlen(response), 0);
		close(new_fd);
		return;
	}


}


int main(void){

    int sockfd;                         //Socket file descriptor
    struct sockaddr_in server_addr;     //Struct type, with format of sockaddr_in, named server_addr


	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd == -1){
		perror("Socket Error");
		return 1;
	}
	printf("------------------------------------------------\n");
	printf("Socket created successfully! \nSocket File Descriptor = %d\n", sockfd);


	//Defining socket info
	server_addr.sin_family = AF_INET;       //IPv4
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY; 
	memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

	if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
		perror("Bind Error");
		close(sockfd);
		return 1;
	}


	printf("------------------------------------------------\n");
	printf("Bind Successful! Bound to Port: 5656\n");


	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		close(sockfd);
		return 1;
	}
	printf("------------------------------------------------\n");
	printf("Server is listening on port 5656...\n");

	//Connection while loop begins here
	while(1){

		struct sockaddr_in client_addr;
		socklen_t addr_size = sizeof(client_addr);
		int new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);

		if (new_fd == -1) {
			perror("accept");
			continue;
		}
		printf("------------------------------------------------\n");
		printf("Accepted a connection!\n");

		//Buffer is a temporary storage for sent/to-be-received information
		char buffer[BUFFER_SIZE];
		int bytes_received = recv(new_fd, buffer, sizeof(buffer) -1, 0);
		if(bytes_received < 0){
			perror("Error with recv() function");
			close(new_fd);
			continue;
		}
		buffer[bytes_received] = '\0';
		printf("Received: %s\n", buffer);

		if (strncmp(buffer, "OPTIONS", 7) == 0){
			const char* options_response = 
				"HTTP/1.1 204 No Content\r\n"
				"Access-Control-Allow-Origin: *\r\n"
				"Access-Control-Allow-Methods: POST, OPTIONS\r\n"
				"Access-Control-Allow-Headers: Content-Type\r\n"
				"Content-Length: 0\r\n"
				"\r\n";

			send(new_fd, options_response, strlen(options_response), 0);
			close(new_fd);

			continue;
		}

		char *json_start = strstr(buffer, "\r\n\r\n");
		if (json_start == NULL) {
			printf("Error with HTTP request\n");
			close(new_fd);
			continue;
		}

		//Removes the \r\n\r\n to begin json_start from the start
		json_start += 4;

		//Parsing the JSON file to be read by C
		cJSON *root = cJSON_Parse(json_start);

		//Error check for correct parsing
		if (root == NULL) {
			printf("Error parsing JSON file\n");
			close(new_fd);
			continue;
		}

		cJSON *action = cJSON_GetObjectItem(root, "action");
		if(strcmp(action->valuestring, "login") == 0){
			login_handler(new_fd, root);
		}
		else if (strcmp(action->valuestring, "register") == 0){
			sqlite3 *db;
			register_handler(new_fd, root);
		}
	
		cJSON_Delete(root);
		close(new_fd);

	}

	close(sockfd);
	return 0;

}
