#pragma once
namespace WellDoneDB
{
    Column::Column(std::string name, std::string tableName, Types type, std::vector<Type *> elements, bool not_null, bool index, bool autoincrent, Column *references, std::vector<Column *> referenced) : not_null{not_null}, index{index}, autoincrement{autoincrement}, reference{reference}, tableName{tableName}, referenced{referenced}
    {
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
        if (reference != nullptr)
        {
            if (!reference->exist(elem))
                throw new Bad_Column("Adding element not present in the referenced column : " + reference->tableName + "(" + reference->name + ")" + "<-->" + tableName + "(" + name + ":" + elem.toString() + ")");
        }
        if (elem.getType() != type)
            throw new Bad_Column("Adding element of type: " + typeToString(elem.getType()) + "to a column of type: " + typeToString(type));
        if (data.empty())
        {
            data.push_back(Pair<Type *, int>{&elem, 0});
        }
        else
        {
            data.push_back(Pair<Type *, int>{&elem, data[data.size() - 1].key++});
        }
    }

    void Column::add(Type *elem) { add(*elem); }

    bool Column::removable(Type &elem)
    {
        if (referenced.size() > 0)
            for (int i = 0; i < referenced.size(); i++)
                if (!referenced[i]->exist(elem))
                    return false;
        return true;
    }

    bool Column::removable(int index) { return removable(*data[index].value); }

    void Column::remove(Type &elem, bool drop)
    {
        if (referenced.size() > 0)
            for (int i = 0; i < referenced.size(); i++)
                if (!referenced[i]->exist(elem))
                    throw new Bad_Column("Removing element: " + elem.toString() + " from column: " + name + "violates external reference: " + tableName + "(" + name + ")" + "<-->" + referenced[i]->tableName + "(" + referenced[i]->name + ")");
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

    void Column::remove(int index, bool drop)
    {
        if (referenced.size() > 0)
            for (int i = 0; i < referenced.size(); i++)
                if (!referenced[i]->exist(*data[index].value))
                    throw new Bad_Column("Removing element: " + data[i].value->toString() + " from column: " + name + "violates external reference: " + tableName + "(" + name + ")" + "<-->" + referenced[i]->tableName + "(" + referenced[i]->name + ")");
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

    Type &Column::at(int index) { return *data[index].value; }
    Type &Column::operator[](int index) { return *data[index].value; }
    Column Column::select(Conditions condition, Type &compareData)
    {
        std::vector<Pair<Type *, int>> selection;
        for (int i = 0; i < data.size(); i++)
        {
            switch (condition)
            {
            case Conditions::EQUAL:
                if (*data[i].value == compareData)
                    selection.push_back(data[i]);
                break;
            case Conditions::GREATEREQTHAN:
                if (*data[i].value >= compareData)
                    selection.push_back(data[i]);
                break;

            case Conditions::GREATHERTHAN:
                if (*data[i].value > compareData)
                    selection.push_back(data[i]);
                break;

            case Conditions::LESSEQTHAN:
                if (*data[i].value <= compareData)
                    selection.push_back(data[i]);
                break;

            case Conditions::LESSTHAN:
                if (*data[i].value < compareData)
                    selection.push_back(data[i]);
                break;

            case Conditions::NOT:
                if (*data[i].value != compareData)
                    selection.push_back(data[i]);
                break;

            default:
                break;
            }
        }
        return Column(std::string{tableName + "." + name}, std::string{}, type, selection, not_null, false, autoincrement, reference, referenced);
    }
    Column Column::select(std::vector<int> positions)
    {
        if (positions.empty())
            return Column(tableName + "." + name, std::string{}, type, std::vector<Pair<Type*,int>>{}, not_null, false, autoincrement, reference, referenced);
        if (positions.size() > data.size())
            return *this;
        std::vector<Pair<Type *, int>> selection;

        for (int i = 0; i < positions.size(); i++)
        {
            if (data[positions[i]].key == positions[i])
                selection.push_back(data[positions[i]]);
            else
            {
                for (int j = 0; j < data.size(); j++)
                {
                    if (data[j].key == positions[i])
                        selection.push_back(data[j]);
                }
            }
        }
        return Column(tableName + "." + name, std::string{}, type, selection, not_null, false, autoincrement, reference, referenced);
    }

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

} // namespace WellDoneDB