#ifndef SERVER_BLUEPRINT_HEADER
#define SERVER_BLUEPRINT_HEADER

#include <RxNet/socket.h>

#define SOCK_ALL -1

static rx_socket_t *IPV4_SOCKET;
static rx_socket_t *IPV6_SOCKET;
static void (**event_table)(network_event *);

/*
 * @brief Starts the server, creates the sockets, binds the socket and opens
 * them
 * @param port The desired port
 * @param family The types of socket to be accepted(UNSPEC for both)
 * @return 1 on success 0 on failure and also pushes a network_event
 */
short server_startup(char *port, int family);

/*
 * @brief Starts the event handler
 * @return 1 on success 0 on failure and also pushes a network_event
 */
short event_handler_startup();

/*
 * @brief A method to get socket connections
 * @param type The type of connection
 * @return A linked list with connections
 */
rx_connection_t *get_connection_list(int type);

/*
 * @brief A method to send a buffer to a connected socket
 * @param socket_indx The socket to send to(-1 for all)
 * @param buffer The buffer to send
 */
void send_buffer(int socket_indx, char *buffer, int size_of_buffer);

#endif
