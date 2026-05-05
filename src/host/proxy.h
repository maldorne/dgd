/*
 * PROXY protocol v1 parser (Maldorne fork).
 *
 * Public interface for parsing a PROXY protocol v1 header from a freshly
 * accepted TCP socket. Used to expose the real client address when DGD
 * runs behind a reverse proxy that supports the protocol (Traefik,
 * HAProxy, ...).
 *
 * The whole feature is gated by SUPPORT_PROXY_PROTOCOL. When that macro
 * is not defined, this header declares no symbols and the corresponding
 * translation unit is empty.
 */

#ifndef PROXY_H
#define PROXY_H

#ifdef SUPPORT_PROXY_PROTOCOL

# include <netinet/in.h>

struct ProxyAddr {
    int family;                       /* AF_INET, AF_INET6, or 0 (none) */
    union {
	struct in_addr  v4;
	struct in6_addr v6;
    };
    unsigned short port;
};

/*
 * If the connection on `fd` starts with a valid PROXY protocol v1 header,
 * consume it from the socket, fill *out with the real client address,
 * and return true. Otherwise leave the socket untouched and return false.
 *
 * `fd` must be a non-blocking accepted socket. The function waits up to
 * ~100 ms for the header to arrive before bailing out.
 */
extern bool proxyParseV1(int fd, ProxyAddr *out);

#endif /* SUPPORT_PROXY_PROTOCOL */

#endif /* PROXY_H */
