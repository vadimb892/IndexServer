#include "HashTable.h"
#include "InvertedIndex.h"
#include "FileManager.h"
#include "windows.h"
#include "gtest/gtest.h"

struct HashTableTest : public testing::Test {
	const long long size = 5;
	HashTable* ht;

	void SetUp() {
		ht = new HashTable(size);
	}

	void TearDown() {
		delete ht;
	}
};

TEST_F(HashTableTest, add_get_Test1) {
	EXPECT_EQ(ht->add(L"a", {L"a"}), 0);
	EXPECT_STREQ(ht->get(L"a")[0].c_str(), L"a");
}

TEST_F(HashTableTest, increaseTable_Test1) {
	std::vector<std::wstring> list{L"a",L"b",L"c"};
	for (unsigned int i = 0; i < list.size(); i++) {
		ht->add(list[i], { list[i] });
	}
	EXPECT_EQ(ht->add(L"abcd", { L"abcd" }), 1);
	ht->increaseTable();
	ht->add(L"abcd", { L"abcd" });
	EXPECT_EQ(ht->getCount(), 4);
	EXPECT_EQ(ht->getSize(), 2*size);
}

TEST_F(HashTableTest, saveTable_Test1) {
	std::wstring fileName = L"Test-1";
	std::wstring type = L"Test";
	FileManager* fm = FileManager::getInstance();
	std::vector<std::wstring> list{ L"1",L"2",L"3",L"4",L"5"};
	for (unsigned int i = 0; i < list.size(); i++) {
		ht->add(list[i], { list[i] });
	}
	EXPECT_TRUE(ht->getCount() > 0);
	ht->saveTable(fm->getPath() + fileName);
	//std::vector<std::wstring> a = InvertedIndex::loadItemValue(fm->getPath() + fileName + L".txt", L"2");
	fm->setSections(fm->readSectionsInfo());
	if (fm->countFilesOfType(fileName) > 0) {
		std::string path = ws2s(fm->getPath() + fileName + L".txt");
		EXPECT_TRUE(remove(path.c_str()) == 0);
	}
}

TEST(HashTable, instanceAssigment_Test1) {
	HashTable table = HashTable(100);
	EXPECT_TRUE(table.getSize() == 100);
}

TEST(hashFunction, hashFunction_Test1) {
	EXPECT_EQ(HashTable::hashFunction(L"10", 20),
		HashTable::hashFunction(L"10", 20));
	EXPECT_EQ(HashTable::hashFunction(L"25", 154),
		HashTable::hashFunction(L"25", 154));
	EXPECT_EQ(HashTable::hashFunction(L"34", 758),
		HashTable::hashFunction(L"34", 758));
}