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
 * Використовується як представлення клієнта в середині сервера і містить
 * набір методів для встановлення захищеного зв'язку із клієнтом і його скидання.
 * @see ProtectedServer::handleConnection()
 * @see ProtectedServer::taskHandler()
 * @see Task
 */

/**
* @fn Client::Client()
* Конструктор Client.
*/

/**
* @fn Client::Client(const SOCKET _socket)
* Конструктор Client.
* @param _socket сокет клієнта.
* @see Client::socket
*/

/**
 * @fn SOCKET Client::getSocket()
 * Метод для отримання Client::socket.
 * @see Client::socket
 * @return сокет клієнта.
 */

/**
 * @fn SSL* Client::getSSL()
 * Метод для отримання Client::pSSL.
 * @see Client::pSSL
 * @return вказівник на SSL.
 */

/**
 * @fn int Client::acceptSSL()
 * Метод для встановлення захищеного протоколом SSL з'єднання із клієнтом за Client::socket.
 * @see Client::socket
 * @return статус підключення.
 */

class Client {
public:
	sockaddr_in client_addr = {0};
	std::wstring recvBuffer; /**< буфер для прийому даних клієнта. */
	std::wstring sendBuffer; /**< буфер для відправки даних з серверу. */
	Client();
	Client(const SOCKET _socket);
	Client& operator=(const Client& other);
	SOCKET getSocket();
	void close();
	int send_(std::wstring message);
	int receive(std::wstring* answer);

private:
	SOCKET socket; /**< Сокет клієнта уже встановленого TCP з'єднання, для якого відбувається SSL з'єднання. */
};