#include "HashTable.h"

const std::wstring HashTable::delimiter = L"$#@<!_site_>";
const size_t HashTable::cellSize = sizeof(long long) / sizeof(wchar_t);

HashTable::HashTable(long long size) {
	sizeMutex.lock();
	this->size.store(size);
	sizeMutex.unlock();
	items = createNewVector(size);
	if (size < itemsSizeOnItemsMutexRatio)
		itemsMutexesSize = 1;
	else
		itemsMutexesSize = floor((double)size
		/ (double)itemsSizeOnItemsMutexRatio);
	itemsMutexes = std::vector<std::mutex>(itemsMutexesSize);
}

HashTable::HashTable(const HashTable& other) {
	if (this == &other)
		return;

	this->size.store(other.size);
	this->count.store(other.count);
	this->currentCapacity.store(other.currentCapacity);
	this->items = other.items;
	this->colisions = other.colisions;
	this->itemsMutexesSize.store(other.itemsMutexesSize);
	this->itemsMutexes = std::vector<std::mutex>(other.itemsMutexesSize);
	this->busyThreadCount.store(other.busyThreadCount.load());
}

HashTable& HashTable::operator=(const HashTable& other) {
	if (this == &other)
		return *this;

	this->size.store(other.size);
	this->count.store(other.count);
	this->currentCapacity.store(other.currentCapacity);
	this->items = other.items;
	this->colisions = other.colisions;
	this->itemsMutexesSize.store(other.itemsMutexesSize);
	this->itemsMutexes = std::vector<std::mutex>(other.itemsMutexesSize);
	this->busyThreadCount.store(other.busyThreadCount.load());
	return *this;
}

void HashTable::setBusyThreadCount(int busyThreadCount) {
	this->busyThreadCount = busyThreadCount;
}

long long HashTable::getSize() {
	return this->size;
}

long long HashTable::getCount() {
	return this->count;
}

double HashTable::getPercent() {
	return this->percent;
}

long long HashTable::getCapacity() {
	return this->currentCapacity;
}

void HashTable::checkIncreaseProcess() {
	if (increaseInProcess.load()) {
		currentStopedThreadCount.store(currentStopedThreadCount.load() + 1);
		increaseInProcessMutex.lock();
		increaseInProcessMutex.unlock();
		currentStopedThreadCount.store(currentStopedThreadCount.load() - 1);
	}
}

int HashTable::add(std::wstring key, std::vector<std::wstring> value) {
	HashItem item{ key,value,getVectorByteSize(value),-1 };
	sizeMutex.lock();
	long long s = size.load();
	sizeMutex.unlock();
	long long index = hashFunction(key, s);
	long long mutexIndex;
	HashItem currentItem;

	checkIncreaseProcess();
	mutexIndex = getMutexIndex(index);
	itemsMutexes[mutexIndex].lock();
	currentItem = this->items[index];
	itemsMutexes[mutexIndex].unlock();

	long long itemCapacity = getItemCapacity(item);
	if (this->currentCapacity.load() + itemCapacity >= this->maxCapacity) {
		return 2;
	}
	if (currentItem.value.size() == 0) {
		if (this->count.load() >= (long long)((double)this->size.load() * this->percent)) {
			return 1;
		}
		itemsMutexes[mutexIndex].lock();
		this->items[index] = item;
		itemsMutexes[mutexIndex].unlock();

		this->count.store(this->count.load() + 1);
		this->currentCapacity.store(this->currentCapacity.load() + itemCapacity);
		return 0;
	}else {
		if (item.key == currentItem.key) {
			itemsMutexes[mutexIndex].lock();
			this->items[index].value = extendVector(currentItem.value, item.value);
			itemsMutexes[mutexIndex].unlock();

			this->currentCapacity.store(this->currentCapacity.load() + itemCapacity);
			currentItem.byteSize += itemCapacity;
			return 0;
		}
		else {
			handleCollision(item, index);
			return 0;
		}
	}
	return 0;
}

std::vector<std::wstring> HashTable::get(std::wstring key) {
	sizeMutex.lock();
	long long s = size.load();
	sizeMutex.unlock();
	long long index = hashFunction(key, s);
	long long mutexIndex;
	HashItem item;

	checkIncreaseProcess();
	mutexIndex = getMutexIndex(index);
	itemsMutexes[mutexIndex].lock();
	item = this->items[index];
	itemsMutexes[mutexIndex].unlock();

	if (item.value.size() > 0) {
		while (true) {
			if (key == item.key) 
				return item.value;
			else if(item.next == -1) 
				break;
			item = colisions[item.next];
		}
	}
	return {};
}

std::vector<HashItem> HashTable::createNewVector(long long s) {
	std::vector<HashItem> vector;
	HashItem hi{ L"", {}, -1 };
	vector.resize(s, hi);
	return vector;
}

long long HashTable::hashFunction(std::wstring key, long long tableSize) {
	long long hash = 5381;
	long long M = 33;
	for (long long i = 0; i < key.size(); i++) {
		hash = M * hash + key[i];
	}
	return abs(hash) % tableSize;
}

void HashTable::handleCollision(HashItem item, long long colisionIndex) {
	long long index = colisionIndex;
	long long valueSize;
	long long colisionSize;
	long long mutexIndex;
	HashItem currentItem;

	checkIncreaseProcess();
	mutexIndex = getMutexIndex(index);
	itemsMutexes[mutexIndex].lock();
	currentItem = this->items[index];
	itemsMutexes[mutexIndex].unlock();
	while (true) {
		if (currentItem.key == item.key) {
			checkIncreaseProcess();
			mutexIndex = getMutexIndex(index);
			itemsMutexes[mutexIndex].lock();
			this->items[index].value = extendVector(currentItem.value, item.value);
			itemsMutexes[mutexIndex].unlock();

			valueSize = getVectorByteSize(item.value);
			this->currentCapacity += valueSize;
			currentItem.byteSize += valueSize;
			return;
		}
		else if (currentItem.next == -1) {
			colisionsMutex.lock();
			colisions.push_back(item);
			colisionSize = colisions.size();
			colisionsMutex.unlock();
			checkIncreaseProcess();
			mutexIndex = getMutexIndex(index);
			itemsMutexes[mutexIndex].lock();
			this->items[index].next = colisionSize - 1;
			itemsMutexes[mutexIndex].unlock();
			return;
		}
		colisionsMutex.lock();
		currentItem = colisions[currentItem.next];
		colisionsMutex.unlock();
		this->count++;
		this->currentCapacity += getItemCapacity(item);
	}
}

long long HashTable::getItemCapacity(HashItem item) {
	long long itemCapacity = sizeof(long long) * 2;
	itemCapacity += item.key.size();
	itemCapacity += getVectorByteSize(item.value);
	return itemCapacity;
}

long long HashTable::getMutexIndex(long long itemIndex) {
	long long mutexIndex = floor((double)itemIndex / (double)itemsSizeOnItemsMutexRatio);
	if (mutexIndex >= itemsMutexesSize)
		mutexIndex = itemsMutexesSize - 1;
	return mutexIndex;
}

int HashTable::increaseTable() {
	long long oldSize = this->size.load();
	long long newSize = 2 * oldSize;
	long long index = 0;
	long long mutexIndex = 0;
	long long mutexCount = 1;
	HashItem item, currentItem;
	if (newSize >= maxCapacity) 
		return 1;

	long long oldMutexSize = itemsMutexes.size();
	if (size > itemsSizeOnItemsMutexRatio)
		mutexCount = floor((double)newSize 
		/ (double)itemsSizeOnItemsMutexRatio);

	std::vector<HashItem> newItemsVector = createNewVector(newSize);
	std::vector<HashItem> newColisionsVector;
	std::vector<HashItem> tmpColisions;
	for (long long i = 0; i < oldSize; i++) {
		checkIncreaseProcess();
		mutexIndex = getMutexIndex(i);
		itemsMutexes[mutexIndex].lock();
		item = items[i];
		itemsMutexes[mutexIndex].unlock();
		if (item.value.size() > 0)
			continue;
		index = hashFunction(item.key, newSize);
		currentItem = newItemsVector[index];
		if (currentItem.value.size() > 0) {
			if (currentItem.key != item.key) {
				tmpColisions.push_back(item);
			}
			else {
				newItemsVector[index].value = extendVector<std::wstring>(
					newItemsVector[index].value, item.value);
				newItemsVector[index].byteSize += item.byteSize;
			}
		}
		else
			newItemsVector[index] = item;
	}
	colisionsMutex.lock();
	long long colisionsSize = colisions.size();
	colisionsMutex.unlock();
	for (long long i = 0; i < colisionsSize; i++) {
		colisionsMutex.lock();
		item = colisions[i];
		colisionsMutex.unlock();
		index = hashFunction(item.key, newSize);
		currentItem = newItemsVector[index];
		item.next = -1;
		if (currentItem.value.size() > 0) {
			if (currentItem.key != item.key) {
				tmpColisions.push_back(item);
			}else {
				newItemsVector[index].value = extendVector<std::wstring>(
					newItemsVector[index].value,  item.value);
				newItemsVector[index].byteSize += item.byteSize;
			}
		}else
			newItemsVector[index] = item;
	}

	increaseInProcessMutex.lock();
	increaseInProcess.store(true);
	while (currentStopedThreadCount < busyThreadCount) {
		Sleep(200);
	}
	/*for (int i = 0; i < oldMutexSize; i++)
		itemsMutexes[i].unlock();*/

	itemsMutexes = std::vector<std::mutex>(mutexCount);
	itemsMutexesSize.store(itemsMutexes.size());
	items = newItemsVector;
	sizeMutex.lock();
	this->size.store(newSize);
	sizeMutex.unlock();
	increaseInProcess.store(false);
	increaseInProcessMutex.unlock();

	colisionsMutex.lock();
	colisions = newColisionsVector;
	colisionsMutex.unlock();
	for (HashItem hi : tmpColisions) {
		add(hi.key, hi.value);
	}
	return 0;
}

void HashTable::saveTable(std::wstring path) {
	path += L".txt";
	colisionsMutex.lock();
	long long colisionSize = colisions.size();
	colisionsMutex.unlock();
	long long currentKeyAddress = 0;
	sizeMutex.lock();
	long long _size = size.load();
	sizeMutex.unlock();
	long long currentValueAddress = ((_size + colisionSize) * cellSize * 4) + 1;
	long long mutexIndex = 0;

	std::array<long long, 4> dlongArray;
	std::vector<std::wstring> value;
	std::wstring buffer;
	HashItem item;

	std::wofstream newFile(path, std::ios::binary);
	if (newFile.bad())
		return;
	newFile.write(reinterpret_cast<wchar_t*>(&_size), cellSize);
	currentKeyAddress += cellSize;
	for (long long i = 0; i < _size; i++) {
		checkIncreaseProcess();
		mutexIndex = getMutexIndex(i);
		itemsMutexes[mutexIndex].lock();
		item = items[i];
		itemsMutexes[mutexIndex].unlock();
		dlongArray = itemToArray(item, cellSize, currentValueAddress);
		for(int i = 0; i < 4; i++)
			newFile.write(reinterpret_cast<wchar_t*>(&dlongArray[0]), 1);
		currentValueAddress += item.byteSize;
	}
	for (long long i = 0; i < colisionSize; i++) {
		colisionsMutex.lock();
		item = colisions[i];
		colisionsMutex.unlock();
		dlongArray = itemToArray(item, cellSize, currentValueAddress);
		for (int i = 0; i < 4; i++)
			newFile.write(reinterpret_cast<wchar_t*>(&dlongArray[0]), 1);
		currentValueAddress += item.byteSize;
	}
	currentValueAddress = ((_size + colisionSize) * cellSize * 4) + 1;
	for (long long i = 0; i < _size; i++) {
		checkIncreaseProcess();
		mutexIndex = getMutexIndex(i);
		itemsMutexes[mutexIndex].lock();
		value = items[i].value;
		itemsMutexes[mutexIndex].unlock();
		if (value.size() == 0)
			continue;
		/*itemsMutexes[mutexIndex].lock();
		value = items[i].value;
		itemsMutexes[mutexIndex].unlock();*/
		buffer = writeVectorToString(value);
		newFile << buffer;
	}

	for (long long i = 0; i < colisionSize; i++) {
		colisionsMutex.lock();
		value = colisions[i].value;
		colisionsMutex.unlock();
		buffer = writeVectorToString(value);
		newFile << buffer;
	}
	newFile.close();
}

std::array<long long, 4> HashTable::itemToArray(HashItem item,
	size_t cellSize, long long valueAddress) {
	long long key; 
	long long nextAddress;
	std::array<long long, 4> dlongArray;
	cellSize /= 2;

	if (item.key == L"") 
		key = ZERO;
	else 
		key = stoll(item.key);
	nextAddress = item.next != ZERO ? ((size + item.next) * cellSize * 4) + cellSize : ZERO;
	if (item.value.size() == 0) 
		dlongArray = { ZERO,ZERO,ZERO,ZERO };
	else 
		dlongArray = { key,valueAddress,item.byteSize,nextAddress };

	return dlongArray;
}

std::wstring HashTable::writeVectorToString(std::vector<std::wstring> vector) {
	std::wstring result;
	for (long long i = 0; i < vector.size(); i++) {
		result += vector[i] + delimiter;
	}
	return result;
}