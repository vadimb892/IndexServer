#include "SearchFunctions.h"
#include "VectorFunctions.h"
#include <gtest/gtest.h>

TEST(readFile, readFile_Test1) {
	std::string fileName = "Test.txt";
	std::string path = HOME_S + fileName;
	std::ofstream newFile(path, std::ios::binary);
	EXPECT_FALSE(newFile.bad());
	std::string string = "test";
	newFile.write(string.c_str(), string.size());
	newFile.close();
	EXPECT_EQ(Search::readFile(path), string);
	remove(path.c_str());
}

TEST(wstrings, wstrings_Test1) {
	EXPECT_TRUE(Search::key.size() > 0);
	EXPECT_TRUE(Search::cx.size() > 0);
}

TEST(getSearchResultSite, getSearchResultSite_Test1) {
	int count = 10;
	std::string markup = Search::getSearchResultSite(L"test", count);
	std::string ch{ markup[0] };
	EXPECT_EQ(ch, "{");
}

TEST(scrapeSiteInfo, scrapeSiteInfo_Test1) {
	int count = 10;
	std::string markup = Search::getSearchResultSite(L"test", count);
	std::vector<Site> sites = Search::scrapeSiteInfo(markup);
	EXPECT_EQ(sites.size(), count);
	if (sites.size() > 0) {
		EXPECT_TRUE(sites[0].getLink().size() > 0);
	}
}

TEST(search, search_Test1) {
	int count = 10;
	std::vector<Site> sites = Search::search(L"test", count);
	EXPECT_EQ(sites.size(), count);
	if (sites.size() > 0) {
		EXPECT_TRUE(sites[0].getTitle().size() > 0);
	}
}