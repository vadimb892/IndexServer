#pragma once
#include <queue>
#include <condition_variable>

/**
 * ̳����� �������� ��� �������, �� ��������� �� HashTable, 
 * ��� ������� ���� �� ����������� � ��������� ��.
 * @see InvertedIndex::exceptionHandlers
 * @see InvertedIndex::exception
 * @see InvertedIndex::serviceLoop()
 * @see ProtectedServer::taskHandler()
 */
enum Exception {
	OK, /**< ���, �� ������ ��������� �������.*/
	EXCEPTION_FULL_TABLE, /**< �������, �� ������, ���� InvertedIndex::table ���������� 
						  ����� �� HashTable::percent �� HashTable::size, ���
						  �� ������� �� ��������� ���������� ������ HashTable::maxCapacity.*/
	EXCEPTION_FULL_DICT, /**< �������, �� ������, ���� InvertedIndex::dictionary ���������� 
						  ����� �� HashTable::percent �� HashTable::size, ���
						  �� ������� �� ��������� ���������� ����� HashTable::maxCapacity.*/
	EXCEPTION_MAX_TABLE, /**< �������, �� ������, ���� InvertedIndex::table ���������� 
						  ��������, �� �� ��������� ��������� ����� HashTable::maxCapacity 
						  � ����� ������� ��� �������� �� ��� � �������� ���'���.*/
	EXCEPTION_MAX_DICT, /**< �������, �� ������, ���� InvertedIndex::dictionary ���������� 
						  ��������, �� �� ��������� ��������� ����� HashTable::maxCapacity 
						  � ����� ������� ��� �������� �� ��� � �������� ���'���.*/
	EXCEPTION_MAX_MEMORY /**< �������, �� ������, ���� �������� ��� ����������� ��'�� 
						 FileManager::maxStoringCapacity ����.*/
};