#ifndef STRINGFUNCTION_H
#define STRINGFUNCTION_H

#include <vector>
#include <string>
#include <codecvt>
#include <json/json.h>
#include <mutex>
#include <iostream>

/** @fn std::vector<std::wstring> split(std::wstring s, std::wstring delimiter)
 * Розділяє строку на частини за строкою у параметрі delimiter і повертає результат у векторі.
 */
std::vector<std::wstring> split(std::wstring s, std::wstring delimiter);

/** @fn std::string ws2s(const std::wstring& wstr)
 * Перетворює wstring у string
 */
std::string ws2s(const std::wstring& wstr);

/** @fn std::wstring s2ws(std::string value)
 * Перетворює string у wstring
 */
std::wstring s2ws(std::string value);

/** @fn Json::Value StringToJson(std::wstring string)
 * Перетворює строку із розміткою Json у повноцінний об'єкт Json::Value
 */
Json::Value StringToJson(std::wstring string);

#endif