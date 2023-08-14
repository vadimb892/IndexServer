#include "Server.h"
#include <gtest/gtest.h>
#include <chrono>
#include <iostream>

#define CLIENT_SERVER 0
#define CONSOLE_SERVER 1
#define UNIT_TEST_SERVER 2
#define PERFOMANCE_TEST_SERVER 3

#define CHOOSE 2

int main(int argc, char* argv[]) {
	Server s;
#if CHOOSE == CLIENT_SERVER
	s.loop();
#elif CHOOSE == CONSOLE_SERVER
	s.interact();
#elif CHOOSE == UNIT_TEST_SERVER
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();

#elif CHOOSE == PERFOMANCE_TEST_SERVER
	long maxThreadCount = 6;
	long queriesCount = 100;
	std::function<void(long, long)> f = std::bind(&Server::test, &s, std::placeholders::_1, std::placeholders::_2);
	for(long i = 1; i <= maxThreadCount; i++)
		std::cout << "ThreadCount[" << i << ", " << queriesCount << "]> " 
		<< countDuration(f, i, queriesCount) << "ms\n";

#endif
}