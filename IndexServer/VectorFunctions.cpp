#include "VectorFunctions.h"

long long getVectorByteSize(std::vector<std::wstring> vector) {
	long long vectorSize = 0;
	for (std::wstring s : vector) {
		vectorSize += s.size();
	}
	return vectorSize;
}