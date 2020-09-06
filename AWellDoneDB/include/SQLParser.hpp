#pragma once
#include <iostream>
#include <exception>

#include "../include/Database.hpp"

namespace WellDoneDB {

	/**
	 * @brief Classe per la gestione delle eccezioni durante la parsificazione
	*/
	class Bad_Query : public std::exception{
	public:
		std::string message;
		explicit Bad_Query(std::string message) : message{message}
		{std::cout << message << std::endl; }
	};

	/**
	 * @brief Enumerazione contenente i tag degli operatori di linguaggio
	*/
	enum class TAG {
		SELECT, UPDATE, SET, ORDER_BY, ASC, DESC, CREATE_TABLE, INSERT_INTO, DROP, TRUNCATE, DELETE_FROM, COLUMN, VALUE,
		NOT_NULL, AUTOINCREMENT, OPENBRACKET, CLOSEBRACKET, WHERE, TABLE, CONDITION, AND, OR, NOT, ASTERISKOP, BETWEEN,
		COMMA, LIKE, COMMAPOINT, TYPE, FROM, COLUMNORTABLE , VALUES, REFERENCES, PRIMARY_KEY, FOREIGN_KEY,NULLVALUE
	};

	/**
	 * @brief Token per il parser SQL
	 * non viene usata la struttura generica Pair per utilizzare un solo oggetto specifico per il parsing
	*/
	struct SQLToken {
		TAG tag;
		std::string value;
		explicit SQLToken(TAG tag, std::string value = std::string{}) : tag{tag},value{value}{}
	};

	/**
	 * @brief Classe oggetto funzionale Tokenizer divide la query in token
	 * @warnings Questa classe viene istanziata autonomamente dall SQLlexer
	*/
	class SQLTokenizer {
	private:
		TAG _wordIdentificator(std::string string);
		std::string query;
		std::vector<SQLToken> tokens;
	public:
		std::vector<SQLToken>::iterator begin() { return tokens.begin(); }
		std::vector<SQLToken>::iterator end() { return tokens.end(); }
		std::vector<SQLToken> getTokens() { return tokens; }

		/**
		 * @brief Costruttore della classe
		 * @param query stringa contenente la query da parsificare
		 * il tokenizer puó occuparsi di un numero arbitrario di query
		 * purché queste vengano divise da ; tuttavia questo non é necessario se
		 * per esempio stiamo specificando una query singola
		*/
		explicit SQLTokenizer(std::string query);
	};

	/**
	 * @brief Classe oggetto funzionale Lexer, controlla la correttezza del linguaggio
	 * Il lexer é implementato seguendo un opportuno schema ricorsivo discendente
	 * che riconosce la correttezza della query
	 * @warnings Questa classe viene istanziata autonomamente dal SQLParser
	*/
	class SQLlexer {
	private:
		/**
		 * @brief Funzioni di supporto al lexer per il controllo della correttezza
		*/
		void _col();
		void _table();
		void _operand(TAG requested);
		void _end();
		void _conditions();
		void _optoperand();
		void _values();
		void _start();
		std::vector<SQLToken>::iterator c;
		std::string query;
		std::vector<std::vector<SQLToken>> transaction;
		SQLTokenizer * tokenizer;

	public:
		/**
		 * @brief Costruttore della classe SQLlexer
		 * @param query query da parsificare
		 * @warnings Questo oggetto istanzia autonomamente un SQLTokenizer
		 * a questo oggetto possono essere passate una serie di query divise da ; tranne l'ultima, si 
		 * occuperá autonomamente di scomporre le query in un vettore di query che possano essere lette 
		 * singolarmente dal parser
		*/
		explicit SQLlexer(std::string query);
		inline std::vector<SQLToken> getTokens() { return tokenizer->getTokens(); }
		inline std::vector<std::vector<SQLToken>> getTransaction() { return transaction; }
		inline std::vector<SQLToken> operator[](int index) { return getQuery(index); }
		inline std::vector<SQLToken> getQuery(int index) { return transaction[index]; }
	};

	/**
	 * @brief Classe oggetto funzionale del parser
	 * si occupa di parsificare i comandi e agire sul database di conseguenza
	*/
	class SQLParser {
	private:
		SQLlexer lexer;
		std::vector<SQLToken>::iterator c;
		Database* connected_db;
		std::vector<SQLToken> actualQuery;
		/**
		 * @brief Funzioni di supporto al parser per ogni comando disponibile
		*/
		void parseSelect();
		void parseCreate();
		void parseDelete();
		void parseTruncate();
		void parseInsert();
		void parseDrop();
	public:
		/**
		 * @brief Costruttore della classe SQLParser
		 * @param query query da eseguire sul database
		 * @param db puntatore a un database preallocato su cui eseguire l'azione
		*/
		SQLParser(std::string query, Database * db);
	};

	/**
	 * @brief Converte una stringa in un types cercando di dedurne il tipo.
	 * @param string valore da parsificare
	*/
	Types typeRetriever(std::string);
}