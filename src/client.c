#include "RxNet/socket.h"
#include <RxNet/client.h>

short client_startup(char *addr, char *port, int family) {
  net_init();
  client_socket =
      make_socket(addr, port, get_socket_type(family, 0), CLIENT_SOCKET);
  if (!client_socket)
    return 0;
  connect_socket(client_socket);
  return 1;
}

rx_socket_t *get_socket() { return client_socket; }
