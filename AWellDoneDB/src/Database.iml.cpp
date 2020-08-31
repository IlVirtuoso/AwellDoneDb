#include<fstream>
#include "../include/XmlParser.hpp"
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
			for (int k = 0; k < this->keyReferences[i]->columnRefer.size(); i++)
				xml += "<referenceColumn>" + this->keyReferences[i]->columnRefer[k]->getName() + "</referenceColumn>\n";
			for (int k = 0; k < this->keyReferences[i]->columnReferenced.size(); i++)
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
		std::string file;
		char buffer[1024];
		while (stream.good()) {
			stream.getline(buffer, 1024);
			file += buffer;
		}
		XmlParser parser(file);
		auto token = parser.begin();
	}

	void Database::loadXml() {
		std::ifstream file(this->name + ".DB", std::ifstream::in);
		std::string xml;
		while (file.good()) {
			xml += file.get();
		}
		XmlParser parser(xml);
		auto token = parser.begin();
		while (token != parser.end()) {
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
				while (token->tag != "externalKeys") {
					token++;
					std::string table;
					std::string referencedTable;
					std::vector<std::string> referenceColumn;
					std::vector<std::string> referencedColumns;
					if (token->tag == "externalKey") {
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