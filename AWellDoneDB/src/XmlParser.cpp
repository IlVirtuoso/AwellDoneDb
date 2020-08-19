#include "../include/XmlParser.hpp"

XmlParser::XmlParser(string xmlString) {
	string buffer;
	string valueBuffer;
	string closeTagBuffer;
	this->xmlString = xmlString;
	for (int i = 0; i < xmlString.size(); i++) {
		if (xmlString[i++] == '<') {
			while (xmlString[i] != '>') {
				buffer.insert(buffer.end(), xmlString[i++]);
			}
			if (xmlString[++i] == '\n') {
				this->tokens.insert(this->tokens.end(),Token(buffer,"",TokenID::GROUPTAG));
				buffer.erase();
			}
			else {
				while (xmlString[i] != '<')
					valueBuffer.insert(valueBuffer.end(), xmlString[i++]);
				i++;
				while (xmlString[i] != '>') {
					closeTagBuffer.insert(closeTagBuffer.end(), xmlString[i++]);
				}
				if (!closeTagBuffer.compare(buffer) == 0)
					if (!this->checkCloseTag(closeTagBuffer))
						return;
				this->tokens.insert(this->tokens.end(), Token(buffer, valueBuffer, TokenID::VALUETAG));
				buffer.erase();
				valueBuffer.erase();
				closeTagBuffer.erase();
				i++;
			}
		}
		else {
			return;
		}
	}
}

bool XmlParser::checkCloseTag(string closeTag) {
	for (int i = 0; i < this->tokens.size(); i++) {
		if (this->tokens.at(i).tag.compare(closeTag) == 1)
			return true;
	}
	return false;
}

bool XmlParser::tagExists(string tag) {
	for(int i = 0; i < this->tokens.size(); i++) {
		if (this->tokens.at(i).tag.compare(tag) == 0)
			return true;
	}
	return false;
}

Token::Token(string tag, string Value, TokenID id)
{
	this->tag = tag;
	this->value = Value;
	this->id = id;
}

int XmlParser::size() {
	return this->tokens.size();
}

Token XmlParser::getToken(int index) {
	return this->tokens.at(index);
}