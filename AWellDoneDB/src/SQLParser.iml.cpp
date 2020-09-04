#include "../include/SQLParser.hpp"

namespace WellDoneDB {

	std::string _toLower(std::string string) {
		auto c = string.begin();
		std::string ret;
		while (c != string.end())
			ret += std::tolower(*c++);
		return ret;
	}

	TAG SQLTokenizer::_wordIdentificator(std::string word) {
		if (word == "SELECT" || word == "select") return TAG::SELECT;
		else if (word == "WHERE" || word == "where") return TAG::WHERE;
		else if (word == "INSERT INTO" || word == "insert into") return TAG::INSERT_INTO;
		else if (word == "DELETE FROM" || word == "delete from") return TAG::DELETE_FROM;
		else if (word == "UPDATE" || word == "update") return TAG::UPDATE;
		else if (word == "CREATE TABLE" || word == "create table") return TAG::CREATE_TABLE;
		else if (word == "AND" || word == "and") return TAG::AND;
		else if (word == "OR" || word == "or") return TAG::OR;
		else if (word == "SET" || word == "set")return TAG::SET;
		else if (word == "NOT NULL" || word == "not null") return TAG::NOT_NULL;
		else if (word == "NOT" || word == "not") return TAG::NOT;
		else if (word == "ASC" || word == "asc") return TAG::ASC;
		else if (word == "DESC" || word == "desc") return TAG::DESC;
		else if (word == "BETWEEN" || word == "between") return TAG::BETWEEN;
		else if (word == "LIKE" || word == "like") return TAG::LIKE;
		else if (word == "DROP TABLE" || word == "drop from") return TAG::DROP;
		else if (word == "TRUNCATE TABLE" || word == "truncate table") return TAG::TRUNCATE;
		else if (word == "ORDER BY" || word == "order by") return TAG::ORDER_BY;
		else if (word == "FROM" || word == "from") return TAG::FROM;
		else if (word == "PRIMARY KEY" || word == "primary key") return TAG::PRIMARY_KEY;
		else if (word == "FOREIGN KEY" || word == "foreign key") return TAG::FOREIGN_KEY;
		else if (word == "REFERENCES" || word == "references") return TAG::REFERENCES;
		else if (word == "VALUES" || word == "values") return TAG::VALUES;
		else if (stringToType(word) != Types::NOTVALID) return TAG::TYPE;
		else return TAG::COLUMNORTABLE;

	}


	bool _isLecter(char c) {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
	}

	bool _isCharNumber(char c) {
		return (c >= '0' || c <= '9');
	}

	bool _isSpecialChar(char c) {
		std::vector<char> specials = { '<', '>', ';', ',','(', ')', ' '};
		for (int i = 0; i < specials.size(); i++) {
			if (c == specials[i])
				return true;
		}
		return false;
	}

	SQLTokenizer::SQLTokenizer(std::string query) : query{ query } {
		if (query.empty())
			throw new Bad_Query("Empty Query");
		auto c = query.begin();
		std::string buffer;
		while (c != query.end()) {
			switch (*c)
			{
			case '(':
				this->tokens.push_back(SQLToken(TAG::OPENBRACKET, "("));
				c++;
				break;

			case ')':
				this->tokens.push_back(SQLToken(TAG::CLOSEBRACKET, ")"));
				c++;
				break;

			case '\'':
				c++;
				while (*c != '\'') {
					buffer += *c;
					c++;
				}
				this->tokens.push_back(SQLToken(TAG::VALUE, buffer));
				c++;
				buffer.clear();
				break;

			case '>':
				buffer += *c;
				c++;
				if (*c == ' ') this->tokens.push_back(SQLToken(TAG::CONDITION, buffer));
				else if (*c == '=') { buffer += *c; this->tokens.push_back(SQLToken(TAG::CONDITION, buffer)); }
				else throw new Bad_Query("Query error, wrong condition near: " + this->tokens[this->tokens.size()].value + "at char: " + *c);
				c++;
				buffer.clear();
				break;

			case '<':
				buffer += *c;
				c++;
				if (*c == ' ') this->tokens.push_back(SQLToken(TAG::CONDITION, buffer));
				else if (*c == '>' || *c == '=') { buffer += *c; this->tokens.push_back(SQLToken(TAG::CONDITION, buffer)); }
				else throw new Bad_Query("Query error, wrong condition near: " + this->tokens[this->tokens.size()].value + "at char: " + *c);
				c++;
				buffer.clear();
				break;

			case '=':
				buffer += *c;
				c++;
				if (*c == ' ') this->tokens.push_back(SQLToken(TAG::CONDITION, buffer));
				else if (*c == '=') { buffer += *c; this->tokens.push_back(SQLToken(TAG::CONDITION, buffer)); }
				else throw new Bad_Query("Query error, wrong condition near: " + this->tokens[this->tokens.size()].value + "at char: " + *c);
				c++;
				buffer.clear();
				break;

			case ' ':
				c++;
				break;

			case '*':
				buffer += *c;
				c++;
				if (*c == ' ') {
					this->tokens.push_back(SQLToken(TAG::ASTERISKOP, buffer));
					buffer.clear();
					c++;
				}
				break;

			case '.':
				c++;
				while (*c != ' ' && !_isSpecialChar(*c)) {
					buffer += *c;
					c++;
				}
				this->tokens[this->tokens.size() - 1].tag == TAG::TABLE;
				this->tokens.push_back(SQLToken(TAG::COLUMN, buffer));
				buffer.clear();
				break;

			case ',':
				c++;
				this->tokens.push_back(SQLToken(TAG::COMMA, ","));
				break;

			case ';':
				c++;
				this->tokens.push_back(SQLToken(TAG::COMMAPOINT, ";"));
				break;

			default:
				if (_isLecter(*c)) {
					while ((_isLecter(*c) || _isCharNumber(*c)) && !_isSpecialChar(*c)) {
						buffer += *c;
						c++;
					}
					if (*c == ' ') {
						if (buffer == "INSERT" || buffer == "DELETE" || buffer == "DROP" || buffer == "ORDER" || buffer == "CREATE" || buffer == "TRUNCATE" || buffer == "PRIMARY" || buffer == "FOREIGN") {
							buffer += *c;
							c++;
							while (_isLecter(*c)) {
								buffer += *c;
								c++;
							}
						}
						if (buffer == "NOT") {
							std::string secondaryBuffer;
							c++;
							while (_isLecter(*c)) {
								secondaryBuffer += *c;
								c++;
							}
							if (secondaryBuffer == "NULL") {
								buffer += " " + secondaryBuffer;
								this->tokens.push_back(SQLToken(_wordIdentificator(buffer), buffer));
							}
							else {
								this->tokens.push_back(SQLToken(_wordIdentificator(buffer), buffer));
								this->tokens.push_back(SQLToken(_wordIdentificator(secondaryBuffer), secondaryBuffer));
							}
							buffer.clear();
							break;
						}
					}
					this->tokens.push_back(SQLToken(_wordIdentificator(buffer), buffer));
					buffer.clear();
				}
				else if (_isCharNumber(*c)) {
					while (*c != ' ' && !_isSpecialChar(*c)) {
						buffer += *c;
						c++;
					}
					this->tokens.push_back(SQLToken(TAG::VALUE, buffer));
					buffer.clear();
				}
				break;
			}
		}
	}

	

	void SQLlexer::_start()
	{
		switch (c->tag)
		{

		case TAG::CREATE_TABLE:
			c++;
			_table();
			_operand(TAG::OPENBRACKET);
			_col();
			_optoperand();
			_operand(TAG::CLOSEBRACKET);
			_end();
			break;

		case TAG::DELETE_FROM:
			c++;
			_table();
			_operand(TAG::WHERE);
			_end();
			break;
		case TAG::SELECT:
			c++;
			_col();
			_operand(TAG::FROM);
			_table();
			_optoperand();
			_end();
			break;

		case TAG::DROP:
			c++;
			_table();
			_end();
			break;

		case TAG::UPDATE:
			c++;
			_table();
			_operand(TAG::SET);
			_conditions();
			_optoperand();
			_end();
			break;

		case TAG::INSERT_INTO:
			c++;
			_table();
			_operand(TAG::OPENBRACKET);
			_col();
			_operand(TAG::CLOSEBRACKET);
			_operand(TAG::VALUES);
			_operand(TAG::OPENBRACKET);
			_values();
			_operand(TAG::CLOSEBRACKET);
			_end();
			break;

		case TAG::TRUNCATE:
			c++;
			_table();
			_end();
			break;

		default:
			throw new Bad_Query("Bad Operand at start: " + c->value);
			break;
		}
	}

	void SQLlexer::_col()
	{
		switch (c->tag)
		{
		case TAG::COLUMNORTABLE:
			c->tag = TAG::COLUMN;
			_col();
			break;

		case TAG::COLUMN:
			c++;
			if ((c->tag == TAG::TYPE || c->tag == TAG::NOT_NULL || c->tag == TAG::AUTOINCREMENT) && (c[-2].tag == TAG::OPENBRACKET || c[-2].tag == TAG::COMMA)) {
				while (c->tag == TAG::TYPE || c->tag == TAG::NOT_NULL || c->tag == TAG::AUTOINCREMENT) {
					c++;
				}
			}
			if (c->tag == TAG::COMMA)
				c++;
				_col();
			break;

		case TAG::ASTERISKOP:
			if (!(c[-1].tag == TAG::SELECT))
				throw new Bad_Query("Error * operator used without SELECT");
			else
				c++;
			break;

		default:
			break;
		}
	}

	void SQLlexer::_table()
	{
		switch (c->tag)
		{
		case TAG::COLUMNORTABLE:
			c->tag = TAG::TABLE;
			c++;
			if (c->tag == TAG::COMMA) {
				c++;
				_table();
			}
			break;

		case TAG::TABLE:
			c++;
			if (c->tag == TAG::COMMA)
				c++;
				_table();
			break;
		default:
			break;
		}
	}

	void SQLlexer::_operand(TAG requested)
	{
		if (requested != c->tag)
			throw new Bad_Query("Bad operand " + c->value + " near token " + c[-1].value);
		else if(requested != TAG::WHERE)
			c++;
		else {
			c++;
			_conditions();
		}
	}

	void SQLlexer::_values() {
		switch (c->tag)
		{
		case TAG::VALUE:
			c++;
			if (c->tag == TAG::COMMA)
				c++;
				_values();
			break;
		default:
			break;
		}
	}

	void SQLlexer::_optoperand()
	{
		switch (c->tag)
		{
		case TAG::WHERE:
			c++;
			_conditions();
			_optoperand();
			break;

		case TAG::AND:
			c++;
			_conditions();
			_optoperand();
			break;

		case TAG::OR:
			c++;
			_conditions();
			_optoperand();
			break;

		case TAG::NOT:
			c++;
			_conditions();
			_optoperand();
			break;

		case TAG::BETWEEN:
			c++;
			_values();
			_operand(TAG::AND);
			_values();
			_optoperand();
			break;
		case TAG::ORDER_BY:
			c++;
			_col();
			_optoperand();
			break;

		case TAG::ASC:
			c++;
			_end();
			break;

		case TAG::DESC:
			c++;
			_end();
			break; 



		case TAG::PRIMARY_KEY:
			c++;
			_operand(TAG::OPENBRACKET);
			_col();
			_operand(TAG::CLOSEBRACKET);
			if (c->tag == TAG::COMMA) {
				c++;
				_optoperand();
			}
			break;

		case TAG::FOREIGN_KEY:
			c++;
			_operand(TAG::OPENBRACKET);
			_col();
			_operand(TAG::CLOSEBRACKET);
			_operand(TAG::REFERENCES);
			_table();
			_operand(TAG::OPENBRACKET);
			_col();
			_operand(TAG::CLOSEBRACKET);
			if (c->tag == TAG::COMMA) {
				c++;
				_optoperand();
			}
			break;
		default:
			break;
		}
	}

	void SQLlexer::_conditions() {
		while (c->tag == TAG::COLUMN || c->tag == TAG::CONDITION || c->tag == TAG::VALUE || c->tag == TAG::COLUMNORTABLE)
			c++;
	}


	void SQLlexer::_end() {
		if (!(c->tag == TAG::COMMAPOINT))
			throw new Bad_Query("Error at: " + c->value + " near :" + c[-1].value);
	}

	SQLlexer::SQLlexer(std::string query) : query{ query }, tokenizer{ SQLTokenizer(query) } {
		auto c = tokenizer.begin();
		while (c != tokenizer.end()) {
			std::vector<SQLToken> query;
			while (c->tag != TAG::COMMAPOINT && c != tokenizer.end()) {
				query.push_back(*c);
				c++;
			}
			query.push_back(*c);
			this->transaction.push_back(query);
			query.clear();
			c++;
		}
		for (int i = 0; i < this->transaction.size(); i++) {
			this->c = this->transaction[i].begin();
			_start();
		}
	}



	SQLParser::SQLParser(std::string query, Database* db) : lexer{ SQLlexer(query) }, connected_db{ db } {
		for (int i = 0; i < lexer.getTransaction().size(); i++) {
			this->actualQuery = lexer[i];
			this->c = actualQuery.begin();
			switch (c->tag)
			{
			case TAG::SELECT:
				parseSelect();
				break;

			case TAG::CREATE_TABLE:
				parseCreate();
				break;

			case TAG::DROP:
				parseDrop();
				break;

			case TAG::DELETE_FROM:
				parseDelete();
				break;

			case TAG::INSERT_INTO:
				parseInsert();
				break;
			default:
				break;
			}
		}
	}

	

	void SQLParser::parseSelect()
	{
		std::vector<std::string> cols;
		std::string orderColumn;
		bool desc = false;
		std::string table;
		Selection * sel = nullptr;
		c++;
		if (!(c->tag == TAG::ASTERISKOP)) {
			while (c->tag != TAG::FROM) {
				cols.push_back(c->value);
				c++;
			}
		}
		else {
			c++;
		}
		c++;
		table = c->value;
		if(cols.empty())
			cols = this->connected_db->get(table)->getColNames();
		c++;
		if (c->tag == TAG::WHERE) {
			std::string column;
			Conditions condition;
			Type* value;
			c++;
			while (c->tag != TAG::ORDER_BY && c->tag != TAG::COMMAPOINT)
			{
				if (c->tag == TAG::VALUE) {
					column = c[-2].value;
					condition = conditionToString(c[-1].value);
					value = stringToType(c->value, typeRetriever(c->value));
					if (sel == nullptr)
						sel = new Selection(*this->connected_db->get(table)->get(column), condition, value);
					else if (sel != nullptr && c[-3].tag == TAG::AND)
						sel = new Selection((*sel && Selection(*this->connected_db->get(table)->get(column), condition, value)));
					else if (sel != nullptr && c[-3].tag == TAG::OR)
						sel = new Selection((*sel || Selection(*this->connected_db->get(table)->get(column), condition, value)));
				}
				else if (c->tag == TAG::BETWEEN) {
					TAG chainCond = c[-2].tag;
					column = c[-1].value;
					c++;
					Type* valueA = stringToType(c->value, typeRetriever(c->value));
					c += 2;
					Type* valueB = stringToType(c->value, typeRetriever(c->value));
					if (chainCond == TAG::AND)
						sel = &(*sel && Selection(*this->connected_db->get(table)->get(column), valueA, valueB));
					else if (chainCond == TAG::OR)
						sel = &(*sel || Selection(*this->connected_db->get(table)->get(column), valueA, valueB));
					else
						sel = new Selection(*this->connected_db->get(table)->get(column), valueA, valueB);
				}
				c++;
			}
			if (c->tag == TAG::ORDER_BY) {
				c++;
				orderColumn = c->value;
				c++;
				if (c->tag == TAG::DESC)
					desc = true;
				c++;
			}
		}
		Table* temp = this->connected_db->get(table);
		if (sel == nullptr)
			std::cout<<temp->project(cols)->toString()<<std::endl;
		else if (!(sel == nullptr) && orderColumn != "")
			std::cout<<temp->sort(orderColumn, desc)->project(cols)->select(*sel)->toString()<<std::endl;
		else
			std::cout<<temp->select(*sel)->project(cols)->toString()<<std::endl;
	}

	void SQLParser::parseCreate() {

	}

	void SQLParser::parseDelete()
	{
	}

	void SQLParser::parseTruncate()
	{
	}

	void SQLParser::parseInsert() {

	}

	void SQLParser::parseDrop()
	{

	}

	
	Types typeRetriever(std::string value) {
		auto c = value.begin();
		if (_isLecter(*c)) {
			if (value.size() == 1)
				return Types::CHAR;
			else
				return Types::TEXT;
		}
		else if (_isCharNumber(*c)) {
			while (c != value.end()) {
				if (*c == '.' && _isCharNumber(c[1])) {
					return Types::FLOAT;
				}
				if (*c == ':' && _isCharNumber(c[1]))
					return Types::TIME;
				if (*c == '-' && _isCharNumber(c[1]))
					return Types::DATE;
				c++;
			}
			return Types::INT;
		}
	}
	
}