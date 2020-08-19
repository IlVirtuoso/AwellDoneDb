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
        NOT
    };

    class Column
    {
    private:
        std::vector<Pair<Type *, int>> data;
        Types type;
        Column *reference;
        std::vector<Column *> referenced;
        std::string name;
        std::string tableName;
        void add(Type &data, int position);

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
        Column select(Conditions condition, Type &dataCompare);
        Column select(std::vector<int> positions);
        Type &at(int index);
        Type &operator[](int index);
        std::vector<int> getPositions();
        std::string toString();
        std::string inline getName() { return this->name; }
        int inline getSize() { return this->data.size(); }
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
        virtual void addRow(std::vector<Type *> data, std::vector<string> columnNames) = 0;
        virtual void removeRow(int index, bool drop = false) = 0;
        virtual void removeRows(std::vector<int> indexes, bool drop = false) = 0;
        virtual bool columnExist(std::string name) = 0;
        virtual Column* operator[](std::string columnName) = 0;
    };

    class VectorizedTable : public Table
    {
    private:
        int getMaxSize();
        std::vector<Column*> columns;
        std::string name;
    protected:
    void loadColumn(Column* column);
    public:
        VectorizedTable(std::string name, std::vector<Column*> cols = std::vector<Column*>{});
        void createColumn(std::string columnName, Types columnType, bool not_null = false, bool autoincrement = false, bool index = false) override;
        void addRow(std::vector<Type *> data, std::vector<string> columnNames) override;
        void removeRow(int index, bool drop = false) override;
        bool columnExist(std::string name) override;
        void removeRows(std::vector<int> indexes, bool drop = false) override;
        Column* operator[](std::string columnName) override;
    };

} // namespace WellDoneDB
