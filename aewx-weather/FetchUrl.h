#pragma once
#include <string>
#include <regex>

class FetchUrl
{
private:
	static int writeBuffer(char * data, size_t size, size_t nmemb, std::string * writerData);
	std::string parseJson(std::string rawJson);

public:
	static const unsigned short MODE_RAW;
	static const unsigned short MODE_JSON;

	FetchUrl();
	~FetchUrl();

	std::string fetch(std::string url, unsigned short fetchMode = FetchUrl::MODE_RAW, std::string apiKey = "");
	std::string fetch(std::string url, std::string icaoCode, unsigned short fetchMode = FetchUrl::MODE_RAW, std::string apiKey = "");

};
