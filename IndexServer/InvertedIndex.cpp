#include "InvertedIndex.h"

InvertedIndex::InvertedIndex() {
}

void InvertedIndex::setBusyThreadCount(int busyThreadCount) {
	table.setBusyThreadCount(busyThreadCount);
	dictionary.setBusyThreadCount(busyThreadCount);
}

std::wstring InvertedIndex::getUnicName(std::wstring type){
	int typeCount = fm->countFilesOfType(type);
	std::wstring name = type + L"-" + std::to_wstring(typeCount) + L".txt";
	std::ifstream fileExist = std::ifstream(name);
	int i = typeCount + 1;

	while (fileExist) {
		fileExist = std::ifstream(name);
		name = type + L"-" + std::to_wstring(i) + L".txt";
		i++;
	}
	return name;
}

std::vector<std::wstring> InvertedIndex::get(std::wstring key) {
	std::vector<std::wstring> wordIdVector = dictionary.get(key);
	if (wordIdVector.size() == 0) {
		return {};
	}
	std::vector<std::wstring> result;
	result = table.get(wordIdVector[0]);
	std::wstring storingPath = fm->getPath();
	std::vector<SectionInfo> sectionsInfo = fm->getSections();
	for (SectionInfo s : sectionsInfo) {
		if (s.name.find(L"Section") == std::wstring::npos) 
			continue;
		result = extendVector(result, loadItemValue(storingPath + s.name, wordIdVector[0]));
	}
	return result;
}

void InvertedIndex::add(std::wstring queue, std::vector<std::wstring> sources) {
	std::vector<std::wstring> wordId;
	std::vector<std::wstring> wordList = split(queue, L" ");
	int code = 0;
	for (std::wstring word : wordList) {
		if ((wordId = dictionary.get(word)).size() == 0) {
			std::wstring strLong = std::to_wstring(this->dictIdCounter);
			code = dictionary.add(word, { strLong });
			if (code == 1) {
				exceptionQueueMutex.lock();
				exceptionQueue.push(EXCEPTION_FULL_DICT);
				exceptionQueueMutex.unlock();
				exception.notify_one();
				return;
			}
			else if(code == 2){
				exceptionQueueMutex.lock();
				exceptionQueue.push(EXCEPTION_MAX_DICT);
				exceptionQueueMutex.unlock();
				exception.notify_one();
				return;
			}
			wordId = { strLong };
			this->dictIdCounter++;
		}
		code = table.add(wordId[0], sources);
		if (code == 1) {
			exceptionQueueMutex.lock();
			exceptionQueue.push(EXCEPTION_FULL_TABLE);
			exceptionQueueMutex.unlock();
			exception.notify_one();
			return;
		}
		else if (code == 2) {
			exceptionQueueMutex.lock();
			exceptionQueue.push(EXCEPTION_MAX_TABLE);
			exceptionQueueMutex.unlock();
			exception.notify_one();
			return;
		}
	}
}

std::vector<std::wstring> InvertedIndex::loadItemValue(std::wstring path, std::wstring wordId) {
	long long hash;
	long long address;
	long long sectionSize;
	long long savedId;
	long long valueAddress;
	long long valueSize;
	long long nextAddress;
	std::wstring buffer;
	size_t cellSize = HashTable::cellSize;
	std::wifstream in(path, std::ios::binary);

	in.seekg(std::ios::beg);
	in.read(&buffer[0], cellSize);
	sectionSize = std::stoll(ws2s(buffer));
	hash = HashTable::hashFunction(wordId, sectionSize);
	address = (hash + 1) * cellSize;
	in.seekg(address, std::ios::beg);
	in.read(&buffer[0], (long)(4 * cellSize));
	long long* item = (long long*)buffer.c_str();
	savedId = item[0];
	if (savedId == 0) {
		return {};
	}
	while (wordId != std::to_wstring(savedId)) {
		nextAddress = item[3];
		if (nextAddress == -1) {
			return {};
		}
		address = (nextAddress + 1) * cellSize;
		in.seekg(address, std::ios::beg);
		in.read(&buffer[0], (long)(4 * cellSize));
		long long* item = (long long*)buffer.c_str();
		savedId = item[0];
	}
	valueAddress = item[1];
	in.seekg(valueAddress, std::ios::beg);
	valueSize = item[2];
	in.read(&buffer[0], valueSize);
	return split(buffer,HashTable::delimiter);
}
 
void InvertedIndex::clear() {
	clear(&table);
	clear(&dictionary);
}

void InvertedIndex::shutdownService(bool evenNonEmptyQueue, long waitDuration) {
	if (!evenNonEmptyQueue) {
		while (exceptionQueue.size() > 0) {
			Sleep(waitDuration);
		}
	}
	mServiceFinish.lock();
	serviceFinish.store(FINISH);
	mServiceFinish.unlock();
	exception.notify_one();
}

bool InvertedIndex::isOverflowed() {
	bool isTableFlowed = table.getPercent() <= ((double)table.getCount() / (double)table.getSize());
	bool isDictionaryFlowed = dictionary.getPercent() <= ((double)dictionary.getCount() / (double)dictionary.getSize());
	return isTableFlowed || isDictionaryFlowed;
}

bool InvertedIndex::serviceLoop() {
	if (!serviceHandle.try_lock()) {
		return false;
	}
	std::function<void()> handler;
	Exception e;
	bool isFinished;
	int exceptionQueueSize = 0;

	mServiceFinish.lock();
	serviceFinish.store(WORK);
	mServiceFinish.unlock();

	while (true) {
		mServiceFinish.lock();
		isFinished = serviceFinish.load();
		mServiceFinish.unlock();
		if (isFinished == FINISH)
			break;
		std::unique_lock<std::mutex> lk(lkm);
		this->exception.wait(lk, [&]() {
			exceptionQueueMutex.lock();
			long long size = exceptionQueue.size();
			exceptionQueueMutex.unlock();
			mServiceFinish.lock();
			bool contin = this->serviceFinish.load();
			mServiceFinish.unlock();
			return size == 0 && !contin ? false : true;
			});
		exceptionQueueMutex.lock();
		exceptionQueueSize = exceptionQueue.size();
		exceptionQueueMutex.unlock();
		while (exceptionQueueSize > 0) {
			exceptionQueueMutex.lock();
			e = exceptionQueue.back();
			exceptionQueueMutex.unlock();
			handler = exceptionHandlers[e];
			handler();
			exceptionQueueMutex.lock();
			exceptionQueue.pop();
			exceptionQueueSize = exceptionQueue.size();
			exceptionQueueMutex.unlock();
		}
	}
	serviceHandle.unlock();
	return true;
}

void InvertedIndex::clear(HashTable* ht) {
	try {
		*ht = HashTable(startSize);
	}
	catch (...) {
		return;
	}
	return;
}