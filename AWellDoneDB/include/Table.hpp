
#pragma once
#include <map>
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
        Column *reference;
        std::vector<Column *> referenced;
        std::string name;
        std::string tableName;

    public:
        class Bad_Column : exception
        {
        public:
            std::string message;
            Bad_Column(std::string message) : message{message} { std::cout << "Column Exception:" << message << std::endl; }
        };
        bool not_null, index, autoincrement;
        Column(std::string name, std::string tableName, Types type, std::vector<Type *> elements = std::vector<Type *>{}, bool not_null = false, bool index = false, bool autoincrement = false, Column *references = nullptr, std::vector<Column *> referenced = std::vector<Column *>{});
        Column(std::string name, std::string tableName, Types type, std::vector<Pair<Type *, int>> elements = std::vector<Pair<Type *, int>>{}, bool not_null = false, bool index = false, bool autoincrement = false, Column *references = nullptr, std::vector<Column *> referenced = std::vector<Column *>{}) : name{name}, tableName{tableName}, type{type}, data{elements}, index{index}, not_null{not_null}, autoincrement{autoincrement}, reference{references}, referenced{referenced} {}
        void add(Type &data);
        void add(Type *data);
        bool removable(Type &data);
        bool removable(int index);
        void remove(Type &data, bool drop = false);
        void remove(Type *data, bool drop = false);
        void remove(int index, bool drop = false);
        bool exist(Type &data);
        void inline addReferenced(Column *column) { referenced.push_back(column); }
        void inline setReference(Column *column) { reference = column; }
        Types inline getType() { return this->type; }
        Type *at(int index);
        Pair<Type*, int> get(int index) { return this->data[index]; }
        Type *operator[](int index);
        std::vector<int> getPositions();
        std::string toString();
        std::string inline getName() { return this->name; }
        int inline getSize() { return this->data.size(); }
    };

    /**
     * @brief 
     * @TODO implement between operator
     * 
     */
    class Selection {
    private:
        std::vector<Pair<Type*, int>> positions;
        Conditions condition;
        Type* dataCompare,* high, * low;
        Column* col;
        bool exist(Pair<Type*, int>& check);
    public:
        Selection(Column& col, Conditions condition, Type* dataCompare);
        Selection(Column& col, Type* lowInterval, Type* highInterval);
        Selection(){}
        Selection operator||(Selection& sel);
        Selection operator&&(Selection& sel);
        std::vector<int> getPos();
        
    };

    class Table
    {
    
    protected:
    virtual void loadColumn(Column* col) = 0;
    public:
        class Bad_Table
        {
        public:
            std::string message;
            Bad_Table(std::string message) { std::cout << message << std::endl; }
        };
        virtual void createColumn(std::string columnName, Types columnType, bool not_null = false, bool autoincrement = false, bool index = false) = 0;
        virtual void addRow(std::vector<Type*> data, std::vector<string> columnNames, bool check = true) = 0;
        virtual void removeRow(int index, bool drop = false) = 0;
        virtual void removeRows(std::vector<int> indexes, bool drop = false) = 0;
        virtual bool columnExist(std::string name) = 0;
        virtual Column* operator[](std::string columnName) = 0;
        virtual Column* at(int index) = 0;
        virtual std::string getName() = 0;
        virtual std::vector<Type*> getRow(int index) = 0;
        virtual int getColNum() = 0;
        virtual void copy(Table& table) = 0;
        virtual std::vector<Type*> operator[](int index) = 0;
        virtual std::string toString() = 0;
        virtual std::vector<std::string> getColNames() = 0;
    };

    class VectorizedTable : public Table
    {
    private:
        std::string name;
        int getMaxSize();
        std::vector<Column*> columns;
    protected:
    void loadColumn(Column* column);
    public:
        VectorizedTable(std::string name, std::vector<Column*> cols = std::vector<Column*>{});
        void createColumn(std::string columnName, Types columnType, bool not_null = false, bool autoincrement = false, bool index = false) override;
        void addRow(std::vector<Type*> data, std::vector<string> columnNames, bool check = true) override;
        void addRow(std::vector<Type*> data);
        void removeRow(int index, bool drop = false) override;
        bool columnExist(std::string name) override;
        void removeRows(std::vector<int> indexes, bool drop = false) override;
        std::vector<Type*> getRow(int index) override;
        Column* operator[](std::string columnName) override;
        std::vector<Type*> operator[](int index) override { return getRow(index); }
        std::string getName() { return this->name; }
        int getColNum() { return this->columns.size(); }
        Column* at(int index) { return this->columns[index]; }
        VectorizedTable operator*(Table& table);
        void copy(Table& table) override;
        std::vector<std::string> getColNames() override;
        std::string toString() override;
        VectorizedTable select(Selection& sel);
    };


    

} // namespace WellDoneDB
