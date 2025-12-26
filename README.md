# rx-socket – Simple Cross-Platform Socket Library in C

A lightweight, cross-platform (Windows + Linux) socket wrapper library written in C.  
It provides an easy-to-use API for TCP server/client sockets, threaded connection acceptance, asynchronous data listening, and a basic event queue.

Designed to be simple, portable, and suitable for learning, small tools, games, or embedded projects.

## Features

- Cross-platform: Windows (Winsock2) and POSIX systems (Linux/macOS)
- Threaded connection acceptance (`accept_socket`)
- Threaded data receiving (`listen_for_data`)
- Simple event queue for connection & data events
- IPv4 support (easy to extend to IPv6)
- Clean separation: `network.h` for low-level init/cleanup, `socket.h` for higher-level socket objects

## Current API Overview

```c
// network.h
int         net_init(void);
void        net_cleanup(void);
void        sock_err(socket_t sock, char* msg);
void        push_event(event_type type, void* caller);
socket_event* pop_event();

// socket.h
rx_socket_t* make_socket(int family, int protocol, int type);  // CLIENT_SOCKET or SERVER_SOCKET
void         def_socket(rx_socket_t* sock, char* addr, int port);
int          connect_socket(rx_socket_t* sock);
int          accept_socket(rx_socket_t* sock);                 // starts accept thread
int          send_data(rx_socket_t* sock, char* buf, int len);
int          listen_for_data(rx_socket_t* sock);               // starts recv thread
