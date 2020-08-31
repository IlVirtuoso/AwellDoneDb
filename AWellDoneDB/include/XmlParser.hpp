#pragma once
#include<string>
#include<vector>
#include<iostream>




enum class TokenID {
	GROUPTAG,
	VALUETAG
};

class Token {
public:
	std::string tag;
	std::string value;
	explicit Token(std::string Tag, std::string Value) : tag{Tag},value{Value}{}
};

class XmlParser
{
	std::vector<Token> tokens;
	std::string xmlString;
	bool checkCloseTag(std::string closeTag);
public:
	class Bad_Parser {
	public:
		std::string message;
		Bad_Parser(std::string message) { std::cout << message << std::endl; }
	};
	auto begin() { return this->tokens.begin(); }
	std::vector<Token>::iterator end() { return this->tokens.end(); }

	explicit XmlParser(std::string xmlString);
	bool tagExists(std::string tag);
	int size();
	Token getToken(int index);
};





