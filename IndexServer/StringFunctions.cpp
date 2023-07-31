#include "StringFunctions.h"

std::vector<std::wstring> split(std::wstring s, std::wstring delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::wstring token;
    std::vector<std::wstring> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::wstring::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

std::string ws2s(const std::wstring& wstr){
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;
    return converterX.to_bytes(wstr);
}

std::wstring s2ws(std::string value) {
    const size_t cSize = value.size() + 1;
    std::wstring wc;
    wc.resize(cSize);
    size_t cSize1;
    mbstowcs_s(&cSize1, (wchar_t*)&wc[0], cSize, value.c_str(), cSize);
    wc.pop_back();
    return wc;
}

Json::Value StringToJson(std::wstring string){
    Json::CharReaderBuilder builder;
    Json::CharReader *reader = builder.newCharReader();

    Json::Value root;
    JSONCPP_STRING err;
    std::string buffer = ws2s(string);

    if (!reader->parse(buffer.c_str(), buffer.c_str() + buffer.size(), &root, &err)) {
        std::cout << "S) String to Json error: " << err << "." << std::endl;
        return root;
    }
    return root;
}