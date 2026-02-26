#include <RxNet/server.h>
#include <assert.h>

int main(int argc, char **argv) {
  printf("-------------Server-Test-Executable--------------\n");
  assert(argc == 2);
  server_startup(argv[1], UNSPEC);
  event_handler_startup();
  while (1) {
  }
}
