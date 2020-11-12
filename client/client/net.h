#ifndef NET_LIB_H
#define NET_LIB_H

#include <stddef.h>

extern int listen_net(const char *address);
extern int connect_net(const char *address);
extern int close_net(int conn);

extern int send_net(int conn, const char *data, size_t size);
extern int recv_net(int conn, char *data, size_t size);
extern int accept_net(int listener);

#endif // NET_LIB_H
