#include "../include/XmlParser.hpp"

using namespace std; 
XmlParser::XmlParser(string xmlString) {
	string tag;
	string closeTag;
	string value;
	auto c = xmlString.begin();
	while (c != xmlString.end()) {
		switch (*c)
		{
		case '<':
			c++;
			if (*c == '/') {
				c++;
				while (*c != '>') {
					closeTag += *c;
					c++;
				}
				c++;
			}
			else {
				while (*c != '>') {
					tag += *c;
					c++;
				}
				c++;
			}
			break;

		case '\n':
			if (!tag.empty() && !closeTag.empty()) {
				if (!(tag == closeTag))
					throw new Bad_Parser("Tag: " + tag + " don't match with close tag: " + closeTag);
				this->tokens.push_back(XmlToken(tag, value));
				tag.clear();
				closeTag.clear();
				value.clear();	
			}
			else if (!closeTag.empty() && tag.empty()) {
				if (!checkCloseTag(closeTag))
					throw new Bad_Parser("CloseTag: " + closeTag + " don't match with any open tag");
				this->tokens.push_back(XmlToken(closeTag, value));
				tag.clear();
				closeTag.clear();
				value.clear();
			}
			else if (closeTag.empty() && !tag.empty()) {
				this->tokens.push_back(XmlToken(tag, value));
				tag.clear();
				closeTag.clear();
				value.clear();
			}
			c++;
			break;

		case EOF:
			return;
		default:
			while (*c != '<') {
				if (*c == EOF)
					throw new Bad_Parser("Error reached end of while in unexpected way");
				value += *c;
				c++;
			}
			break;
		}
	}
	
}

bool XmlParser::checkCloseTag(string closeTag) {
	for (int i = 0; i < this->tokens.size(); i++) {
		if (this->tokens.at(i).tag.compare(closeTag) == 0)
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

size_t XmlParser::size() {
	return this->tokens.size();
}

XmlToken XmlParser::getToken(int index) {
	return this->tokens.at(index);
}