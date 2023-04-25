#ifndef _IPADDR_H_
#define _IPADDR_H_

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/route.h>
#include <sys/ioctl.h>
#include <strings.h>
#include <unistd.h>
#include <string.h>

static inline int get_interface_ipaddr(const char *interface_name, char *ip, int len)
{
	int i = 0;
	int sock_fd = -1;
	struct in_addr addr_temp;
	struct ifconf ifconf;
	struct ifreq *ifr = NULL;
	unsigned char buf[512] = {0};
	
	bzero(&addr_temp, sizeof(addr_temp));
	bzero(&ifconf, sizeof(ifconf));

	if ((NULL == interface_name) || (NULL == ip) || len < 16)
		return -1;

	if ((sock_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		return -1;

	ifconf.ifc_len = 512;
	ifconf.ifc_buf = (char*)buf;

	/*
	 * Get all interfaces list
	 */
	if (ioctl(sock_fd, SIOCGIFCONF, &ifconf) < 0) {
		close(sock_fd);
		return -1;
    }

	close(sock_fd);

	ifr = (struct ifreq*)buf;
	for(i = (ifconf.ifc_len/sizeof(struct ifreq)); i > 0; i--) {
		if(AF_INET == ifr->ifr_flags 
			&& 0 == memcmp(ifr->ifr_name, interface_name, strlen(interface_name))) {
			break;
		}
		ifr++;
	}

	if(0 == i)
		return -1;
	
	addr_temp = ((struct sockaddr_in*)&(ifr->ifr_addr))->sin_addr;
	
	(void)inet_ntop(AF_INET, &addr_temp, ip, len);
	
	return 0;
}

#endif
