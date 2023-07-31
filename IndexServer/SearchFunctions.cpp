#include "SearchFunctions.h"

void Search::initCurl() {
	curl_global_init(CURL_GLOBAL_ALL);
}

void Search::closeCurl() {
	curl_global_cleanup();
}

std::string Search::readFile(std::string path) {
	std::ifstream file(path);
	std::stringstream ss;
	std::string result;
	ss << file.rdbuf();
	result = ss.str();
	ss.clear();
	file.close();
	return result;
}

std::string Search::getSearchResultSite(std::wstring queue, int count) {
	if (count > 10) {
		count = 10;
	}
	std::replace(queue.begin(), queue.end(), ' ', '+');
	std::string result;
	std::string url = "https://customsearch.googleapis.com/customsearch/v1?";
	url += "q=" + ws2s(queue);
	url += "&cx=" + Search::cx;
	url += "&key=" + Search::key;
	url += "&num=" + std::to_string(count);

	CURLcode res_code = CURLE_FAILED_INIT;
	CURL* curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl,
			CURLOPT_WRITEFUNCTION,
			static_cast <curl_write> ([](char* contents, size_t size,
				size_t nmemb, std::string* data) -> size_t {
					size_t new_size = size * nmemb;
					if (data == NULL) {
						return 0;
					}
					data->append(contents, new_size);
					return new_size;
				}));
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "link scraper");

		res_code = curl_easy_perform(curl);

		if (res_code != CURLE_OK) {
			fprintf(stderr,curl_easy_strerror(res_code));
			return "";
		}
	}

	curl_easy_cleanup(curl);
	return result;
}

std::vector<Site> Search::scrapeSiteInfo(std::string markup) {
	std::vector<Site> sites;
	if (markup.size() == 0)
		return sites;
	Json::Value root = StringToJson(s2ws(markup));
	Json::Value items = root["items"];
	for (Json::Value item : items) {
		sites.push_back(
			Site(s2ws(item["title"].asString().c_str()),
				s2ws(item["link"].asString().c_str()),
				s2ws(item["snippet"].asString().c_str()))
		);
	}
	return sites;
}

std::vector<Site> Search::search(std::wstring queue, int count) {
	std::string markup = Search::getSearchResultSite(queue, count);
	return scrapeSiteInfo(markup);
}

/*std::vector<Site> scrapeSiteInfo(std::string markup) {
	std::vector<Site> siteObjList;
	std::vector<GumboNode*> snippetContainer;
	GumboOutput* output = gumbo_parse(markup.c_str());
	std::vector<GumboNode*> siteNodeList = findByAttrValue(output->root, GUMBO_TAG_DIV, "class",
		"Gx5Zad fP1Qef xpd EtOod pkphOe");
	for (unsigned int i = 0; i < siteNodeList.size(); i++) {
		snippetContainer = findByAttrValue(siteNodeList[i], GUMBO_TAG_DIV,
			"class", "BNeawe s3v9rd AP7Wnd");
		siteObjList.push_back(Site(
			findByAttrValue(siteNodeList[i], GUMBO_TAG_DIV, "class", "BNeawe vvjwJb AP7Wnd")[0]->v.text.original_text.data,//v.element.original_tag.data,
		findByAttrString(siteNodeList[i], GUMBO_TAG_A, "href")[0],
			findByAttrValue(snippetContainer[0], GUMBO_TAG_DIV, "class", "BNeawe s3v9rd AP7Wnd")[0]->v.text.original_text.data
			));
	}

	gumbo_destroy_output(&kGumboDefaultOptions, output);

	return siteObjList;
}

std::vector<GumboNode*> findByAttrValue(GumboNode* node, GumboTag tag, std::string attrName, std::string attrValue) {
	std::vector<GumboNode*> nodes;
	std::vector<GumboNode*> childResult;
	GumboAttribute* attr;
	if (node->type != GUMBO_NODE_ELEMENT) {
		return nodes;
	}
	if ((attr = gumbo_get_attribute(&node->v.element.attributes, "class")) != NULL) {
		std::cout << attr->value << "\n";
	}

	if (node->v.element.tag == tag && (attr = gumbo_get_attribute(&node->v.element.attributes, attrName.c_str())) &&
		strstr(attr->value, attrValue.c_str()) != NULL) {
		nodes.push_back(node);
	}

	GumboVector* children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; i++) {
		childResult = findByAttrValue(static_cast<GumboNode*>(children->data[i]), tag, attrName, attrValue);
		nodes.reserve(nodes.size() + childResult.size());
		nodes.insert(nodes.end(), childResult.begin(), childResult.end());
	}
	return nodes;
}

std::vector<std::string> findByAttrString(GumboNode* node, GumboTag tag, std::string attrName) {
	std::vector<std::string> dataList;
	std::vector<std::string> childResult;
	GumboAttribute* attr;
	std::string text;
	if (node->type != GUMBO_NODE_ELEMENT) {
		return dataList;
	}
	if ((attr = gumbo_get_attribute(&node->v.element.attributes, "class")) != NULL) {
		std::cout << attr->value << "\n";
	}
	if (attrName != "text") {
		if (node->v.element.tag == tag &&
			(attr = gumbo_get_attribute(&node->v.element.attributes, attrName.c_str())) != NULL) {
			dataList.push_back(attr->original_value.data);
		}
	}
	else {
		if (node->v.element.tag == tag &&
			&node->v.text.original_text.length > 0) {
			text = *(&node->v.text.original_text.data);
			dataList.push_back(text);
		}
	}

	GumboVector* children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; i++) {
		childResult = findByAttrString(static_cast<GumboNode*>(children->data[i]), tag, attrName);
		dataList.reserve(dataList.size() + childResult.size());
		dataList.insert(dataList.end(), childResult.begin(), childResult.end());
	}
	return dataList;
}

std::vector<GumboNode*> findByAttr(GumboNode* node, GumboTag tag, std::string attrName) {
	std::vector<GumboNode*> dataList;
	std::vector<GumboNode*> childResult;
	GumboAttribute* attr;
	std::string text;
	if (node->type != GUMBO_NODE_ELEMENT) {
		return dataList;
	}
	if ((attr = gumbo_get_attribute(&node->v.element.attributes, "class")) != NULL) {
		std::cout << attr->value << "\n";
	}

	if (node->v.element.tag == tag &&
		(attr = gumbo_get_attribute(&node->v.element.attributes, attrName.c_str())) != NULL) {
		dataList.push_back(node);
	}

	GumboVector* children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; i++) {
		childResult = findByAttr(static_cast<GumboNode*>(children->data[i]), tag, attrName);
		dataList.reserve(dataList.size() + childResult.size());
		dataList.insert(dataList.end(), childResult.begin(), childResult.end());
	}
	return dataList;
}*/