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
  err_type *type = malloc(sizeof(err_type));
  type->cause = cause;
  type->err = sock_errno;
  printf("Network error occured: (%s). Last Error code: (%d)\n", msg,
         type->err);
  push_event(EVENT_NETWORK_ERROR, (void *)type);
}

network_event *pop_event() {
  network_event *event = event_queue;
  if (event == NULL)
    return event;
  event_queue = event_queue->next_event;
  return event;
}

void push_event(event_type type, void *caller) {
  network_event *event = malloc(sizeof(network_event));
  event->type = type;
  event->caller = caller;
  event->next_event = NULL;
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
