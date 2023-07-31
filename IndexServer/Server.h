#pragma once
#include <vector>
#include <string> 
#include "StringFunctions.h"
#include "VectorFunctions.h"
#include "InvertedIndex.h"
#include "ThreadPool.h"
#include "SearchFunctions.h"
#include "SiteFilter.h"
#include "Client.h"
#include <json/json.h>
#include <condition_variable>
#include <functional>
#include <random>

#ifndef FINISH
#define FINISH true
#define WORK false
#endif

/**
 * ��������� ��� �����, �� �������� ��� �������������� �������� �볺���.
 */
struct Task {
	std::wstring* result = nullptr; /**< �������� ��� ���������� ���������� �� ������� ����, �� ���� �������� �������� */
	Client client; /**< ������������� �볺��� �� ������ �������. */
	InvertedIndex* index = nullptr; /**< �������� �� ������������ ������. */
	int resultCount = 0; /**< ʳ������ �����, �� ��������� �볺���. */ 
	std::mutex* printMutex = nullptr;
};

/**
 * @fn void SET_NONBLOCK(SOCKET* socket)
 * ���������� ����� � ����������� ����.
 */

/**
 * @fn int taskHandler(Task task)
 * ̳����� ����������� ������ ��� ������� ������� �� ������ �������� �볺���.
 */

void SET_NONBLOCK(SOCKET* socket);
int clientTaskHandler(Task task);
int consoleTaskHandler(Task task);

/**
 * @class ProtectedServer
 * ����������� �������� ������� �񳺿 ������� ������� � ������ ��� ����� �� ������ ����������, 
 * ������ ���������� �볺��� � ��������� ������, ������ ��� ��� ���� ������ � ������ 
 * �������� ��������, ����� � �������� ��������� ������ �� ������������.
 */

/**
 * @fn ProtectedServer::ProtectedServer()
 * ����������� ����� ProtectedServer.
 */

/**
 * @fn void ProtectedServer::loop()
 * ���� ��������� ������, � ����� ����������� ���������� � ����������� ��� � ����� ThreadPool::tasks.
 */

/**
 * @fn void ProtectedServer::test(long maxThreadsCount, long queriesCount)
 * ������ ���������� ������䳿 ��� ������ ������� ������ � ������� ������.
 */

/**
 * @fn void ProtectedServer::interact()
 * ������� � ���� ����� �����䳿 ����������� � ������� ����� �������, �������� ����� ���������� ������. 
 */

/**
 * @fn Client ProtectedServer::handleConnetions()
 * �������� ���������� � ������� ���� ��� � ������ ���������� Client.
 */

template <typename T>
double countDuration(std::function<void(T, T)> function, T _1, T _2) {
	auto start = std::chrono::steady_clock::now();
	function(_1, _2);
	auto finish = std::chrono::steady_clock::now();
	std::chrono::duration<double> period = finish - start;
	return period.count() * 1000;
}

std::wstring randomString(std::size_t length);
std::vector<Site> generateRandomSiteVector(int strLength, int vectorLength);

class Server {
public:
	const static int siteCount = 10; /**< �������� �� ������������� ������� ���������� �����.*/

	Server();
	void loop();
	void test(long maxThreadsCount, long queriesCount);
	void interact();

private:
	struct sockaddr_in sa_serv = {0};
	const int port = 1111; /**< ���� TCP �� ����� ���� ����������� �볺��. */
	SOCKET listenSocket = INVALID_SOCKET; /**< ����� �������. */
	int threadCount = 4; /**< ʳ������ ������ � ProtectedServer::pool. */
	std::future<bool> serviceThread; /**< �������� ���� ��� ������� ���������. */
	std::future<bool> asyncJoin;
	InvertedIndex index; /**< ������������ ������. */
	ThreadPool<Task> pool = ThreadPool<Task>(consoleTaskHandler, threadCount); /**< ��� ������, ���� �������� ���������� ������� ��������. */
	std::mutex printMutex;

	Client handleConnection();
};

void check_net_error(int code, const char* message, SOCKET sock);