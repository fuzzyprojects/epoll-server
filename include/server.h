int setup_server(int port);
int accept_new_connection(int server_socket_fd);
void echo(int client_socket_fd);
void *echo_threaded(void *arg);
