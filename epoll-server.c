#include <stdio.h>

#include <sys/epoll.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "server.h"
#include "events.h"

#define PORT 9000
#define MAX_BUF_SIZE 4096
#define MAX_EVENTS 100

struct epoll_event events[MAX_EVENTS];
int connected_clients = 0;

int main(void)
{
  int server_socket_fd;
  int epoll_fd;
  int event_count;

  // Char size should be 1 byte, but just in case
  char *data = calloc(MAX_BUF_SIZE, sizeof(char));

  // Create a new server
  server_socket_fd = setup_server(PORT);

  // Create a new epoll and receive a file descriptor as response
  // We will use this epoll_fd for registering and deleting clients sockets
  // The 1 argument is completely arbitrary, epoll_create since Linux 2.6.8
  // ignores this argument but for compatibility it should be greater than 0
  epoll_fd = epoll_create(1);

  // Store the server fd in the epoll so we know if the event is from
  // this file descriptor (a new connection) or from other
  // (data from a previouslu connected client)
  add_new_event(epoll_fd, server_socket_fd, EPOLLIN);

  // Loop forever accepting new connections and reading clients sockets
  while (1)
  {
    // Wait for new events, the last parameter is a ms timeout.
    // If -1 waits forever
    event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    printf("We have %d new events\n", event_count);

    // Iterate over the events fired
    for (int i = 0; i < event_count; i++)
    {
      int current_fd = events[i].data.fd;
      int current_event = events[i].events;

      int is_server = current_fd == server_socket_fd;
      int is_data_from_client_incoming = current_event & EPOLLIN;

      int server_is_accepting_new_connection = is_server && is_data_from_client_incoming;

      // The event is from the server_socket it means that a new client connected
      // Store the new client socket file descriptor in the epoll to avoid blocking
      // until it sends information
      if (server_is_accepting_new_connection)
      {
        int new_client_socket_fd = accept_new_connection(server_socket_fd);
        printf("Hey! a new client: <%d>\n", new_client_socket_fd);
        add_new_event(epoll_fd, new_client_socket_fd, EPOLLIN);
        connected_clients++;
        printf("We have %d clients\n", connected_clients);
        break;
      }

      // The event seems to be from a client (because we are sure that is not
      // from the server), read the information client send, we are not blocking
      // in this recv because data is already there.
      // NOTE: Only one read (recv) is being done, if the input is longer than 4096
      // we will have problems, this is only a proof of concept.
      if (is_data_from_client_incoming)
      {
        int read_bytes = recv(current_fd, data, MAX_BUF_SIZE, 0);
        int client_closed_connection = read_bytes == 0;
        if (client_closed_connection) {
          delete_event(epoll_fd, current_fd, EPOLLIN);
          close(current_fd);
          connected_clients--;
          printf("Client disconnected\n");
          printf("We have %d clients\n", connected_clients);
          break;
        }

        printf("<%d> client says: %s", current_fd, data);
        memset(data, 0, read_bytes);
        break;
      }
    }
  }
}
