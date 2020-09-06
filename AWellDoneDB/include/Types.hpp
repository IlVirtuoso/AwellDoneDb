#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <exception>
namespace WellDoneDB
{
    /**
     * @brief Enumerazione dei tipi supportati
    */
    enum class Types
    {
        INT,
        CHAR,
        TEXT,
        FLOAT,
        DATE,
        TIME,
        NULLED,
        NOTVALID = 1
    };

    /**
     * @brief Converte un enumerazione Types in una stringa
     * @param type 
     * @return 
    */
    std::string typeToString(Types type);

    /**
     * @brief Interfaccia per la rappresentazione di tipi
    */
    class Type
    {
    protected:
        bool is_null = false;
        Types type;

    public:
        /**
         * @brief Classe per le eccezioni dei tipi
        */
        class Bad_Type : std::exception
        {
        public:
            std::string message;
            Bad_Type(std::string message) : message{message} { std::cout << "Exception: " << message << std::endl; }
        };

        /**
         * @brief Ritorna il tipo di oggetto
         * @return un enumerazione Types
        */
        Types getType() { return type; }

        /**
         * @brief Converte il contenuto dell'oggetto in una stringa
         * @return 
        */
        virtual std::string toString() = 0;

        bool isNull() { return is_null; }
        void setNull(bool value) { is_null = value; }

        /**
         * @brief Operatori di confronto per i tipi
         * @param type tipo con cui confrontare l'oggetto
         * @warning lancia un eccezione se il tipo é diverso dal tipo di questo oggetto
        */
        virtual bool operator<(Type &type) = 0;
        virtual bool operator>(Type &type) = 0;
        virtual bool operator==(Type &type) = 0;
        bool operator<=(Type &type) { return *this < type || *this == type; }
        bool operator>=(Type &type) { return *this > type || *this == type; }
        bool operator!=(Type &type) { return !(*this == type); }
    };

    /**
     * @brief Classe per rappresentare gli interi
    */
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

    /**
     * @brief Classe per rappresentare i caratteri
    */
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

    /**
     * @brief Classe per rappresentare le stringhe di lunghezza arbitraria
    */
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


    /**
     * @brief Classe per rappresentare i numeri a virgola mobile
    */
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

    /**
     * @brief Classe per rappresentare i tipi nulli
     * @warning questo tipo di oggetto si puó inserire in tutte le colonne
     * di qualunque tipo a patto che non siano not null
    */
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

    /**
     * @brief Classe per rappresentare la data
     * di default verrá usato il formato DD_MM_YYYY, che é il formato standard
     * ma é possibile usarne anche altri implementati nelle loro enumerazione
    */
    class Date : public Type
    {
    protected:
        int dd, mm, yy;
        bool leap;
        int format;

    public:
        /**
         * @brief Classe di eccezioni per le date
        */
        class Bad_Date : std::exception
        {
        public:
            std::string message;
            Bad_Date(std::string message) : message{message} { std::cout << "Date Exception: " << message << std::endl; }
        };

        /**
         * @brief Enumerazione di vari formati disponibili
        */
        enum FORMAT
        {
            DD_MM_YYYY,
            MM_DD_YYYY,
            YYYY_MM_DD
        };
        
        /**
         * @brief Costruttore per stringhe
         * @param date data in formato stringa
         * @param format formato della stringa dall'enumerazione FORMAT
        */
        Date(std::string date, int format);

        /**
         * @brief Costruttore rapido per la data
         * @param dd giorno
         * @param mm mese
         * @param yy anno
         * @param format formato dall'enum FORMAT
        */
        Date(int dd, int mm, int yy, int format = DD_MM_YYYY);

        std::string toString();
        bool operator<(Type &type) override;
        bool operator>(Type &type) override;
        bool operator==(Type &type) override;
    };

    /**
     * @brief Classe per la rappresentazione dle tempo
    */
    class Time : public Type
    {
    protected:
        int h, m, s, ms;
    public:
        /**
         * @brief Classe per le eccezione della classe Time
        */
        class Bad_Time : std::exception
        {
        public:
            Bad_Time(std::string message) : youWillHaveABadTime{message} { std::cout << "Bad Time Exception: " << message << std::endl; }
            std::string youWillHaveABadTime;
        };
        /**
         * @brief Costruttore rapido della classe time
         * @param h ora
         * @param m minuti
         * @param s secondi
         * @param ms millisecondi
        */
        Time(int h = 0, int m = 0, int s = 0, int ms = 0);

        /**
         * @brief Costruttore per la conversione di una stringa in formato tempo
         * @param time stringa rappresentante il tempo
         * é possibile non specificare sempre il formato ora:minuti:secondi.millisecondi
         * il costruttore si occupera di parsificare la stringa e di impostare solo i valori passati
         * per tutti gli altri si assumerá siano 0
        */
        Time(std::string time);
        std::string toString();
        bool operator>(Type &type);
        bool operator<(Type &type);
        bool operator==(Type &type);
    };

    template <class T, typename K>

    /**
     * @brief Classe template per le coppie di valori Chiave-Valore
     * @tparam T Tipo del valore
     * @tparam K Tipo della chiave
    */
    class Pair
    {
    public:
        T value;
        K key;
        Pair(T value, K key) : value{value}, key{key} {}
    };

} // namespace WellDoneDB
