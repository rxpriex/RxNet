#ifndef SOCKET_H
#define SOCKET_H

#include <RxNet/network.h>
#include <sys/socket.h>

#define CLIENT_SOCKET 1
#define SERVER_SOCKET 2

typedef struct {
  void *socket;
  void *next_connection;
} rx_connection_t;

typedef struct {
  socket_t sock_index;

  struct sockaddr_storage param;
  socklen_t addrlen;
  rx_connection_t *active_connections;

  int type;
  char *address;

  char buffer[1024];
} rx_socket_t;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief Creates a socket
 * @param family The socket family(IPV4,IPV6,UNIX)
 * @param protocl The protocol used(TCP,UDP)
 * @param type The type of socket(server or client socket)
 */
extern rx_socket_t *make_socket(char *, char *, struct addrinfo, int);

struct addrinfo get_socket_type(int, int);

char *resolve_host(struct sockaddr *, socklen_t);

/*
 * @brief Connects to a remote socket(defined in socket parameters)
 * @param socket The socket to connect
 */
extern int connect_socket(rx_socket_t *);

/*
 * @brief Accept remote connections in a seperate thread and push events in case
 * of a connection
 * @param socket The socket that accepts connections
 */
extern int accept_socket(rx_socket_t *);

/*
 * @brief Send data to the connected socket
 * @param socket The socket that sends data
 */
extern int send_data(rx_socket_t *, char *, int);

/*
 * @brief Listen for data in a seperate thread and push events in case data
 * arrives
 * @param socket The socket that listens
 */
extern int listen_for_data(rx_socket_t *);

/*
 * @brief Terminate a socket
 * @param socket Socket to be terminated
 */
extern void terminate_socket(rx_socket_t *, rx_socket_t *);

#ifdef __cplusplus
}
#endif

#endif
