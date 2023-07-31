#include "InvertedIndex.h"
#include "gtest/gtest.h"

struct InvertedIndexTest : public testing::Test {
	const long long size = 5;
	InvertedIndex* ii;

	void SetUp() {
		ii = new InvertedIndex();
	}

	void TearDown() {
		delete ii;
	}
};

TEST_F(InvertedIndexTest, add_get_Test1) {
	ii->add(L"a", { L"a" });
	EXPECT_STREQ(ii->get(L"a")[0].c_str(), L"a");
}