#pragma once
#include <iostream>
#include <exception>

#include "../include/Database.hpp"

namespace WellDoneDB {

	class Bad_Query : std::exception{
	public:
		std::string message;
		explicit Bad_Query(std::string message) : message{message}
		{std::cout << message << std::endl; }
	};

	enum class TAG {
		SELECT, UPDATE, SET, ORDER_BY, ASC, DESC, CREATE_TABLE, INSERT_INTO, DROP, TRUNCATE, DELETE_FROM, COLUMN, VALUE,
		NOT_NULL, AUTOINCREMENT, OPENBRACKET = '(', CLOSEBRACKET = ')', WHERE, TABLE, CONDITION, AND, OR, NOT, ASTERISKOP, BETWEEN,
		COMMA = ',', LIKE, COMMAPOINT = ';', TYPE, FROM, COLUMNORTABLE = -2, VALUES, REFERENCES, PRIMARY_KEY, FOREIGN_KEY
	};

	struct SQLToken {
		TAG tag;
		std::string value;
		explicit SQLToken(TAG tag, std::string value = std::string{}) : tag{tag},value{value}{}
	};

	class SQLTokenizer {
	private:
		TAG _wordIdentificator(std::string string);
		std::string query;
		std::vector<SQLToken> tokens;
	public:
		std::vector<SQLToken>::iterator begin() { return tokens.begin(); }
		std::vector<SQLToken>::iterator end() { return tokens.end(); }
		std::vector<SQLToken> getTokens() { return tokens; }
		explicit SQLTokenizer(std::string query);
	};


	class SQLlexer {
	private:
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
		SQLTokenizer tokenizer;

	public:
		explicit SQLlexer(std::string query);
		inline std::vector<SQLToken> getTokens() { return tokenizer.getTokens(); }
		inline std::vector<std::vector<SQLToken>> getTransaction() { return transaction; }
		inline std::vector<SQLToken> operator[](int index) { return getQuery(index); }
		inline std::vector<SQLToken> getQuery(int index) { return transaction[index]; }
	};

	class SQLParser {
	private:
		SQLlexer lexer;
		std::vector<SQLToken>::iterator c;
		Database* connected_db;
		std::vector<SQLToken> actualQuery;
		void parseSelect();
		void parseCreate();
		void parseDelete();
		void parseTruncate();
		void parseInsert();
		void parseDrop();
	public:
		SQLParser(std::string query, Database * db);
	};

	Types typeRetriever(std::string);
}