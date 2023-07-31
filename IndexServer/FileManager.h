#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <condition_variable>
#include "io.h"

#include "StringFunctions.h"

#define STORING_PATH L"..\\cache"

/**
 * ���� ����� ��� ������� ���� ��� ����� FileManager
 */
struct SectionInfo {
	std::wstring name; /**< ��'� �����*/
	std::wstring type; /**< ��� ����� (������/�������)*/
	long long size; /**< ����� �����*/
	long long createTime; /**< ��� ��������� ����� �� ���������� ��������� ����� �����*/
};

/**
 * @class FileManager 
 * ���� ������ ������� ����� �� ������ FileManager::storingPath, ��� �����: 1, ����� 
 * ���������� ��� �� ����� � ����� �� ��������� FileManager::readSectionsInfo(), 
 * FileManager::createSectionsInfo(), 2, �������� �� ����� ����� �� ��������� ������
 * FileManager::countFilesOfType(). 3, ������ ��'��, �� �������� ��� ������ ������������
 * �������� � ����� FileManager::decreaseStoredData(), FileManager::clearMemory().
 */

/**
 * @fn FileManager* FileManager::getInstance() 
 * ������� ������ ��������� ����� FileManager, �� ��������� � 
 * FileManager::instance ���, ���� ���� ���� ������� �����, � �������� ��������� ������� Singleton.  
 */

/**
 * @fn std::vector<SectionInfo> FileManager::getSections() 
 * ������� FileManager::sections.
 */

/**
 * @fn void FileManager::setSections(std::vector<SectionInfo> si) 
 * ������ ��� ��������� ��� ����� � FileManager::sections.
 */

/**
 * @fn std::wstring FileManager::getPath() 
 * ������� �������� ���� �� ������������ �����, �� ��������� � 
 * FileManager::storingPath.
 */

/**
 * @fn long long FileManager::getMaxStoringCapacity() 
 * ������� �������� �������� ��������� ��'���
 * ��������� ����� FileManager::maxStoringCapacity.
 */

/**
 * @fn long long FileManager::getCurrentStoringCapacity() 
 * ������� �������� �������� ��'���
 * ��������� ����� FileManager::currentStoringCapacity.
 */

/**
 * @fn void FileManager::decreaseStoredData(long long freeSpaceAmount) 
 * ������ ��'�� ��������� ����� ����� �����, ��� �'������ ������ ��'�� 
 * ��������� � ��������� �������� freeSpaceAmount.  
 */

/**
 * @fn void FileManager::clearMemory() 
 * ����� ����� ��'�� ��������� ����� �� ������� FileManager::sections.
 */

/**
 * @fn int FileManager::countFilesOfType(std::wstring type) 
 * ϳ������� ����� ��������� ���� � �������� type.
 */

/**
 * @fn std::vector<SectionInfo> FileManager::readSectionsInfo() 
 * ���� ���������� ��� �� ����� �� ������� �����.
 */

/**
 * @fn FileManager::FileManager()
 * ����������� ����� FileManager.
 */

/**
 * @fn SectionInfo FileManager::createSectionInfo(_finddata_t fileData) 
 * ������� ��������� SectionInfo � ���� � �����
 * ���������� ��� ���� ���������� ����.
 */

/**
 * @fn long long FileManager::deleteFileOfType(std::wstring type) 
 * ������� ����� ����������� ���� � �������� type ����, ���� �� 
 * ���������� �������� ��'�� ���'��.
 */

class FileManager{
public:
	static FileManager* getInstance();
	std::vector<SectionInfo> getSections();
	void setSections(std::vector<SectionInfo> si);
	std::wstring getPath();
	long long getMaxStoringCapacity();
	long long getCurrentStoringCapacity();
	void decreaseStoredData(long long freeSpaceAmount);
	void clearMemory();
	int countFilesOfType(std::wstring type);
	std::vector<SectionInfo> readSectionsInfo();

private:
	static FileManager* instance; /**< �������� �� ������� ���������� �����. */
	const std::wstring storingPath = STORING_PATH; /**< ���� �� ������������ �����. */
	const long long maxStoringCapacity = 5000000000; /**< ����������� �������� ��'�� ��������� �����. */
	const long long maxFileCount = 5; /**< ����������� ������� �����, �� �� ���������� � �����. */
	long long currentStoringCapacity = 0; /**< �������� ��'�� ��������� �����. */
	std::vector<SectionInfo> sectionsInfo = readSectionsInfo(); /**< ������ ���������� ��� ����� � ������������ �����. */
	std::mutex mClearBool; /**< �'����� ��� ����� FileManager::mClearBool. */
	bool clearInProcess = false; /**< ����� �� ��������� ������� �������. */

	FileManager(FileManager& other) = delete;
	void operator=(const FileManager& other) = delete;
	FileManager();
	SectionInfo createSectionInfo(_finddata_t fileData);
	long long deleteFileOfType(std::wstring type);
};
