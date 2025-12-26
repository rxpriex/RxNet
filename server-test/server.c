#include <RxNet/socket.h>

int main(int argc, char **argv) {
  net_init();
  rx_socket_t *socket = make_socket(IPV4, SOCK_STREAM, SERVER_SOCKET);
  def_socket(socket, "127.0.0.1", 8080);
  accept_socket(socket);

  socket_event *event;
  while (1) {
    event = pop_event();
    if (event != NULL) {
      rx_socket_t *caller = (rx_socket_t *)event->caller;
      switch (event->type) {
      case EVENT_CONNECTION:
        printf("Client connected from: %lu\n",
               ((rx_socket_t *)event->caller)->param.sin_addr.S_un.S_addr);
        listen_for_data((rx_socket_t *)event->caller);
        break;
      case EVENT_DATA_RECEIVED:
        printf("Data received from %lu: %s\n",
               caller->param.sin_addr.S_un.S_addr, caller->buffer);
        break;
      }
    }
    free(event);
  }
}
