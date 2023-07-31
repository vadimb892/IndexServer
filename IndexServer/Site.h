#ifndef SITE_H
#define SITE_H

#include <string>
#include "StringFunctions.h"

/**
 * @class Site
 * Контейнер для інформації про окремий сайт, що отримана із функції Search::search.
 */

/**
 * @fn Site::Site()
 * Конструктор класу Site.
 */

/**
 * @fn Site::Site(std::wstring _title, std::wstring _link, std::wstring _snippet)
 * Конструктор класу Site.
 */

/**
 * @fn std::wstring Site::getTitle()
 * Видає заголовок title.
 */

/**
 * @fn std::wstring Site::getLink()
 * Видає посилання link.
 */

/**
 * @fn std::wstring Site::getSnippet()
 * Видає короткий опис snippet.
 */

/**
 * @fn std::wstring Site::toString()
 * Перетворює екземпляр класу Site у строку, об'єднуючи усі залежні змінні, 
 * Між змінними вставляється розділювач Site::delimiter.
 */

/**
 * @fn Site Site::toSite()
 * Перетворює строку у екземпляр класу Site.
 */

class Site {
public:
	const static std::wstring delimiter; /**< Розділювач між змінними у методі @fn Site::toString(). */

	Site();
	Site(std::wstring _title, std::wstring _link, std::wstring _snippet);
	std::wstring getTitle();
	std::wstring getLink();
	std::wstring getSnippet();
	std::wstring toString();
	static Site toSite(std::wstring string);

private:
	std::wstring title = L""; /**< Заголовок сайту. */
	std::wstring link = L""; /**< Посилання на сайт. */
	std::wstring snippet = L""; /**< Короткий опис сайту. */
};

#endif