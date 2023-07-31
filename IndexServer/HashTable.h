#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <mutex>
#include <iostream>
#include <sstream>
#include <array>
#include <windows.h>

#include "VectorFunctions.h"

#define ZERO LLONG_MAX

/**
 * ��������� ������, �� ������ ���������� ���� - ��������, �����
 * �������� �� ������ ���������� �������� � ������ ����� 
 */
struct HashItem {
	std::wstring key; /**< ���� �� ���� ������������ ������ ������� �������� � ������ HashItem::items. */
	std::vector<std::wstring> value; /**< �������� � ������ ������ �������. */
	long long byteSize = 0; /**< ����� ��������. */
	long long next = -1; /**< ������ ���������� ���������� HashItem � ������ HashItem::colisions,
						 �� ���� �������� �����. */
};

/**
 * @class HashTable
 * ����������� ���������� � ��������� ��� �������, ����� � �������� ���� � ��������.
 */

/**
 * @fn HashTable::HashTable(long long size);
 * ����������� ����� HashTable.
 */

 /**
  * @fn HashTable::HashTable(const HashTable& other);
  * ����������� ����� HashTable.
  */

/**
* @fn HashTable::HashTable& operator=(const HashTable& other);
* ����������� ����� HashTable.
*/

/**
 * @fn int HashTable::add(std::wstring key, std::vector<std::wstring> value)
 * ���� �� ���������� �������� ���� ��������: 
 * 1. ���� ���� �� ����, ������������ ��������; 
 * 2. ���� ���� ���� � �� ����� ��, ��� ���� �������� �� ��������� ��������;
 * 3. ���� ���� ���� � �� �� ��������, ��� ������������ �����.
 */

/**
 * @fn std::vector<std::wstring> HashTable::get(std::wstring key)
 * ���� �� ��������� ������� �������� �� ������.
 */

/**
 * @fn int HashTable::increaseTable()
 * ��������� ������ ������� � ������ ������� ������.
 */

/**
 * @fn void HashTable::saveTable(std::wstring path)
 * ���������� � ������������ ����� ������ � ����������� ������.
 */

/**
 * @fn long long HashTable::hashFunction(std::wstring key, long long size)
 * ��� �������, �� ���������� ����-������ � long long, �� ��������
 * ���������� �������� ����� � ������ �� ������� �����. 
 */

/**
 * @fn long long HashTable::getCapacity()
 * ������� �������� ��'�� ��������� � ���'�� ����� �� ����� HashTable::currentCapacity.
 */

/**
 * @fn long long HashTable::getCount()
 * ������� ������� ������� ��������� ������ �� ����� HashTable::count.
 */

/**
 * @fn long long HashTable::getSize()
 * ������� �������� ����� ��������� ������ HashTable::items, �������� ������� 
 * ������ �� ����� HashTable::size.
 */

 /**
  * @fn double HashTable::getPercent()
  * ������� ��������� % ���������� ��������� ������ HashTable::items.
  */

/**
 * @fn std::vector<HashItem> HashTable::createNewVector(long long s)
 * ������� ������ �� ������� ��'���� ������.
 */

/**
 * @fn void HashTable::handleCollision(HashItem item, long long colisionIndex)
 * �������� ���糿, ������� �� � ������� ������ HashTable::colisions.
 */

/**
 * @fn long long HashTable::getItemCapacity(HashItem item)
 * ������� ����� � ������ HashItem.
 */

/**
 * @fn std::array<long long, 4> HashTable::itemToArray(HashItem item, size_t cellSize, long long valueAddress)
 * ���������� HashItem � ����� long long, ���� ������ ��������� 
 * �� ������, �� ���������� ��������, ��� ������ � ����.
 */

/**
 * @fn std::wstring HashTable::writeVectorToString(std::vector<std::wstring> vector)
 * ������������ ������� � ������ �� ����������� @var HashTable::delimiter.
 */

class HashTable {
public:
	const static std::wstring delimiter;  /**< ��������� �� �������� ���������� ������ HashItem::value.
										  ���������� �� ��� ��'������� ������ HashItem::value � ����� 
										  ������ ��� ������ � ���� �񳺿 ������� � ��������� ���������� �������
										  ��������� ����� �� ���������� ��������. */
	const static size_t cellSize;  /**< ����� ����������� ��������� ����� ��� ��������� ������� ��������
								   HashItem � �������� ������. */

	HashTable(long long size);
	HashTable(const HashTable& other);
	HashTable& operator=(const HashTable& other);
	int add(std::wstring key, std::vector<std::wstring> value);
	std::vector<std::wstring> get(std::wstring key);
	int increaseTable();
	void setBusyThreadCount(int busyThreadCount);
	void saveTable(std::wstring path);
	static long long hashFunction(std::wstring key, long long size);
	long long getCapacity();
	long long getCount();
	long long getSize();
	double getPercent();

private:
	const double percent = 0.75; /**< ��������� ������� ���������� �������. */
	const long long maxCapacity = 2000000000; /**< ��������� ��'�� �񳺿 �������, �� �������� � ���'��. */
	const long long itemsSizeOnItemsMutexRatio = 32;
	std::atomic<int> busyThreadCount = 0;
	std::atomic<long long> size; /**< �������� �����, ������� ������ �������. */
	std::atomic<long long> count = 0; /**< ������� ������� �������� ������. */
	std::atomic<long long> currentCapacity = 0; /**< �������� ����� � ������ ����� �������. */
	std::atomic<long long> itemsMutexesSize = 0;
	std::atomic<bool> increaseInProcess = false;
	std::atomic<unsigned int> currentStopedThreadCount = 0;
	std::vector<HashItem> items; /**< �������� ��������� ��� HashItem. */
	std::vector<HashItem> colisions; /**< ��������� ��� HashItem, � ���� ������� �����
									 �� ���������� ��������� ���������� HashTable::items. */
	std::vector<std::mutex> itemsMutexes; /**< �'����� ��� ��������� ���������� HashTable::items. */
	std::mutex colisionsMutex; /**< �'����� ��� ���������� ����� HashTable::colisions. */
	//std::mutex itemsMutexesSizeMutex;
	std::mutex sizeMutex;
	std::mutex countMutex;
	std::mutex currentCapacityMutex;
	std::mutex increaseInProcessMutex;

	std::vector<HashItem> createNewVector(long long s);
	void handleCollision(HashItem item, long long colisionIndex);
	long long getItemCapacity(HashItem item);
	std::array<long long, 4> itemToArray(HashItem item,size_t cellSize, long long valueAddress);
	std::wstring writeVectorToString(std::vector<std::wstring> vector);
	long long getMutexIndex(long long itemIndex);
	void checkIncreaseProcess();
};