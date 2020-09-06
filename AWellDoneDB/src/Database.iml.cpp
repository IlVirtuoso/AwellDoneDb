#include<fstream>
#include "../include/XmlParser.hpp"
#include "../include/Database.hpp"

namespace WellDoneDB{
	void Database::createTable(std::string tableName, Table::TableType allocMethod) {
		switch (allocMethod)
		{
		case WellDoneDB::Table::TableType::VECTORIZED:
			this->tables.insert(std::make_pair(tableName, new VectorizedTable(tableName)));
			this->tableNames.push_back(tableName);
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

	std::vector<std::string> Database::getReferenced(std::string tableName)
	{
		std::vector<std::string> tables;
		for (int i = 0; i < keyReferences.size(); i++) {
			if (keyReferences[i]->reference->getName() == tableName) {
				tables.push_back(keyReferences[i]->reference->getName());
			}
		}
		return tables;
	}

	std::vector<std::string> Database::getReference(std::string tableName)
	{
		std::vector<std::string> tables;
		for (int i = 0; i < keyReferences.size(); i++) {
			if (keyReferences[i]->table->getName() == tableName) {
				tables.push_back(keyReferences[i]->reference->getName());
			}
		}
		return tables;
	}


	void Database::deleteExternalKey(std::string tableName, std::string referencedTable) {
		this->get(tableName)->unsetReference();
		this->get(referencedTable)->removeReferenced(tableName);
		for (int i = 0; i < this->keyReferences.size(); i++) {
			if (this->keyReferences[i]->table->getName() == tableName && this->keyReferences[i]->reference->getName() == referencedTable)
				this->keyReferences.erase(this->keyReferences.begin() + i);
		}
	}

	void Database::deleteTable(std::string tableName)
	{
		if (!this->getReferenced(tableName).empty()) {
			throw new Bad_Database("Error this table contain column referenced");
		}
		if (!this->getReference(tableName).empty())
			for (int i = 0; i < this->getReference(tableName).size(); i++) {
				this->deleteExternalKey(tableName, this->getReference(tableName)[i]);
			}
		this->get(tableName)->unsetReference();
		this->tables.erase(tableName);
	}

	void Database::loadTable(Table* tab)
	{
		this->tables.insert(std::make_pair(tab->getName(), tab));
	}

	std::string Database::toXml() {
		std::string xml("<Database>\n");
		xml += "<name>" + this->name + "</name>\n";
		xml += "<tables>\n";
		std::vector<std::string> names = this->getTableNames();
		for (int i = 0; i < this->tables.size(); i++) {
			xml += "<table>" + names[i] + "</table>\n";
		}
		xml += "</tables>\n";
		xml += "<externalKeys>\n";
		for (int i = 0; i < this->keyReferences.size(); i++) {
			xml += "<externalKey>\n";
			xml += "<referenceTable>" + this->keyReferences[i]->table->getName() + "</referenceTable>\n";
			xml += "<referencedTable>" + this->keyReferences[i]->reference->getName() + "</referencedTable>\n";
			for (int k = 0; k < this->keyReferences[i]->columnRefer.size(); k++) //genera un eccezione
				xml += "<referenceColumn>" + this->keyReferences[i]->columnRefer[k]->getName() + "</referenceColumn>\n";
			for (int k = 0; k < this->keyReferences[i]->columnReferenced.size(); k++)
				xml += "<referencedColumn>" + this->keyReferences[i]->columnReferenced[k]->getName() + "</referencedColumn>\n";
			xml += "</externalKey>\n";
		}
		xml += "</externalKeys>\n";
		xml += "</Database>\n";
		return xml;
	}

	void Database::init() {
		std::ifstream stream(this->name + ".DB");
		if (!stream.is_open()) {
			std::ofstream created(this->name + ".DB",std::ofstream::out);
			created.close();
		}
	}


	void Database::save() {
		std::ofstream stream(this->name + ".DB");
		stream.write(this->toXml().c_str(),this->toXml().size());
		for (int i = 0; i < this->getTableNames().size(); i++) {
			std::ofstream tablestream(this->getTableNames()[i] + ".table");
			tablestream.write(this->get(this->getTableNames()[i])->toXml().c_str(), this->get(this->getTableNames()[i])->toXml().size());
			tablestream.close();
		}
		stream.close();
	}

	void Database::loadXml() {
		std::ifstream file(this->name + ".DB", std::ifstream::in);
		std::string xml;
		while (file.good()) {
			xml += file.get();
		}
		if (xml[0] == -1)
			return;
		XmlParser parser(xml);
		auto token = parser.begin();
		token++;
		while (token != parser.end()) {
			if(token->tag == "name"){
				this->name = token->value;
				token++;
			}
			if (token->tag == "tables") {
				token++;
				while (token->tag != "tables") {
					this->createTable(token->value); //this method has to be edited if another type of table would be implemented
					token++;
				}
				for (int i = 0; i < this->getTableNames().size(); i++) {
					this->get(this->getTableNames()[i])->loadXml();
				}
			}
			else if (token->tag == "externalKeys") {
				token++;
				std::string table;
				std::string referencedTable;
				std::vector<std::string> referenceColumn;
				std::vector<std::string> referencedColumns;
				while (token->tag != "externalKeys") {
					if (token->tag == "externalKey") { //lancia un eccezione
						if (!table.empty()) {
							this->createExternalKey(table, referencedTable, referenceColumn, referencedColumns);
						}
						table.clear();
						referencedTable.clear();
						referenceColumn.clear();
						referencedColumns.clear();
					}
					else if (token->tag == "referenceTable")
						table = token->value;
					else if (token->tag == "referencedTable")
						referencedTable = token->value;
					else if (token->tag == "referenceColumn")
						referenceColumn.push_back(token->value);
					else if (token->tag == "referencedColumn")
						referencedColumns.push_back(token->value);
					token++;
				}
			}
			token++;
		}
	}
}