#include "stdafx.h"
#include <regex>
#include "FetchUrl.h"
#include <curl/curl.h>

const unsigned short FetchUrl::MODE_RAW = 0;
const unsigned short FetchUrl::MODE_JSON = 1;

size_t FetchUrl::writeBuffer(char *data, size_t size, size_t nmemb, std::string *writerData)
{
	if (writerData == NULL) {
		return 0;
	}
	writerData->append(data, size*nmemb);

	return size * nmemb;
}

std::string FetchUrl::parseJson(std::string rawJson)
{
	std::smatch match;
	if (std::regex_search(rawJson, match, std::regex("\"([A-Z]+ \\d+Z [^\"]+)\""))) {
		return std::regex_replace(
			match[1].str(),
			std::regex("\\\\"),
			""
		);
	}

	return "";
}

FetchUrl::FetchUrl()
{
}

FetchUrl::~FetchUrl()
{
}

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
		struct curl_slist *httpHeaders = NULL;
		httpHeaders = curl_slist_append(httpHeaders, "User-Agent: AeroflyWX");
		if (fetchMode == FetchUrl::MODE_JSON) {
			httpHeaders = curl_slist_append(httpHeaders, "Accept: application/json");
		}
		if (apiKey != "") {
			auto apiKeyX = "X-API-Key: " + apiKey;
			httpHeaders = curl_slist_append(httpHeaders, apiKeyX.c_str());
			auto apiKeyA = "Authorization: " + apiKey;
			httpHeaders = curl_slist_append(httpHeaders, apiKeyA.c_str());
		}

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, this->writeBuffer);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, httpHeaders);
		curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); //s

		// Do request
		res = curl_easy_perform(curl);
		curl_slist_free_all(httpHeaders);
		curl_easy_cleanup(curl);

		if (res == CURLE_HTTP_RETURNED_ERROR) {
			long response_code;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

			if (response_code == 400) {
				throw std::invalid_argument("Wrong request (e.g. unrecognized airport) encountered while calling " + url);
			}
			else if (response_code == 401 || response_code == 402 || response_code == 403) {
				throw std::invalid_argument("Missing or invalid API key supplied for " + url);
			}
			else if (response_code == 404) {
				throw std::invalid_argument("API is missing for " + url);
			}
			else if (response_code >= 500) {
				throw std::invalid_argument("Server error encountered while calling " + url);
			}
			else {
				throw std::invalid_argument("Invalid HTTP status code " + std::to_string(response_code) + " returned for " + url);
			}

		}
		else if (res == CURLE_OPERATION_TIMEDOUT) {
			throw std::invalid_argument("Timeout for " + url);
		}
		else if (res != CURLE_OK) {
			throw std::invalid_argument(curl_easy_strerror(res));
		}

		if (fetchMode == FetchUrl::MODE_JSON) {
			buffer = this->parseJson(buffer);
		}
		if (buffer == "") {
			throw std::invalid_argument("Empty or invalid response returned for " + url + " - possibily unkonw ICAO code");
		}

		return buffer;
	}

	throw std::invalid_argument("Could no initiate CURL");
}

std::string FetchUrl::fetch(std::string url, std::string icaoCode, unsigned short fetchMode, std::string apiKey, bool lowercase)
{
	icaoCode = std::regex_replace(
		icaoCode,
		std::regex("[^a-zA-z0-9]"),
		""
	);
	if (lowercase) {
		transform(icaoCode.begin(), icaoCode.end(), icaoCode.begin(), ::tolower);
	}
	else {
		transform(icaoCode.begin(), icaoCode.end(), icaoCode.begin(), ::toupper);
	}
	url = std::regex_replace(url, std::regex("XXXX"), icaoCode);
	return this->fetch(url, fetchMode, apiKey);
}
