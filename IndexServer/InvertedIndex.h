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
 * Представляє інвертований індекс і містить для цього усі необхідні компоненти. Отримує 
 * пари запит - список_ресурсів і записує їх у відповідних таблицях. Видає за окремими 
 * ключами список джерел із відповідних таблиць, виконує пошук також у записаних на носії секціях,
 * містить набір функцій для обробки виключень.
 */

/**
 * @fn InvertedIndex::InvertedIndex()
 * Конструктор класу InvertedIndex.
 */

/**
 * @fn std::vector<std::wstring> InvertedIndex::get(std::wstring key)
 * За отриманим ключем проводить пошук ресурсів, що містять його, у власних таблицях і
 * записаних файлах. Повертає результат у вигляді вектору.
 */

/**
 * @fn void InvertedIndex::add(std::wstring queue, std::vector<std::wstring> sources)
 * За отриманим запитом і списком джерел, шукає збіги і записує у таблиці значення за вказаним ключем,
 * у разі відсутності збігів записує у основну таблицю результат, інакше дозаписує нові значення при 
 * збігові ключів, при колізії переводить у значення у окрему таблицю колізій. 
 */

/**
* @fn bool InvertedIndex::serviceLoop()
* Містить цикл для слідкування і обробки виключень. Отримує виключення із черги 
* InvertedIndex::exceptionQueue і запускає функцію обробник із InvertedIndex::exceptionHandlers.
*/

/**
 * @fn std::vector<std::wstring> InvertedIndex::loadItemValue(std::wstring path, std::wstring wordId)
 * Завантажує значення окремого @struct HashItem із визначеного файлу за ключем.
 */

/**
 * @fn std::wstring InvertedIndex::getUnicName(std::wstring type)
 * Обчислює унікальне ім'я для нового файлу у підконтрольній папці.
 */

/**
 * @fn void InvertedIndex::clear(HashTable* ht)
 * Очищує одну із вказаних таблиць HashTable.
 */

 /**
  * @fn void InvertedIndex::clear()
  * Очищує обидві таблиці HashTable, основну і словник.
  */

/**
 * @fn void shutdownService(evenNonEmptyQueue evenNonEmptyQueue, long waitDuration)
 * Зупиняє потік сервісу, якщо evenNonEmptyQueue, тоді очікує поки сервіс завершить
 * свою роботу, виконуючи у циклі Sleep(waitDuration).
 */

 /**
  * @fn bool InvertedIndex::isOverflowed();
  * Вказує чи переповнений хоча б одна із таблиць. Сигналізує про роботу сервісу обробки винятків.
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
	const long long startSize = 20; /**< Початковий розмір будь-якого екземпляру HashTable. */
	std::atomic<long long> dictIdCounter = 1; /**< Лічильник унікальних ключів у словнику. */
	std::mutex mServiceFinish;
	std::atomic<bool> serviceFinish = false;
	
	std::function<void()> increaseTable = [&]() {
		if (table.increaseTable()) {
			exceptionQueueMutex.lock();
			exceptionQueue.push(EXCEPTION_MAX_TABLE);
			exceptionQueueMutex.unlock();
			exception.notify_one();
		}
	}; /**< Обробник виключення Exception::EXCEPTION_FULL_TABLE.
	   Збільшує таблицю InvertedIndex::table і у разі виникнення
	   Exception::EXCEPTION_MAX_TABLE
	   запускає це в чергу виключень InvertedIndex::exceptionQueue.  */

	std::function<void()> increaseDictionary = [&]() {
		if (dictionary.increaseTable()) {
			exceptionQueueMutex.lock();
			exceptionQueue.push(EXCEPTION_MAX_DICT);
			exceptionQueueMutex.unlock();
			exception.notify_one();
		}
	}; /**< Обробник виключення Exception::EXCEPTION_FULL_DICT.
	   Збільшує таблицю InvertedIndex::dictionary і у разі виникнення
	   Exception::EXCEPTION_MAX_DICT
	   запускає це в чергу виключень InvertedIndex::exceptionQueue.  */

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
	}; /**< Обробник виключення Exception::EXCEPTION_MAX_TABLE.
	   Збільшує таблицю і у разі виникнення Exception::EXCEPTION_MAX_DICT
	   запускає це в чергу виключень InvertedIndex::exceptionQueue.  */

	std::function<void()> fullMemoryClear = [&]() {
		fm->clearMemory();
		fm->setSections(fm->readSectionsInfo());
		clear(&table);
		clear(&dictionary);
	}; /**< Обробник виключення Exception::EXCEPTION_MAX_DICT.
	   Повністю очищає пам'ять і носії від підконтрольних файлів кешу(секції, таблиці).  */

	std::function<void()> decreaseData = [&]() {
		fm->decreaseStoredData(table.getCapacity() + dictionary.getCapacity());
	}; /**< Обробник виключення Exception::EXCEPTION_MAX_MEMORY. Зменшує обсяг записаних даних. */


	std::map<Exception, std::function<void()>> exceptionHandlers{ 
		{OK, []() {}},
		{EXCEPTION_FULL_TABLE, increaseTable},
		{EXCEPTION_FULL_DICT, increaseDictionary},
		{EXCEPTION_MAX_TABLE, saveTable},
		{EXCEPTION_MAX_DICT, fullMemoryClear},
		{EXCEPTION_MAX_MEMORY, decreaseData}
	}; /**< Словник кодів виключень Exception і їх обробників. */

	std::condition_variable exception; /**< Умовна змінна, що сповіщає сервісний потік про виникнення виключення. */
	std::queue<Exception> exceptionQueue; /**< Черга із ще необробленими виключеннями. */
	std::mutex exceptionQueueMutex; /**< М'ютекс черги виключень InvertedIndex::exceptionQueue. */
	std::mutex serviceHandle; /**< М'ютекс сервісної функції InvertedIndex::serviceLoop. */
	std::mutex lkm; /**< М'ютекс умовної змінної InvertedIndex::exception. */

	HashTable table = HashTable((startSize != NULL) ? startSize : 1000); /**< Таблиця InvertedIndex::table, 
																		 що містить списки ресурсів за ключами із
																		 словника InvertedIndex::dictionary. */
	HashTable dictionary = HashTable((startSize != NULL) ? startSize : 1000); /**< Словник унікальних ключів і їхніх id. */
	FileManager* fm = FileManager::getInstance(); /**< Менеджер збереженої на носіях пам'яті. */

	std::wstring getUnicName(std::wstring type);
	void clear(HashTable* ht);
};