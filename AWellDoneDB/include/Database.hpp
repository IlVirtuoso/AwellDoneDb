#pragma once
#include "Table.hpp"
#include <exception>
#include <map>
namespace WellDoneDB {

	struct ExternKey {
		Table* table;
		Table* reference;
		std::vector<Column*> columnRefer;
		std::vector<Column*> columnReferenced;
		explicit ExternKey(Table* table, Table* reference, std::vector<Column*> columnRefer, std::vector<Column*> referenced)
			: table{ table }, reference{ reference }, columnRefer{ columnRefer }, columnReferenced{ referenced }{}
	};

	class Database {
	private:
		std::string name;
		std::map<std::string,Table*> tables;
		std::vector<ExternKey*> keyReferences;
		void init();
	public:
		class Bad_Database {
		public:
			std::string message;
			Bad_Database(std::string message) : message{ message } { std::cout << "Exception Database:" + message << std::endl; }
		};
		Database(std::string name = std::string{ "default" }) : name{ name } { init(); }
		Database(std::string name, std::map<std::string,Table*> tables, std::vector<ExternKey>)
			:name{ name }, tables{ tables }, keyReferences{ keyReferences }{init(); }
		Table* operator[](std::string tableName) { return this->tables[tableName];}
		Table* get(std::string tableName) { return this->tables[tableName]; }
		bool inline exist(std::string tableName) { return this->tables.find(tableName) != this->tables.end(); }
		void createTable(std::string tableName, Table::TableType allocMethod = Table::TableType::VECTORIZED);
		void createExternalKey(std::string tableName,
			std::string referencedTableName,
			std::vector<std::string> tableColumn, 
			std::vector<std::string> referencedTableColumn);
		std::vector<std::string> getReferenced(std::string tableName);
		std::vector<std::string> getReference(std::string tableName);
		std::vector<std::string> getTableNames();
		void deleteExternalKey(std::string tableName, std::string referencedTable);
		void deleteTable(std::string tableName);
		void loadTable(Table* tab);
		std::string toXml();
		void loadXml();
		void save();
	};

}