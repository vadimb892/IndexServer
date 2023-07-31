#include "FileManager.h"

FileManager* FileManager::instance = nullptr;

FileManager* FileManager::getInstance(){
	if (instance == nullptr) {
		instance = new FileManager();
	}
	return instance;
}

FileManager::FileManager() {}

std::wstring FileManager::getPath() {
	return storingPath;
}

std::vector<SectionInfo> FileManager::getSections() {
	mClearBool.lock();
	if (clearInProcess) {
		mClearBool.unlock();
		return {};
	}
	mClearBool.unlock();
	return sectionsInfo;
}

void FileManager::setSections(std::vector<SectionInfo> si) {
	sectionsInfo = si;
}

long long FileManager::getMaxStoringCapacity() {
	return maxStoringCapacity;
}
long long FileManager::getCurrentStoringCapacity() {
	return currentStoringCapacity;
}

void FileManager::decreaseStoredData(long long freeSpaceAmount) {
	mClearBool.lock();
	clearInProcess = true;
	mClearBool.unlock();

	long long storedSize = 0;
	long long deletedSize = 0;
	for (SectionInfo si : sectionsInfo) {
		storedSize += si.size;
	}
	storedSize += freeSpaceAmount;
	sort(sectionsInfo.begin(), sectionsInfo.end(), [](SectionInfo f, SectionInfo s) {
		return f.size >= s.size;
	});
	long long sectionFilesCount = countFilesOfType(L"Section");
	while (storedSize >= this->maxStoringCapacity && sectionFilesCount > 0) {
		deletedSize = deleteFileOfType(L"Section");
		if (deletedSize != 0) {
			sectionFilesCount--;
			storedSize -= deletedSize;
		}
		else {
			mClearBool.lock();
			clearInProcess = false;
			mClearBool.unlock();
			return;
		}
	}
	if (storedSize >= this->maxStoringCapacity) {
		long long sectionFilesCount = countFilesOfType(L"Dictionary");
		while (storedSize >= this->maxStoringCapacity && sectionFilesCount > 0) {
			deletedSize = deleteFileOfType(L"Dictionary");
			if (deletedSize != 0) {
				sectionFilesCount--;
				storedSize -= deletedSize;
			}
			else
				break;
		}
	}
	mClearBool.lock();
	clearInProcess = false;
	mClearBool.unlock();
	return;
}

long long FileManager::deleteFileOfType(std::wstring type) {
	long long index = 0;
	std::string fullFilePath;
	long long fileSize;
	while (index < sectionsInfo.size()) {
		if (sectionsInfo[index].type == L"Section") {
			fullFilePath = ws2s(storingPath + sectionsInfo[index].name);
			if (remove(fullFilePath.c_str()) == 0) {
				index++;
				continue;
			}
			fileSize = sectionsInfo[index].size;
			sectionsInfo.erase(sectionsInfo.begin() + index);
			return fileSize;
		}
		index++;
		if (index == sectionsInfo.size()) {
			return 0;
		}
	}
	return 0;
}

std::vector<SectionInfo> FileManager::readSectionsInfo() {
	std::vector<SectionInfo> sectionsInfo;
	int count = 0;
	long long storedSize = 0;
	std::string dirPath = ws2s(this->storingPath + L"*.*");

	_finddata_t fileData;
	int ff = _findfirst(dirPath.c_str(), &fileData);
	if (ff != -1) {
		int res = 0;
		while (res != -1){
			sectionsInfo.push_back(createSectionInfo(fileData));
			res = _findnext(ff, &fileData);
			count++;
		}
	}else return {};
	_findclose(ff);
	if (count <= 2) 
		sectionsInfo.clear();
	else {
		sectionsInfo.erase(sectionsInfo.begin(), 
			sectionsInfo.begin()+1);
	}
	for (SectionInfo si : sectionsInfo) {
		storedSize += si.size;
	}
	currentStoringCapacity = storedSize;
	return sectionsInfo;
}

SectionInfo FileManager::createSectionInfo(_finddata_t fileData) {
	std::wstring type = split(s2ws(fileData.name), L"-")[0];
	return SectionInfo{ 
		s2ws(fileData.name), 
		type, 
		fileData.size,  
		(intmax_t)fileData.time_create 
	};
}

int FileManager::countFilesOfType(std::wstring type) {
	int count = 0;
	for (SectionInfo si : sectionsInfo) {
		if (si.name.find(type) != std::wstring::npos) count++;
	}
	return count;
}

void FileManager::clearMemory() {
	mClearBool.lock();
	clearInProcess = true;
	mClearBool.unlock();
	std::string fullFilePath;
	for (SectionInfo si : sectionsInfo) {
		fullFilePath = ws2s(storingPath + si.name);
		remove(fullFilePath.c_str());
	}
	mClearBool.lock();
	clearInProcess = false;
	mClearBool.unlock();
}