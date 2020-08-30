#include<fstream>
#include "../include/Database.hpp"

namespace WellDoneDB{
	void Database::createTable(std::string tableName, Table::TableType allocMethod) {
		switch (allocMethod)
		{
		case WellDoneDB::Table::TableType::VECTORIZED:
			this->tables.insert(std::make_pair(tableName, new VectorizedTable(tableName)));
			break;
		default:
			break;
		}
	}


	void Database::createExternalKey(
		std::string tableName,
		std::string referencedTableName,
		std::vector<std::string> tableColumn,
		std::vector<std::string> referencedTableColumn) 
	{
		Table* ref = this->get(tableName);
		Table* referenced = this->get(referencedTableName);
		std::vector<Column*> refCols = ref->getColumns(tableColumn);
		std::vector<Column*> referencedCols = referenced->getColumns(referencedTableColumn);
		this->keyReferences.push_back(new ExternKey(ref, referenced, refCols, referencedCols));
		ref->setReference(new Table::Reference(referenced, tableColumn, referencedTableColumn));
	}

	std::vector<std::string> Database::getTableNames() {
		std::vector<std::string> names;
		std::map<std::string,Table*>::iterator map = this->tables.begin();
		while (map != this->tables.end()) {
			names.push_back(map->first);
			map++;
		}
		return names;
	}

	void Database::deleteExternalKey(std::string tableName, std::string referencedTable) {
		this->get(tableName)->unsetReference();
		this->get(referencedTable)->removeReferenced(tableName);
	}
}