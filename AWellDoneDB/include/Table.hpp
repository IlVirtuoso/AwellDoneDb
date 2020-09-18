#pragma once
#include "Types.hpp"
namespace WellDoneDB
{
    enum class Conditions
    {
        EQUAL,
        LESSTHAN,
        GREATHERTHAN,
        LESSEQTHAN,
        GREATEREQTHAN,
        NOT,
        BETWEEN
    };


    class Selection;

    class Column
    {
    private:
        std::vector<Pair<Type *, int>> data;
        Types type;
        std::string name;
        std::string tableName;
        /**
         * @brief Metodo di sort della tabella
         * @param left 
         * @param right 
        */

    public:
        /**
         * @brief Classe per le eccezioni della colonna
        */
        class Bad_Column : std::exception
        {
        public:
            std::string message;
            Bad_Column(std::string message) : message{message} { std::cout << "Column Exception:" << message << std::endl; }
        };
        bool not_null, index, autoincrement;
        /**
         * @brief Costruttore della colonna
         * @param name nome della colonna
         * @param tableName nome della tabella di appartenenza
         * @param type tipo della tabella tra i tipi dell'enumerazione supportati
         * @param elements vettore per il caricamento rapido di oggetti Type* preallocati
         * @param not_null 
         * @param index 
         * @param autoincrement 
        */
        Column(std::string name, std::string tableName, Types type, std::vector<Type *> elements = std::vector<Type *>{}, bool not_null = false, bool index = false, bool autoincrement = false);

        /**
         * @brief Costruttore rapido della colonna
         * @param name nome della colonna
         * @param tableName nome della tabella di appartenenza
         * @param type tipo della tabella tra i tipi dell'enumerazione supportati
         * @param elements vettore per il caricamento rapido di oggetti Pair<Type*,int> preallocati
         * @param not_null 
         * @param index 
         * @param autoincrement 
        */
        Column(std::string name, std::string tableName, Types type, std::vector<Pair<Type *, int>> elements = std::vector<Pair<Type *, int>>{}, bool not_null = false, bool index = false, bool autoincrement = false) : name{name}, tableName{tableName}, type{type}, data{elements}, index{index}, not_null{not_null}, autoincrement{autoincrement} {}

        /**
         * @brief Aggiunge un elemento alla colonna
         * @param data elemento da aggiungere
        */
        void add(Type &data);
        void add(Type *data);

        /**
         * @brief Imposta il valore index della colonna
         * @param value 
        */
        void inline setIndex(bool value) { this->index = value; }

        /**
         * @brief imposta il valore notNull della colonna
         * @param value 
        */
        void inline setNotNull(bool value) { this->not_null = value; }

        /**
         * @brief imposta il valore autoincrement della colonna
         * @param value 
        */
        void setAutoIncrement(bool value);

        /**
         * @brief Imposta la cella della colonna indicata con un nuovo valore
         * @param index numero della cella
         * @param newType nuovo dato
        */
        void set(int index, Type* newType);

        /**
         * @brief imposta il primo valore trovato uguale a oldData con il tipo indicato in newData
         * @param oldData dati da sostituire
         * @param newData nuovo dato
         * @return vero se la sostituzione é stata effettuata, falso altrimenti
        */
        bool set(Type* oldData, Type* newData);

        /**
         * @brief Rimuove tutte le righe uguali a data
         * @param data dato di comparazione
         * @param drop se true le righe verranno eliminate altrimenti verranno impostate a null
        */
        void remove(Type &data, bool drop = false);
        void remove(Type *data, bool drop = false);

        /**
         * @brief rimuove la riga indicata dall'indice
         * @param index numero della riga da rimuovere
         * @param drop se true le righe verranno eliminate altrimenti verranno impostate a null
        */
        void remove(int index, bool drop = false);

        /**
         * @brief Indica se un dato esiste nella colonna
         * @param data dato da cercare
         * @return true se esiste false altrimenti
        */
        bool exist(Type &data);

        /**
         * @brief Ritorna il tipo di dato contenuto nella colonna
         * @return 
        */
        Types inline getType() { return this->type; }

        /**
         * @brief ritorna il dato all'indice indicato
         * @param index numero di cella da puntare
         * @return un puntatore a un Type* generico
        */
        Type *at(int index);

        /**
         * @brief Ritorna un token con la posizione della colonna e il dato associato
         * @param index indice cercato
         * @return un token Pair<> con il dato e la posizione
        */
        Pair<Type*, int> get(int index) { return this->data[index]; }

        /**
         * @brief Operatore[] array style per spostarsi nella colonna
         * @param index indice cercato
         * @return un puntatore al tipo indicato
        */
        Type *operator[](int index);

        /**
         * @brief 
         * @return Ritorna un vettore di interi contenente le posizioni di tutti gli elementi presenti
        */
        std::vector<int> getPositions();

        /**
         * @brief
         * @return ritorna la colonna in formato stringa 
        */
        std::string toString();

        /**
         * @brief 
         * @return ritorna il nome della colonna 
        */
        std::string inline getName() { return this->name; }

        /**
         * @brief 
         * @return ritorna la dimensione del vettore che contiene i dati 
        */
        size_t inline getSize() { return this->data.size(); }

        /**
         * @brief ordina gli elementi all'interno della colonna in senso normalmente crescente
         * @param desc indica se si vogliono gli elementi in senso decrescente
        */
        void sort(bool desc = false);

        /**
         * @brief Riordina gli elementi in base al vettore orderVector
         * @param orderVector vettore di dimensione uguale al vettore dei dati contenente le posizioni dei singoli dati
         * ex. se l'array orderVector é {3,1,0,4} allora il primo elemento nella colonna diventerá l'elemento
         * 3 e l'ultimo l'elemento 4
        */
        void order(std::vector<int> orderVector);

        /**
         * @brief Converte la colonna in formato xml
         * @return stringa xml che contiene la colonna
        */
        std::string toXml();

        void inline clear() { this->data.clear(); }
    };

    
    /**
     * @brief Classe oggetto funzionale per la selezione degli elementi all'interno di una colonna.
     *ex.corrisponde all'operatore WHERE all'interno di SQL
    */
    class Selection {
    private:
        std::vector<Pair<Type*, int>> positions;
        Conditions condition;
        Type* dataCompare,* high, * low;
        Column* col;
        bool exist(Pair<Type*, int>& check);
    public:
        /**
         * @brief Costruttore della selezione per comparazione
         * @param col colonna su cui effettuare la selezione
         * @param condition condizione appartenente all'enum condition
         * @param dataCompare dato di comparazione
        */
        Selection(Column col, Conditions condition, Type* dataCompare);

        /**
         * @brief Costruttore della selezione per intervalli
         * @param col colonna su cui effettuare la selezione
         * @param lowInterval minimo dell'intervallo
         * @param highInterval massimo dell'intervallo
         * ex. corrisponde all'operatore BETWEEN in SQL
        */
        Selection(Column col, Type* lowInterval, Type* highInterval);

        /**
         * @brief Costruttore vuoto usato dagli operators
        */
        Selection(){}

        /**
         * @brief Operatore OR della selezione
         * @param sel selezione su cui effettuare l'OR
         * @return una nuova selezione che contiene gli elementi in OR con l'altra
        */
        Selection operator||(Selection sel);

        /**
         * @brief Operatore AND della selezione
         * @param sel selezione su cui effettuare l'AND
         * @return una nuova selezione in cui viene verificata se tutti gli elementi soddisfano le loro condizioni sulla stessa riga
        */
        Selection operator&&(Selection sel);

        /**
         * @brief 
         * @return Ritorna il vettore di posizionamento degli elementi 
        */
        std::vector<int> getPos();
    };



    /**
     * @brief Interfaccia pure per la descrizione delle tabelle
    */
    class Table
    {
    protected:
    virtual void loadColumn(Column* col) = 0;

    public:
        struct Reference {
            Table* tab;
            std::vector<std::string> reference;
            std::vector<std::string> referenced;
            explicit Reference(Table* tab, std::vector<std::string> referenceCols, std::vector<std::string> referencedCols) :
                tab{ tab }, reference{ referenceCols }, referenced{ referencedCols }{}
        };
        class Bad_Table : std::exception
        {
        public:
            std::string message;
            Bad_Table(std::string message) { std::cout << message << std::endl; }
        };
        enum class TableType {
            VECTORIZED
            //HASMAP coming soon
        };
        /**
         * @brief Crea una colonna all'interno della tabella 
         * @param columnName nome della colonna
         * @param columnType tipo della colonna
         * @param not_null 
         * @param autoincrement 
         * @param index 
        */
        virtual void createColumn(std::string columnName, Types columnType, bool not_null = false, bool autoincrement = false, bool index = false) = 0;

        /**
         * @brief Aggiunge una riga alla tabella
         * @param data vettori con i dati
         * @param columnNames vettore con i nomi delle colonne
         * @param check indica se eseguire il check sui dati inseriti
        */
        virtual void addRow(std::vector<Type*> data, std::vector<std::string> columnNames, bool check = true) = 0;

        /**
         * @brief Rimuove una riga dalla tabella
         * @param index indice da rimuovere
         * @param drop indica se rimuovere completamente la riga oppure se rendere tutti i valori nulli
        */
        virtual void removeRow(int index, bool drop = false) = 0;

        /**
         * @brief Rimuove una serie di righe dalla tabella
         * @param indexes vettore contenente gli indici
         * @param drop indica se rimuovere o rendere nulli i valori
        */
        virtual void removeRows(std::vector<int> indexes, bool drop = false) = 0;

        /**
         * @brief Cerca una colonna nella tabella
         * @param name nome della colonna
         * @return true se la colonna esiste false altrimenti
        */
        virtual bool columnExist(std::string name) = 0;

        /**
         * @brief Operatore[] in stile array associativo
         * @param columnName nome della colonna
         * @return ritorna una colonna se esiste
         * @throw se la tabella non esiste lancia un eccezzione
        */
        virtual Column* operator[](std::string columnName) = 0;

        /**
         * @brief ritorna una colonna all'indice selezionato
         * @param index indice da ritornare
         * @return un puntatore a una colonna
        */
        virtual Column* at(int index) = 0;

        /**
         * @brief Get esplicito dell'operatore[]
         * @param columnName nome della colonna
         * @return puntatore a una colonna
        */
        virtual Column* get(std::string columnName) { return (*this)[columnName]; }

        /**
         * @brief 
         * @return ritorna il nome della tabella 
        */
        virtual std::string getName() = 0;

        /**
         * @brief ritorna una riga per intero
         * @param index indice della riga da ritornare
         * @return un vettore di puntatori a Type* con i dati
        */
        virtual std::vector<Type*> getRow(int index) = 0;

        /**
         * @brief 
         * @param index indice della riga da ritornare 
         * @param columns nomi delle colonne di cui si vogliono i dati
         * @return ritorna un vettore di dati presi dalle colonne passate per parametro
        */
        virtual std::vector<Type*> getRow(int index, std::vector<std::string> columns) = 0;

        /**
         * @brief Cerca una riga contenente i dati passati per parametro nelle colonne 
         * @param  data vettore di dati
         * @param columns nomi delle colonne
         * @return true se esiste false altrimenti
        */
        virtual bool rowExist(std::vector<Type*>data , std::vector<std::string> columns) = 0;

        /**
         * @brief 
         * @param  dato di ritorno
         * @return Ritorna true se la riga passata per parametro esiste false altrimenti 
        */
        virtual bool rowExist(std::vector<Type*> data) = 0;

        /**
         * @brief 
         * @return Ritorna il numero di colonne presente nella tabella 
        */
        virtual size_t getColNum() = 0;

        /**
         * @brief copia lo scheletro di un'altra tabella, senza cioé i dati contenuti
         * @param table tabella da copiare
        */
        virtual void copy(Table& table) = 0;

        /**
         * @brief Operatore[] in stile array
         * @param index indice della riga da ritornare
         * @return vettore contente i dati della riga scelta
        */
        virtual std::vector<Type*> operator[](int index) = 0;

        /**
         * @brief Converte la tabella in un formato leggibile 
         * @return una stringa di cui fare l'output
        */
        virtual std::string toString() = 0;

        /**
         * @brief Ritorna un vettore contenete i nomi delle colonne
         * @return 
        */
        virtual std::vector<std::string> getColNames() = 0;

        /**
        * @brief imposta la tabella a cui referenzia
        */
        virtual void setReference(Reference * ref) = 0;

        /**
         * @brief Rimuove la referenza per la tabella 
        */
        virtual void unsetReference() = 0;

        /**
         * @brief rimuove una referenza
         * @param TableName nome della tabella che referenziava
        */
        virtual void removeReferenced(std::string TableName) = 0;

        /**
         * @brief aggiunge una referenza alla tabella
         * @param ref struttura contenente la referenza
        */
        virtual void addReferenced(Reference * ref) = 0;

        /**
         * @brief Ritorna una nuova tabella conforme alla selezione
         * @param sel oggetto Selection usato per la selezione
         * @return una nuova tabella
        */
        virtual Table* select(Selection& sel) = 0;

        /**
         * @brief ritorna un vettore con tutte le colonne
         * @return 
        */
        virtual std::vector<Column*> getColumns() = 0;

        /**
         * @brief Ritorna un vettore con tutte le colonne
         * @param columns nomi delle colonne da ritornare
         * @return 
        */
        virtual std::vector<Column*> getColumns(std::vector<std::string> columns) = 0;

        /**
         * @brief Esegue una proiezione della tabella
         * @param columns nomi delle colonne
         * @return una nuova tabella con le colonne selezionate
        */
        virtual Table* project(std::vector<std::string> columns) = 0;

        /**
         * @brief Ritorna il tipo della tabella tra quelle previste nell'enum TableType
         * @return 
        */
        virtual TableType getTableType() = 0;

        /**
         * @brief Ordina la tabella in senso crescente o decrescente
         * @param columnName nome della colonna a cui fare riferimento
         * @param desc 
         * @return 
        */
        virtual Table * sort(std::string columnName,bool desc = false) = 0;

        /**
         * @brief Operator stream >> copia la tabella nella tabella selezionata inclusi i dati
         * @param table tabella da riempire
        */
        virtual void operator>>(Table* table) = 0;

        /**
         * @brief Converte la tabella in modo che possa salvata su un file xml
         * @return una stringa xml
        */
        virtual std::string toXml() = 0;

        /**
         * @brief Carica l'xml dal file nomeTabella.table
        */
        virtual void loadXml() = 0;

        /**
         * @brief imposta le posizioni indicate nella colonna indicata col nuovo dato
         * @param positions vettore di interi per le posizioni
         * @param columnName nome della colonna
         * @param newData nuovo dato
        */
        virtual void update(std::vector<int> positions, std::string columnName, Type* newData) = 0;

        /**
         * @brief Svuota la tabella del suo contenuto
        */
        virtual void truncate() = 0;
    };


    /**
     * @brief Specializzazione della classe Table
     * la tabella in questo caso é rappresentata come vettore di colonne
    */
    class VectorizedTable : public Table
    {
    private:
        Reference* references;
        std::vector<Reference*> referenced;
        std::string name;
        int getMaxSize();
        std::vector<Column*> columns;
    protected:
        void loadColumn(Column* column);
    public:
        VectorizedTable(std::string name, std::vector<Column*> cols = std::vector<Column*>{});
        void createColumn(std::string columnName, Types columnType, bool not_null = false, bool autoincrement = false, bool index = false) override;
        void addRow(std::vector<Type*> data, std::vector<std::string> columnNames, bool check = true) override;
        void addRow(std::vector<Type*> data);
        void removeRow(int index, bool drop = false) override;
        bool columnExist(std::string name) override;
        void removeRows(std::vector<int> indexes, bool drop = false) override;
        std::vector<Type*> getRow(int index) override;
        Column* operator[](std::string columnName) override;
        std::vector<Type*> operator[](int index) override { return getRow(index); }
        std::string getName() { return this->name; }
        size_t getColNum() { return this->columns.size(); }
        inline Column* at(int index) { return this->columns[index]; }
        VectorizedTable operator*(Table& table);
        void copy(Table& table) override;
        std::vector<std::string> getColNames() override;
        std::string toString() override;
        Table * select(Selection& sel) override;
        std::vector<Type*> getRow(int index, std::vector<std::string> columns);
        bool rowExist(std::vector<Type*> data, std::vector<std::string> columns);
        bool rowExist(std::vector<Type*> data);
        void setReference(Reference * ref);
        void unsetReference();
        void removeReferenced(std::string TableName);
        void addReferenced(Reference * ref);
        inline TableType getTableType() { return Table::TableType::VECTORIZED; }
        inline std::vector<Column*> getColumns() { return this->columns; }
        std::vector<Column*> getColumns(std::vector<std::string> colNames);
        inline Table* project(std::vector<std::string> columns) { return new VectorizedTable("projection of: " + this->name, this->getColumns(columns)); }
        Table* sort(std::string columnName ,bool desc = false);
        void operator>>(Table* table);
        std::string toXml();
        void loadXml();
        void update(std::vector<int> positions, std::string columnName, Type* newData);
        void truncate();
    };
    
    /**
     * @brief Converte una stringa in un enumerazione types
     * @param  
     * @return 
    */
    Types stringToType(std::string);

    /**
     * @brief converte un valore stringa in un tipo definito da type 
     * @param value stringa con il valore
     * @param type tipo dell'oggetto da creare
     * @return un puntatore a un Type*
    */
    Type* stringToType(std::string value, Types type);

    /**
     * @brief Converte una stringa in una condizione dell'enumerazione condizioni
     * @param  
     * @return 
    */
    Conditions conditionToString(std::string);
} // namespace WellDoneDB
