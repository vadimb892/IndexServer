#pragma once
#include <queue>
#include <condition_variable>

/**
 * Містить значення для винятків, що виникають із HashTable, 
 * щоб окремий потік міг розпізнавати і вирішувати їх.
 * @see InvertedIndex::exceptionHandlers
 * @see InvertedIndex::exception
 * @see InvertedIndex::serviceLoop()
 * @see ProtectedServer::taskHandler()
 */
enum Exception {
	OK, /**< Код, що передає відсутність винятків.*/
	EXCEPTION_FULL_TABLE, /**< Виняток, що виникає, коли InvertedIndex::table заповнений 
						  більше за HashTable::percent від HashTable::size, але
						  за розміром не перевищив порогового розміру HashTable::maxCapacity.*/
	EXCEPTION_FULL_DICT, /**< Виняток, що виникає, коли InvertedIndex::dictionary заповнений 
						  більше за HashTable::percent від HashTable::size, але
						  за розміром не перевищив порогового розіру HashTable::maxCapacity.*/
	EXCEPTION_MAX_TABLE, /**< Виняток, що виникає, коли InvertedIndex::table заповнений 
						  настільки, що він перевищив пороговий розмір HashTable::maxCapacity 
						  і тепер таблицю слід записати на носії і очистити пам'ять.*/
	EXCEPTION_MAX_DICT, /**< Виняток, що виникає, коли InvertedIndex::dictionary заповнений 
						  настільки, що він перевищив пороговий розмір HashTable::maxCapacity 
						  і тепер таблицю слід записати на носії і очистити пам'ять.*/
	EXCEPTION_MAX_MEMORY /**< Виняток, що виникає, коли збережені дані перевищують об'єм 
						 FileManager::maxStoringCapacity байт.*/
};