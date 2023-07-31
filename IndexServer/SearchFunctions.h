#pragma once
#include <curl/curl.h>
#include "gumbo.h"
#include "Site.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <json/json.h>
#include <fstream>

#ifndef HOME_S
#define HOME_S "..\\"
#endif

#define CX_F HOME_S "pseCx.txt"
#define KEY_F HOME_S "pseOwnApiKey.txt"

/**
 * @namespace Search
 * Локалізує і згруповує функції у файлі SearchFunctions.h 
 */

/**
 * @fn Search::readFile(std::string path)
 * Читає увесь вміст файлу у окрему строку.
 */

/**
 * @fn Search::initCurl()
 * Ініціалізує бібліотеку libcurl.
 */

/**
 * @fn Search::closeCurl()
 * Закриває бібліотеку libcurl.
 */

/**
 * @fn std::string Search::getSearchResultSite(std::wstring queue, int count)
 * Шукає за допомогою біліотеки libcurl за запитом сайти із Custom Google
 * Search і повертає строку із Json розміткою.
 */

/**
 * @fn std::vector<Site> Search::scrapeSiteInfo(std::string markup)
 * Виконує парсинг строки із json вмістом і видає вектор із екземплярами Site.
 */

/**
 * @fn std::vector<Site> Search::search(std::wstring queue, int count)
 * Об'єднує можливості функцій Search::getSearchResultSite() і Search::scrapeSiteInfo().
 * Отримує запит і повертає вектор із екземплярами Site.
 */

namespace Search {
	std::string readFile(std::string path);

	const std::string key = readFile(KEY_F); /**< ID клієнту пошукової системи. */
	const std::string cx = readFile(CX_F); /**< ID custom пошукової системи. */

	typedef size_t(*curl_write)(char*, size_t, size_t, std::string*);

	void initCurl();

	void closeCurl();

	std::string getSearchResultSite(std::wstring queue, int count);

	std::vector<Site> scrapeSiteInfo(std::string markup);

	std::vector<Site> search(std::wstring queue, int count);
}

/*

std::vector<GumboNode*> findByAttrValue(GumboNode* node, GumboTag tag,
	std::wstring attrName, std::wstring attrValue);

std::vector<std::wstring> findByAttrString(GumboNode* node, GumboTag tag, std::wstring attrName);

std::vector<GumboNode*> findByAttr(GumboNode* node, GumboTag tag, std::wstring attrName);
*/