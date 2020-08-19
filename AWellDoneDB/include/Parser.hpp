#pragma once
/*
 * Classe atta ad ospitare i dati ottenuti
 * dal parsing di una condizione attrName = value
 */
class parsedCondition {
public:
	string attrName;
	string value;
	parsedCondition(string attrName, string name);
};


class Parser
{
	/*
	 * Vettore che ospita tutte le condizioni espresse
	 * in una query. Tale vettore viene svuotato prima 
	 * che il Parser inizi la scansione della query.
	 */
	vector<parsedCondition> parsedConditions;

	/*
	 * Vettore che ospita tutte le foreign key
	 * espresse in una query CREATE.
	 */
	vector<ExternKey> tempkeys;
	
	/*
	 * Vettore che contiene tutti gli attributi
	 * selezionati per la stampa in una query SELECT.
	 */
	vector<string> selected;
	
	/*
	 * Tabella temporanea di supporto costruita durante 
	 * il parsing di una query CREATE.
	 */
	Table* temporary;

	/*
	 * Puntatore alla struttura database in uso dal programma.
	 */
	Database* db;
	
	/*
	 * Struttura di supporto usata durante il parsing di una 
	 * query CREATE atta a memorizzare le caratteristiche
	 * di ogni colonna dichiarata.
	 */
	Column* col;

	/*
	 * Restituisce True se l'attributo attribute esiste nella
	 * tabella tab.
	 */
	bool checkAttribute(string attribute, string tab);
	
	/*
	 * Restituisce True se la tabella table esiste in db.
	 */
	bool checkTable(string tableName);


	/*
	 * Controlla se l'attributo attribute esiste nella tabella
	 * provvisoria table.
	 */
	bool checkAttrTemp(string attribute, string table);

	public:

		/*
		 * Scorre l'offset p fino al prossimo carattere di s
		 * diverso da " ", "\n" e "\t".
		 */
		int skipSpace(string s, int p);

		/*
	     * Metodo di parsing delle condizioni che compaiono
	     * in forma attributo = valore.
	     * Ritorna l'offset successivo alla condizione parsata
		 * Il metodo crea una struttura ParsedCondition e
	     * la aggiunge al vettore ParsedConditions.
		 */
		int parseCondition(string query, int p, string tab);
		
		/*
		 * Metodo di parsing di una query CREATE
		 * Il parsing è strutturato ricalcando un 
		 * automa a stati finiti costituito
		 * dai seguenti stati:
		 *	0 - Nome della tabella
		 * 	1 - Nome della prima colonna
		 * 	2 - Tipo do dato da associare ad una colonna
		 *	3 - Vincoli da associare ad una colonna
		 *	4 - Nomi colonne successive alla prima/primary key
		 * 	5 - Foreign key
		 *	6 - Caratteri di terminazione della query
		 * Al termine del parsing aggiunge una tabella al database
		 */
		bool parseCreate(string query);
		
		/*
		 * Metodo di parsing di una query DROP
		 * Al termine del parsing cancella la tabella 
		 * indicata dal database
		 */
		bool parseDrop(string query);
		
		/*
		 * Metodo di parsing di una query INSERT
		 * Al termine del parsing inserisce nella tabella 
		 * indicata una nuova riga
		 */
		bool parseInsert(string query);
		
		/*
		 * Metodo di parsing di una query DELETE
		 * Al termine del parsing cancella le righe 
		 * in cui viene soddisfatta la condizione WHERE
		 * dalla tabella indicata
		 */
		bool parseDelete(string query);
		
		/*
		 * Metodo di parsing di una query TRUNCATE
		 * Al termine del parsing svuota la tabella 
		 * indicata
		 */
		bool parseTruncate(string query);
		
		/*
		 * Metodo di parsing di una query UPDATE
		 * Al termine del parsing aggiorna la tabella 
		 * indicata con i nuovi valori indicati nelle 
		 * condizioni SET dove la condizione WHERE
		 * è soddisfatta
		 */
		bool parseUpdate(string query);
		
		/*
		 * Metodo di parsing di una query SELECT
		 * Il metodo prevede il parsing sia per SELECT
		 * semplici che con clausole WHERE ed ORDER BY.
		 */
		bool parseSelect(string query);
		
		/*
		 * Metodo di parsing di una query QUIT.
		 * Al termine del parsing salva la struttura 
		 * database e termina il programma
		 */
		bool parseQuit(string query);
		
		/*
		 * Metodo che sceglie il tipo 
		 * della query in input
		 */
		bool scanner(string query);
		
		/* Costruttore */
		Parser(Database* db);
};

