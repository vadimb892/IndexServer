#include "Site.h" 

const std::wstring Site::delimiter = L"$#@<!_space_>";

Site::Site() {}

Site::Site(std::wstring _title, std::wstring _link, std::wstring _snippet) {
	title = _title;
	link = _link;
	snippet = _snippet;
}

std::wstring Site::getTitle() {
	return title;
}

std::wstring Site::getLink() {
	return link;
}

std::wstring Site::getSnippet() {
	return snippet;
}

std::wstring Site::toString() {
	return title + delimiter + link + delimiter + snippet;
}

Site Site::toSite(std::wstring string) {
	std::vector<std::wstring> s = split(string, Site::delimiter);
	return Site(s[0],s[1],s[2]);
}