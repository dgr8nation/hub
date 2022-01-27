/*
 * Network.h
 *
 * Stream socket routines
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#ifndef WH_BASE_NETWORK_H_
#define WH_BASE_NETWORK_H_
#include "ipc/inet.h"

namespace wanhive {
/**
 * Stream socket routines
 */
class Network {
public:
	//Returns a bound socket
	static int serverSocket(const char *service, SocketAddress &sa,
			bool blocking);
	//Returns a connected socket
	static int connectedSocket(const char *name, const char *service,
			SocketAddress &sa, bool blocking);
	//Same as above, but uses nameInfo
	static int connectedSocket(const NameInfo &ni, SocketAddress &sa,
			bool blocking);
	//Listen for incoming connections
	static void listen(int sfd, int backlog);
	/*
	 * Accept an incoming connection. If <blocking> is true then the new
	 * connection is configured for blocking IO.
	 */
	static int accept(int listenfd, SocketAddress &sa, bool blocking);
	//Wrapper for the shutdown(2) system call
	static int shutdown(int sfd, int how = SHUT_RDWR) noexcept;
	//Wrapper for the close(2) system call
	static int close(int sfd) noexcept;
	//set a socket's blocking/non-blocking IO state
	static void setBlocking(int sfd, bool block);
	//Test whether the socket is blocking
	static bool isBlocking(int sfd);
	//Creates a unix domain socket and binds it to the given address
	static int unixServerSocket(const char *path, SocketAddress &sa,
			bool blocking);
	//Connects to a Unix domain socket (connection may be in progress)
	static int unixConnectedSocket(const char *path, SocketAddress &sa,
			bool blocking);
	//Creates unnamed pair of connected sockets (unix domain)
	static void socketPair(int sv[2], bool blocking);
	//=================================================================
	/**
	 * Blocking IO utilities
	 */

	/*
	 * Writes <length> bytes from the buffer <buf> to the socket <sockfd>.
	 * Returns the actual number of bytes transferred.
	 */
	static size_t sendStream(int sockfd, const unsigned char *buf,
			size_t length);
	/*
	 * Reads at most <length> bytes from the socket <sockfd> into the buffer <buf>.
	 * If <strict> is true then throws exception on connection close or timeout.
	 * Returns the actual number of bytes transferred.
	 */
	static size_t receiveStream(int sockfd, unsigned char *buf, size_t length,
			bool strict = true);
	/*
	 * Set send and receive time-outs for blocking sockets.
	 * If the timeout value is 0 then the socket blocks forever.
	 */
	static void setReceiveTimeout(int sfd, int milliseconds);
	static void setSendTimeout(int sfd, int milliseconds);
	/*
	 * Combines the previous two functions. Both timeout values are measured
	 * in milliseconds. Negative timeout value is ignored.
	 */
	static void setSocketTimeout(int sfd, int recvTimeout, int sendTimeout);
};

} /* namespace wanhive */

#endif /* WH_BASE_NETWORK_H_ */
