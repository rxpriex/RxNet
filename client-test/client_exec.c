#include "RxNet/network.h"
#include "RxNet/socket.h"
#include <RxNet/client.h>
#include <assert.h>
#include <threads.h>

int handle_events(void *args) {
  while (1) {
    network_event *event = pop_event();
    if (!event)
      continue;
    switch (event->type) {
    case EVENT_DATA_RECEIVED:
      printf("%s\n", ((rx_socket_t *)event->caller)->buffer);
      break;
    case EVENT_NETWORK_ERROR:
      n_error *error = (n_error *)event->param;
      printf("A network error occured, terminating...(%s: %i)\n",
             error->error_msg, error->err_code);
      exit(-1);
    }
  }
}

int main(int argc, char **argv) {
  printf("-------------Client-Test-Executable--------------\n");
  assert(argc == 3);
  client_startup(argv[1], argv[2], IPV4);
  listen_for_data(get_socket());
  thrd_t thread;
  thrd_create(&thread, handle_events, NULL);
  thrd_detach(thread);

  char buffer[256];
  while (1) {
    scanf("%s", buffer);
    if (!send_data(get_socket(), buffer, sizeof(buffer))) {
      break;
    }
  }
}
