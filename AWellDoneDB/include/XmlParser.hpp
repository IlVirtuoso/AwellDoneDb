#pragma once
#include<string>
#include<vector>
#include<iostream>




/**
 * @brief Token per il parsing xml
*/
struct XmlToken {
	std::string tag;
	std::string value;
	explicit XmlToken(std::string Tag, std::string Value) : tag{Tag},value{Value}{}
};

/**
 * @brief Classe oggetto funzionale per parsificare file xml scritti con formato <{tag}>value</{tag}>
*/
class XmlParser
{
	std::vector<XmlToken> tokens;
	std::string xmlString;
	bool checkCloseTag(std::string closeTag);
public:
	/**
	 * @brief Classe per eccezioni del parser
	*/
	class Bad_Parser : public std::exception {
	public:
		std::string message;
		Bad_Parser(std::string message) { std::cout << message << std::endl; }
	};


	auto begin() { return this->tokens.begin(); }
	std::vector<XmlToken>::iterator end() { return this->tokens.end(); }

	/**
	 * @brief Costruttore funzionale per la stringa xml da parsificare
	 * @param xmlString stringa xml di cui eseguire il parsing
	*/
	explicit XmlParser(std::string xmlString);
	/**
	 * @brief Controlla l'esistenza di un particolare token nel vettore
	 * @param tag 
	 * @return true se esiste, false altrimenti
	*/
	bool tagExists(std::string tag);
	size_t size();
	/**
	 * @brief 
	 * @param index 
	 * @return ritorna il token all'indirizzo specificato 
	*/
	XmlToken getToken(int index);
};





