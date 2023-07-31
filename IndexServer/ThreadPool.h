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
 * �������� ������, �� ��������� �������� �� ��������� ������� ������, 
 * �� ���������� ���� ������� taskHandler.
 */

/**
 * @fn ThreadPool::ThreadPool<T>()
 * ����������� ����� ThreadPool.
 */

/**
 * @fn ThreadPool::ThreadPool<T>(std::function<int(T)> _taskHandler, int threadCount)
 * ����������� ����� ThreadPool.
 */

/**
 * @fn ThreadPool::ThreadPool<T>(const ThreadPool<T>& other)
 * ����������� ����� ThreadPool.
 */

 /**
  * @fn void ThreadPool::setTaskHandler(std::function<int(T)> _taskHandler)
  * ���������� ���� ������� ������� ������� ��� ������ ������� ������.
  */


/**
 * @fn void ThreadPool::initThreads(int newThreadCount) 
 * ��������/���������� �� ������, ��� �� ������� ��.
 */

/**
 * @fn void ThreadPool::addTask(T task)
 * ���� ���� �������� � �����.
 */

/**
 * @fn T ThreadPool::removeTask()
 * ������ �������� �������� �� �����.
 */

/**
 * @fn void ThreadPool::shutdown()
 * ������ ����� finish, �� ��������� �� ������ ��������� ������. 
 */

/**
 * @fn void ThreadPool::workFunction(std::function<int(T)>& taskHandler)
 * �������-��������, �� ������ �������� � ���������� ���� ��������, 
 * ������ ����������� ����, � ����� ������� ����������� ��������.
 */

/**
 * @fn void ThreadPool::joinAll()
 * ������� � ������ �� ������ ������.
 */

template <typename T>
class ThreadPool {
public:
	std::mutex mfinish;
	std::atomic<bool> finish = false; /**< ����� �� ��� ��������� ������ ��� ������. */

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
	int threadCount; /**< ʳ������ ������. */
	std::atomic<int> busyThreadCount = 0;
	std::vector<std::thread> threads; /**< ̳����� ���������� �������� ������. */
	std::queue<T> tasks; /**< ����� �� ����������. */
	std::mutex taskQueueMutex; /**< �'����� ��� ����� �������. */
	std::mutex lkm; /**< �'����� ��� ������ ����� task_added. */
	std::condition_variable task_added; /**< ������ �����, ��� ������� ������, �� ������. */
	std::mutex mtskHndl;
	std::mutex busyThreadCountMutex;
	std::function<int(T)> taskHandler; /**< �������, ���� ��� ��������� ������ ������ ��� ������� ��������. */

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