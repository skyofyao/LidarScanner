#include "Socket.hpp"

#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;

const string ipAddress = "192.168.33.1";
const int port = 503;

int main()
{
	Socket socket;
	if (socket.connectToServer(ipAddress, port))
	{
		cout << "Connected!\n";
	}
	else
	{
		return 1;
	}

	socket.sendString("EE 1\r\n"); // Encoder Enable
	socket.sendString("A 10000\r\n"); // Set Acceleration
	socket.sendString("D 50000\r\n"); // Set Decleration
	socket.sendString("VI 100\r\n"); // Set Initial Velocity
	socket.sendString("VM 1000\r\n"); // Set Maximum Velocity
	socket.sendString("RC 80\r\n"); // Set Run Current to 80%
	socket.sendString("HC 80\r\n"); // Set Hold Current to 80%
	socket.sendString("MR 100\r\n"); // Move Relative
	sleep(2);
	socket.sendString("MR -100\r\n"); // Move Relative
}
