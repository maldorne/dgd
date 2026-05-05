/*
 * PROXY protocol v1 parser (Maldorne fork).
 *
 * When DGD runs behind a TCP reverse proxy that speaks the HAProxy PROXY
 * protocol v1 (Traefik, HAProxy, ...), each accepted connection arrives
 * prefixed with a single ASCII line of the form
 *
 *   "PROXY TCP4 <src_ip> <dst_ip> <src_port> <dst_port>\r\n"
 *
 * (or "PROXY TCP6 ..." for IPv6 clients). This file implements a small
 * stateless parser for that line so the host layer can replace the
 * connection's getpeername() address with the real client address.
 *
 * The whole translation unit is gated by SUPPORT_PROXY_PROTOCOL; without
 * that define the resulting object file contains no symbols and the
 * driver behaves exactly like upstream DGD.
 *
 * Spec: https://www.haproxy.org/download/1.8/doc/proxy-protocol.txt
 */

#include "proxy.h"

#ifdef SUPPORT_PROXY_PROTOCOL

# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <sys/socket.h>
# include <sys/select.h>
# include <arpa/inet.h>

bool proxyParseV1(int fd, ProxyAddr *out)
{
    char buf[108];                    /* v1 max line length */
    fd_set readfds;
    struct timeval tv;
    int n;
    char *end;
    int hdr_len;
    char proto[6], src_ip[46], dst_ip[46];
    int src_port, dst_port;

    out->family = 0;
    out->port = 0;

    /*
     * The accepted socket is non-blocking. Wait briefly for data to
     * avoid a race where MSG_PEEK returns EWOULDBLOCK because the proxy
     * has not yet sent the header line.
     */
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    tv.tv_sec = 0;
    tv.tv_usec = 100000;              /* 100 ms */
    if (select(fd + 1, &readfds, NULL, NULL, &tv) <= 0) {
	return false;
    }

    n = recv(fd, buf, sizeof(buf) - 1, MSG_PEEK);
    if (n < 6 || memcmp(buf, "PROXY ", 6) != 0) {
	return false;
    }

    end = (char *) memchr(buf, '\n', n);
    /* spec mandates CRLF; reject bare LF */
    if (end == NULL || end == buf || *(end - 1) != '\r') {
	return false;
    }

    hdr_len = (int) (end - buf) + 1;
    /* consume the header bytes from the socket */
    (void) recv(fd, buf, hdr_len, 0);
    buf[hdr_len] = '\0';

    if (sscanf(buf, "PROXY %5s %45s %45s %d %d",
	       proto, src_ip, dst_ip, &src_port, &dst_port) != 5) {
	return false;
    }

    if (strcmp(proto, "TCP4") == 0) {
	struct in_addr v4;
	if (inet_pton(AF_INET, src_ip, &v4) != 1) {
	    return false;
	}
	out->family = AF_INET;
	out->v4 = v4;
	out->port = (unsigned short) src_port;
	return true;
    }
    if (strcmp(proto, "TCP6") == 0) {
	struct in6_addr v6;
	if (inet_pton(AF_INET6, src_ip, &v6) != 1) {
	    return false;
	}
	out->family = AF_INET6;
	out->v6 = v6;
	out->port = (unsigned short) src_port;
	return true;
    }
    /* UNKNOWN or unrecognized proto: header consumed, addr unchanged */
    return false;
}

#endif /* SUPPORT_PROXY_PROTOCOL */
