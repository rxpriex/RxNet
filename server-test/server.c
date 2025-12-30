#include "RxNet/network.h"
#include <RxNet/socket.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32 // Windows specific debug functionality
#include <psapi.h>
#include <stdio.h>
#include <windows.h>

void print_memory_info() {
  PROCESS_MEMORY_COUNTERS pmc;
  if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
    printf("Private bytes (heap + other) : %llu  bytes\n", pmc.PagefileUsage);
  }
}
#endif

char *comb_str(char *a, int strlenA, char *b, int strlenB) {
  char *res = malloc(strlenA + strlenB - 1);
  char *indx = res;
  while (*indx++ = *a++)
    ;
  indx--;
  while (*indx++ = *b++)
    ;
  return res;
}

void write_to_clients(rx_socket_t *socket, char *buffer, int size) {
  rx_connection_t *head = socket->active_connections;
  while (head) {
    rx_socket_t *socket = (rx_socket_t *)head->socket;
    send_data(socket, buffer, size);
    head = head->next_connection;
  }
}

void print_connections(rx_socket_t *socket) {
  rx_connection_t *head = socket->active_connections;
  if (!head)
    printf("No active connections\n");
  else {
    int i = 0;
    do {
      printf("Active connection Nr.%i -> %p\n", i, head->socket);
      head = head->next_connection;
      i++;
    } while (head);
  }
}

int main(int argc, char **argv) {
  net_init();
  rx_socket_t *socket = make_socket(IPV4, SOCK_STREAM, SERVER_SOCKET);
  def_socket(socket, NULL, 54521);
  accept_socket(socket);

  network_event *event;
  while (1) {
    event = pop_event();
    if (event != NULL) {
      rx_socket_t *caller = (rx_socket_t *)event->caller;
      switch (event->type) {
      case EVENT_CONNECTION:
        printf("Client %llu connected from: %lu\n",
               ((rx_socket_t *)event->caller)->sock_index,
               ((rx_socket_t *)event->caller)->param.sin_addr.s_addr);
        listen_for_data((rx_socket_t *)event->param);
        print_connections(socket);
        char buffer[] = {"A new Client connected"};
        write_to_clients(socket, buffer, sizeof(buffer));
        break;
      case EVENT_DATA_RECEIVED:
        printf("Data received from %lu: %s\n", caller->param.sin_addr.s_addr,
               caller->buffer);
        char* msg = malloc(strlen(caller->buffer) + 20);
          snprintf(msg, strlen(caller->buffer)+20, "%llu:%s", caller->sock_index, caller->buffer);
        write_to_clients(socket, msg, strlen(caller->buffer)+20);
          free(msg);
        break;
      case EVENT_NETWORK_ERROR:
        printf("Network error encountered: %i\n", event->err);
        switch (event->err) {
        case PEER_DISCONNECT:
        case CONNECTION_TERMINATED:
          printf("A remote host has terminated the connection\n");
          terminate_socket(socket, (rx_socket_t *)(event->caller));
          print_connections(socket);
          break;
        }
      }
    }
    free(event);
  }
}
