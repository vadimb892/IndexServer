#pragma once

#include <string>
#include <vector>
#include <atomic>
#include <functional>
#include <map>

#include "HashTable.h"
#include "FileManager.h"
#include "StringFunctions.h"
#include "ExceptionsEntities.h"
#include "Windows.h"

#ifndef FINISH
#define FINISH true
#define WORK false
#endif

/**
 * @class InvertedIndex
 * ����������� ������������ ������ � ������ ��� ����� �� �������� ����������. ������ 
 * ���� ����� - ������_������� � ������ �� � ��������� ��������. ���� �� �������� 
 * ������� ������ ������ �� ��������� �������, ������ ����� ����� � ��������� �� ��� �������,
 * ������ ���� ������� ��� ������� ���������.
 */

/**
 * @fn InvertedIndex::InvertedIndex()
 * ����������� ����� InvertedIndex.
 */

/**
 * @fn std::vector<std::wstring> InvertedIndex::get(std::wstring key)
 * �� ��������� ������ ��������� ����� �������, �� ������ ����, � ������� �������� �
 * ��������� ������. ������� ��������� � ������ �������.
 */

/**
 * @fn void InvertedIndex::add(std::wstring queue, std::vector<std::wstring> sources)
 * �� ��������� ������� � ������� ������, ���� ���� � ������ � ������� �������� �� �������� ������,
 * � ��� ��������� ���� ������ � ������� ������� ���������, ������ �������� ��� �������� ��� 
 * ����� ������, ��� ���糿 ���������� � �������� � ������ ������� �����. 
 */

/**
* @fn bool InvertedIndex::serviceLoop()
* ̳����� ���� ��� ���������� � ������� ���������. ������ ���������� �� ����� 
* InvertedIndex::exceptionQueue � ������� ������� �������� �� InvertedIndex::exceptionHandlers.
*/

/**
 * @fn std::vector<std::wstring> InvertedIndex::loadItemValue(std::wstring path, std::wstring wordId)
 * ��������� �������� �������� @struct HashItem �� ����������� ����� �� ������.
 */

/**
 * @fn std::wstring InvertedIndex::getUnicName(std::wstring type)
 * �������� �������� ��'� ��� ������ ����� � ������������ �����.
 */

/**
 * @fn void InvertedIndex::clear(HashTable* ht)
 * ����� ���� �� �������� ������� HashTable.
 */

 /**
  * @fn void InvertedIndex::clear()
  * ����� ����� ������� HashTable, ������� � �������.
  */

/**
 * @fn void shutdownService(evenNonEmptyQueue evenNonEmptyQueue, long waitDuration)
 * ������� ���� ������, ���� evenNonEmptyQueue, ��� ����� ���� ����� ���������
 * ���� ������, ��������� � ���� Sleep(waitDuration).
 */

 /**
  * @fn bool InvertedIndex::isOverflowed();
  * ����� �� ������������ ���� � ���� �� �������. �������� ��� ������ ������ ������� �������.
  */

class InvertedIndex {
public:
	InvertedIndex();
	std::vector<std::wstring> get(std::wstring key);
	void add(std::wstring queue, std::vector<std::wstring> sources);
	bool serviceLoop();
	static std::vector<std::wstring> loadItemValue(std::wstring path, std::wstring wordId);
	void shutdownService(bool evenNonEmptyQueue, long waitDuration);
	void clear();
	bool isOverflowed();
	void setBusyThreadCount(int busyThreadCount);
private:
	const long long startSize = 20; /**< ���������� ����� ����-����� ���������� HashTable. */
	std::atomic<long long> dictIdCounter = 1; /**< ˳������� ��������� ������ � ��������. */
	std::mutex mServiceFinish;
	std::atomic<bool> serviceFinish = false;
	
	std::function<void()> increaseTable = [&]() {
		if (table.increaseTable()) {
			exceptionQueueMutex.lock();
			exceptionQueue.push(EXCEPTION_MAX_TABLE);
			exceptionQueueMutex.unlock();
			exception.notify_one();
		}
	}; /**< �������� ���������� Exception::EXCEPTION_FULL_TABLE.
	   ������ ������� InvertedIndex::table � � ��� ����������
	   Exception::EXCEPTION_MAX_TABLE
	   ������� �� � ����� ��������� InvertedIndex::exceptionQueue.  */

	std::function<void()> increaseDictionary = [&]() {
		if (dictionary.increaseTable()) {
			exceptionQueueMutex.lock();
			exceptionQueue.push(EXCEPTION_MAX_DICT);
			exceptionQueueMutex.unlock();
			exception.notify_one();
		}
	}; /**< �������� ���������� Exception::EXCEPTION_FULL_DICT.
	   ������ ������� InvertedIndex::dictionary � � ��� ����������
	   Exception::EXCEPTION_MAX_DICT
	   ������� �� � ����� ��������� InvertedIndex::exceptionQueue.  */

	std::function<void()> saveTable = [&]() {
		std::wstring fileName = getUnicName(L"Section");
		if (table.getCapacity() + fm->getCurrentStoringCapacity()
			>= fm->getMaxStoringCapacity()) {
			exceptionQueueMutex.lock();
			exceptionQueue.push(EXCEPTION_MAX_MEMORY);
			exceptionQueueMutex.unlock();
			exception.notify_one();
		}
		table.saveTable(fm->getPath() + fileName);
		fm->setSections(fm->readSectionsInfo());
		clear(&table);
	}; /**< �������� ���������� Exception::EXCEPTION_MAX_TABLE.
	   ������ ������� � � ��� ���������� Exception::EXCEPTION_MAX_DICT
	   ������� �� � ����� ��������� InvertedIndex::exceptionQueue.  */

	std::function<void()> fullMemoryClear = [&]() {
		fm->clearMemory();
		fm->setSections(fm->readSectionsInfo());
		clear(&table);
		clear(&dictionary);
	}; /**< �������� ���������� Exception::EXCEPTION_MAX_DICT.
	   ������� ����� ���'��� � ��� �� ������������� ����� ����(������, �������).  */

	std::function<void()> decreaseData = [&]() {
		fm->decreaseStoredData(table.getCapacity() + dictionary.getCapacity());
	}; /**< �������� ���������� Exception::EXCEPTION_MAX_MEMORY. ������ ����� ��������� �����. */


	std::map<Exception, std::function<void()>> exceptionHandlers{ 
		{OK, []() {}},
		{EXCEPTION_FULL_TABLE, increaseTable},
		{EXCEPTION_FULL_DICT, increaseDictionary},
		{EXCEPTION_MAX_TABLE, saveTable},
		{EXCEPTION_MAX_DICT, fullMemoryClear},
		{EXCEPTION_MAX_MEMORY, decreaseData}
	}; /**< ������� ���� ��������� Exception � �� ���������. */

	std::condition_variable exception; /**< ������ �����, �� ������ �������� ���� ��� ���������� ����������. */
	std::queue<Exception> exceptionQueue; /**< ����� �� �� ������������� ������������. */
	std::mutex exceptionQueueMutex; /**< �'����� ����� ��������� InvertedIndex::exceptionQueue. */
	std::mutex serviceHandle; /**< �'����� ������� ������� InvertedIndex::serviceLoop. */
	std::mutex lkm; /**< �'����� ������ ����� InvertedIndex::exception. */

	HashTable table = HashTable((startSize != NULL) ? startSize : 1000); /**< ������� InvertedIndex::table, 
																		 �� ������ ������ ������� �� ������� ��
																		 �������� InvertedIndex::dictionary. */
	HashTable dictionary = HashTable((startSize != NULL) ? startSize : 1000); /**< ������� ��������� ������ � ���� id. */
	FileManager* fm = FileManager::getInstance(); /**< �������� ��������� �� ����� ���'��. */

	std::wstring getUnicName(std::wstring type);
	void clear(HashTable* ht);
};