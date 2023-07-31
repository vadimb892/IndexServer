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
 * Контейнер для даних, що необхідні для обслуговування окремого клієнта.
 */
struct Task {
	std::wstring* result = nullptr; /**< Вказівник для повернення результату за потреби туди, де було створено завдання */
	Client client; /**< Представлення клієнта на стороні сервера. */
	InvertedIndex* index = nullptr; /**< Вказівник на інвертований індекс. */
	int resultCount = 0; /**< Кількість сайтів, що необхідна клієнту. */ 
	std::mutex* printMutex = nullptr;
};

/**
 * @fn void SET_NONBLOCK(SOCKET* socket)
 * Переводить сокет у неблокуючий стан.
 */

/**
 * @fn int taskHandler(Task task)
 * Містить послідовність команд для обробки завдань від одного окремого клієнта.
 */

void SET_NONBLOCK(SOCKET* socket);
int clientTaskHandler(Task task);
int consoleTaskHandler(Task task);

/**
 * @class ProtectedServer
 * Представляє серверну частину усієї системи проекту і містить для цього усі основні компоненти, 
 * отримує підключення клієнтів у основному потоці, передає їхні дані пулу потоків у вигляді 
 * окремого завдання, слідкує в окремому сервісному потоці за виключеннями.
 */

/**
 * @fn ProtectedServer::ProtectedServer()
 * Конструктор класу ProtectedServer.
 */

/**
 * @fn void ProtectedServer::loop()
 * Цикл основного потоку, у якому приймаються підключення і передаються дані у чергу ThreadPool::tasks.
 */

/**
 * @fn void ProtectedServer::test(long maxThreadsCount, long queriesCount)
 * Виконує тестування швидкодії для заданої кількості потоків і кількості запитів.
 */

/**
 * @fn void ProtectedServer::interact()
 * Запускає у циклі сеанс взаємодії користувача і серверу через консоль, дозволяє тільки виконувати запити. 
 */

/**
 * @fn Client ProtectedServer::handleConnetions()
 * Ініціалізує підключення і повертає його дані у вигляді екземпляру Client.
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
	const static int siteCount = 10; /**< Значення за замовчуванням кількості необхідних сайтів.*/

	Server();
	void loop();
	void test(long maxThreadsCount, long queriesCount);
	void interact();

private:
	struct sockaddr_in sa_serv = {0};
	const int port = 1111; /**< Порт TCP до якого буде підключатися клієнт. */
	SOCKET listenSocket = INVALID_SOCKET; /**< Сокет сервера. */
	int threadCount = 4; /**< Кількість потоків у ProtectedServer::pool. */
	std::future<bool> serviceThread; /**< Сервісний потік для обробки виключень. */
	std::future<bool> asyncJoin;
	InvertedIndex index; /**< Інвертований індекс. */
	ThreadPool<Task> pool = ThreadPool<Task>(consoleTaskHandler, threadCount); /**< Пул потоків, який обробляє паралельно отримані завдання. */
	std::mutex printMutex;

	Client handleConnection();
};

void check_net_error(int code, const char* message, SOCKET sock);