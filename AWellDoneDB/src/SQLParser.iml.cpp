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
		else if (word == "AUTO_INCREMENT" || word == "auto_increment") return TAG::AUTOINCREMENT;
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
		else if (word == "DROP TABLE" || word == "drop table") return TAG::DROP;
		else if (word == "TRUNCATE TABLE" || word == "truncate table") return TAG::TRUNCATE;
		else if (word == "ORDER BY" || word == "order by") return TAG::ORDER_BY;
		else if (word == "FROM" || word == "from") return TAG::FROM;
		else if (word == "PRIMARY KEY" || word == "primary key") return TAG::PRIMARY_KEY;
		else if (word == "FOREIGN KEY" || word == "foreign key") return TAG::FOREIGN_KEY;
		else if (word == "REFERENCES" || word == "references") return TAG::REFERENCES;
		else if (word == "NULL") return TAG::NULLVALUE;
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
		std::vector<char> specials = { '<', '>', ';', ',','(', ')', ' ','=','<','>','\0' };
		for (int i = 0; i < specials.size(); i++) {
			if (c == specials[i])
				return true;
		}
		return false;
	}


	bool _concatop(std::string buffer) {
		std::vector<const char *> operators = { "INSERT","DELETE","DROP","ORDER", "CREATE",
			"TRUNCATE","PRIMARY","FOREIGN","insert" , "delete" ,"drop","order","create","truncate","primary","foreign"};
		for (int i = 0; i < operators.size(); i++) {
			if (buffer == operators[i])
				return true;
		}
		return false;
	}

	SQLTokenizer::SQLTokenizer(std::string query) : query{ query } {
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
					if (c == query.end())
						throw new Bad_Query("Error while parsing string: " + buffer + "reached end of line unexpected check string close");
				}
				this->tokens.push_back(SQLToken(TAG::VALUE, buffer));
				c++;
				buffer.clear();
				break;

			case '>':
				buffer += *c;
				c++;
				if (*c == ' ') {
					this->tokens.push_back(SQLToken(TAG::CONDITION, buffer));
					c++;
					buffer.clear();
				}
				else if (*c == '=') {
					buffer += *c; this->tokens.push_back(SQLToken(TAG::CONDITION, buffer));
					c++;
					buffer.clear();
				}
				else if (!(_isCharNumber(*c) || _isLecter(*c))) {
					throw new Bad_Query("Query error, wrong condition near: " + this->tokens[this->tokens.size() - 1].value + "at char: " + *c);
				}
				else {
					this->tokens.push_back(SQLToken(TAG::CONDITION, buffer));
					buffer.clear();
				}

				break;

			case '<':
				buffer += *c;
				c++;
				if (*c == ' ') {
					this->tokens.push_back(SQLToken(TAG::CONDITION, buffer));
					c++;
					buffer.clear();
				}
				else if (*c == '>' || *c == '=') { 
					buffer += *c; this->tokens.push_back(SQLToken(TAG::CONDITION, buffer)); buffer.clear(); c++;
				}
				else if (!(_isCharNumber(*c) || _isLecter(*c))) {
					throw new Bad_Query("Query error, wrong condition near: " + this->tokens[this->tokens.size() - 1].value + "at char: " + *c);
				}
				else {
					this->tokens.push_back(SQLToken(TAG::CONDITION, buffer));
					buffer.clear();
				}

				break;

			case '=':
				buffer += *c;
				c++;
				if (*c == ' ') {
					this->tokens.push_back(SQLToken(TAG::CONDITION, buffer));
					c++;
					buffer.clear();
				}
				else if (*c == '=') { buffer += *c; this->tokens.push_back(SQLToken(TAG::CONDITION, buffer)); }
				else if(!(_isCharNumber(*c) || _isLecter(*c))){
					throw new Bad_Query("Query error, wrong condition near: " + this->tokens[this->tokens.size() - 1].value + "at char: " + *c);
				}
				else {
					this->tokens.push_back(SQLToken(TAG::CONDITION, buffer));
					buffer.clear();
				}
				break;

			case ' ':
				c++;
				break;

			case '\n':
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
				this->tokens[this->tokens.size() - 1].tag = TAG::TABLE;
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

			case '\0':
				break;
			default:
				if (_isLecter(*c)) {
					while (c != query.end() && (_isLecter(*c) || _isCharNumber(*c)) && !_isSpecialChar(*c)) {
						buffer += *c;
						c++;
					}
					if (c == query.end()) {
						this->tokens.push_back(SQLToken(_wordIdentificator(buffer), buffer));
						break;
					}
					if (*c == ' ') {
						if (_concatop(buffer)) {
							buffer += *c;
							c++;
							while (_isLecter(*c)) {
								buffer += *c;
								c++;
							}
						}
						if (buffer == "NOT" || buffer == "not") {
							std::string secondaryBuffer;
							c++;
							while (c != query.end() && _isLecter(*c)) {
								secondaryBuffer += *c;
								c++;
							}
							if (secondaryBuffer == "NULL" || secondaryBuffer == "null") {
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
					while (c != query.end() &&  *c != ' ' && !_isSpecialChar(*c)) {
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
		else if (requested != TAG::WHERE)
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

		case TAG::NULLVALUE:
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

		case TAG::LIKE:
			c++;
			_values();
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
		while (c->tag == TAG::COLUMN || c->tag == TAG::CONDITION || c->tag == TAG::VALUE || c->tag == TAG::COLUMNORTABLE) {
			if (c->tag == TAG::COLUMNORTABLE)
				c->tag = TAG::COLUMN;
			c++;
		}
	}


	void SQLlexer::_end() {
		if (!(c->tag == TAG::COMMAPOINT))
			throw new Bad_Query("Error at: " + c->value + " near :" + c[-1].value);
	}

	SQLlexer::SQLlexer(std::string query) : query{ query } {
		try {
			tokenizer = new SQLTokenizer(query);
		}
		catch (...) {
			return;
		}
		auto c = tokenizer->begin();
		while (c != tokenizer->end()) {
			std::vector<SQLToken> query;
			while (c != tokenizer->end() && c->tag != TAG::COMMAPOINT  ) {
				query.push_back(*c);
				c++;
			}
			if (c == tokenizer->end())
				query.push_back(SQLToken(TAG::COMMAPOINT, "QUERY END AUTOGENERATED"));
			else if (c->tag == TAG::COMMAPOINT) {
				query.push_back(*c);
				c++;
			}
			this->transaction.push_back(query);
			query.clear();
		}
		for (int i = 0; i < this->transaction.size(); i++) {
			this->c = this->transaction[i].begin();
			try {
				_start();
			}
			catch (...) {
				this->transaction.clear();
			}
		}
	}



	SQLParser::SQLParser(std::string query, Database* db) : connected_db{ db }, lexer{ SQLlexer(query) } {
		for (int i = 0; i < lexer.getTransaction().size(); i++) {

			this->actualQuery = lexer[i];
			this->c = actualQuery.begin();
			switch (c->tag)
			{
			case TAG::UPDATE:
				try {
					parseUpdate();
				}
				catch(...){}
				break;
			case TAG::SELECT:
				try {
					parseSelect();
				}
				catch(...){}
				break;

			case TAG::CREATE_TABLE:
				try {
					parseCreate();
				}
				catch (...) { 
					connected_db->deleteTable(connected_db->getTableNames()[connected_db->getTableNames().size() - 1]);
				}
				break;

			case TAG::DROP:
				try {
					parseDrop();
				}
				catch (...) { return; }
				break;

			case TAG::DELETE_FROM:
				try {
					parseDelete();
				}
				catch (...) { return; }
				break;

			case TAG::INSERT_INTO:
				try {
					parseInsert();
				}
				catch(...){}
				break;

			case TAG::TRUNCATE:
				try {
					parseTruncate();
				}
				catch(...){}
			default:
				break;
			}
		}
	}


#define __AssertTableExist__(tableName)\
	if (!this->connected_db->exist(tableName))\
		throw new Bad_Query("Error table " + tableName + " don't exist in this database")

	/*
	void SQLParser::parseSelect()
	{
		std::vector<std::string> cols;
		std::string orderColumn;
		bool desc = false;
		std::string table;
		Selection* sel = nullptr;
		c++;
		if (!(c->tag == TAG::ASTERISKOP)) {
			while (c->tag != TAG::FROM) {
				cols.push_back(c->value);
				c++;
			}
			c++;
		}
		else {
			c += 2;
		}
		table = c->value;
		__AssertTableExist__(table);
		if (cols.empty())
			cols = this->connected_db->get(table)->getColNames();
		else {
			for (int i = 0; i < cols.size(); i++) {
				if (!this->connected_db->get(table)->columnExist(cols[i]))
					throw new Bad_Query("Error column " + cols[i] + " don't exist in table " + table);
			}
		}
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
						sel = new Selection(*sel && Selection(*this->connected_db->get(table)->get(column), valueA, valueB));
					else if (chainCond == TAG::OR)
						sel = new Selection(*sel || Selection(*this->connected_db->get(table)->get(column), valueA, valueB));
					else
						sel = new Selection(*this->connected_db->get(table)->get(column), valueA, valueB);
				}
				else if (c->tag == TAG::LIKE) {
					column = c[-1].value;
					std::string likeval = c[1].value;
					if (sel == nullptr)
						sel = new Selection(*this->connected_db->get(table)->get(column),likeval);
					else if (sel != nullptr && c[-2].tag == TAG::AND)
						sel = new Selection((*sel && Selection(*this->connected_db->get(table)->get(column), likeval)));
					else if (sel != nullptr && c[-2].tag == TAG::OR)
						sel = new Selection((*sel || Selection(*this->connected_db->get(table)->get(column), likeval)));
					c++;
				}
				c++;
			}
			if (c->tag == TAG::ORDER_BY) {
				c++;
				orderColumn = c->value;
				if (!this->connected_db->get(table)->columnExist(orderColumn))
					throw new Bad_Query("Error column " + orderColumn + " don't exist in table " + table);
				c++;
				if (c->tag == TAG::DESC)
					desc = true;
				c++;
			}
		}
		Table* temp = this->connected_db->get(table);
		if (sel == nullptr)
			std::cout << temp->project(cols)->toString() << std::endl;
		else if (!(sel == nullptr) && orderColumn != "")
			std::cout << temp->sort(orderColumn, desc)->project(cols)->select(*sel)->toString() << std::endl;
		else
			std::cout << temp->select(*sel)->project(cols)->toString() << std::endl;
	}
	*/

#define __normalizeColumn__(column)\
if (column.find(".") == std::string::npos && tables.size() > 1) {\
for (int i = 0; i < cols.size(); i++) {\
	if (cols[i].find(column) != std::string::npos)\
		column = cols[i];\
}\
}

	void SQLParser::parseSelect() {
		using String = std::string;
		using StringArray = std::vector<std::string>;
		c++;
		StringArray cols;
		StringArray tables;
		bool asteriskop = false;
		//trova le colonne
		if (c->tag == TAG::ASTERISKOP) { asteriskop = true; c++;}
		else {
			while (c->tag != TAG::FROM) {
				if (c->tag == TAG::COLUMN)
					cols.push_back(c->value);
				c++;
			}
		}
		//trova le tabelle
		if (c->tag == TAG::FROM) {
			while (c->tag != TAG::WHERE && c->tag != TAG::COMMAPOINT && c->tag != TAG::ORDER_BY) {
				if (c->tag == TAG::TABLE)
					tables.push_back(c->value);
				c++;
			}
		}
		//aggiunge tutte le colonne se asterisco
		if (asteriskop) {
			for (int i = 0; i < tables.size(); i++) {
				StringArray columns = connected_db->get(tables[i])->getColNames();
				for (int k = 0; k < columns.size(); k++) cols.push_back(columns[k]);
			}
		}
		//normalizza i nomi di colonne in caso di join
		if (tables.size() > 1) {
			for (int i = 0; i < cols.size(); i++) {
				if (cols[i].find(".", 0) == std::string::npos) {
					bool founded = false;
					String currentcol = cols[i];
					for (int k = 0; k < tables.size(); k++) {
						if (connected_db->get(tables[k])->columnExist(currentcol)) {
							if (founded) throw new Bad_Query("Error same column:" + currentcol + " not specified and present in two tables");
							founded = true;
							cols[i] = tables[k] + "." + cols[i];
						}
					}
				}
			}
		}
		//crea un unica tabella su cui effettuare le selezioni
		VectorizedTable* temp = nullptr;
		if (tables.size() == 1) { __AssertTableExist__(tables[0]); temp = dynamic_cast<VectorizedTable*>(connected_db->get(tables[0])); }
		else {
			for (int i = 0; i < tables.size(); i++) {
				__AssertTableExist__(tables[i]);
				if (temp == nullptr) {
					VectorizedTable* get = dynamic_cast<VectorizedTable*>(connected_db->get(tables[i]));
					temp = new VectorizedTable(get->getName());
					*get >> temp;
				}
				else {
					VectorizedTable* join = dynamic_cast<VectorizedTable*>(connected_db->get(tables[i]));
					*temp = *temp * *join;
				}
			}
		}
		//genera le selezioni in caso di where
		Selection* sel = nullptr;
		if (c->tag == TAG::WHERE) {
			while (c->tag != TAG::ORDER_BY && c->tag != TAG::COMMAPOINT) {
				std::string column;
				Conditions condition;
				Type* value;
				if (c->tag == TAG::VALUE) {
					column = c[-2].value;
					//standardizza il nome della colonna
					if (column.find(".")==std::string::npos && tables.size() > 1) {
						for (int i = 0; i < cols.size(); i++) {
							if (cols[i].find(column))
								column = cols[i];
						}
					}
					//trova la condizione e continua
					condition = conditionToString(c[-1].value);
					value = stringToType(c->value, typeRetriever(c->value));
					if (sel == nullptr)
						sel = new Selection(*temp->get(column), condition, value);
					else if (sel != nullptr && c[-3].tag == TAG::AND)
						sel = new Selection((*sel && Selection(*temp->get(column), condition, value)));
					else if (sel != nullptr && c[-3].tag == TAG::OR)
						sel = new Selection((*sel || Selection(*temp->get(column), condition, value)));
				}
				//caso between
				else if (c->tag == TAG::BETWEEN) {
					TAG chainCond = c[-2].tag;
					column = c[-1].value;
					//standardizza il nome della colonna
					__normalizeColumn__(column);
					//continua
					c++;
					Type* valueA = stringToType(c->value, typeRetriever(c->value));
					c += 2;
					Type* valueB = stringToType(c->value, typeRetriever(c->value));
					if (chainCond == TAG::AND)
						sel = new Selection(*sel && Selection(*temp->get(column), valueA, valueB));
					else if (chainCond == TAG::OR)
						sel = new Selection(*sel || Selection(*temp->get(column), valueA, valueB));
					else
						sel = new Selection(*temp->get(column), valueA, valueB);
				}
				//caso like
				else if (c->tag == TAG::LIKE) {
					column = c[-1].value;
					//standardizza la colonna
					__normalizeColumn__(column);
					std::string likeval = c[1].value;
					if (sel == nullptr)
						sel = new Selection(*temp->get(column), likeval);
					else if (sel != nullptr && c[-2].tag == TAG::AND)
						sel = new Selection((*sel && Selection(*temp->get(column), likeval)));
					else if (sel != nullptr && c[-2].tag == TAG::OR)
						sel = new Selection((*sel || Selection(*temp->get(column), likeval)));
					c++;
				}
				//caso where tra due colonne
				else if (tables.size() > 1 && c->tag == TAG::COLUMN && c[-2].tag == TAG::COLUMN && c[-1].tag == TAG::CONDITION) {
					String col1 = c[-2].value;
					__normalizeColumn__(col1);
					Conditions condition = conditionToString(c[-1].value);
					String col2 = c->value;
					__normalizeColumn__(col2);
					if (sel == nullptr)
						sel = new Selection(*temp->get(col1),condition, *temp->get(col2));
					else if (sel != nullptr && c[-2].tag == TAG::AND)
						sel = new Selection((*sel && Selection(*temp->get(col1), condition, *temp->get(col2))));
					else if (sel != nullptr && c[-2].tag == TAG::OR)
						sel = new Selection((*sel || Selection(*temp->get(col1), condition, *temp->get(col2))));
				}
				c++;
			}
		}
		//ordina la tabella secondo la colonna desiderata
		if (c->tag == TAG::ORDER_BY) {
			String column;
			bool desc = false;
			while (c->tag != TAG::COMMAPOINT) {
				switch (c->tag)
				{
				case TAG::COLUMN:
					column = c->value;
					break;

				case TAG::DESC:
					desc = true;
					break;
				default:
					break;
				}
				c++;
			}
			temp = dynamic_cast<VectorizedTable*>(temp->sort(column, desc));
		}
		//applica le selezioni
		if (sel != nullptr)
			temp = dynamic_cast<VectorizedTable*>(temp->select(*sel));
		//stampa la tabella
		std::cout << temp->project(cols)->toString() << std::endl;
	}

	void SQLParser::parseCreate() {
		c++;
		std::string table = c->value;
		if (this->connected_db->exist(table))
			throw new Table::Bad_Table("Error table " + table + " already exists");
		this->connected_db->createTable(table);
		c += 2;
		std::string columnName;
		std::string columnType;
		bool not_null = false;
		bool auto_increment = false;
		while (c->tag != TAG::CLOSEBRACKET && c->tag != TAG::FOREIGN_KEY && c->tag != TAG::PRIMARY_KEY) {

			switch (c->tag)
			{
			case TAG::COLUMN:
				columnName = c->value;
				break;

			case TAG::TYPE:
				columnType = c->value;
				break;

			case TAG::AUTOINCREMENT:
				auto_increment = true;
				break;

			case TAG::NOT_NULL:
				not_null = true;
				break;


			case TAG::COMMA:
				if (columnName.empty())
					throw new Bad_Query("Name not specified near token " + c[-1].value);
				if (columnType.empty())
					throw new Bad_Query("Type not specified near token " + c[-1].value);
				if (auto_increment && (stringToType(columnType) != Types::INT)) {
					throw new Bad_Query("column of type " + columnType + " cannot be auto_increment");
				}
				this->connected_db->get(table)->createColumn(columnName, stringToType(columnType), not_null, auto_increment);
				not_null = false;
				auto_increment = false;
				columnName.clear();
				columnType.clear();
				break;
			default:
				break;
			}
			c++;
		}
		if (!columnName.empty()) {
			if (auto_increment && (stringToType(columnType) != Types::INT)) {
				throw new Bad_Query("column of type " + columnType + " cannot be auto_increment");
			}
			this->connected_db->get(table)->createColumn(columnName, stringToType(columnType), not_null, auto_increment);
		}
		while (c->tag != TAG::COMMAPOINT) {
			if (c->tag == TAG::PRIMARY_KEY) {
				std::vector<std::string> cols;
				c += 2;
				while (c->tag != TAG::CLOSEBRACKET) {
					if (c->tag == TAG::COLUMN)
						cols.push_back(c->value);
					c++;
				}
				for (int i = 0; i < cols.size(); i++) {
					if (!connected_db->get(table)->columnExist(cols[i]))
						throw new Bad_Query("Error column " + cols[i] + " not specified");
					connected_db->get(table)->get(cols[i])->setIndex(true);
				}
				c++;
			}
			else if (c->tag == TAG::FOREIGN_KEY) {
				std::vector<std::string> referenceColumns;
				std::string tableReferenced;
				std::vector<std::string> columnReferenced;
				c += 2;
				while (c->tag != TAG::CLOSEBRACKET) {
					if (c->tag == TAG::COLUMN)
						referenceColumns.push_back(c->value);
					c++;
				}
				c += 2;
				tableReferenced = c->value;
				__AssertTableExist__(tableReferenced);
				c += 2;
				while (c->tag != TAG::CLOSEBRACKET) {
					if (c->tag == TAG::COLUMN)
						columnReferenced.push_back(c->value);
					c++;
				}
				for (int i = 0; i < referenceColumns.size(); i++) {
					if (!connected_db->get(table)->columnExist(referenceColumns[i]))
						throw new Bad_Query("Error column " + referenceColumns[i] + " not specified");
				}
				for (int i = 0; i < columnReferenced.size(); i++) {
					if (!connected_db->get(tableReferenced)->columnExist(columnReferenced[i]))
						throw new Bad_Query("Error column " + columnReferenced[i] + " not specified");
				}
				connected_db->createExternalKey(table, tableReferenced, referenceColumns, columnReferenced);
			}
			c++;
		}
		std::cout << "Table " + table + " created succesfully" << std::endl;

	}
	




	void SQLParser::parseDelete()
	{
		Selection* sel = nullptr;
		std::string table;
		c++;
		table = c->value;
		__AssertTableExist__(table);
		c += 2;
		while (c->tag != TAG::COMMAPOINT)
		{
			std::string column;
			Conditions condition;
			Type* value;

			if (c->tag == TAG::VALUE) {
				column = c[-2].value;
				__AssertTableExist__(table);
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
				__AssertTableExist__(table);
				c++;
				Type* valueA = stringToType(c->value, typeRetriever(c->value));
				c += 2;
				Type* valueB = stringToType(c->value, typeRetriever(c->value));
				if (chainCond == TAG::AND)
					sel = new Selection(*sel && Selection(*this->connected_db->get(table)->get(column), valueA, valueB));
				else if (chainCond == TAG::OR)
					sel = new Selection(*sel || Selection(*this->connected_db->get(table)->get(column), valueA, valueB));
				else
					sel = new Selection(*this->connected_db->get(table)->get(column), valueA, valueB);
			}
			c++;
		}
   		if (sel != nullptr && sel->getPos().size() > 0) {
			try {
				this->connected_db->get(table)->removeRows(sel->getPos(), true);
			}
			catch (...) {
				return;
			}
		}
		std::cout << "Removed " + std::to_string(sel->getPos().size()) + " rows from table " + table;
	}

	void SQLParser::parseTruncate()
	{
		c++;
		std::string table = c->value;
		__AssertTableExist__(table);
		this->connected_db->get(table)->truncate();
		std::cout << "Table " + table + " Truncated" << std::endl;
	}

	void SQLParser::parseInsert() {
		c++;
		std::string table = c->value;
		__AssertTableExist__(table);
		std::vector<std::string> columns;
		std::vector<Type*> data;
		c += 2;
		while (c->tag != TAG::CLOSEBRACKET) {
			if (c->tag == TAG::COLUMN)
				columns.push_back(c->value);
			c++;
		}
		c += 3;
		while (c->tag != TAG::CLOSEBRACKET) {
			if (c->tag == TAG::VALUE)
				data.push_back(stringToType(c->value, typeRetriever(c->value)));
			else if (c->tag == TAG::NULLVALUE)
				data.push_back(new Null());
			c++;
		}
		c++;
		try {
			connected_db->get(table)->addRow(data, columns);
		}
		catch (...) { return; }
		std::cout << "Insert successfull" << std::endl;
	}

	void SQLParser::parseDrop()
	{
		c++;
		std::string table = c->value;
		__AssertTableExist__(table);
		this->connected_db->deleteTable(table);
		std::cout << "Table " + table + " Deleted";
	}


	void SQLParser::parseUpdate() {
		using namespace std;
		c++;
		string table = c->value;
		__AssertTableExist__(table);
		c += 2;
		vector<string> columns;
		vector<Type*> data;
		while (c->tag != TAG::WHERE && c->tag != TAG::COMMAPOINT) {
			if (c->tag == TAG::COLUMN)
				columns.push_back(c->value);
			else if (c->tag == TAG::VALUE)
				data.push_back(stringToType(c->value, typeRetriever(c->value)));
			c++;
		}
		string column;
		Conditions condition;
		Type* value;
		Selection* sel = nullptr;
		while(c->tag != TAG::COMMAPOINT) {
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
					sel = new Selection(*sel && Selection(*this->connected_db->get(table)->get(column), valueA, valueB));
				else if (chainCond == TAG::OR)
					sel = new Selection(*sel || Selection(*this->connected_db->get(table)->get(column), valueA, valueB));
				else
					sel = new Selection(*this->connected_db->get(table)->get(column), valueA, valueB);
			}
			else if (c->tag == TAG::LIKE) {
				column = c[-1].value;
				std::string likeval = c[1].value;
				if (sel == nullptr)
					sel = new Selection(*this->connected_db->get(table)->get(column), likeval);
				else if (sel != nullptr && c[-2].tag == TAG::AND)
					sel = new Selection((*sel && Selection(*this->connected_db->get(table)->get(column), likeval)));
				else if (sel != nullptr && c[-2].tag == TAG::OR)
					sel = new Selection((*sel || Selection(*this->connected_db->get(table)->get(column), likeval)));
				c++;
			}
			c++;
		}
		if(sel != nullptr)
			for (int i = 0; i < columns.size(); i++) {
				this->connected_db->get(table)->update(sel->getPos(), columns[i], data[i]);
			}
		else {
			for (int i = 0; i < columns.size(); i++) {
				auto positions = this->connected_db->get(table)->getColumns()[0]->getPositions();
				this->connected_db->get(table)->update(positions, columns[i], data[i]);
			}
		}
		cout << "Updated Table " + table <<endl;
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
		return Types::NOTVALID;
	}
}