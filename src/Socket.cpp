#include "Socket.hpp"

#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

const unsigned int Socket::CONNECTION_QUEUE_LENGTH = 5;
const unsigned int Socket::RECEIVE_BUFFER_SIZE = 256;
const unsigned int Socket::TIMEOUT_MILLISECONDS = 50;

bool Socket::hostServer(const unsigned int port)
{
	struct sockaddr_in svradd;

	// create the server socket
	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	// the socket function gives a negative value on not connecting
	if (listenfd < 0)
	{
		cerr << "[Error] Cannot open socket on port " << port << endl;
		return false;
	}

	// set port
	memset((char *) &svradd, '\0',sizeof(svradd));
	svradd.sin_family = AF_INET;
	svradd.sin_addr.s_addr = INADDR_ANY;
	svradd.sin_port = htons(500);

	// bind
	if (bind(listenfd, (struct sockaddr*)&svradd, sizeof(svradd)) < 0)
	{
		cerr << "[Error] Error on binding port " << port << endl;
		return false;
	}

	// listen
	if (listen(listenfd, CONNECTION_QUEUE_LENGTH) < 0)
	{
		cerr << "[Error] Failed to listen on port " << port << endl;
		return false;
	}
	else
	{
		cout << "[Info] Listening on port " << port << endl;
	}
	return true;
}

bool Socket::acceptConnection()
{
	struct sockaddr_in cliadd;
	socklen_t clilen = sizeof(cliadd);

	// accept connection
	connfd = accept(listenfd, (struct sockaddr*) &cliadd, &clilen);
	if (connfd < 0)
	{
		cerr << "[Error] Error receiving connection on port " << endl;
		return false;
	}
	else
	{
		cout << "[Info] Got connection from " << inet_ntoa(cliadd.sin_addr) << ":" << ntohs(cliadd.sin_port) << endl;
	}

	if (!setTimeout())
	{
		return false;
	}

	return true;
}

bool Socket::connectToServer(const string& ipAddress, const unsigned int port)
{
	struct sockaddr_in svradd;

	// create the client socket
	connfd = socket(AF_INET, SOCK_STREAM, 0);

	// the socket function gives a negative value on not connecting
	if (connfd < 0)
	{
		cerr << "[Error] Cannot open socket to " << ipAddress << ":" << port << endl;
		return false;
	}

	// set address and port
	memset((char *) &svradd, '\0',sizeof(svradd));
	svradd.sin_family = AF_INET;
	svradd.sin_addr.s_addr = inet_addr(ipAddress.c_str());
	svradd.sin_port = htons(port);

	// connect
	if (connect(connfd, (struct sockaddr*)&svradd, sizeof (svradd)) < 0)
	{
		cerr << "[Error] Error connecting to " << ipAddress << ":" << port << endl;
		return false;
	}

	if (!setTimeout())
	{
		return false;
	}

	cout << "[Info] Connected to " << ipAddress << ":" << port << endl;
	return true;
}

bool Socket::sendString(const string& message)
{
	if (write(connfd, message.c_str(), message.length()) < 0)
	{
		cerr << "[Error] Error sending string" << endl;
		return false;
	}
	return true;
}

string Socket::receiveString()
{
	char buffer[RECEIVE_BUFFER_SIZE];
	memset((char *) &buffer, '\0',sizeof(buffer));

	if (read(connfd, buffer, RECEIVE_BUFFER_SIZE) < 0)
	{
		cerr << "[Error] Error receiving string" << endl;
	}

	return buffer;
}

bool Socket::setTimeout()
{
	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = TIMEOUT_MILLISECONDS * 1000;

	if (setsockopt(connfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
	{
		cerr << "[Error] Error setting socket timeout" << endl;
		return false;
	}

	if (setsockopt(connfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
	{
		cerr << "[Error] Error setting socket timeout" << endl;
		return false;
	}

	return true;
}

