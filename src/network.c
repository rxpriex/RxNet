#include <RxNet/network.h>

int net_init() {
#ifdef _WIN32
  WSADATA wsaData;
  return WSAStartup(MAKEWORD(2, 2), &wsaData);
#else
  return 0;
#endif
}

void net_cleanup() {
#ifdef _WIN32
  WSACleanup();
#else
  // No cleanup needed for Unix-like systems
#endif
}

void net_err(void *cause, char *msg) {
  printf("Network error occured: (%s).\n", msg);
}

network_event *pop_event() {
  network_event *event = event_queue;
  if (event == NULL)
    return event;
  event_queue = event_queue->next_event;
  return event;
}

network_event *make_event(event_type type, void *caller, void *param) {
  network_event *event = malloc(sizeof(network_event));
  memset(event, 0, sizeof(network_event));

  event->type = type;
  event->caller = caller;
  event->param = param;
  event->err = sock_errno;

  return event;
}

void push_event(network_event *event) {
  if (event_queue == NULL) {
    event_queue = event;
    return;
  }
  network_event *head = event_queue;
  while (head->next_event != NULL) {
    head = head->next_event;
  }
  head->next_event = event;
}
