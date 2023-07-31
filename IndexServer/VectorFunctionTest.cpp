#include "VectorFunctions.h"
#include "gtest/gtest.h"

TEST(extendVector, extendVector_Test1) {
	std::vector<int> v = extendVector<int>({1, 2}, {3, 4});
	EXPECT_EQ(v.size(), 4);
	std::vector<int> result = { 1, 2, 3, 4 };
	for (unsigned int i = 0; i < result.size(); i++) {
		EXPECT_EQ(v[i], result[i]);
	}
}

TEST(getVectorByteSize, getVectorByteSize_Test1) {
	long long size = getVectorByteSize({L"12", L"A", L"\x0012"});
	EXPECT_EQ(size, 4);
}