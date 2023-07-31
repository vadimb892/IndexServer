#ifndef SITE_H
#define SITE_H

#include <string>
#include "StringFunctions.h"

/**
 * @class Site
 * ��������� ��� ���������� ��� ������� ����, �� �������� �� ������� Search::search.
 */

/**
 * @fn Site::Site()
 * ����������� ����� Site.
 */

/**
 * @fn Site::Site(std::wstring _title, std::wstring _link, std::wstring _snippet)
 * ����������� ����� Site.
 */

/**
 * @fn std::wstring Site::getTitle()
 * ���� ��������� title.
 */

/**
 * @fn std::wstring Site::getLink()
 * ���� ��������� link.
 */

/**
 * @fn std::wstring Site::getSnippet()
 * ���� �������� ���� snippet.
 */

/**
 * @fn std::wstring Site::toString()
 * ���������� ��������� ����� Site � ������, ��'������� �� ������ ����, 
 * ̳� ������� ������������ ��������� Site::delimiter.
 */

/**
 * @fn Site Site::toSite()
 * ���������� ������ � ��������� ����� Site.
 */

class Site {
public:
	const static std::wstring delimiter; /**< ��������� �� ������� � ����� @fn Site::toString(). */

	Site();
	Site(std::wstring _title, std::wstring _link, std::wstring _snippet);
	std::wstring getTitle();
	std::wstring getLink();
	std::wstring getSnippet();
	std::wstring toString();
	static Site toSite(std::wstring string);

private:
	std::wstring title = L""; /**< ��������� �����. */
	std::wstring link = L""; /**< ��������� �� ����. */
	std::wstring snippet = L""; /**< �������� ���� �����. */
};

#endif