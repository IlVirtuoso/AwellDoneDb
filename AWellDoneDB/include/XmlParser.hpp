#pragma once
#include<string>
#include<vector>
#include<iostream>





struct XmlToken {
	std::string tag;
	std::string value;
	explicit XmlToken(std::string Tag, std::string Value) : tag{Tag},value{Value}{}
};

class XmlParser
{
	std::vector<XmlToken> tokens;
	std::string xmlString;
	bool checkCloseTag(std::string closeTag);
public:
	class Bad_Parser {
	public:
		std::string message;
		Bad_Parser(std::string message) { std::cout << message << std::endl; }
	};
	auto begin() { return this->tokens.begin(); }
	std::vector<XmlToken>::iterator end() { return this->tokens.end(); }

	explicit XmlParser(std::string xmlString);
	bool tagExists(std::string tag);
	int size();
	XmlToken getToken(int index);
};





