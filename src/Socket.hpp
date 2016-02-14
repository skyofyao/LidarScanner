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

	static const unsigned int CONNECTION_QUEUE_LENGTH = 5;
	static const unsigned int RECEIVE_BUFFER_SIZE = 256;

private:
	// listen socket file descriptor
	int listenfd;

	// connection socket file descriptor
	int connfd;
};
