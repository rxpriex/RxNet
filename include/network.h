#ifndef NETWORK_H
#define NETWORK_H

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET socket_t;
#define sock_close(s)        closesocket(s)
#define sock_errno           WSAGetLastError()
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
typedef int socket_t;
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define sock_close(s)        close(s)
#define sock_errno           errno
#endif

#define IPV4 AF_INET
#define IPV6 AF_INET6
#define UNIX AF_UNIX

#define TCP SOCK_STREAM
#define UDP SOCK_DGRAM


#include <stdio.h>

typedef enum {
    EVENT_CONNECTION,
    EVENT_ACCEPTING_ERROR,
    EVENT_DATA_RECEIVED,
    EVENT_ERROR_WHILE_WAITING,
}event_type;

typedef struct {
    event_type type;
    void* caller;

    void* next_event;
}socket_event;

static socket_event* event_queue;

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
extern void sock_err(socket_t, char*);

/*
 * @brief Pushes a new socket event to the event_queue
 * @param type The type of event
 * @param caller The object that called the event(might be 0)
 */
extern void push_event(event_type, void*);

/*
 * @brief Pops the oldest socket event from the event_queue
 */
extern socket_event* pop_event();

#ifdef __cplusplus
}
#endif

#endif
