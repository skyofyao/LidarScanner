#pragma once

#include <string>

using namespace std;

class Socket
{
public:
	bool hostServer(const unsigned int port);
	bool connectToServer(const string& ipAddress, const unsigned int port);
	bool sendString(const string& message);
	bool acceptConnection();
	string receiveString();

private:
	bool setTimeout();

	// listen socket file descriptor
	int listenfd;

	// connection socket file descriptor
	int connfd;

	static const unsigned int CONNECTION_QUEUE_LENGTH;
	static const unsigned int RECEIVE_BUFFER_SIZE;

	static const unsigned int TIMEOUT_MILLISECONDS;
};
