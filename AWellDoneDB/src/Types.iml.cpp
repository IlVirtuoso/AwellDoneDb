#pragma once
#include "../include/Types.hpp"
namespace WellDoneDB
{
    bool checkLeap(int yy)
    {
        if (yy % 400 == 0)
            return true;
        else if ((yy % 4 == 0) && !(yy % 100 == 0))
            return true;
        return false;
    }

    Date::Date(std::string date, int format) : format{format}
    {
        type = Types::DATE;
        char *field;
        char delim[] = {'/', '-', '_'};
        char *string = (char *)date.c_str();
        field = strtok(string, delim);
        std::vector<int> values;
        for (int i = 0; i < 3; i++)
        {
            values.push_back(atoi(field));
            field = strtok(NULL, delim);
        }
        switch (format)
        {
        case DD_MM_YYYY:
            dd = values[0], mm = values[1], yy = values[2];
            break;
        case MM_DD_YYYY:
            mm = values[0], dd = values[1], yy = values[2];
            break;

        case YYYY_MM_DD:
            yy = values[0], mm = values[1], dd = values[2];
            break;

        default:
            break;
        }
        leap = checkLeap(yy);
        if (dd == 29 && mm == 2 && !leap)
            throw new Bad_Date("29 february on a non leap year");
    }

    Date::Date(int dd, int mm, int yy, int format) : format{format}, dd{dd}, mm{mm}, yy{yy}
    {
        type = Types::DATE;
        leap = checkLeap(yy);
        if (dd == 29 && mm == 2 && !leap)
            throw new Bad_Date("29 february on a non leap year");
    }

    std::string Date::toString()
    {
        switch (format)
        {
        case DD_MM_YYYY:
            return std::string("" + std::to_string(dd) + "-" + std::to_string(mm) + "-" + std::to_string(yy));
            break;

        case MM_DD_YYYY:
            return std::string("" + std::to_string(mm) + "-" + std::to_string(dd) + "-" + std::to_string(yy));
            break;

        case YYYY_MM_DD:
            return std::string("" + std::to_string(yy) + "-" + std::to_string(mm) + "-" + std::to_string(dd));
            break;
        default:
            return std::string("" + std::to_string(dd) + "-" + std::to_string(mm) + "-" + std::to_string(yy));
            break;
        }
    }


    bool Date::operator<(Type &type)
    {
        if (this->type != type.getType())
            throw new Bad_Type("Cannot compare " + typeToString(this->type) + " with " + typeToString(type.getType()));
        Date typeDate = dynamic_cast<Date &>(type);
        int calc = (yy - typeDate.yy) * 1000 + (mm - typeDate.mm) * 100 + (dd - typeDate.dd);
        if (calc < 0)
            return true;
        return false;
    }
    bool Date::operator>(Type &type)
    {
        if (this->type != type.getType())
            throw new Bad_Type("Cannot compare " + typeToString(this->type) + " with " + typeToString(type.getType()));
        Date typeDate = dynamic_cast<Date &>(type);
        int calc = (yy - typeDate.yy) * 1000 + (mm - typeDate.mm) * 100 + (dd - typeDate.dd);
        if (calc > 0)
            return true;
        return false;
    }
    bool Date::operator==(Type &type)
    {
        if (this->type != type.getType())
            throw new Bad_Type("Cannot compare " + typeToString(this->type) + " with " + typeToString(type.getType()));
        Date typeDate = dynamic_cast<Date &>(type);
        return typeDate.dd == dd && typeDate.mm == mm && typeDate.yy == yy;
    }

    Time::Time(int h, int m, int s, int ms) : h{h}, m{m}, s{s}, ms{ms}
    {
        type = Types::TIME;
        if (h > 24 || m > 60 || s > 60 || ms > 9999)
            throw new Bad_Time("wrong values on time");
    }

    Time::Time(std::string time)
    {
        this->type = Types::TIME;
        h = 0, m = 0, s = 0, ms = 0;
        char *field;
        char delim[] = {':'};
        char *string = (char *)time.c_str();
        field = strtok(string, delim);
        std::vector<int> values;
        while (field != NULL)
        {
            values.push_back(atoi(field));
            field = strtok(NULL, delim);
        }
        for (int i = 0; i < values.size(); i++)
        {
            switch (i)
            {
            case 0:
                h = values[i];
                break;
            case 1:
                m = values[i];
                break;

            case 2:
                s = values[i];
                break;

            case 3:
                ms = values[i];
                break;
            default:
                break;
            }
        }
    }

    std::string Time::toString()
    {
        return std::string("" + std::to_string(h) + ":" + std::to_string(m) + ":" + std::to_string(s) + "." + std::to_string(ms));
    }

    bool Time::operator>(Type &type)
    {
        if (type.getType() != this->getType())
            throw new Bad_Type("Cannot compare " + typeToString(this->type) + " with " + typeToString(type.getType()));
        Time typeTime = dynamic_cast<Time &>(type);
        long calc = (h - typeTime.h) * pow(10, 6) + (m - typeTime.m) * pow(10, 4) + (s - typeTime.s) * pow(10, 2);
        if (calc > 0)
            return true;
        else if(calc == 0 && ms > typeTime.ms)
            return true;
        return false;
    }

    bool Time::operator<(Type &type)
    {
        if (type.getType() != this->getType())
            throw new Bad_Type("Cannot compare " + typeToString(this->type) + " with " + typeToString(type.getType()));
        Time typeTime = dynamic_cast<Time &>(type);
        long calc = (h - typeTime.h) * pow(10, 6) + (m - typeTime.m) * pow(10, 4) + (s - typeTime.s) * pow(10, 2);
        if (calc < 0)
            return true;
        else if(calc == 0 && ms < typeTime.ms)
            return true;
        return false;
    }

    bool Time::operator==(Type &type)
    {
        if (type.getType() != this->getType())
            throw new Bad_Type("Cannot compare " + typeToString(this->type) + " with " + typeToString(type.getType()));
        Time typeTime = dynamic_cast<Time &>(type);
        return h = typeTime.h && m == typeTime.m && typeTime.s == s && ms == typeTime.ms;
    }

    std::string typeToString(Types type)
    {
        using String = std::string;
        switch (type)
        {
        case Types::INT:
            return String{"INT"};
        case Types::CHAR:
            return String{"char"};
        case Types::DATE:
            return String{"DATE"};
        case Types::FLOAT:
            return String{"FLOAT"};
        case Types::NULLED:
            return String{"NULLED"};
        case Types::TEXT:
            return String{"TEXT"};
        case Types::TIME:
            return String{"TIME"};
        default:
            return std::string{};
            break;
        }
    }

    
} // namespace WellDoneDB