#ifndef VECTORFUNCTION_H
#define VECTORFUNCTION_H

#include <vector>
#include <string>

/**
 * @fn std::vector<T> extendVector(std::vector<T> extended, std::vector<T> appended)
 * �������� ������ ������� ��������� ���������� �� 
 * ������� ������� ��������� � ������� ���������.
 */
template <typename T>
std::vector<T> extendVector(std::vector<T> extended,
	std::vector<T> appended) {
	extended.reserve(extended.size() + appended.size());
	extended.insert(extended.end(), appended.begin(), appended.end());
	return extended;
}

/**
 * @fn long long getVectorByteSize(std::vector<std::wstring> vector)
 * ������� ����� ������� � ������.
 */
long long getVectorByteSize(std::vector<std::wstring> vector);
#endif