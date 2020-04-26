/* socket.c: Simple Socket Functions */

#include "spidey.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

/**
 * Allocate socket, bind it, and listen to specified port.
 *
 * @param   port        Port number to bind to and listen on.
 * @return  Allocated server socket file descriptor.
 **/
int socket_listen(const char *port) {
    /* Lookup server address information */

    struct addrinfo *results;
    struct addrinfo hints = {
        .ai_family   = AF_UNSPEC,   // IPv4 or IPv6
        .ai_socktype = SOCK_STREAM, // TCP
        .ai_flags    = AI_PASSIVE   // listen on socket
    };

    int status = getaddrinfo(NULL, port, &hints, &results);
    if(status != 0) {
        fprintf(stdout, "Error message: getaddrinfo\n");
        return -1;
    }

    /* For each server entry, allocate socket and try to connect */
    int socket_fd = -1;
    for (struct addrinfo *p = results; p != NULL && socket_fd < 0; p = p->ai_next) {
	/* Allocate socket */
        socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(socket_fd < 0) {
            continue;
        }

        // Setsockopt
        int optval = 1;
        setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

	/* Bind socket */
        if(bind(socket_fd, p->ai_addr, p->ai_addrlen) <0) {
            close(socket_fd);
            socket_fd = -1;
            continue;
        }

    	/* Listen to socket */
        if(listen(socket_fd, SOMAXCONN) < 0) {
            close(socket_fd);
            socket_fd = -1;
            continue;
        }
    }

    freeaddrinfo(results);
    return socket_fd;
}

/* vim: set expandtab sts=4 sw=4 ts=8 ft=c: */
