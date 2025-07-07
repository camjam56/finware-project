#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "libs/cJSON.h"
#include <sqlite3.h>


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
		printf("Error: Failed to prepare statement");
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


int main(void){

    int sockfd;                         //socket file descriptor
    struct sockaddr_in server_addr;     //struct type, with format of sockaddr_in, named server_addr


	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd == -1){
		perror("Socket Error");
		return 1;
	}
	printf("------------------------------------------------\n");
	printf("Socket created successfully! \nSocket File Descriptor = %d\n", sockfd);


	//defining socket info
	server_addr.sin_family = AF_INET;       //ipv4
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

	//connection while loop begins here
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

		//buffer is a temporary storage for sent/to-be-received information
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
		json_start += 4; //removes the \r\n\r\n to begin json_start from the start

		//parsing the JSON file to be read by C
		cJSON *root = cJSON_Parse(json_start);
		if (root == NULL) {
			printf("Error parsing JSON file\n");
			close(new_fd);
			continue;
		}

		//extracting the fields into respective variables
		cJSON *username = cJSON_GetObjectItem(root, "username");
		cJSON *password = cJSON_GetObjectItem(root, "password");

		if (!cJSON_IsString(username) || !cJSON_IsString(password)) {
			printf("Invalid JSON structure, failed to parse\n");
			cJSON_Delete(root);
			close(new_fd);
			continue;
		}

		int login_success = 0;

		sqlite3* db;
		if (sqlite3_open("data_be/users.db", &db) != SQLITE_OK){
			fprintf(stderr, "Failed to access database: %s\n", sqlite3_errmsg(db));
			continue;
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

		cJSON_Delete(root);
		close(new_fd);

	}

	close(sockfd);
	return 0;

}
