#include "../include/Table.hpp"
#include <fstream>
#include "../include/XmlParser.hpp"
#include <algorithm>
namespace WellDoneDB
{
    Column::Column(std::string name, std::string tableName, Types type, std::vector<Type*> elements, bool not_null, bool index, bool autoincrement) : not_null{ not_null }, index{ index }, autoincrement{ autoincrement }, tableName{ tableName }, name{ name }, type{ type }
    {
        if (autoincrement && type != Types::INT)
            throw new Bad_Column("cannot declare autoincrement column " + name + "of type not INT");
        if (!elements.empty())
        {
            for (int i = 0; i < elements.size(); i++)
            {
                data.push_back(Pair<Type*, int>{elements[i], i});
            }
        }
    }

    void Column::add(Type& elem)
    {
        if (not_null)
        {
            if (elem.isNull() || elem.getType() == Types::NULLED)
                throw new Bad_Column("Adding NULL element to a not null column: " + name);
        }
        if (index)
        {
            if (exist(elem))
                throw new Bad_Column("Adding duplicate element :" + elem.toString() + "on an index Column");
            if (elem.getType() == Types::NULLED)
            {
                throw new Bad_Column("Adding NULL element on index column :" + name);
            }
        }
        if (elem.getType() != type && !elem.isNull())
            throw new Bad_Column("Adding element of type: " + typeToString(elem.getType()) + "to a column of type: " + typeToString(type));
        if (data.empty())
        {
            data.push_back(Pair<Type*, int>{&elem, 0});
        }
        else
        {
            data.push_back(Pair<Type*, int>{&elem, data[data.size() - 1].key + 1});
        }
    }

    void Column::add(Type* elem) { add(*elem); }

    void Column::remove(Type& elem, bool drop)
    {
        for (int i = 0; i < data.size(); i++)
        {
            if (elem == *data[i].value)
            {
                if (drop)
                {
                    data.erase(data.begin() + i);
                    for (int j = i; j < data.size(); j++)
                    {
                        data[j].key--;
                    }
                }
                else
                    data[i].value->setNull(true);
            }
        }
    }

#define __AssertType__(Type_a, Type_b)\
     if ( Type_a != Type_b)\
        throw new Bad_Column("Cannot set a dataType: " + typeToString(Type_a) + "into a column of type: " + typeToString(Type_b))

    void Column::setAutoIncrement(bool value)
    {
        if (this->getType() != Types::INT)
            throw new Bad_Column("Error Cannot set AutoIncrement on a column of type " + typeToString(this->getType()));
        this->autoincrement = true;
    }

    void Column::set(int index, Type* newData) {
        __AssertType__(this->getType(), newData->getType());
        this->data[index].value = newData;
    }

    bool Column::set(Type* oldData, Type* newData) {

        __AssertType__(this->getType(), newData->getType());
        for (int i = 0; i < this->data.size(); i++)
            if (*data[i].value == *oldData) {
                data[i].value = newData;
                return true;
            }
        return false;
    }

    bool _comparePairAsc(Pair<Type*, int> a, Pair<Type*, int> b) {
        return (*a.value < *b.value);
    }

    bool _comparePairDesc(Pair<Type*, int> a, Pair<Type*, int> b) {
        return (*a.value > * b.value);
    }
    void Column::sort(bool desc) {
        if (!desc) std::sort(this->data.begin(), this->data.end(), _comparePairAsc);
        else std::sort(this->data.begin(), this->data.end(), _comparePairDesc);
    }

    void Column::remove(int index, bool drop)
    {
        if (drop)
        {
            data.erase(data.begin() + index);
            for (int i = index; i < data.size(); i++)
                data[i].key--;
        }
        else
        {
            data[index].value->setNull(true);
        }
    }

    void Column::remove(Type* elem, bool drop) { remove(*elem, drop); }

    Type* Column::at(int index) { return data[index].value; }
    Type* Column::operator[](int index) { return data[index].value; }




    bool Column::exist(Type& elem)
    {
        for (int i = 0; i < data.size(); i++)
            if (*data[i].value == elem)
                return true;
        return false;
    }

    std::string Column::toString()
    {
        std::string str;
        for (int i = 0; i < data.size(); i++)
        {
            str.append(" " + std::to_string(data[i].key) + " | " + data[i].value->toString() + "\n");
        }
        return str;
    }
    std::vector<int> Column::getPositions()
    {
        std::vector<int> pos;
        for (int i = 0; i < data.size(); i++)
        {
            pos.push_back(data[i].key);
        }
        return pos;
    }

    void Column::order(std::vector<int> order) {
        std::vector<Pair<Type*, int>> newData;
        for (int i = 0; i < order.size(); i++) {
            newData.push_back(this->data[order[i]]);
        }
        this->data.swap(newData);
    }

    /**
     * @brief Funzione di comparazione per tipi
     * @param a dato Type*
     * @param condition condizione
     * @param b dato Type*, in caso la condizione é between sará il valore minimo dell'intervallo
     * @param c dato Type*, é il valore massimo dell'intervallo altrimenti non viene usato
     * @return
    */
    bool _typeCompare(Type* a, Conditions condition, Type* b, Type* c = nullptr) {
        switch (condition)
        {
        case Conditions::EQUAL:
            return (*a == *b);


        case Conditions::GREATEREQTHAN:
            return (*a >= *b);


        case Conditions::GREATHERTHAN:
            return (*a > * b);


        case Conditions::LESSEQTHAN:
            return (*a <= *b);

        case Conditions::LESSTHAN:
            return (*a < *b);

        case Conditions::NOT:
            return (*a != *b);

        case Conditions::BETWEEN:
            if (c == nullptr)
                return 0;
            else {
                return (*a >= *b && *a <= *c);
            }
        default:
            return 0;
            break;
        }
    }
    Selection::Selection(Column col, Conditions condition, Type* dataCompare) : condition{ condition }, dataCompare{ dataCompare }, col{ &col } {
        for (int i = 0; i < col.getSize(); i++) {
            if (_typeCompare(col[i], condition, dataCompare))
                positions.push_back(col.get(i));
        }
    }

    bool _strlike(std::string start, std::string compare, bool end) {
        auto c = start.begin();
        auto b = compare.begin();
        while (c != start.end() && b != compare.end()) {
            if (*c == *b) { c++; b++; }
            else if (*b == '_') { c++; b++; }
            else if (*c != *b) { b = compare.begin(); c++; }
        }
        if (end) { if (b == compare.end() && c == start.end()) return true; }
        else { if (b == compare.end() && c != start.end()) return true; }
        return false;
    }

#define __castTypeToString__(type) (dynamic_cast<Text*>(type))->getData()
    Selection::Selection(Column col, std::string like) : condition{ Conditions::EQUAL }, dataCompare{ nullptr }, col{ &col } {
        if (col.getType() != Types::TEXT) throw "Bad like operator";
        bool end = false, start = false;
        if (like[0] == '%') end = true;
        if (like[like.size() - 1] == '%') start = true;
        if (start && end) throw "Bad like operator";
        for (int i = 0; i < like.size(); i++)
            if (like[i] == '%') like.erase(like.begin() + i);
        for (int i = 0; i < col.getSize(); i++) {
            if (_strlike(__castTypeToString__(col.get(i).value), like,end))
                this->positions.push_back(col.get(i));
        }
    }


    Selection::Selection(Column col1, Conditions condition, Column col2) {
        if (col1.getSize() != col2.getSize()) { throw new Bad_Selection("Bad columns, you should use * operator on table first"); }
        for (int i = 0; i < col1.getSize(); i++) {
            if (_typeCompare(col1.get(i).value, condition, col2.get(i).value)) {
                this->positions.push_back(col1.get(i));
            }
        }
    }


    Selection Selection::operator&&(Selection sel) {
        Selection newSel;
        newSel.col = this->col;
        newSel.condition = this->condition;
        newSel.dataCompare = this->dataCompare;
        for (int i = 0; i < this->positions.size(); i++) {
            for (int j = 0; j < sel.positions.size(); j++) {
                if (this->positions[i].key == sel.positions[j].key)
                    newSel.positions.push_back(this->positions[i]);
            }
        }
        return newSel;
    }

    bool Selection::exist(Pair<Type*, int>& check) {
        for (int i = 0; i < positions.size(); i++) {
            if (positions[i].key == check.key)
                return true;
        }
        return false;
    }
    
    bool _compareKey(Pair<Type*, int> a, Pair<Type*, int> b) {
        return (a.key < b.key);
    }


    Selection Selection::operator||(Selection sel) {
        Selection newSel;
        newSel.col = this->col;
        newSel.condition = this->condition;
        newSel.dataCompare = this->dataCompare;
        newSel.positions = this->positions;
        for (int i = 0; i < sel.positions.size(); i++) {
            if (!newSel.exist(sel.positions[i])) {
                newSel.positions.push_back(sel.positions[i]);
            }
        }
        std::sort(newSel.positions.begin(), newSel.positions.end(), _compareKey);
        return newSel;
    }

    VectorizedTable::VectorizedTable(std::string name, std::vector<Column*> cols) : name{ name }, references{ nullptr } {
        if (!cols.empty()) {
            for (int i = 0; i < cols.size(); i++) {
                columns.push_back(cols[i]);
            }
        }
    }

    void VectorizedTable::createColumn(std::string columnName, Types columnType, bool not_null, bool autoincrement, bool index) {
        Column* col = new Column(columnName, this->name, columnType, std::vector<Type*>{}, not_null, index, autoincrement);
        columns.push_back(col);
    }

    void VectorizedTable::loadColumn(Column* col) {
        if (columnExist(col->getName()))
            throw new Bad_Table("Column " + col->getName() + " already present in the table");
        columns.push_back(col);

    }
    int VectorizedTable::getMaxSize() {
        size_t max = 0;
        for (int i = 0; i < columns.size(); i++) {
            if (columns[i]->getSize() > max)
                max = columns[i]->getSize();
        }
        return max;
    }
    void VectorizedTable::addRow(std::vector<Type*> data, std::vector<std::string> columnNames, bool check) {

        if (data.size() != columnNames.size())
            throw new Bad_Table("vector of data and names of different sizes");
        if (this->references != nullptr) {
            std::vector<std::string> columnChecks;
            std::vector<Type*>dataChecks;
            for (int i = 0; i < columnNames.size(); i++) {
                for (int j = 0; j < this->references->reference.size(); j++) {
                    if (columnNames[i] == this->references->reference[j]) {
                        columnChecks.push_back(this->references->referenced[j]);
                        dataChecks.push_back(data[i]);
                        continue;
                    }
                }
            }
            if (!this->references->tab->rowExist(dataChecks, columnChecks))
                throw new Bad_Table("Error this row violates foreign key references on table: " + this->references->tab->getName());
        }
        for (int i = 0; i < data.size(); i++) {
            try {
                (*this)[columnNames[i]]->add(data[i]);
            }
            catch (...) {
                for (int k = i - 1; k >= 0; k--) {
                    (*this)[columnNames[k]]->remove((*this)[columnNames[k]]->getSize() - 1,true);
                }
                throw new Bad_Table("Error while inserting a row");
            }
        }
        if (check) {
            int normSize = this->getMaxSize();
            for (int i = 0; i < columns.size(); i++) {
                if (columns[i]->getSize() < normSize) {
                    if (columns[i]->autoincrement) {
                        if (columns[i]->getSize() > 0) {
                            Integer& prev = dynamic_cast<Integer&>(*columns[i]->at(columns[i]->getSize() - 1));
                            columns[i]->add(new Integer(prev.getData() + 1));
                        }
                        else {
                            columns[i]->add(new Integer(0));
                        }
                    }
                    else if (!columns[i]->not_null) {
                        columns[i]->add(new Null());
                    }
                    else {
                        for (int j = 0; j < columns.size(); j++) {
                            if (columns[j]->getSize() == normSize)
                                columns[j]->remove(normSize - 1, true);
                        }
                        throw new Bad_Table("error Missing data for column: " + columns[i]->getName());
                    }
                }
            }
        }
    }


    void VectorizedTable::removeRow(int index, bool drop) {
        /*
        for (int i = 0; i < this->referenced.size(); i++) {
            auto referencedColumns = this->referenced[i]->referenced;
            auto referenceColumns = this->referenced[i]->reference;
            auto referenceTab = this->referenced[i]->tab;
            auto row = this->getRow(index, referencedColumns);
            if (referenceTab->rowExist(row, referenceColumns))
                throw new Bad_Table("Error this delete violates foreign key with: " + referenceTab->getName());
        }
        funzione di check commentata perché non richiesta dal progetto
        */
        for (int i = 0; i < this->columns.size(); i++) {
            columns[i]->remove(index, drop);
        }
    }

    void VectorizedTable::removeRows(std::vector<int> indexes, bool drop) {
        for (int k = 0; k < indexes.size(); k++) {
            removeRow(indexes[k], drop);
        }
    }

    bool VectorizedTable::columnExist(std::string name) {
        for (int i = 0; i < columns.size(); i++) {
            if (columns[i]->getName() == name)
                return true;
        }
        return false;
    }

    Column* VectorizedTable::operator[](std::string columnName) {
        for (int i = 0; i < columns.size(); i++) {
            if (columns[i]->getName() == columnName)
                return columns[i];
        }
        throw new Bad_Table("Column: " + columnName + " don't exist");
    }


    void VectorizedTable::addRow(std::vector<Type*> data) {
        if (data.size() != this->getColNum())
            throw new Bad_Table("cannot use autoAdd method with vector of different size of columns");
        for (int i = 0; i < data.size(); i++) {
            this->at(i)->add(data[i]);
        }
    }

    std::vector<Type*> VectorizedTable::getRow(int index) {
        std::vector<Type*> vec;
        for (int i = 0; i < this->columns.size(); i++) {
            vec.push_back(this->columns[i]->at(index));
        }
        return vec;
    }

    void VectorizedTable::copy(Table& table) {
        for (int i = 0; i < table.getColNum(); i++) {
            this->createColumn(table.at(i)->getName(), table.at(i)->getType(), false, false, false);
        }
    }

    std::vector<std::string> VectorizedTable::getColNames() {
        std::vector<std::string> vec;
        for (int i = 0; i < this->columns.size(); i++) {
            vec.push_back(this->columns[i]->getName());
        }
        return vec;
    }
    void Table::setColumnName(std::string oldName, std::string newName) {
        this->get(oldName)->setName(newName);
    }
    VectorizedTable VectorizedTable::operator*(VectorizedTable& table) {
        VectorizedTable tab("X");
        tab.copy(*this);
        tab.copy(table);
        std::vector<std::string> thisreferences = this->getColNames();
        std::vector<std::string> tablereferences = table.getColNames();
        for (int i = 0; i < thisreferences.size(); i++) {
            if (thisreferences[i].find(".", 0) == std::string::npos) {
                std::string oldName = thisreferences[i];
                thisreferences[i] = this->getName() + "." + thisreferences[i];
                tab.setColumnName(oldName, thisreferences[i]);
            }
        }
        for (int i = 0; i < tablereferences.size(); i++) {
            if (tablereferences[i].find(".", 0) == std::string::npos) {
                std::string oldName = tablereferences[i];
                tablereferences[i] = table.getName() + "." + tablereferences[i];
                tab.setColumnName(oldName, tablereferences[i]);
            }
        }
        int thisColSizeReference = this->columns.at(0)->getSize();
        int tableColSizeReference = table.at(0)->getSize();
        for (int i = 0; i < thisColSizeReference; i++) {
            for (int j = 0; j < tableColSizeReference; j++) {
                tab.addRow(this->getRow(i), thisreferences, false);
                tab.addRow(table.getRow(j), tablereferences, false);
            }
        }
        return tab;
    }

    std::vector<int> Selection::getPos() {
        std::vector<int> newVec;
        for (int i = 0; i < this->positions.size(); i++) {
            newVec.push_back(this->positions[i].key);
        }
        return newVec;
    }

    Selection::Selection(Column col, Type* low, Type* high) : condition{ Conditions::BETWEEN }, low{ low }, high{ high }, col{ &col }, dataCompare{ nullptr } {
        for (int i = 0; i < col.getSize(); i++) {
            if (*col[i] <= *high && *col[i] >= *low)
                this->positions.push_back(col.get(i));
        }
    }

    std::string VectorizedTable::toString() {
        if (this->columns.empty()) {
            return "VOID TABLE";
        }
        std::string str("COLUMNS:");
        int columnReferenceSize = this->columns.at(0)->getSize();
        for (int i = 0; i < this->getColNum(); i++) {
            str = str + "|" + this->getColNames().at(i) + "(" + typeToString(this->columns[i]->getType()) + ")";
        }
        str += "\n\n";
        for (int i = 0; i < columnReferenceSize; i++) {
            str += std::to_string(this->columns[0]->get(i).key) + " |";
            for (int j = 0; j < this->columns.size(); j++) {
                if (!this->columns[j]->at(i)->isNull())
                    str += "|" + this->columns[j]->at(i)->toString();
                else
                    str += "| NULL";
            }
            str += "\n";
        }
        return str;
    }

    Table* VectorizedTable::select(Selection& sel) {
        VectorizedTable* newTab = new VectorizedTable("Selection of:" + this->name);
        newTab->copy(*this);
        auto positions = sel.getPos();
        for (int i = 0; i < positions.size(); i++) {
            newTab->addRow(this->getRow(positions[i]));
        }
        return newTab;
    }

    std::vector<Type*> VectorizedTable::getRow(int index, std::vector<std::string> columns)
    {
        std::vector<Type*> vec;
        for (int i = 0; i < columns.size(); i++) {
            vec.push_back(this->get(columns[i])->at(index));
        }
        return vec;
    }

    bool elemExist(Type* data, std::vector<Type*> container) {
        for (int i = 0; i < container.size(); i++) {
        if(data->getType() == container[i]->getType())
            if (*data == *container[i])
                return true;
        }
        return false;
    }

    bool vectorSubEquals(std::vector<Type*> container, std::vector<Type*> container2) {
        for (int i = 0; i < container.size(); i++) {
            if (!elemExist(container[i], container2))
                return false;
        }
        return true;
    }

    bool VectorizedTable::rowExist(std::vector<Type*> data, std::vector<std::string> cols)
    {
        for (int i = 0; i < this->getMaxSize(); i++) {
            if (vectorSubEquals(data, this->getRow(i,cols)))
                return true;
        }
        return false;
    }


    bool VectorizedTable::rowExist(std::vector<Type*> data)
    {
        return rowExist(data, this->getColNames());
    }

    void VectorizedTable::setReference(Reference* ref) {
        if (this->references != nullptr)
            this->references->tab->removeReferenced(this->name);
        this->references = ref;
        ref->tab->addReferenced(new Reference(this,ref->reference,ref->referenced));
    }

    void VectorizedTable::unsetReference() {
        if (this->references != nullptr) {
            this->references->tab->removeReferenced(this->name);
        }
        this->references = nullptr;
    }

    void VectorizedTable::addReferenced(Reference* ref) {
        this->referenced.push_back(ref);
    }

    std::vector<Column*> VectorizedTable::getColumns(std::vector<std::string> colNames)
    {
        std::vector<Column*> ret;
        for (int i = 0; i < this->columns.size(); i++) {
            for (int k = 0; k < colNames.size(); k++)
                if (colNames[k] == columns[i]->getName()) {
                    ret.push_back(columns[i]);
                    continue;
                }
        }
        return ret;
    }

    void VectorizedTable::removeReferenced(std::string tableName) {
        if (this->referenced.size() == 0)
            return;
        for (int i = 0; i < this->referenced.size(); i++) {
            if (referenced[i]->tab->getName() == tableName)
                referenced.erase(referenced.begin() + i);
        }
    }

    void VectorizedTable::operator>>(Table* table) {
        table->copy(*this);
        for (int i = 0; i < this->columns[0]->getSize(); i++) {
            for (int j = 0; j < this->columns.size(); j++) {
                table->at(j)->add(this->at(j)->at(i));
            }
        }
    }

    Table* VectorizedTable::sort(std::string columnName, bool desc) {
        VectorizedTable* tab = new VectorizedTable(this->name);
        *this >> tab;
        (*tab)[columnName]->sort(desc);
        for (int i = 0; i < this->columns.size(); i++) {
            tab->columns[i]->order((*tab)[columnName]->getPositions());
        }
        return tab;
    }


    std::string Column::toXml() {
        std::string xml("<column>\n");
        xml += "<name>" + this->name + "</name>\n";
        xml += "<type>" + typeToString(this->type) + "</type>\n";
        xml += "<tableName>" + this->tableName + "</tableName>\n";
        xml += "<autoincrement>" + std::to_string(this->autoincrement) + "</autoincrement>\n";
        xml += "<notNull>" + std::to_string(this->not_null) + "</notNull>\n";
        xml += "<index>" + std::to_string(this->index) + "</index>\n";
        xml += "<container>\n";
        for (int i = 0; i < this->data.size(); i++) {
            xml += "<position>" + std::to_string(this->data[i].key) + "</position>\n";
            xml += "<data>" + this->data[i].value->toString() + "</data>\n";
        }
        xml += "</container>\n";
        xml += "</column>\n";
        return xml;
    }

    std::string tableTypeToString(Table::TableType type) {
        switch (type)
        {
        case WellDoneDB::Table::TableType::VECTORIZED:
            return std::string("VECTORIZED");
            break;
        default: return std::string("VECTORIZED");
            break;
        }
    }

    Types stringToType(std::string type) {
        if (type == "INT" || type == "int")
            return Types::INT;
        else if (type == "FLOAT" || type == "float")
            return Types::FLOAT;
        else if (type == "DATE" || type == "date")
            return Types::DATE;
        else if (type == "TIME" || type == "time")
            return Types::TIME;
        else if (type == "TEXT" || type == "text")
            return Types::TEXT;
        else if (type == "CHAR" || type == "char");
        else if (type == "NULLED")
            return Types::NULLED;
        else return Types::NOTVALID;
    }


    std::string VectorizedTable::toXml() {
        std::string xml("<table>\n");
        xml += "<name>" + this->name + "</name>\n";
        xml += "<type>" + tableTypeToString(this->getTableType()) + "</type>\n";
        xml += "<columns>\n";
        for (int i = 0; i < this->columns.size(); i++) {
            xml += columns[i]->toXml();
        }
        xml += "</columns>\n";
        xml += "</table>\n";
        return xml;
    }


    bool stringToBool(std::string val) {
        if (val == "false")
            return false;
        return true;
    }

    Type * stringToType(std::string value, Types type) {
        if (value == "NULL")
            return new Null();
        switch (type)
        {
        case WellDoneDB::Types::NULLED:
            return new Null();
        case WellDoneDB::Types::INT:
            return new Integer(atoi(value.c_str()));
        case WellDoneDB::Types::CHAR:
            return new Char(value[0]);
        case WellDoneDB::Types::TEXT:
            return new Text(value);
        case WellDoneDB::Types::FLOAT:
            return new Float(static_cast<float>(atof(value.c_str())));
        case WellDoneDB::Types::DATE:
            return new Date(value,Date::DD_MM_YYYY);
        case WellDoneDB::Types::TIME:
            return new Time(value);
        default: return new Null();
            break;
        }
    }

    void VectorizedTable::loadXml()
    {
        std::ifstream file(this->name + ".table");
        std::string xml;
        while (file.good())
            xml += file.get();
        XmlParser parser(xml);
        auto token = parser.begin();
        while (token != parser.end()) {
            if (token->tag == "columns") {
                token++;
                std::string name, type, tableName;
                bool auto_increment = false, index = false, not_null = false;
                std::vector<Type*> data;
                while (token->tag != "columns") {
                    if (token->tag == "column") {
                        if (!name.empty())
                            this->columns.push_back(new Column(name, tableName, stringToType(type), data, not_null, index, auto_increment));
                        name.clear();
                        type.clear();
                        tableName.clear();
                        data.clear();
                    }
                    else if (token->tag == "name")
                        name = token->value;
                    else if (token->tag == "type")
                        type = token->value;
                    else if (token->tag == "tableName")
                        tableName = token->value;
                    else if (token->tag == "autoincrement")
                        auto_increment = atoi(token->value.c_str());
                    else if (token->tag == "notNull")
                        not_null = atoi(token->value.c_str());
                    else if (token->tag == "index")
                        index = atoi(token->value.c_str());
                    else if (token->tag == "container") {
                        token++;
                        while (token->tag != "container") {
                            if (token->tag == "data")
                                data.push_back(stringToType(token->value, stringToType(type)));
                            token++;
                        }
                    }
                    token++;
                }
            }
            token++;
        }
    }

    void VectorizedTable::update(std::vector<int> positions, std::string columnName, Type* newData)
    {
        if (positions.size() > (*this)[columnName]->getSize())
            throw new Bad_Table("too much positions for this column");
        for (int i = 0; i < positions.size(); i++)
            if (positions[i] > (*this)[columnName]->getSize())
                throw new Bad_Table("invalid position specified, row: " + std::to_string(i) + " in column: " + this->name + " with max rows: " +
                    std::to_string((*this)[columnName]->getSize()));
        for (int i = 0; i < positions.size(); i++) {
            (*this)[columnName]->set(positions[i], newData);
        }
    }

    void VectorizedTable::truncate()
    {
        for (int i = 0; i < this->columns.size(); i++) {
            this->columns[i]->clear();
        }
    }
 
    Conditions conditionToString(std::string value) {
        if (value == ">") return Conditions::GREATHERTHAN;
        else if (value == "<") return Conditions::LESSTHAN;
        else if (value == ">=") return Conditions::GREATEREQTHAN;
        else if (value == "<=") return Conditions::LESSEQTHAN;
        else if (value == "<>") return Conditions::NOT;
        else return Conditions::EQUAL;

    }
} // namespace WellDoneDB