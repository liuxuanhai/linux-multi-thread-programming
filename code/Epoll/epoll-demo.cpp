#include <stdio.h>     // for fprintf()
#include <unistd.h>    // for close(), read()
#include <sys/epoll.h> // for epoll_create1(), epoll_ctl(), struct epoll_event
#include <string.h>    // for strncmp

#define MAX_EVENTS 5
#define READ_SIZE 1024

int main()
{
  int epoll_fd = epoll_create1(0);
  if(epoll_fd == -1)
  {
    fprintf(stderr, "Failed to create epoll file descriptor\n");
    return 1;
  }
 
  struct epoll_event event, events[MAX_EVENTS];
  event.events = EPOLLIN;
  event.data.fd = 0; // monitor stdin (0)
  if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &event))
  {
    fprintf(stderr, "Failed to add file descriptor to epoll\n");
    close(epoll_fd);
    return 1;
  }
 
  char read_buffer[READ_SIZE + 1];
  bool stop = false;
  while(!stop)
  {
    printf("\nBlock for polling std input...\n");

    auto event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, 30000);
    printf("\n%d ready events\n", event_count);

    for(auto i = 0; i < event_count; i++)
    {
      printf("Reading file descriptor '%d', event_count '%d'\n", events[i].data.fd, event_count);

      auto bytes_read = read(events[i].data.fd, read_buffer, READ_SIZE);
      read_buffer[bytes_read - 1] = '\0';
      printf("Read: '%s'\n", read_buffer);
 
      if(!strncmp(read_buffer, "stop", 4)) stop = true;
    }
  }
 
  if(close(epoll_fd))
  {
    fprintf(stderr, "Failed to close epoll file descriptor\n");
    return 1;
  }

  return 0;
}
