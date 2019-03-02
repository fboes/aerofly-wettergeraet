#pragma once
#include <string>
#include <regex>

/**
 * Make HTTP request to fetch METAR data from remote services.
 */
class FetchUrl
{
private:
	// https://curl.haxx.se/libcurl/c/htmltitle.html
	static int writeBuffer(char * data, size_t size, size_t nmemb, std::string * writerData);

	// Actually this does not parse JSON, but finds the METAR string
	std::string parseJson(std::string rawJson);

public:
	static const unsigned short MODE_RAW;
	static const unsigned short MODE_JSON;

	FetchUrl();
	~FetchUrl();

	// Make HTTP request.
	// Throws std::invalid_argument
	std::string fetch(std::string url, unsigned short fetchMode = FetchUrl::MODE_RAW, std::string apiKey = "");

	// Like other fetch, but substitutes XXXX with ICAO code
	std::string fetch(std::string url, std::string icaoCode, unsigned short fetchMode = FetchUrl::MODE_RAW, std::string apiKey = "");

};
