#ifndef CLIENT_BLUEPRINT_HEADER
#define CLIENT_BLUEPRINT_HEADER

#include <RxNet/socket.h>

static rx_socket_t *client_socket;
short client_startup(char *addr, char *port, int family);
rx_socket_t *get_socket();

#endif
