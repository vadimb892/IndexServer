#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include "Windows.h"

#ifndef FINISH
#define FINISH true
#define WORK false
#endif

/**
 * @class ThreadPool
 * Менеджер потоків, що розподіляє завдання між обмеженою кількістю потоків, 
 * які виконоують вміст функції taskHandler.
 */

/**
 * @fn ThreadPool::ThreadPool<T>()
 * Конструктор класу ThreadPool.
 */

/**
 * @fn ThreadPool::ThreadPool<T>(std::function<int(T)> _taskHandler, int threadCount)
 * Конструктор класу ThreadPool.
 */

/**
 * @fn ThreadPool::ThreadPool<T>(const ThreadPool<T>& other)
 * Конструктор класу ThreadPool.
 */

 /**
  * @fn void ThreadPool::setTaskHandler(std::function<int(T)> _taskHandler)
  * Встановлює нову функцію обробки завдань для робочої функції потоків.
  */


/**
 * @fn void ThreadPool::initThreads(int newThreadCount) 
 * Ініціалізує/перезаписує усі потоки, але не запускає їх.
 */

/**
 * @fn void ThreadPool::addTask(T task)
 * Додає нове завдання у чергу.
 */

/**
 * @fn T ThreadPool::removeTask()
 * Отримує наступне завдання із черги.
 */

/**
 * @fn void ThreadPool::shutdown()
 * Зміннює змінну finish, що сповістить усі потоки завершити роботу. 
 */

/**
 * @fn void ThreadPool::workFunction(std::function<int(T)>& taskHandler)
 * Функція-обгортка, що отримує завдання і намагається його виконати, 
 * містить нескінченний цикл, у якому постійно приймаються завдання.
 */

/**
 * @fn void ThreadPool::joinAll()
 * Запускає в роботу усі виділені потоки.
 */

template <typename T>
class ThreadPool {
public:
	std::mutex mfinish;
	std::atomic<bool> finish = false; /**< Вказує чи слід завершити роботу усіх потоків. */

	ThreadPool<T>() {}
	ThreadPool<T>(std::function<int(T)> _taskHandler, int threadCount);
	ThreadPool<T>(const ThreadPool<T>& other);
	ThreadPool<T>& operator=(const ThreadPool<T>& other);
	void addTask(T task);
	T removeTask();
	void shutdown(bool evenNonEmptyQueue, long waitDuration);
	void initThreads(int newThreadCount);
	void setTaskHandler(std::function<int(T)> _taskHandler);
	bool joinAll();
	int getBusyThreadCount();

private:
	int threadCount; /**< Кількість потоків. */
	std::atomic<int> busyThreadCount = 0;
	std::vector<std::thread> threads; /**< Містить екземпляри виділених потоків. */
	std::queue<T> tasks; /**< Черга із завданнями. */
	std::mutex taskQueueMutex; /**< М'ютекс для черги завдань. */
	std::mutex lkm; /**< М'ютекс для умовної змінної task_added. */
	std::condition_variable task_added; /**< Умовна змінна, яка запускає потоки, що сплять. */
	std::mutex mtskHndl;
	std::mutex busyThreadCountMutex;
	std::function<int(T)> taskHandler; /**< функція, вміст якої виконують виділені потоки для кожного завдання. */

	void workFunction(std::function<int(T)>& taskHandler);
};

template <typename T>
ThreadPool<T>::ThreadPool(std::function<int(T)> _taskHandler, int threadCount) :
	taskHandler(_taskHandler), threadCount(threadCount) {
}

template <typename T>
void ThreadPool<T>::initThreads(int newThreadCount){
	threadCount = newThreadCount;
	threads.clear();
	for (int i = 0; i < threadCount; i++) {
		mtskHndl.lock();
		auto rf = std::ref(taskHandler);
		mtskHndl.unlock();
		std::thread t(&ThreadPool<T>::workFunction, this, rf);
		threads.push_back(std::move(t));
	}
}

template <typename T>
int ThreadPool<T>::getBusyThreadCount() {
	busyThreadCountMutex.lock();
	int result = busyThreadCount.load();
	busyThreadCountMutex.unlock();
	return result;
}

template <typename T>
void ThreadPool<T>::setTaskHandler(std::function<int(T)> _taskHandler) {
	mtskHndl.lock();
	this->taskHandler = _taskHandler;
	mtskHndl.unlock();
}

template <typename T>
ThreadPool<T>& ThreadPool<T>::operator=(const ThreadPool<T>& other) {
	if (this == &other)
		return *this;

	this->finish = other->finish;
	this->threads = other->threads;
	this->tasks = other->tasks;
	this->taskQueueMutex = other->taskQueueMutex;
	this->lkm = other->lkm;
	this->task_added = other->task_added;
	this->taskHandler = other->taskHandler;
	return *this;
}

template <typename T>
ThreadPool<T>::ThreadPool(const ThreadPool<T>& other) {
	if (this == &other)
		return;

	this->finish = other->finish;
	this->threads = other->threads;
	this->tasks = other->tasks;
	this->taskQueueMutex = other->taskQueueMutex;
	this->lkm = other->lkm;
	this->task_added = other->task_added;
	this->taskHandler = other->taskHandler;
}

template <typename T>
void ThreadPool<T>::addTask(T task) {
	taskQueueMutex.lock();
	tasks.push(task);
	taskQueueMutex.unlock();
	task_added.notify_one();
}

template <typename T>
T ThreadPool<T>::removeTask() {
	T task;
	std::unique_lock<std::mutex> lk(lkm);
	this->task_added.wait(lk, [&]() {
		taskQueueMutex.lock();
		long long size = tasks.size();
		taskQueueMutex.unlock();
		mfinish.lock();
		bool contin = this->finish.load();
		mfinish.unlock();
		return size == 0 && !contin ? false : true;
	});
	taskQueueMutex.lock();
	if (tasks.size() > 0) {
		task = tasks.back();
		tasks.pop();
	}
	taskQueueMutex.unlock();
	return task;
}

template <typename T>
void ThreadPool<T>::shutdown(bool evenNonEmptyQueue, long waitDuration) {
	if (!evenNonEmptyQueue) {
		while (tasks.size() > 0) {
			Sleep(waitDuration);
		}
	}
	mfinish.lock();
	this->finish.store(FINISH);
	mfinish.unlock();
	task_added.notify_all();
}

template <typename T>
void  ThreadPool<T>::workFunction(std::function<int(T)>& taskHandler) {
	T task;
	bool fns;
	while (true) {
		mfinish.lock();
		fns = this->finish.load();
		mfinish.unlock();
		if (fns == FINISH) {
			return;
		}
		task = removeTask();
		if (task.index == NULL) {
			return;
		}
		busyThreadCountMutex.lock();
		busyThreadCount.store(busyThreadCount.load() + 1);
		busyThreadCountMutex.unlock();
		if (taskHandler(task) == -1) {
			addTask(task);
		}
		busyThreadCountMutex.lock();
		busyThreadCount.store(busyThreadCount.load() - 1);
		busyThreadCountMutex.unlock();
	}
}

template <typename T>
bool ThreadPool<T>::joinAll() {
	finish.store(WORK);
	for (int i = 0; i < this->threadCount; i++) {
		threads[i].join();
	}
	return true;
}