#include "Client.h"

Client::Client() :
	socket(INVALID_SOCKET) {}

Client::Client(const SOCKET _socket) :
	socket(_socket){}

Client& Client::operator=(const Client& other) {
	if (this == &other)
		return *this;

	this->recvBuffer = other.recvBuffer;
	this->sendBuffer = other.sendBuffer;
	this->socket = other.socket;
	return *this;
}

SOCKET Client::getSocket() {
	return socket;
}

void Client::close() {
	if (socket != INVALID_SOCKET)
		closesocket(socket);
}

int Client::send_(std::wstring message) {
	std::string smessage = ws2s(message);
	smessage += std::string(BUFFER_SIZE - (smessage.size() % BUFFER_SIZE), ' ');
	long long dataLength = smessage.size();
	char lenBuf[sizeof(long long)];
	memcpy(&lenBuf[0], &dataLength, sizeof(long long));
	if (send(socket, &lenBuf[0], sizeof(long long), 0) == -1) {
		printf("S) Calling send error!\n");
		return -1;
	}
	char sndBuf[BUFFER_SIZE];
	long long i = 0;
	while (i < dataLength) {
		memcpy(&sndBuf[0], &smessage[i], BUFFER_SIZE);
		if (send(socket, &sndBuf[0], BUFFER_SIZE, 0) == -1) {
			printf("S) Calling send error!\n");
			return -1;
		}
		i += BUFFER_SIZE;
	}
	return 0;
}

int Client::receive(std::wstring* answer) {
	long long dataLength = 0;
	char lenBuf[sizeof(long long)];
	std::string receivedString;

	if (recv(socket, &lenBuf[0], sizeof(long long), 0) == -1) {
		printf("S) Calling recv error!\n");
		return -1;
	}
	memcpy(&dataLength, lenBuf, sizeof(long long));
	receivedString.resize(dataLength);
	char rcvBuf[BUFFER_SIZE];
	long long i = 0;
	while (i < dataLength) {
		if (recv(socket, &rcvBuf[0], BUFFER_SIZE, 0) == -1) {
			printf("S) Calling recv error!\n");
			return -1;
		}
		memcpy(&receivedString[i], &rcvBuf[0], BUFFER_SIZE);
		i += BUFFER_SIZE;
	}
	std::string::size_type pos = receivedString.find_last_not_of(' ') + 1;
	receivedString = std::string(receivedString.begin(), receivedString.begin() + pos);
	*answer = s2ws(receivedString);
	return dataLength;
}