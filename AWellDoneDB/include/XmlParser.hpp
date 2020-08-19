#pragma once
#include<string>
#include<vector>
#include<iostream>

using namespace std;


enum class TokenID {
	GROUPTAG,
	VALUETAG
};

class Token {
public:
	string tag;
	string value;
	TokenID id;
	Token(string Tag, string Value, TokenID id);
};

class XmlParser
{
	vector<Token> tokens;
	string xmlString;
	bool checkCloseTag(string closeTag);
public:
	XmlParser(string xmlString);
	bool tagExists(string tag);
	int size();
	Token getToken(int index);
};





