#pragma once
namespace WellDoneDB
{
    enum class Types
    {
        INT,
        CHAR,
        TEXT,
        FLOAT,
        DATE,
        TIME,
        NULLED
    };

    std::string typeToString(Types type);
    class Type
    {
    protected:
        bool is_null;
        Types type;

    public:
        class Bad_Type
        {
        public:
            std::string message;
            Bad_Type(std::string message) : message{message} { std::cout << "Exception: " << message << std::endl; }
        };
        Types getType() { return type; }
        virtual std::string toString() = 0;
        bool isNull() { return is_null; }
        void setNull(bool value) { is_null = value; }
        virtual bool operator<(Type &type) = 0;
        virtual bool operator>(Type &type) = 0;
        virtual bool operator==(Type &type) = 0;
        bool operator<=(Type &type) { return *this < type || *this == type; }
        bool operator>=(Type &type) { return *this > type || *this == type; }
        bool operator!=(Type &type) { return !(*this == type); }
    };

    class Integer : public Type
    {
    private:
        int data;

    public:
        Integer(int data) : data{data}
        {
            type = Types::INT;
        }
        std::string toString() override
        {
            return std::string{std::to_string(data)};
        }

        int getData() { return data; }

        bool operator<(Type &type) override
        {
            if (type.getType() != this->type)
                throw new Bad_Type("Cannot compare " + typeToString(this->type) + " with " + typeToString(type.getType()));
            return this->data < (dynamic_cast<Integer &>(type)).data;
        }

        bool operator>(Type &type) override
        {
            if (type.getType() != this->type)
                throw new Bad_Type("Cannot compare " + typeToString(this->type) + " with " + typeToString(type.getType()));
            return this->data > (dynamic_cast<Integer &>(type)).data;
        }

        bool operator==(Type &type) override
        {
            if (type.getType() != this->type)
                throw new Bad_Type("Cannot compare " + typeToString(this->type) + " with " + typeToString(type.getType()));
            return this->data == (dynamic_cast<Integer &>(type)).data;
        }
    };

    class Char : public Type
    {
    private:
        char data;

    public:
        std::string toString() { return std::string{data}; }
        Char(char data) : data{data} { type = Types::CHAR; }

        char getData() { return data; }

        bool operator<(Type &type) override
        {
            if (this->type != type.getType())
                throw new Bad_Type("Cannot compare " + typeToString(this->type) + " with " + typeToString(type.getType()));
            return this->data < (dynamic_cast<Char &>(type)).data;
        }

        bool operator>(Type &type) override
        {
            if (this->type != type.getType())
                throw new Bad_Type("Cannot compare " + typeToString(this->type) + " with " + typeToString(type.getType()));
            return this->data > (dynamic_cast<Char &>(type)).data;
        }

        bool operator==(Type &type) override
        {
            if (this->type != type.getType())
                throw new Bad_Type("Cannot compare " + typeToString(this->type) + " with " + typeToString(type.getType()));
            return this->data == (dynamic_cast<Char &>(type)).data;
        }
    };

    class Text : public Type
    {
    private:
        std::string data;

    public:
        Text(std::string data) : data{data} { type = Types::TEXT; }
        Text(const char *data) : data{data} { type = Types::TEXT; }
        std::string getData() { return data; }
        std::string toString(){return data;}
        bool operator<(Type &type)
        {
            if (this->type != type.getType())
                throw new Bad_Type("Cannot compare " + typeToString(this->type) + " with " + typeToString(type.getType()));
            return this->data < (dynamic_cast<Text &>(type)).data;
        }

        bool operator>(Type &type)
        {
            if (this->type != type.getType())
                throw new Bad_Type("Cannot compare " + typeToString(this->type) + " with " + typeToString(type.getType()));
            return this->data > (dynamic_cast<Text &>(type)).data;
        }

        bool operator==(Type &type)
        {
            if (this->type != type.getType())
                throw new Bad_Type("Cannot compare " + typeToString(this->type) + " with " + typeToString(type.getType()));
            return this->data == (dynamic_cast<Text &>(type)).data;
        }
    };

    class Float : public Type
    {
    private:
        float data;

    public:
        Float(float data) : data{data} { type = Types::FLOAT; }
        std::string toString() override { return std::string{std::to_string(data)}; }

        float getData() { return this->data; }

        bool operator<(Type &type) override
        {
            if (this->type != type.getType())
                throw new Bad_Type("Cannot compare " + typeToString(this->type) + " with " + typeToString(type.getType()));
            return this->data < (dynamic_cast<Float &>(type)).data;
        }

        bool operator>(Type &type) override
        {
            if (this->type != type.getType())
                throw new Bad_Type("Cannot compare " + typeToString(this->type) + " with " + typeToString(type.getType()));
            return this->data > (dynamic_cast<Float &>(type)).data;
        }

        bool operator==(Type &type) override
        {
            if (this->type != type.getType())
                throw new Bad_Type("Cannot compare " + typeToString(this->type) + " with " + typeToString(type.getType()));
            return this->data == (dynamic_cast<Float &>(type)).data;
        }
    };

    class Null : public Type
    {
    public:
        Null()
        {
            is_null = true;
            type = Types::NULLED;
        }
        std::string toString() { return std::string{"NULL"}; }
        bool operator>(Type &type)
        {
            return false;
        }

        bool operator<(Type &type)
        {
            if (type.getType() != Types::NULLED && !type.isNull())
                return true;
            else
                return false;
        }
        bool operator==(Type &type)
        {
            if (type.getType() == Types::NULLED || type.isNull())
                return true;
            return false;
        }
    };

    class Date : public Type
    {
    protected:
        int dd, mm, yy;
        bool leap;
        int format;

    public:
        class Bad_Date
        {
        public:
            std::string message;
            Bad_Date(std::string message) : message{message} { std::cout << "Date Exception: " << message << std::endl; }
        };
        enum FORMAT
        {
            DD_MM_YYYY,
            MM_DD_YYYY,
            YYYY_MM_DD
        };
        Date(std::string date, int format);
        Date(int dd, int mm, int yy, int format = DD_MM_YYYY);
        std::string toString();
        bool operator<(Type &type) override;
        bool operator>(Type &type) override;
        bool operator==(Type &type) override;
    };

    class Time : public Type
    {
    protected:
        int h, m, s, ms;

    public:
        class Bad_Time
        {
        public:
            Bad_Time(std::string message) : youWillHaveABadTime{message} { std::cout << "Bad Time Exception: " << message << std::endl; }
            std::string youWillHaveABadTime;
        };

        Time(int h = 0, int m = 0, int s = 0, int ms = 0);
        Time(std::string time);
        std::string toString();
        bool operator>(Type &type);
        bool operator<(Type &type);
        bool operator==(Type &type);
    };

    template <class T, typename K>

    class Pair
    {
    public:
        T value;
        K key;
        Pair(T value, K key) : value{value}, key{key} {}
    };

} // namespace WellDoneDB
