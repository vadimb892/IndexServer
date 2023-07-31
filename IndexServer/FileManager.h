#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <condition_variable>
#include "io.h"

#include "StringFunctions.h"

#define STORING_PATH L"..\\cache"

/**
 * Носій даних про окремий файл для класу FileManager
 */
struct SectionInfo {
	std::wstring name; /**< Ім'я файлу*/
	std::wstring type; /**< Тип файлу (секція/словник)*/
	long long size; /**< Розмір файлу*/
	long long createTime; /**< Час створення файлу із стандартної початкової точки відліку*/
};

/**
 * @class FileManager 
 * Керує вмістом вказаної папки за шляхом FileManager::storingPath, для цього: 1, збирає 
 * інформацію про всі файли у папці за допомогою FileManager::readSectionsInfo(), 
 * FileManager::createSectionsInfo(), 2, підраховує за типом файли із допомогою методу
 * FileManager::countFilesOfType(). 3, зменшує об'єм, що займають дані шляхом пріоритетного
 * очищення у методі FileManager::decreaseStoredData(), FileManager::clearMemory().
 */

/**
 * @fn FileManager* FileManager::getInstance() 
 * повертає єдиний екземпляр класу FileManager, що записаний у 
 * FileManager::instance або, якщо його немає створює новий, є частиною реалізації патерну Singleton.  
 */

/**
 * @fn std::vector<SectionInfo> FileManager::getSections() 
 * повертає FileManager::sections.
 */

/**
 * @fn void FileManager::setSections(std::vector<SectionInfo> si) 
 * записує вже вирахувані дані файлів у FileManager::sections.
 */

/**
 * @fn std::wstring FileManager::getPath() 
 * повертає поточний шлях до підконтрольної папки, що записаний у 
 * FileManager::storingPath.
 */

/**
 * @fn long long FileManager::getMaxStoringCapacity() 
 * повертає порогове значення можливого об'єму
 * записаних даних FileManager::maxStoringCapacity.
 */

/**
 * @fn long long FileManager::getCurrentStoringCapacity() 
 * повертає порогове значення об'єму
 * записаних даних FileManager::currentStoringCapacity.
 */

/**
 * @fn void FileManager::decreaseStoredData(long long freeSpaceAmount) 
 * Зменшує об'єм записаних даних таким чином, щоб з'явився вільний об'єм 
 * записаним у вказаному параметрі freeSpaceAmount.  
 */

/**
 * @fn void FileManager::clearMemory() 
 * Очищує увесь об'єм записаних даних за списком FileManager::sections.
 */

/**
 * @fn int FileManager::countFilesOfType(std::wstring type) 
 * Підраховує файли вказаного типу в параметрі type.
 */

/**
 * @fn std::vector<SectionInfo> FileManager::readSectionsInfo() 
 * Читає інформацію про усі файли із вказаної папки.
 */

/**
 * @fn FileManager::FileManager()
 * Конструктор класу FileManager.
 */

/**
 * @fn SectionInfo FileManager::createSectionInfo(_finddata_t fileData) 
 * Створює екземпляр SectionInfo і читає в нього
 * інформацію про один конкретний файл.
 */

/**
 * @fn long long FileManager::deleteFileOfType(std::wstring type) 
 * Видаляє файли конкретного типу в параметрі type доти, поки не 
 * звільниться достатній об'єм пам'яті.
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
	static FileManager* instance; /**< Вказівник до єдиного екземпляру класу. */
	const std::wstring storingPath = STORING_PATH; /**< Шлях до підконтрольної папки. */
	const long long maxStoringCapacity = 5000000000; /**< Максимально можливий об'єм записаних даних. */
	const long long maxFileCount = 5; /**< Максимальна кількість файлів, що має зберігатися у папці. */
	long long currentStoringCapacity = 0; /**< Поточний об'єм записаних даних. */
	std::vector<SectionInfo> sectionsInfo = readSectionsInfo(); /**< Зберігає інформацію про файли у підконтрольній папці. */
	std::mutex mClearBool; /**< М'ютекс для змінної FileManager::mClearBool. */
	bool clearInProcess = false; /**< Вказує на активність процесу очистки. */

	FileManager(FileManager& other) = delete;
	void operator=(const FileManager& other) = delete;
	FileManager();
	SectionInfo createSectionInfo(_finddata_t fileData);
	long long deleteFileOfType(std::wstring type);
};
