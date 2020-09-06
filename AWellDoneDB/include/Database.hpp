#pragma once
#include "Table.hpp"
#include <exception>
#include <map>
namespace WellDoneDB {

	/**
	 * @brief Struttura per specificare le chiavi esterne
	*/
	struct ExternKey {
		Table* table; 
		Table* reference; 
		std::vector<Column*> columnRefer; 
		std::vector<Column*> columnReferenced; 

		/**
		 * @brief Costruttore della struttura
		 * @param table tabella che referenzia
		 * @param reference tabella da referenziare
		 * @param columnRefer colonne di table che referenziano
		 * @param referenced colonne di reference che vengono referenziate
		*/
		explicit ExternKey(Table* table, Table* reference, std::vector<Column*> columnRefer, std::vector<Column*> referenced)
			: table{ table }, reference{ reference }, columnRefer{ columnRefer }, columnReferenced{ referenced }{}
	};

	class Database {
	private:
		std::string name;
		std::map<std::string,Table*> tables;
		std::vector<std::string> tableNames;
		std::vector<ExternKey*> keyReferences;
		/**
		 * @brief Metodo per l'inizializzazione del database
		*/
		void init();
	public:
		/**
		 * @brief Classe per la gestione delle eccezioni del database
		*/
		class Bad_Database {
		public:
			std::string message;
			Bad_Database(std::string message) : message{ message } { std::cout << "Exception Database:" + message << std::endl; }
		};

		/**
		 * @brief Costruttore del database
		 * @param name nome del database
		*/
		Database(std::string name = std::string{ "default" }) : name{ name } { init(); }

		/**
		 * @brief Costruttore rapido per database con parametri preformati
		 * @param name nome del database
		 * @param tables vettore di tabelle pre allocate
		 * @param externKeys vettore di chiavi esterne preDichiarate
		*/
		Database(std::string name, std::map<std::string,Table*> tables, std::vector<ExternKey> externKeys)
			:name{ name }, tables{ tables }, keyReferences{ keyReferences }{init(); }

		/**
		 * @brief Operatore di navigazione in stile array associativo
		 * @param tableName nome della tabella da recuperare
		 * @return puntatore alla tabella se esiste altrimenti nullptr
		*/
		Table* operator[](std::string tableName) { return this->tables[tableName];}
		/**
		 * @brief Getter esplicito dell'operatore [] 
		 * @param tableName nome della tabella da recuperare
		 * @return un puntatore alla tabella se esiste altrimenti nullptr
		*/
		Table* get(std::string tableName) { return this->tables[tableName]; }

		/**
		 * @brief Metodo per la ricerca di una tabella
		 * @param tableName nome della tabella da cercare
		 * @return true se la tabella esiste, false altrimenti
		*/
		bool inline exist(std::string tableName) { return this->tables.find(tableName) != this->tables.end(); }

		/**
		 * @brief Crea una tabella all'interno del database
		 * @param tableName nome della tabella da cercare
		 * @param allocMethod metodo di allocamento della tabella
		*/
		void createTable(std::string tableName, Table::TableType allocMethod = Table::TableType::VECTORIZED);

		/**
		 * @brief Crea una chiave esterna passando i parametri richiesti dalla struttura
		 * @param tableName nome della tabella che referenzia
		 * @param referencedTableName nome della tabella da referenziare
		 * @param tableColumn colonne in tableName che referenziano referencedTableName
		 * @param referencedTableColumn colonne referenziate
		*/
		void createExternalKey(std::string tableName,
			std::string referencedTableName,
			std::vector<std::string> tableColumn, 
			std::vector<std::string> referencedTableColumn);

		/**
		 * @brief Ritorna un array contenente le tabelle che referenziano tableName
		 * @param tableName nome della tabella
		 * @return 
		*/
		std::vector<std::string> getReferenced(std::string tableName);

		/**
		 * @brief ritorna la tabella referenziata da tableName
		 * @param tableName nome della tabella
		 * @return 
		*/
		std::vector<std::string> getReference(std::string tableName);

		/**
		 * @brief Ritorna un array contenente i nomi di tutte le tabelle
		 * @return 
		*/
		std::vector<std::string> getTableNames() { return this->tableNames; }

		/**
		 * @brief Elimina la chiave esterna associata alla due tabelle
		 * @param tableName nome della tabella che referenzia
		 * @param referencedTable nome della tabella referenziata
		*/
		void deleteExternalKey(std::string tableName, std::string referencedTable);

		/**
		 * @brief elimina la tabella 
		 * @param tableName nome della tabella da eliminare
		*/
		void deleteTable(std::string tableName);

		/**
		 * @brief Caricatore rapido per tabelle pre allocate
		 * @param tab puntatore alla tabella pre allocata
		*/
		void loadTable(Table* tab);

		/**
		 * @brief Converte il database in un formato xml scrivibile su file
		 * @return una stringa xml
		*/
		std::string toXml();

		/**
		 * @brief Cerca e carica il file xml che contiene il database, il nome del file é nome+".db"
		*/
		void loadXml();

		/**
		 * @brief Metodo di salvataggio del file del database e delle sue tabelle
		*/
		void save();
	};

}