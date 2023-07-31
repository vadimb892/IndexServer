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
 * Компонент списків, що містить відповідності ключ - значення, розмір
 * значення та індекс наступного елемента у списку колізій 
 */
struct HashItem {
	std::wstring key; /**< Ключ за яким обчислюється індекс позиції значення у списку HashItem::items. */
	std::vector<std::wstring> value; /**< Значення у вигляді списку ресурсів. */
	long long byteSize = 0; /**< Розмір значення. */
	long long next = -1; /**< Індекс наступного екземпляру HashItem у списку HashItem::colisions,
						 із яким відбулася колізія. */
};

/**
 * @class HashTable
 * Представляє функціонал і контейнер хеш таблиці, ключі й значення яких є строками.
 */

/**
 * @fn HashTable::HashTable(long long size);
 * Конструктор класу HashTable.
 */

 /**
  * @fn HashTable::HashTable(const HashTable& other);
  * Конструктор класу HashTable.
  */

/**
* @fn HashTable::HashTable& operator=(const HashTable& other);
* Конструктор класу HashTable.
*/

/**
 * @fn int HashTable::add(std::wstring key, std::vector<std::wstring> value)
 * Додає за обчисленим індексом нове значення: 
 * 1. якщо ключ не існує, присвоюється значення; 
 * 2. якщо ключ існує і він такий же, тоді нове додається до існуючого значення;
 * 3. якщо ключ існує і він не збігається, тоді обробляється колізія.
 */

/**
 * @fn std::vector<std::wstring> HashTable::get(std::wstring key)
 * Видає за можливості існуючі значення за ключем.
 */

/**
 * @fn int HashTable::increaseTable()
 * Збільшення розміру таблиці і повний перепис списків.
 */

/**
 * @fn void HashTable::saveTable(std::wstring path)
 * Збереження у накопичувачі даних списків у конкретному форматі.
 */

/**
 * @fn long long HashTable::hashFunction(std::wstring key, long long size)
 * Хеш функція, що перетворює ключ-строку у long long, що дозволяє
 * розподілити рівномірно ключі у списку із мінімумом колізій. 
 */

/**
 * @fn long long HashTable::getCapacity()
 * Повертає поточний об'єм записаних у пам'яті даних із змінної HashTable::currentCapacity.
 */

/**
 * @fn long long HashTable::getCount()
 * Повертає поточну кількість записаних ключів із змінної HashTable::count.
 */

/**
 * @fn long long HashTable::getSize()
 * Повертає поточний розмір основного списку HashTable::items, загальної кількості 
 * комірок із змінної HashTable::size.
 */

 /**
  * @fn double HashTable::getPercent()
  * Повертає пороговий % заповнення основного списку HashTable::items.
  */

/**
 * @fn std::vector<HashItem> HashTable::createNewVector(long long s)
 * Створює вектор із заданим об'ємом комірок.
 */

/**
 * @fn void HashTable::handleCollision(HashItem item, long long colisionIndex)
 * Обробляє колізії, додаючи їх у окремий список HashTable::colisions.
 */

/**
 * @fn long long HashTable::getItemCapacity(HashItem item)
 * Повертає розмір у байтах HashItem.
 */

/**
 * @fn std::array<long long, 4> HashTable::itemToArray(HashItem item, size_t cellSize, long long valueAddress)
 * Перетворює HashItem у масив long long, який містить посилання 
 * на адресу, де зберігається значення, для запису у файл.
 */

/**
 * @fn std::wstring HashTable::writeVectorToString(std::vector<std::wstring> vector)
 * Перетворення вектора у строку із розділювачем @var HashTable::delimiter.
 */

class HashTable {
public:
	const static std::wstring delimiter;  /**< Розділювач між окремими елементами списку HashItem::value.
										  Необхідний під час об'єднання списку HashItem::value у єдину 
										  строку для запису у файл усієї таблиці і можливості подальшого читання
										  записаних даних за визначеним форматом. */
	const static size_t cellSize;  /**< Розмір елементарної структури даних для розміщення кожного значення
								   HashItem у майбутніх файлах. */

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
	const double percent = 0.75; /**< Пороговий відсоток заповнення таблиці. */
	const long long maxCapacity = 2000000000; /**< Пороговий об'єм усієї таблиці, що записана у пам'яті. */
	const long long itemsSizeOnItemsMutexRatio = 32;
	std::atomic<int> busyThreadCount = 0;
	std::atomic<long long> size; /**< Поточний розмір, кількість комірок таблиці. */
	std::atomic<long long> count = 0; /**< Поточна кількість зайнятих комірок. */
	std::atomic<long long> currentCapacity = 0; /**< Поточний розмір у байтах даних таблиці. */
	std::atomic<long long> itemsMutexesSize = 0;
	std::atomic<bool> increaseInProcess = false;
	std::atomic<unsigned int> currentStopedThreadCount = 0;
	std::vector<HashItem> items; /**< Основний контейнер для HashItem. */
	std::vector<HashItem> colisions; /**< Контейнер для HashItem, у яких виникла колізія
									 із елементами основного контейнеру HashTable::items. */
	std::vector<std::mutex> itemsMutexes; /**< М'ютекс для основного контейнеру HashTable::items. */
	std::mutex colisionsMutex; /**< М'ютекс для контейнеру колізій HashTable::colisions. */
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