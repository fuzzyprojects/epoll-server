#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int setup_server(int port)
{
  int server_socket_fd;
  struct sockaddr_in srv_addr;

  server_socket_fd = socket(PF_INET, SOCK_STREAM, 0);
  if (server_socket_fd == -1)
    perror("socket");

  int enable = 1;
  if (setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    perror("setsockopt");

  memset(&srv_addr, 0, sizeof(struct sockaddr_in));
  srv_addr.sin_family = AF_INET;
  srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  srv_addr.sin_port = htons(port);

  if (bind(server_socket_fd, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) < 0)
    perror("bind");

  if (listen(server_socket_fd, 10) < 0)
    perror("listen");

  return server_socket_fd;
}

int accept_new_connection(int server_socket_fd)
{
  char *welcome_msg = "Hey welcome!\n";
  int sock_fd = accept(server_socket_fd, NULL, NULL);
  if (sock_fd == -1)
    perror("accept");

  send(sock_fd, welcome_msg, strlen(welcome_msg), 0);

  return sock_fd;
}
