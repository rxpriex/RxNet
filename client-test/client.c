#include <RxNet/socket.h>

int main(int argc, char **argv) {
  net_init();
  rx_socket_t *socket = make_socket(IPV4, SOCK_STREAM, CLIENT_SOCKET);
  def_socket(socket, "127.0.0.1", 54521);
  if (connect_socket(socket)) {
    printf("Connection successful!");

    while (1) {
      char buffer[256];
      scanf("%s", buffer);
      if (!send_data(socket, buffer, sizeof(buffer))) {
        break;
      }
    }
  }
}
