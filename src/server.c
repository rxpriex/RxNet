#include <RxNet/server.h>
#include <pthread.h>
#include <string.h>

void generic_handler(network_event *event) {
  switch (event->type) {
  case EVENT_CONNECTION:
    printf("New client connected from %s\n",
           ((rx_socket_t *)event->param)->address);
    char msg[] = {"Data received"};
    send_data(event->param, msg, sizeof(msg));
    listen_for_data((rx_socket_t *)event->param);
    break;
  case EVENT_DATA_RECEIVED:
    rx_socket_t *socket = (rx_socket_t *)event->caller;
    int x = strlen(socket->buffer);
    int y = strlen(socket->address);
    memcpy((socket->buffer + y + 1), socket->buffer, (x + 1));
    if (socket->address)
      memcpy(socket->buffer, socket->address, y);
    socket->buffer[strlen(socket->address)] = ':';
    printf("Data Received: %s\n", ((rx_socket_t *)event->caller)->buffer);
    send_buffer(SOCK_ALL, ((rx_socket_t *)event->caller)->buffer,
                sizeof(((rx_socket_t *)event->caller)->buffer));
    break;
  case EVENT_NETWORK_ERROR:
    n_error *error = (n_error *)event->param;
    printf("A network error occured:(%s) caused by %p. Last error:%i\n",
           error->error_msg, event->caller, error->err_code);
    switch (error->err_code) {
    case 0:
      printf("Peer disconnect\n");
      terminate_socket(
          event->caller,
          ((rx_socket_t *)(event->caller))->active_connections->socket);
      break;
    default:;
    }
    free(error);
  default:;
  }
}

short server_startup(char *port, int family) {
  net_init();
  short err_code = 1;
  if (family == IPV4 || family == UNSPEC) {
    IPV4_SOCKET =
        make_socket(NULL, port, get_socket_type(IPV4, 0), SERVER_SOCKET);
    if (!IPV4_SOCKET) {
      printf("IPV4 socket creation failed");
      err_code = 0;
    }
  } else {
    IPV4_SOCKET = NULL;
  }

  if (family == IPV6 || family == UNSPEC) {
    IPV6_SOCKET =
        make_socket(NULL, port, get_socket_type(IPV6, 0), SERVER_SOCKET);
    if (!IPV6_SOCKET) {
      printf("IPV6 socket creation failed");
      err_code = 0;
    }
  } else {
    IPV6_SOCKET = NULL;
  }

  if (IPV6_SOCKET) {
    if (!accept_socket(IPV6_SOCKET)) {
      err_code = 0;
    }
  }

  if (IPV4_SOCKET) {
    if (!accept_socket(IPV4_SOCKET)) {
      err_code = 0;
    }
  }

  return err_code;
}

static void *event_handle_routine(void *args) {
  void (**table)(network_event *) = (void (**)(network_event *))args;
  while (table[ENUM_TYPE_MAX_VALUE]) {
    network_event *event = pop_event();
    if (!event)
      continue;
    if (table[event->type])
      table[event->type](event);
    else
      table[ENUM_TYPE_MAX_VALUE](event);
    free(event);
  }

  return NULL;
}

short event_handler_startup() {
  event_table = malloc(sizeof(void *) * (ENUM_TYPE_MAX_VALUE + 1));
  memset(event_table, 0, sizeof(void *) * (ENUM_TYPE_MAX_VALUE + 1));
  event_table[ENUM_TYPE_MAX_VALUE] = &generic_handler;

  pthread_t handler_thread;
  int rc =
      pthread_create(&handler_thread, NULL, event_handle_routine, event_table);
  pthread_detach(handler_thread);

  return 1;
}

rx_connection_t *get_connection_list(int type) {
  if (type == IPV4)
    return IPV6_SOCKET->active_connections;
  else if (type == IPV6)
    return IPV6_SOCKET->active_connections;
  else
    return NULL;
}

void send_buffer(int sock_indx, char *buffer, int size_of_buffer) {
  rx_connection_t *ipv4_connections =
      IPV4_SOCKET ? IPV4_SOCKET->active_connections : NULL;
  rx_connection_t *ipv6_connections =
      IPV6_SOCKET ? IPV6_SOCKET->active_connections : NULL;
  while (ipv6_connections || ipv4_connections) {
    if (ipv4_connections) {
      if (sock_indx == SOCK_ALL ||
          sock_indx == ((rx_socket_t *)ipv4_connections->socket)->sock_index) {

        send_data(ipv4_connections->socket, buffer, size_of_buffer);
      }
      ipv4_connections = ipv4_connections->next_connection;
    }
    if (ipv6_connections) {
      if (sock_indx == SOCK_ALL ||
          sock_indx == ((rx_socket_t *)ipv6_connections->socket)->sock_index) {

        send_data(ipv6_connections->socket, buffer, size_of_buffer);
        ipv6_connections = ipv6_connections->next_connection;
      }
    }
  }
}
