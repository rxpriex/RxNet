#ifndef NETWORK_H
#define NETWORK_H

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET socket_t;
#define sock_close(s) closesocket(s)
#define sock_errno WSAGetLastError()
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
typedef int socket_t;
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define sock_close(s) close(s)
#define sock_errno errno
#endif

#define IPV4 AF_INET
#define IPV6 AF_INET6
#define UNSPEC AF_UNSPEC

#define TCP SOCK_STREAM
#define UDP SOCK_DGRAM

#define CONNECTION_TERMINATED 10054
#define PEER_DISCONNECT 0

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  EVENT_CONNECTION,
  EVENT_ACCEPTING_ERROR,
  EVENT_DATA_RECEIVED,
  EVENT_ERROR_WHILE_WAITING,
  EVENT_NETWORK_ERROR,
} event_type;

/*
 * @brief a struct that will get pushed onto the event queue
 * @field type The type of event
 * @field caller The cause/location of the event
 * @field param An additional parameter if needed
 * @field err The last error code(if an error occured)
 */
typedef struct {
  event_type type;
  void *caller;
  void *param;
  int err;

  void *next_event;
} network_event;

static network_event *event_queue;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief Initializing networking for windows systems
 * @return 0 on success, -1 on failure
 */
extern int net_init(void);

/*
 * @brief Networking cleanup for windows systems
 */
extern void net_cleanup(void);

/*
 * @brief A function when a socket error occurs
 * @param The socket that caused the error
 */
extern void net_err(void *, char *);

/*
 * @brief Pushes a new socket event to the event_queue
 * @param type The type of event
 * @param caller The object that called the event(might be 0)
 */
extern void push_event(network_event *);

/*
 * @brief Pops the oldest socket event from the event_queue
 */
extern network_event *pop_event();

/*
 * @brief Creates a new event
 * @param type The type of event
 * @param caller The cause/location of the event
 * @param param Optional parameter
 */
network_event *make_event(event_type, void *, void *);

#ifdef __cplusplus
}
#endif

#endif
