#include <RxNet/socket.h>
#include <pthread.h>

static void *listen_for_messages(void *args) {
  network_event *event;
  rx_socket_t *caller;
  while (1) {
    event = pop_event();
    if (event) {
      switch (event->type) {
      case EVENT_DATA_RECEIVED:
        caller = (rx_socket_t *)event->caller;
        printf("Data received from server: %s\n", caller->buffer);
        break;
      }
    }
  }
}

int main(int argc, char **argv) {
  net_init();
  rx_socket_t *socket = make_socket(IPV4, SOCK_STREAM, CLIENT_SOCKET);
  def_socket(socket, "127.0.0.1", 54521);
  if (connect_socket(socket)) {
    printf("Connection successful!\n");

    pthread_t thread;
    pthread_create(&thread, NULL, listen_for_messages, NULL);
    pthread_detach(thread);

    listen_for_data(socket);

    while (1) {
      char buffer[256];
      scanf("%s", buffer);
      if (!send_data(socket, buffer, sizeof(buffer))) {
        break;
      }
    }
  }
}
