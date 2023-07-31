#include "Server.h"

Server::Server() {
	index.setBusyThreadCount(pool.getBusyThreadCount());

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf("S) Failed to load Winsock library!\n");
		return;
	}

	listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	CHK_ERR(listenSocket, "socket");
	SET_NONBLOCK(&listenSocket);

	memset(&sa_serv, '\0', sizeof(sa_serv));
	sa_serv.sin_family = AF_INET;
	sa_serv.sin_addr.s_addr = INADDR_ANY;
	sa_serv.sin_port = htons(port);

	int err = ::bind(listenSocket, (struct sockaddr*)&sa_serv, sizeof(sa_serv));
	CHK_ERR(err, "S) Calling bind error!\n");
}

void Server::loop() {
	struct Task task;
	task.index = &index;
	task.resultCount = siteCount;
	task.printMutex = &printMutex;
	long waitDuration = 750;
	int threadsCount = 4;

	pool.setTaskHandler(clientTaskHandler);
	pool.mfinish.lock();
	pool.finish.store(WORK);
	pool.mfinish.unlock();
	pool.initThreads(threadsCount);
	serviceThread = std::async(std::launch::async, &InvertedIndex::serviceLoop, &index);
	asyncJoin = std::async(std::launch::async, &ThreadPool<Task>::joinAll, &pool);

	int err = listen(listenSocket, 5);
	CHK_ERR(err, "S) Calling listen error!\n");
	printMutex.lock();
	std::cout << "S) Server started." << std::endl;
	printMutex.unlock();
	while (task.client.recvBuffer != L"close") {
		task.client = handleConnection();
		if (task.client.getSocket() == INVALID_SOCKET)
			break;
		pool.addTask(task);
		index.setBusyThreadCount(pool.getBusyThreadCount());
	}

	pool.shutdown(false, waitDuration);
	asyncJoin.get();
	pool.mfinish.lock();
	pool.finish.store(FINISH);
	pool.mfinish.unlock();

	index.shutdownService(false, waitDuration);
	serviceThread.get();
	WSACleanup();
	printMutex.lock();
	std::cout << "S) Server finished." << std::endl;
	printMutex.unlock();
}

void Server::test(long threadsCount, long queriesCount) {
	struct Task task;
	task.index = &index;
	task.resultCount = siteCount;
	long waitDuration = 750;

	if (queriesCount <= waitDuration) {
		waitDuration = (float)queriesCount / 2;
	}

	pool.setTaskHandler(consoleTaskHandler);
	pool.mfinish.lock();
	pool.finish.store(WORK);
	pool.mfinish.unlock();
	pool.initThreads(threadsCount);
	serviceThread = std::async(std::launch::async, &InvertedIndex::serviceLoop, &index);
	asyncJoin = std::async(std::launch::async, &ThreadPool<Task>::joinAll, &pool);

	for (int i = 0; i < queriesCount; i++) {
		task.client.recvBuffer = L"{ \"query\" : \"" + randomString(rand() % 10) + L"\" }";
		pool.addTask(task);
		index.setBusyThreadCount(pool.getBusyThreadCount());
	}

	pool.shutdown(false, waitDuration);
	asyncJoin.get();
	pool.mfinish.lock();
	pool.finish.store(FINISH);
	pool.mfinish.unlock();

	index.shutdownService(true, waitDuration);
	serviceThread.get();
	index.clear();
}

void Server::interact() {
	std::wstring query;
	std::wstring result;
	struct Task task;
	task.result = &result;
	task.index = &index;
	task.resultCount = siteCount;
	long waitDuration = 750;
	int threadsCount = 4;

	pool.setTaskHandler(consoleTaskHandler);
	pool.mfinish.lock();
	pool.finish.store(WORK);
	pool.mfinish.unlock();
	pool.initThreads(threadsCount);
	serviceThread = std::async(std::launch::async, &InvertedIndex::serviceLoop, &index);
	asyncJoin = std::async(std::launch::async, &ThreadPool<Task>::joinAll, &pool);

	char* locale = setlocale(LC_ALL, "");
	while (true) {
		std::wcout << L"¬ведiть запит: ";
		getline(std::wcin, query);
		if (query == L"exit")
			break;

		task.client.recvBuffer = query;
		pool.addTask(task);
		while (result.size() == 0) {
			Sleep(20);
		}
		std::wcout << L"–езультати:\n" << result << "\n";
		result.clear();
	}

	pool.shutdown(false, waitDuration);
	asyncJoin.get();
	pool.mfinish.lock();
	pool.finish.store(FINISH);
	pool.mfinish.unlock();

	index.shutdownService(false, waitDuration);
	serviceThread.get();
}

Client Server::handleConnection() {
	struct sockaddr_in sa_cli;
	size_t client_len = sizeof(sa_cli);
	
	const SOCKET clientSocket = accept(listenSocket, (struct sockaddr*)&sa_cli, (int*)&client_len);
	if (clientSocket == INVALID_SOCKET) {
		printMutex.lock();
		printf("S) Calling accept error!\n");
		printMutex.unlock();
		return Client();
	}
	Client client = Client(clientSocket);
	client.client_addr = sa_cli;
	return client;
}

void SET_NONBLOCK(SOCKET* socket) {
	u_long iMode = 0;
	if (*socket != INVALID_SOCKET) {
		ioctlsocket(*socket, FIONBIO, &iMode);
	}
}

int clientTaskHandler(Task task) {
	std::wstring queue;
	std::vector<Site> result;
	std::vector<Site> sites;
	std::vector<std::wstring> value;
	Json::StreamWriterBuilder builder;
	Json::Value jsonArray;
	Json::Value site;
	Json::Value root;
	long long givenFromHeapCount = 0, searchedCount = 0;
	int dataAmount = 0;

	task.printMutex->lock();
	std::wcout << L"+) Client[" << task.client.getSocket() << L"] is connected." << std::endl;
	task.printMutex->unlock();

	//client : recv request
	check_net_error(dataAmount = task.client.receive(&task.client.recvBuffer), 
		"", task.client.getSocket());

	task.printMutex->lock();
	std::wcout << L"M) Client[" << task.client.getSocket() << L"][msg size: "
		<< task.client.recvBuffer.size() << L" B]: " << task.client.recvBuffer
		<< std::endl;
	task.printMutex->unlock();

	//client : read queue in request 
	root = StringToJson(task.client.recvBuffer);
	std::string pChar = root["query"].asString().c_str();
	queue = s2ws(pChar);

	//index : find in cache queue
	std::vector<std::wstring> keys = split(queue, L" ");
	for (std::wstring key : keys) {
		std::vector<std::wstring> keyValue = task.index->get(key);
		for (std::wstring site : keyValue) {
			result.push_back(Site::toSite(site));
		}
	}
	givenFromHeapCount = result.size();

	//scrap functions : if (smaller than n) search by queue
	//index : write result in cache
	if (result.size() < task.resultCount) {
		//sites = Search::search(queue, task.resultCount);
		sites = generateRandomSiteVector(4, task.resultCount);
		searchedCount = sites.size();
		result = extendVector(result, sites);
		for (Site site : sites) {
			value.push_back(site.toString());
		}
		task.index->add(queue, value);
	}

	task.printMutex->lock();
	std::wcout << L"S) Loaded " << givenFromHeapCount << ", searched " << searchedCount
		<< " for Client[" << task.client.getSocket() << L"]." << std::endl;
	task.printMutex->unlock();

	//client : form response
	root.clear();

	for (Site s : result) {
		site["title"] = ws2s(s.getTitle());
		site["link"] = ws2s(s.getLink());
		site["snippet"] = ws2s(s.getSnippet());
		jsonArray.append(site);
	}
	root["result"] = jsonArray;
	std::string resultString = Json::writeString(builder, root);

	//client : send responce
	task.printMutex->lock();
	std::wcout << L"S) Server send result to client[" << task.client.getSocket() << L"][msg size: "
		<< resultString.size() << L" B]." << std::endl;
	task.printMutex->unlock();
	task.client.send_(s2ws(resultString));

	//client : close connection
	task.client.close();
	return 0;
}

void check_net_error(int code, const char* message, SOCKET sock) {
	if (code == -1) {
		if (sock != INVALID_SOCKET)
			closesocket(sock);
		WSACleanup();
		throw std::runtime_error(message);
	}
}

int consoleTaskHandler(Task task) {
	std::wstring queue;
	std::vector<Site> result;
	std::vector<Site> sites;
	std::vector<std::wstring> value;
	Json::StreamWriterBuilder builder;
	Json::Value root;
	Json::Value jsonArray;
	Json::Value site;
	std::string resultString;

	queue = task.client.recvBuffer;

	std::vector<std::wstring> keys = split(queue, L" ");
	for (std::wstring key : keys) {
		std::vector<std::wstring> keyValue = task.index->get(key);
		if(keyValue.size() > task.resultCount)
			keyValue = std::vector<std::wstring>(keyValue.begin(), keyValue.begin() + task.resultCount);
		for (std::wstring site : keyValue) {
			result.push_back(Site::toSite(site));
		}
	}
#if CHOOSE == CONSOLE_SERVER
	if (result.size() < task.resultCount) {
		sites = Search::search(queue, task.resultCount - result.size());
		
		if (!task.index->isOverflowed()) {
			for (std::wstring key : keys) {
				for (Site s : sites) {
					value.push_back(s.toString());
				}
				task.index->add(key, value);
			}
		}

		result = extendVector(result, sites);
		for (Site site : sites) {
			value.push_back(site.toString());
		}
		task.index->add(queue, value);
	}
#elif CHOOSE == PERFOMANCE_TEST_SERVER
	if (!task.index->isOverflowed()) {
		for (std::wstring key : keys) {
			if(task.index->get(key).size() < 20)
				task.index->add(key, { L"a" + Site::delimiter + L"a" + Site::delimiter + L"a"});
		}
	}
#endif
	if (result.size() > 0) {
		for (Site s : result) {
			site["title"] = ws2s(s.getTitle());
			site["link"] = ws2s(s.getLink());
			site["snippet"] = ws2s(s.getSnippet());
			jsonArray.append(site);
		}
		root["result"] = jsonArray;
		resultString = Json::writeString(builder, root);
	}

	if (task.result != nullptr) {
		*task.result = s2ws(resultString);
	}
	return 0;
}

std::wstring randomString(std::size_t length){
	const std::wstring CHARACTERS = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

	std::wstring string;
	std::random_device randomDevice;
	std::mt19937 generator(randomDevice());
	std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

	for (std::size_t i = 0; i < length; ++i)
	{
		string += CHARACTERS[distribution(generator)];
	}
	return string;
}

std::vector<Site> generateRandomSiteVector(int strLength, int vectorLength) {
	std::wstring s = randomString(strLength);
	return std::vector<Site>(vectorLength, { s, s, s });
}
