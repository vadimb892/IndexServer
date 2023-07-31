#include "FileManager.h"
#include <fstream>
#include "windows.h"
#include <gtest/gtest.h>

struct FileManagerTest : public testing::Test {
	FileManager* fm;

	void SetUp() {
		fm = FileManager::getInstance();
	}

	void TearDown(){}
};

TEST_F(FileManagerTest, getInstance_Test1) {
	EXPECT_EQ(FileManager::getInstance(), fm);
}

TEST_F(FileManagerTest, countFilesOfType_Test1) {
	EXPECT_EQ(fm->countFilesOfType(L"Section"), 0);
}

TEST_F(FileManagerTest, getPath_Test1) {
	EXPECT_TRUE(fm->getPath().size() > 0);
}

TEST_F(FileManagerTest, getMaxStoringCapacity_Test1) {
	EXPECT_TRUE(fm->getMaxStoringCapacity() > 0);
}

TEST_F(FileManagerTest, clearMemory_Test1) {
	std::vector<std::string> filesPath{
		"..\\cache\\test.txt",
		"..\\cache\\test1.txt",
		"..\\cache\\test2.txt"
	};
	for (std::string path : filesPath) {
		std::ofstream file(path);
		file << "abc";
		file.close();
	}
	fm->setSections(fm->readSectionsInfo());
	EXPECT_TRUE(fm->getSections().size() > 0);
	fm->clearMemory();
	fm->setSections(fm->readSectionsInfo());
	EXPECT_TRUE(fm->getSections().size() == 0);
}