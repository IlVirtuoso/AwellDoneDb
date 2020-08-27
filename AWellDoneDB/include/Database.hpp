#pragma once
#include "Table.hpp"
#include <exception>
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
		std::vector<Table*> tables;
		std::vector<ExternKey> keyReferences;
	public:
		class Bad_Database {
		public:
			std::string message;
			Bad_Database(std::string message) : message{ message } { std::cout << "Exception Database:" + message << std::endl; }
		};
		Database(std::string name);
		Database(std::string name, std::vector<Table*> tables, std::vector<ExternKey>)
			:name{name},tables{tables},keyReferences{keyReferences}{}
		Table* operator[](std::string tableName);
		void createTable(std::string tableName);
		void createExternalKey(std::string tableName, std::string referencedTableName, std::vector<std::string> tableColumn, std::vector<std::string> referencedTableColumn);
	};
}