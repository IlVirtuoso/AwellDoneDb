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
        void _typeQuickSort(int left, int right);

    public:
        class Bad_Column : std::exception
        {
        public:
            std::string message;
            Bad_Column(std::string message) : message{message} { std::cout << "Column Exception:" << message << std::endl; }
        };
        bool not_null, index, autoincrement;
        Column(std::string name, std::string tableName, Types type, std::vector<Type *> elements = std::vector<Type *>{}, bool not_null = false, bool index = false, bool autoincrement = false);
        Column(std::string name, std::string tableName, Types type, std::vector<Pair<Type *, int>> elements = std::vector<Pair<Type *, int>>{}, bool not_null = false, bool index = false, bool autoincrement = false) : name{name}, tableName{tableName}, type{type}, data{elements}, index{index}, not_null{not_null}, autoincrement{autoincrement} {}
        void add(Type &data);
        void add(Type *data);
        void inline setIndex(bool value) { this->index = value; }
        void inline setNotNull(bool value) { this->not_null = value; }
        void setAutoIncrement(bool value);
        void set(int index, Type* newType);
        bool set(Type* oldData, Type* newData);
        void remove(Type &data, bool drop = false);
        void remove(Type *data, bool drop = false);
        void remove(int index, bool drop = false);
        bool exist(Type &data);
        Types inline getType() { return this->type; }
        Type *at(int index);
        Pair<Type*, int> get(int index) { return this->data[index]; }
        Type *operator[](int index);
        std::vector<int> getPositions();
        std::string toString();
        std::string inline getName() { return this->name; }
        int inline getSize() { return this->data.size(); }
        void sort(bool desc = false);
        void order(std::vector<int> orderVector);
        std::string toXml();
    };

    
    class Selection {
    private:
        std::vector<Pair<Type*, int>> positions;
        Conditions condition;
        Type* dataCompare,* high, * low;
        Column* col;
        bool exist(Pair<Type*, int>& check);
    public:
        Selection(Column col, Conditions condition, Type* dataCompare);
        Selection(Column col, Type* lowInterval, Type* highInterval);
        Selection(){}
        Selection operator||(Selection sel);
        Selection operator&&(Selection sel);
        std::vector<int> getPos();
    };




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
        virtual void createColumn(std::string columnName, Types columnType, bool not_null = false, bool autoincrement = false, bool index = false) = 0;
        virtual void addRow(std::vector<Type*> data, std::vector<std::string> columnNames, bool check = true) = 0;
        virtual void removeRow(int index, bool drop = false) = 0;
        virtual void removeRows(std::vector<int> indexes, bool drop = false) = 0;
        virtual bool columnExist(std::string name) = 0;
        virtual Column* operator[](std::string columnName) = 0;
        virtual Column* at(int index) = 0;
        virtual Column* get(std::string columnName) { return (*this)[columnName]; }
        virtual std::string getName() = 0;
        virtual std::vector<Type*> getRow(int index) = 0;
        virtual std::vector<Type*> getRow(int index, std::vector<std::string> columns) = 0;
        virtual bool rowExist(std::vector<Type*>, std::vector<std::string> columns) = 0;
        virtual bool rowExist(std::vector<Type*>) = 0;
        virtual int getColNum() = 0;
        virtual void copy(Table& table) = 0;
        virtual std::vector<Type*> operator[](int index) = 0;
        virtual std::string toString() = 0;
        virtual std::vector<std::string> getColNames() = 0;
        virtual void setReference(Reference * ref) = 0;
        virtual void unsetReference() = 0;
        virtual void removeReferenced(std::string TableName) = 0;
        virtual void addReferenced(Reference * ref) = 0;
        virtual Table* select(Selection& sel) = 0;
        virtual std::vector<Column*> getColumns() = 0;
        virtual std::vector<Column*> getColumns(std::vector<std::string> columns) = 0;
        virtual Table* project(std::vector<std::string> columns) = 0;
        virtual TableType getTableType() = 0;
        virtual Table * sort(std::string columnName,bool desc = false) = 0;
        virtual void operator>>(Table* table) = 0;
        virtual std::string toXml() = 0;
        virtual void loadXml() = 0;
        virtual void update(std::vector<int> positions, std::string columnName, Type* newData) = 0;
        virtual void truncate() = 0;
    };

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
        int getColNum() { return this->columns.size(); }
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
    
    Types stringToType(std::string);
    Type* stringToType(std::string value, Types type);
    Conditions conditionToString(std::string);
} // namespace WellDoneDB
