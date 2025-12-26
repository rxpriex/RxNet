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

void sock_err(socket_t sock, char *msg) {
#ifdef _WIN32
  printf("Socket error occured: (%s). Last Error code: (%d)\n", msg,
         sock_errno);
  if (sock != INVALID_SOCKET)
    closesocket(sock);
#else
  perror(msg);
  if (sock != INVALID_SOCKET)
    close(sock);
#endif
}

socket_event *pop_event() {
  socket_event *event = event_queue;
  if (event == NULL)
    return event;
  event_queue = event_queue->next_event;
  return event;
}

void push_event(event_type type, void *caller) {
  socket_event *event = malloc(sizeof(socket_event));
  event->type = type;
  event->caller = caller;
  event->next_event = NULL;
  if (event_queue == NULL) {
    event_queue = event;
    return;
  }
  socket_event *head = event_queue;
  while (head->next_event != NULL) {
    head = head->next_event;
  }
  head->next_event = event;
}
