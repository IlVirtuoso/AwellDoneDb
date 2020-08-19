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
        std::vector<Pair<Type*,int>> data;
        Types type;
        Column * reference;
        std::vector<Column*> referenced;
        std::string name;
        std::string tableName;
        bool not_null, index, autoincrement;
        void add(Type& data, int position);

    public:
        class Bad_Column {
            public:
            std::string message;
            Bad_Column(std::string message) : message{message}{std::cout<<"Column Exception:" << message <<std::endl;}
        };
        Column(std::string name, std::string tableName, Types type, std::vector<Type*> elements = std::vector<Type*>{}, bool not_null = false, bool index = false, bool autoincrement = false, Column* references = nullptr, std::vector<Column*> referenced = std::vector<Column*>{});
        Column(std::string name, std::string tableName, Types type, std::vector<Pair<Type*,int>> elements = std::vector<Pair<Type*,int>>{}, bool not_null = false, bool index = false, bool autoincrement = false, Column* references = nullptr, std::vector<Column*> referenced = std::vector<Column*>{}) :
        name{name},tableName{tableName},type{type},data{elements},index{index},not_null{not_null},autoincrement{autoincrement},reference{references},referenced{referenced}{}
        void add(Type &data);
        void add(Type * data);
        bool removable(Type& data);
        bool removable(int index);
        void remove(Type &data, bool drop = false);
        void remove(Type * data, bool drop = false);
        void remove(int index, bool drop = false);
        bool exist(Type& data);
        void inline addReferenced(Column* column){referenced.push_back(column);}
        void inline setReference(Column* column){reference = column;}
        Column select(Conditions condition, Type &dataCompare);
        Column select(std::vector<int> positions);
        Type &at(int index);
        Type &operator[](int index);
        std::vector<int> getPositions();
        std::string toString();
    }; 


    class Table
    {
        public:
        virtual void createColumn(std::string columnName, Types columnType, )
    };

} // namespace WellDoneDB
