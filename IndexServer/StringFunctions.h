#ifndef STRINGFUNCTION_H
#define STRINGFUNCTION_H

#include <vector>
#include <string>
#include <codecvt>
#include <json/json.h>
#include <mutex>
#include <iostream>

/** @fn std::vector<std::wstring> split(std::wstring s, std::wstring delimiter)
 * ������� ������ �� ������� �� ������� � �������� delimiter � ������� ��������� � ������.
 */
std::vector<std::wstring> split(std::wstring s, std::wstring delimiter);

/** @fn std::string ws2s(const std::wstring& wstr)
 * ���������� wstring � string
 */
std::string ws2s(const std::wstring& wstr);

/** @fn std::wstring s2ws(std::string value)
 * ���������� string � wstring
 */
std::wstring s2ws(std::string value);

/** @fn Json::Value StringToJson(std::wstring string)
 * ���������� ������ �� �������� Json � ����������� ��'��� Json::Value
 */
Json::Value StringToJson(std::wstring string);

#endif