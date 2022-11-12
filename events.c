#include <sys/epoll.h>

void delete_event(int epoll_fd, int fd, int type)
{
  struct epoll_event ev;
  ev.events = type;
  ev.data.fd = fd;

  epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &ev);
}

void add_new_event(int epoll_fd, int fd, int type)
{
  struct epoll_event ev;
  ev.events = type;
  ev.data.fd = fd;

  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);
}
