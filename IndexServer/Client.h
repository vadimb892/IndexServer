#pragma once
#include <string>
#include <Winsock.h>
#include "StringFunctions.h"
#pragma comment(lib, "ws2_32.lib")

#ifndef HOME
#define HOME "../"
#endif

#define CHK_ERR(err, s) if ((err) == -1) {perror(s); exit(1);}

#define BUFFER_SIZE 4096

/**
 * @class Client
 * ��������������� �� ������������� �볺��� � ������� ������� � ������
 * ���� ������ ��� ������������ ���������� ��'���� �� �볺���� � ���� ��������.
 * @see ProtectedServer::handleConnection()
 * @see ProtectedServer::taskHandler()
 * @see Task
 */

/**
* @fn Client::Client()
* ����������� Client.
*/

/**
* @fn Client::Client(const SOCKET _socket)
* ����������� Client.
* @param _socket ����� �볺���.
* @see Client::socket
*/

/**
 * @fn SOCKET Client::getSocket()
 * ����� ��� ��������� Client::socket.
 * @see Client::socket
 * @return ����� �볺���.
 */

/**
 * @fn SSL* Client::getSSL()
 * ����� ��� ��������� Client::pSSL.
 * @see Client::pSSL
 * @return �������� �� SSL.
 */

/**
 * @fn int Client::acceptSSL()
 * ����� ��� ������������ ���������� ���������� SSL �'������� �� �볺���� �� Client::socket.
 * @see Client::socket
 * @return ������ ����������.
 */

class Client {
public:
	sockaddr_in client_addr = {0};
	std::wstring recvBuffer; /**< ����� ��� ������� ����� �볺���. */
	std::wstring sendBuffer; /**< ����� ��� �������� ����� � �������. */
	Client();
	Client(const SOCKET _socket);
	Client& operator=(const Client& other);
	SOCKET getSocket();
	void close();
	int send_(std::wstring message);
	int receive(std::wstring* answer);

private:
	SOCKET socket; /**< ����� �볺��� ��� ������������� TCP �'�������, ��� ����� ���������� SSL �'�������. */
};