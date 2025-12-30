#include "RxNet/network.h"
#include <RxNet/socket.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

void add_connection(rx_socket_t *source, rx_socket_t *target) {
  rx_connection_t *head = source->active_connections;
  rx_connection_t *new_connection = malloc(sizeof(rx_connection_t));
  memset(new_connection, 0, sizeof(rx_connection_t));
  new_connection->socket = (void *)target;
  if (head) {
    while (head->next_connection != NULL)
      head = head->next_connection;
    head->next_connection = new_connection;
  } else {
    source->active_connections = new_connection;
  }
}

void remove_connection(rx_socket_t *source, rx_socket_t *target) {
  rx_connection_t *head = source->active_connections;
  rx_connection_t *tail = source->active_connections;
  if (!head)
    return;
  while (((rx_connection_t *)head)->socket != target && head->next_connection) {
    tail = head;
    head = head->next_connection;
  }
  if (((rx_connection_t *)head)->socket != target)
    return;
  printf("Socket found: %p tail is %p\n", head, tail);
  if (head == tail) {
    if (!head->next_connection)
      source->active_connections = NULL;
    else
      source->active_connections = head->next_connection;
  } else if (!head->next_connection)
    tail->next_connection = NULL;
  else
    tail->next_connection = head->next_connection;
  free(head);
}

rx_socket_t *make_socket(int family, int protocol, int type) {
  rx_socket_t *sock = malloc(sizeof(rx_socket_t));
  memset(sock, 0, sizeof(rx_socket_t));
  socket_t sock_indx = socket(family, protocol, 0);
  sock->sock_index = sock_indx;
  sock->type = type;
  sock->param.sin_family = family;

  if (sock_indx == INVALID_SOCKET) {
    net_err((void *)sock, "Socket creation failed");
    free(sock);
    return NULL;
  }
  return sock;
}

void def_socket(rx_socket_t *socket, char *addr, int port) {
  socket->param.sin_addr.s_addr = addr == NULL ? INADDR_ANY : inet_addr(addr);
  socket->param.sin_port = htons(port);
  socket->address = addr;
  if (socket->type == SERVER_SOCKET) {
    if (bind(socket->sock_index, (struct sockaddr *)&socket->param,
             sizeof(socket->param)) == SOCKET_ERROR) {
      net_err((void *)socket, "Error while binding server socket");
    } else {
      printf("Server bound to address:%ui and port:%us\n",
             socket->param.sin_addr.s_addr, socket->param.sin_port);
    }
  }
}

int connect_socket(rx_socket_t *socket) {
  if (socket->type != CLIENT_SOCKET)
    return 0;
  if (connect(socket->sock_index, (struct sockaddr *)&socket->param,
              sizeof(socket->param)) == SOCKET_ERROR) {
    net_err(socket, "Error while connecting socket");
    return 0;
  }
  return 1;
}

static void *wait_for_connection(void *args) {
  rx_socket_t *sock = (rx_socket_t *)args;
CONNECTION:
  rx_socket_t *conn = malloc(sizeof(rx_socket_t));
  memset(conn, 0, sizeof(rx_socket_t));

  socklen_t addrLen = sizeof(conn->param);
  conn->sock_index =
      accept(sock->sock_index, (struct sockaddr *)&conn->param, &addrLen);
  if (conn->sock_index != INVALID_SOCKET) {
    add_connection(sock, conn);
    push_event(make_event(EVENT_CONNECTION, sock, conn));
    goto CONNECTION;
  }

  push_event(make_event(EVENT_NETWORK_ERROR, sock, conn));
  net_err(sock, "Error while accepting connection");

  return NULL;
}

int accept_socket(rx_socket_t *socket) {
  if (socket->type != SERVER_SOCKET)
    return 0;

  if (listen(socket->sock_index, 0) == SOCKET_ERROR) {
    net_err(socket, "Error while listening");
    return 0;
  }

  pthread_t thread;
  int rc = pthread_create(&thread, NULL, wait_for_connection, (void *)socket);
  pthread_detach(thread);

  return 1;
}

int send_data(rx_socket_t *socket, char *data, int data_size) {
  if (send(socket->sock_index, data, data_size, 0) == SOCKET_ERROR) {
    net_err(socket, "Error while sending data");
    return 0;
  }
  return 1;
}

static void *wait_on_data(void *args) {
  rx_socket_t *socket = (rx_socket_t *)args;
LISTEN_FOR_DATA:
  long ret =
      recv(socket->sock_index, socket->buffer, sizeof(socket->buffer), 0);
  if (ret != SOCKET_ERROR && ret != PEER_DISCONNECT) {
    socket->buffer[ret] = '\0';
    push_event(make_event(EVENT_DATA_RECEIVED, socket, NULL));
    goto LISTEN_FOR_DATA;
  }

  push_event(make_event(EVENT_NETWORK_ERROR, socket, NULL));
  net_err(socket, "Error while waiting for data");

  return NULL;
}

int listen_for_data(rx_socket_t *socket) {
  pthread_t thread;
  int rc = pthread_create(&thread, NULL, wait_on_data, (void *)socket);
  pthread_detach(thread);
  return 1;
}

void terminate_socket(rx_socket_t *target, rx_socket_t *socket) {
  remove_connection(target, socket);
  if (socket->address)
    free(socket->address);
  sock_close(socket->sock_index);
  free(socket);
}
