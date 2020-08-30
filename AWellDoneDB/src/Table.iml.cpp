#pragma once
#include "../include/Table.hpp"
namespace WellDoneDB
{
    Column::Column(std::string name, std::string tableName, Types type, std::vector<Type *> elements, bool not_null, bool index, bool autoincrement) : not_null{not_null}, index{index}, autoincrement{autoincrement}, tableName{tableName}, name{name}, type{type}
    {
        if (autoincrement && type != Types::INT)
            throw new Bad_Column("cannot declare autoincrement column " + name + "of type not INT");
        if (!elements.empty())
        {
            for (int i = 0; i < elements.size(); i++)
            {
                data.push_back(Pair<Type *, int>{elements[i], i});
            }
        }
    }

    void Column::add(Type &elem)
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
        if (elem.getType() != type)
            throw new Bad_Column("Adding element of type: " + typeToString(elem.getType()) + "to a column of type: " + typeToString(type));
        if (data.empty())
        {
            data.push_back(Pair<Type *, int>{&elem, 0});
        }
        else
        {
            data.push_back(Pair<Type *, int>{&elem, data[data.size() - 1].key + 1});
        }
    }

    void Column::add(Type *elem) { add(*elem); }

    void Column::remove(Type &elem, bool drop)
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

    void Column::set(int index, Type* newData) {
        if (this->type != newData->getType())
            throw new Bad_Column("Cannot set a dataType: " + typeToString(newData->getType()) + "into a column of type: " + typeToString(this->type));
        this->data[index].value = newData;
    }

   bool Column::set(Type* oldData, Type* newData) {
            
        if(this->type != newData->getType())
            throw new Bad_Column("Cannot set a dataType: " + typeToString(newData->getType()) + "into a column of type: " + typeToString(this->type));
        for (int i = 0; i < this->data.size(); i++)
            if (*data[i].value == *oldData) {
                data[i].value = newData;
                return true;
            }
        return false;
    }

   void _typeQuickSort(std::vector<Pair<Type*, int>> arr, int left, int right) {
       int i = left, j = right;
       Type* tmp;
       auto pivot = arr[(left + right) / 2].value;
       while (i <= j) {
           while (arr[i].value < pivot)
               i++;
           while (arr[j].value > pivot)
               j--;
           if (i <= j) {
               tmp = arr[i].value;
               arr[i].value = arr[j].value;
               arr[j].value = tmp;
               i++;
               j--;
           }
       }
       if (left < j)
           _typeQuickSort(arr, left, j);
       if (i < right)
           _typeQuickSort(arr, i, right);
   }


   void Column::sort(bool desc) {
       _typeQuickSort(this->data, 0, this->data.size() - 1);
       if (desc) {
           std::vector<Pair<Type*, int>> vec;
           for (int i = 0; i < this->data.size(); i++) {
               vec.push_back(this->data[this->data.size() - 1 - i]);
           }
           this->data = vec;
       }
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

    void Column::remove(Type *elem, bool drop) { remove(*elem, drop); }

    Type *Column::at(int index) { return data[index].value; }
    Type *Column::operator[](int index) { return data[index].value; }




    bool Column::exist(Type &elem)
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

    Selection::Selection(Column& col, Conditions condition, Type* dataCompare) : condition{ condition }, dataCompare{ dataCompare }, col{ &col } {
        for (int i = 0; i < col.getSize(); i++) {
            switch (condition)
            {
            case WellDoneDB::Conditions::EQUAL:
                if (*col[i] == *dataCompare)
                    positions.push_back(col.get(i));
                break;
            case WellDoneDB::Conditions::LESSTHAN:
                if (*col[i] < *dataCompare)
                    positions.push_back(col.get(i));
                break;
            case WellDoneDB::Conditions::GREATHERTHAN:
                if (*col[i] > *dataCompare)
                    positions.push_back(col.get(i));
                break;
            case WellDoneDB::Conditions::LESSEQTHAN:
                if (*col[i] <= *dataCompare)
                    positions.push_back(col.get(i));
                break;
            case WellDoneDB::Conditions::GREATEREQTHAN:
                if (*col[i] >= *dataCompare)
                    positions.push_back(col.get(i));
                break;
            case WellDoneDB::Conditions::NOT:
                if (*col[i] != *dataCompare)
                    positions.push_back(col.get(i));
                break;
            default:
                break;
            }
        }
    }

    Selection Selection::operator&&(Selection& sel) {
        Selection newSel;
        for (int i = 0; i < this->positions.size(); i++) {
            int check = this->positions[i].key;
            if (check < sel.positions.size()) {
                switch (sel.condition)
                {
                case Conditions::EQUAL:
                    if (*sel.positions[check].value == *sel.dataCompare)
                        newSel.positions.push_back(this->positions[check]);
                    break;

                case Conditions::GREATEREQTHAN:
                    if (*sel.positions[check].value >= *sel.dataCompare)
                        newSel.positions.push_back(this->positions[check]);
                    break;
                case Conditions::GREATHERTHAN:
                    if (*sel.positions[check].value > *sel.dataCompare)
                        newSel.positions.push_back(this->positions[check]);
                    break;

                case Conditions::LESSEQTHAN:
                    if (*sel.positions[check].value <= *sel.dataCompare)
                        newSel.positions.push_back(this->positions[check]);
                    break;

                case Conditions::LESSTHAN:
                    if (*sel.positions[check].value < *sel.dataCompare)
                        newSel.positions.push_back(this->positions[check]);
                    break;

                case Conditions::NOT:
                    if (*sel.positions[check].value != *sel.dataCompare)
                        newSel.positions.push_back(this->positions[check]);
                    break;
                case Conditions::BETWEEN:
                    if (*sel.positions[check].value <= *sel.high && *sel.positions[check].value >= *sel.high)
                        newSel.positions.push_back(this->positions[check]);
                    break;
                default:
                    break;
                }
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

    Selection Selection::operator||(Selection& sel) {
        Selection newSel;
        newSel.col = this->col;
        newSel.condition = this->condition;
        newSel.dataCompare = this->dataCompare;
        int i = 0, j = 0;
        while (i < this->positions.size() && j < sel.positions.size()) {
            if (this->positions[i].key < sel.positions[j].key) {
                newSel.positions.push_back(this->col->get(this->positions[i].key));
                i++;
            }
            else if (this->positions[i].key > sel.positions[j].key) {
                newSel.positions.push_back(this->col->get(sel.positions[j].key));
                j++;
            }
            else {
                newSel.positions.push_back(this->col->get(this->positions[i].key));
                i++;
                j++;
            }
        }
        while (i < this->positions.size()) {
            if (!this->exist(positions[i]))
                this->positions.push_back(this->col->get(this->positions[i].key));
            i++;
        }
        while (j < sel.positions.size()) {
            if (!this->exist(sel.positions[j]))
                this->positions.push_back(this->col->get(sel.positions[j].key));
            j++;
        }
        return newSel;
    }

    VectorizedTable::VectorizedTable(std::string name, std::vector<Column*> cols) : name{ name } , references{ nullptr } {
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
        int max = 0;
        for (int i = 0; i < columns.size(); i++) {
            if (columns[i]->getSize() > max)
                max = columns[i]->getSize();
        }
        return max;
    }
    void VectorizedTable::addRow(std::vector<Type*> data, std::vector<std::string> columnNames, bool check) {
        
        if (data.size() != columnNames.size())
            throw new Bad_Table("vector of data and names of different sizes");
        for (int i = 0; i < data.size(); i++) {
            (*this)[columnNames[i]]->add(data[i]);
        }
        if (check) {
            int normSize = this->getMaxSize();
            for (int i = 0; i < columns.size(); i++) {
                if (columns[i]->getSize() < normSize) {
                    if (columns[i]->autoincrement) {
                        Integer& prev = dynamic_cast<Integer&>(*columns[i]->at(columns[i]->getSize() - 1));
                        columns[i]->add(new Integer(prev.getData() + 1));
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
            if (this->references != nullptr) {
                if(!references->tab->rowExist(this->getRow(this->getMaxSize() - 1,this->references->referenced))){
                    this->removeRow(this->getMaxSize() - 1, true);
                    std::string error("Error foreign key broken for: ");
                    error += this->getName()+"(";
                    for (int i = 0; i < this->references->reference.size(); i++) {
                        error += this->references->reference[i] + ",";
                    }
                    error += ") references " + references->tab->getName();
                    for (int i = 0; i < this->references->referenced.size(); i++) {
                        error += this->references->referenced[i] + ",";
                    }
                    error += ")";
                    throw new Bad_Table(error);
                }
            }
        }
    }

    
    void VectorizedTable::removeRow(int index, bool drop) {
        for (int i = 0; i < this->columns.size(); i++) {
            columns[i]->remove(index, drop);
       }
    }

    void VectorizedTable::removeRows(std::vector<int> indexes, bool drop) {
        for (int k = 0; k < indexes.size(); k++) {
            for (int i = 0; i < this->columns.size(); i++) {
                columns[i]->remove(indexes[k], drop);
            }
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

    std::vector<Type*> VectorizedTable::getRow(int index){
        std::vector<Type*> vec;
        for (int i = 0; i < this->columns.size(); i++) {
            vec.push_back(this->columns[i]->at(index));
        }
        return vec;
    }

    void VectorizedTable::copy(Table& table) {
        for (int i = 0; i < table.getColNum(); i++) {
            this->createColumn(table.getName() + "." + table.at(i)->getName(), table.at(i)->getType(), false, false, false);
        }
    }

    std::vector<std::string> VectorizedTable::getColNames() {
        std::vector<std::string> vec;
        for (int i = 0; i < this->columns.size(); i++) {
            vec.push_back(this->columns[i]->getName());
        }
        return vec;
    }
    
    VectorizedTable VectorizedTable::operator*(Table& table) {
        VectorizedTable tab("X");
        tab.copy(*this);
        tab.copy(table);
        std::vector<std::string> thisreferences = this->getColNames();
        std::vector<std::string> tablereferences = table.getColNames();
        for (int i = 0; i < thisreferences.size(); i++) {
            thisreferences[i] = this->getName() + "." + thisreferences[i];
        }
        for (int i = 0; i < tablereferences.size(); i++) {
            tablereferences[i] = table.getName() + "." + tablereferences[i];
        }
        int thisColSizeReference = this->columns.at(0)->getSize();
        int tableColSizeReference = table.at(0)->getSize();
        for (int i = 0; i < thisColSizeReference; i++) {
            for (int j = 0; j < tableColSizeReference; j++) {
                tab.addRow(this->getRow(i), thisreferences,false);
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

    Selection::Selection(Column& col, Type* low, Type* high) : condition{ Conditions::BETWEEN }, low{ low }, high{ high }, col{ &col }, dataCompare{ nullptr } {
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
            str = str + "|" +this->getColNames().at(i)+"("+typeToString(this->columns[i]->getType()) + ")";
        }
        str += "\n\n";
        for (int i = 0; i < columnReferenceSize; i++) {
            str += std::to_string(this->columns[0]->get(i).key) +" |";
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

    Table * VectorizedTable::select(Selection& sel) {
        VectorizedTable * newTab = new VectorizedTable("Selection of:"+this->name);
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
           vec.push_back(this->at(i)->at(index));
       }
       return vec;
    }

    bool elemExist(Type* data, std::vector<Type*> container) {
        for (int i = 0; i < container.size(); i++) {
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
            if (vectorSubEquals(data, this->getRow(i)))
                return true;
        }
        return false;
    }


    bool VectorizedTable::rowExist(std::vector<Type*> data)
    {
        return rowExist(data,this->getColNames());
    }

    void VectorizedTable::setReference(Reference * ref) {
        if (this->references != nullptr)
            this->references->tab->removeReferenced(this->name);
        this->references = ref;
        ref->tab->addReferenced(ref);
    }

    void VectorizedTable::unsetReference() {
        if(this->references != nullptr)
            this->references->tab->removeReferenced(this->name);
        this->references = nullptr;
    }

    void VectorizedTable::addReferenced(Reference * ref) {
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
        for (int i = 0; i < this->referenced.size(); i++) {
            if (referenced[i]->tab->getName() == tableName)
                referenced.erase(referenced.begin() + i);
        }
    }

 
} // namespace WellDoneDB