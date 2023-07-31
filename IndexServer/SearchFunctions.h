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
 * ������� � �������� ������� � ���� SearchFunctions.h 
 */

/**
 * @fn Search::readFile(std::string path)
 * ���� ����� ���� ����� � ������ ������.
 */

/**
 * @fn Search::initCurl()
 * �������� �������� libcurl.
 */

/**
 * @fn Search::closeCurl()
 * ������� �������� libcurl.
 */

/**
 * @fn std::string Search::getSearchResultSite(std::wstring queue, int count)
 * ���� �� ��������� ������� libcurl �� ������� ����� �� Custom Google
 * Search � ������� ������ �� Json ��������.
 */

/**
 * @fn std::vector<Site> Search::scrapeSiteInfo(std::string markup)
 * ������ ������� ������ �� json ������ � ���� ������ �� ������������ Site.
 */

/**
 * @fn std::vector<Site> Search::search(std::wstring queue, int count)
 * ��'���� ��������� ������� Search::getSearchResultSite() � Search::scrapeSiteInfo().
 * ������ ����� � ������� ������ �� ������������ Site.
 */

namespace Search {
	std::string readFile(std::string path);

	const std::string key = readFile(KEY_F); /**< ID �볺��� �������� �������. */
	const std::string cx = readFile(CX_F); /**< ID custom �������� �������. */

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