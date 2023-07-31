#include "StringFunctions.h"
#include "gtest/gtest.h"

TEST(ws2s, ws2s_Test1) {
	EXPECT_EQ(ws2s(L"a"), "a");
	EXPECT_EQ(ws2s(L"abcdefg"), "abcdefg");
	EXPECT_EQ(ws2s(L"\x0017\x0035\x0047"), "\x0017\x0035\x0047");
}

TEST(s2ws, s2ws_Test1) {
	EXPECT_EQ(s2ws("a"), L"a");
	EXPECT_EQ(s2ws("abcdefg"), L"abcdefg");
	EXPECT_EQ(s2ws("\x0017\x0035\x0047"), L"\x0017\x0035\x0047");
}

TEST(StringToJson, StringToJson_Test1) {
	std::string jsonString = "{\"test\":[\"1\",\"2\",\"3\"]}";
	std::wstring jsonWstring = s2ws(jsonString);
	EXPECT_EQ(jsonWstring.size(), jsonString.size());
	Json::Value json = StringToJson(jsonWstring);
	int count = 0;
	for (Json::Value v : json["test"]) {
		++count;
	}
	EXPECT_EQ(count, 3);
}

