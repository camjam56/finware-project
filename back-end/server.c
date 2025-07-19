#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "libs/cJSON.h"
#include <sqlite3.h>
#include <regex.h>
#include <time.h>
#include <stdlib.h>

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

int password_validity_check(sqlite3 *db, 
			    cJSON *reg_username, 
			    cJSON *reg_password, 
			    cJSON *reg_password_confirm){

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

	return 1;
}

int username_validity_check(sqlite3 *db, cJSON *reg_username){

	regex_t regex;
	int ret;

	const char *pattern = "^[A-Za-z0-9_]+$";

	ret = regcomp(&regex, pattern, REG_EXTENDED);

	if(ret){
		fprintf(stderr, "Regex failed to compile\n");
		return 0;
	}

	ret = regexec(&regex, reg_username->valuestring, 0, NULL, 0);
	regfree(&regex);

	
	if(ret != 0){
		fprintf(stderr, "Failed to regexec or username, invalid input");
		return 0;
	}

	return 1;

}

int unique_username_check(sqlite3 *db, cJSON *reg_username){

	sqlite3_stmt *stmt;
	const char *sql = "SELECT 1 FROM users WHERE username=?";

	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (rc != SQLITE_OK){
		printf("Error: failed to prepare statement (uuc)");
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

int insert_new_account(

	sqlite3 *db, 
	cJSON *reg_username, 
	cJSON *reg_password){

	const char *new_username = reg_username->valuestring;
	const char *new_password = reg_password->valuestring;

	sqlite3_stmt *stmt;
	const char *sql = "INSERT INTO users (username, password) VALUES (?, ?)";

	int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (rc != SQLITE_OK){
		fprintf(stderr, "Error: failed to prepare statement (ina)");
		return 0;
	}

	sqlite3_bind_text(stmt, 1, new_username, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, new_password, -1, SQLITE_STATIC);

	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE){
		fprintf(stderr, "Error: Failed to insert new user into database (%s)", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return 0;
	}

	sqlite3_finalize(stmt);
	return 1; 

}

void register_handler(int new_fd, cJSON *root){

	cJSON *reg_username = cJSON_GetObjectItem(root, "regUsername");
	cJSON *reg_password = cJSON_GetObjectItem(root, "regPassword");
	cJSON *password_confirm = cJSON_GetObjectItem(root, "passwordConfirm");

	if (!cJSON_IsString(reg_username) 
		|| !cJSON_IsString(reg_password) 
		|| !cJSON_IsString(password_confirm)) {
		printf("Invalid JSON structure, failed to parse\n");
		cJSON_Delete(root);
		close(new_fd);
		return;
	}

	sqlite3 *db;
	if (sqlite3_open("data_be/users.db", &db) != SQLITE_OK){
		fprintf(stderr, "Failed to access database: %s\n", sqlite3_errmsg(db));
		return;
	}



	const char* response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/plain\r\n"
		"Access-Control-Allow-Origin: *\r\n"
		"Content-Length: 1\r\n"
		"\r\n"
		"0";


	if (!password_validity_check(db, reg_username, reg_password, password_confirm)){
		fprintf(stderr, "Failed to ensure password validity");
		send(new_fd, response, strlen(response), 0);
		close(new_fd);
		return;
	}

	if (!username_validity_check(db, reg_username)){
		fprintf(stderr, "Username was not unique, failed to register");
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
	
	if (!insert_new_account(db, reg_username, reg_password)){
		fprintf(stderr, "Failed to complete new account creation");
		send(new_fd, response, strlen(response), 0);
		close(new_fd);
		return;
	}
	else{
		const char* response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/plain\r\n"
		"Access-Control-Allow-Origin: *\r\n"
		"Content-Length: 1\r\n"
		"\r\n"
		"1";

		send((new_fd), response, strlen(response), 0);
	}

	sqlite3_close(db);
}

void trade_handler(int new_fd, cJSON *root){

	cJSON *username = cJSON_GetObjectItem(root, "user");
	cJSON *stockSymbol = cJSON_GetObjectItem(root, "stockSymbol");
	cJSON *tradeType = cJSON_GetObjectItem(root, "tradeType");
	cJSON *quantity = cJSON_GetObjectItem(root, "quantity");
	cJSON *pricePerShare = cJSON_GetObjectItem(root, "pricePerShare");

	if (!cJSON_IsString(username) ||
		!cJSON_IsString(stockSymbol) ||
		!cJSON_IsString(tradeType) ||
		!cJSON_IsNumber(quantity) ||
		!cJSON_IsNumber(pricePerShare)) {

		fprintf(stderr, "Invalid trade JSON structure\n");
		const char* error_response =
			"HTTP/1.1 400 Bad Request\r\n"
			"Content-Type: text/plain\r\n"
			"Access-Control-Allow-Origin: *\r\n"
			"Content-Length: 0\r\n"
			"\r\n";
		send(new_fd, error_response, strlen(error_response), 0);
		return;
	}

	double totalCost = quantity->valuedouble * pricePerShare->valuedouble;

	const char* stockName = "Unknown";
	if (strcmp(stockSymbol->valuestring, "GPU") == 0) {
		stockName = "Graphics Cards Inc.";
	} else if (strcmp(stockSymbol->valuestring, "MBRD") == 0) {
		stockName = "Motherboards Inc.";
	} else if (strcmp(stockSymbol->valuestring, "SSD") == 0) {
		stockName = "Solid-State Drives Inc.";
	}

	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	char tradeTime[64];
	strftime(tradeTime, sizeof(tradeTime), "%Y-%m-%dT%H:%M:%S", t);

	sqlite3 *db;
	if (sqlite3_open("data_be/account_trades.db", &db) != SQLITE_OK){
		fprintf(stderr, "Failed to open trades DB: %s\n", sqlite3_errmsg(db));
		const char* error_response =
			"HTTP/1.1 500 Internal Server Error\r\n"
			"Content-Type: text/plain\r\n"
			"Access-Control-Allow-Origin: *\r\n"
			"Content-Length: 0\r\n"
			"\r\n";
		send(new_fd, error_response, strlen(error_response), 0);
		return;
	}

	const char *sql =
		"INSERT INTO trades (username, symbol, stock_name, quantity, price_per_share, total_cost, trade_type, trade_time) "
		"VALUES (?, ?, ?, ?, ?, ?, ?, ?)";

	sqlite3_stmt *stmt;
	if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK){
		fprintf(stderr, "Failed to prepare insert statement: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return;
	}


	sqlite3_bind_text(stmt, 1, username->valuestring, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, stockSymbol->valuestring, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, stockName, -1, SQLITE_STATIC);
	sqlite3_bind_int(stmt, 4, quantity->valueint);
	sqlite3_bind_double(stmt, 5, pricePerShare->valuedouble);
	sqlite3_bind_double(stmt, 6, totalCost);
	sqlite3_bind_text(stmt, 7, tradeType->valuestring, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 8, tradeTime, -1, SQLITE_STATIC);

	if (sqlite3_step(stmt) == SQLITE_DONE) {
		const char* success_response =
			"HTTP/1.1 200 OK\r\n"
			"Content-Type: application/json\r\n"
			"Access-Control-Allow-Origin: *\r\n"
			"Content-Length: 22\r\n"
			"\r\n"
			"{\"status\":\"success\"}";
		send(new_fd, success_response, strlen(success_response), 0);
		printf("Trade recorded successfully for %s\n", username->valuestring);
	} else {
		fprintf(stderr, "Error inserting trade: %s\n", sqlite3_errmsg(db));
		const char* error_response =
			"HTTP/1.1 500 Internal Server Error\r\n"
			"Content-Type: application/json\r\n"
			"Access-Control-Allow-Origin: *\r\n"
			"Content-Length: 26\r\n"
			"\r\n"
			"{\"status\":\"trade failed\"}";
		send(new_fd, error_response, strlen(error_response), 0);
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);
}

void get_trade_history_handler(int new_fd, const char *path){

	const char *query = strchr(path, '?');
	if (!query || strncmp(query, "?user=", 6) != 0) {
		fprintf(stderr, "Missing or invalid query parameter\n");
		close(new_fd);
		return;
	}

	const char *username = query + 6;
	sqlite3 *db;
	if (sqlite3_open("data_be/account_trades.db", &db) != SQLITE_OK) {
		fprintf(stderr, "Failed to open DB: %s\n", sqlite3_errmsg(db));
		close(new_fd);
        return;
    }

    const char *sql = "SELECT symbol, stock_name, quantity, price_per_share, total_cost, trade_type, trade_time FROM trades WHERE username = ?";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Prepare failed: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);

    cJSON *trades_array = cJSON_CreateArray();

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        cJSON *trade = cJSON_CreateObject();
        cJSON_AddStringToObject(trade, "symbol", (const char *)sqlite3_column_text(stmt, 0));
        cJSON_AddStringToObject(trade, "stockName", (const char *)sqlite3_column_text(stmt, 1));
        cJSON_AddNumberToObject(trade, "quantity", sqlite3_column_int(stmt, 2));
        cJSON_AddNumberToObject(trade, "pricePerShare", sqlite3_column_double(stmt, 3));
        cJSON_AddNumberToObject(trade, "totalCost", sqlite3_column_double(stmt, 4));
        cJSON_AddStringToObject(trade, "tradeType", (const char *)sqlite3_column_text(stmt, 5));
        cJSON_AddStringToObject(trade, "tradeTime", (const char *)sqlite3_column_text(stmt, 6));
        cJSON_AddItemToArray(trades_array, trade);
    }

    cJSON *response_obj = cJSON_CreateObject();
    cJSON_AddItemToObject(response_obj, "trades", trades_array);
    char *response_str = cJSON_PrintUnformatted(response_obj);

    char header[256];
    snprintf(header, sizeof(header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: application/json\r\n"
             "Access-Control-Allow-Origin: *\r\n"
             "Content-Length: %zu\r\n\r\n",
             strlen(response_str));

    send(new_fd, header, strlen(header), 0);
    send(new_fd, response_str, strlen(response_str), 0);

    free(response_str);
    cJSON_Delete(response_obj);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
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

		//CORS (Cross-Origin Resource Sharing) preflight request handling
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


		// Parse HTTP method and path
		char method[8];
		char path[256];
		sscanf(buffer, "%s %s", method, path);

		cJSON *root = NULL;

		// Only parse JSON for POST routes that require a body
		if (strcmp(method, "POST") == 0 &&
			(strcmp(path, "/login") == 0 ||
			strcmp(path, "/register") == 0 ||
			strcmp(path, "/trade") == 0)) {

			char *json_start = strstr(buffer, "\r\n\r\n");
			if (json_start == NULL) {
				printf("Error: Malformed HTTP request (missing body)\n");
				close(new_fd);
				continue;
			}
			json_start += 4;

			root = cJSON_Parse(json_start);
			if (root == NULL) {
				printf("Error parsing JSON file\n");
				close(new_fd);
				continue;
			}
		}

		const char *not_found =
			"HTTP/1.1 404 Not Found\r\n"
			"Content-Type: application/json\r\n"
			"Content-Length: 27\r\n\r\n"
			"{\"error\": \"Route not found\"}";

		if (strcmp(method, "POST") == 0 && strcmp(path, "/login") == 0) {
			login_handler(new_fd, root);
		} else if (strcmp(method, "POST") == 0 && strcmp(path, "/register") == 0) {
			register_handler(new_fd, root);
		} else if (strcmp(method, "POST") == 0 && strcmp(path, "/trade") == 0) {
			trade_handler(new_fd, root);
		} else if (strcmp(method, "GET") == 0 && strncmp(path, "/trades", 7) == 0) {
			get_trade_history_handler(new_fd, path); 
		} else {
			send(new_fd, not_found, strlen(not_found), 0);
		}

		if (root) cJSON_Delete(root);
		close(new_fd);
	}

	close(sockfd);
	return 0;
}
