#include "pch.h"
#include <regex>
#include "FetchUrl.h"
#include <curl/curl.h>

const unsigned short FetchUrl::MODE_RAW = 0;
const unsigned short FetchUrl::MODE_JSON = 1;

// https://curl.haxx.se/libcurl/c/htmltitle.html
int FetchUrl::writeBuffer(char *data, size_t size, size_t nmemb, std::string *writerData)
{
	if (writerData == NULL) {
		return 0;
	}
	writerData->append(data, size*nmemb);

	return size * nmemb;
}

// Actually this does not parse JSON, but finds the METAR string
std::string FetchUrl::parseJson(std::string rawJson)
{
	std::smatch match;
	if (std::regex_search(rawJson, match, std::regex("\"([A-Z]+ \\d+Z [^\"]+)\""))) {
		return match[1].str();
	}

	return rawJson;
}

FetchUrl::FetchUrl()
{
}

FetchUrl::~FetchUrl()
{
}

// Make HTTP request
std::string FetchUrl::fetch(std::string url, unsigned short fetchMode, std::string apiKey)
{
	// https://curl.haxx.se/libcurl/c/htmltitle.html
	// https://stackoverflow.com/questions/9776857/curl-simple-https-request-returns-nothing-c

	CURL *curl;
	CURLcode res;
	std::string buffer;

	curl = curl_easy_init();
	if (curl) {
		// @see https://curl.haxx.se/libcurl/c/CURLOPT_HTTPHEADER.html
		struct curl_slist *list = NULL;
		list = curl_slist_append(list, "User-Agent: AeroflyWX");
		if (fetchMode == FetchUrl::MODE_JSON) {
			list = curl_slist_append(list, "Accept: application/json");
		}
		if (apiKey != "") {
			apiKey = "X-API-Key: " + apiKey;
			list = curl_slist_append(list, apiKey.c_str());
		}

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, this->writeBuffer);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

		// Do request
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (buffer != "") {
			return (fetchMode == FetchUrl::MODE_JSON) ? this->parseJson(buffer) : buffer;
		}
	}

	return "";
}

// Substitute XXXX with ICAO code
std::string FetchUrl::fetch(std::string url, std::string icaoCode, unsigned short fetchMode, std::string apiKey)
{
	url = std::regex_replace(url, std::regex("XXXX"), icaoCode);
	return this->fetch(url, fetchMode, apiKey);
}
