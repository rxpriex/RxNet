#include <RxNet/socket.h>
#include <pthread.h>

rx_socket_t *make_socket(int family, int protocol, int type) {
  rx_socket_t *sock = malloc(sizeof(rx_socket_t));
  memset(&sock->param, 0, sizeof(sock->param));
  socket_t sock_indx = socket(family, protocol, 0);
  if (sock_indx == INVALID_SOCKET)
    sock_err(sock_indx, "Socket creation failed");
  sock->socket = sock_indx;
  sock->type = type;
  sock->param.sin_family = family;
  return sock;
}

void def_socket(rx_socket_t *socket, char *addr, int port) {
  socket->param.sin_addr.s_addr = addr == NULL ? INADDR_ANY : inet_addr(addr);
  socket->param.sin_port = htons(port);
  socket->address = addr;
  if (socket->type == SERVER_SOCKET) {
    if (bind(socket->socket, (struct sockaddr *)&socket->param,
             sizeof(socket->param)) == SOCKET_ERROR) {
      sock_err(socket->socket, "Error while binding server socket");
    }
  }
}

int connect_socket(rx_socket_t *socket) {
  if (socket->type != CLIENT_SOCKET)
    return 0;
  if (connect(socket->socket, (struct sockaddr *)&socket->param,
              sizeof(socket->param)) == SOCKET_ERROR) {
    sock_err(socket->socket, "Error while connecting socket");
    return 0;
  }
  return 1;
}

static void *wait_for_connection(void *args) {
  rx_socket_t *sock = (rx_socket_t *)args;
CONNECTION:
  rx_socket_t *conn = malloc(sizeof(rx_socket_t));

  socklen_t addrLen = sizeof(conn->param);
  conn->socket =
      accept(sock->socket, (struct sockaddr *)&conn->param, &addrLen);
  if (conn->socket != INVALID_SOCKET) {
    push_event(EVENT_CONNECTION, conn);
    goto CONNECTION;
  }

  push_event(EVENT_ACCEPTING_ERROR, args);
  sock_err(sock->socket, "Error while accepting connection");

  return NULL;
}

int accept_socket(rx_socket_t *socket) {
  if (socket->type != SERVER_SOCKET)
    return 0;

  if (listen(socket->socket, 0)) {
    sock_err(socket->socket, "Error while listening");
    return 0;
  }

  pthread_t thread;
  int rc = pthread_create(&thread, NULL, wait_for_connection, (void *)socket);
  pthread_detach(thread);

  return 1;
}

int send_data(rx_socket_t *socket, char *data, int data_size) {
  if (send(socket->socket, data, data_size, 0) == SOCKET_ERROR) {
    sock_err(socket->socket, "Error while sending socket");
    return 0;
  }
  return 1;
}

static void *wait_on_data(void *args) {
  rx_socket_t *socket = (rx_socket_t *)args;
LISTEN_FOR_DATA:
  if (recv(socket->socket, socket->buffer, sizeof(socket->buffer), 0) !=
      SOCKET_ERROR) {
    push_event(EVENT_DATA_RECEIVED, socket);
    goto LISTEN_FOR_DATA;
  }

  push_event(EVENT_ERROR_WHILE_WAITING, socket);
  sock_err(socket->socket, "Error while waiting for data");

  return NULL;
}

int listen_for_data(rx_socket_t *socket) {
  pthread_t thread;
  int rc = pthread_create(&thread, NULL, wait_on_data, (void *)socket);
  pthread_detach(thread);
  return 1;
}
