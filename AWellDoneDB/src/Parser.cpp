#include "../include/Parser.h"
#include <algorithm>
#include <cctype>
#include <string>

string tolower(string original) {
    string newstr;
    for (int i = 0; i < original.size(); i++)
        newstr.push_back(tolower(original[i]));
    return newstr;
}


void* retrieveValue(dataType type,string data) {
    switch (type)
    {
    case dataType::INT:
        return new int(atoi(data.c_str()));
    case dataType::TEXT:
        return new string(data);
    case dataType::CHAR:
        return new char(data.at(0));
    case dataType::DATE:
        return new Date(data);
    case dataType::TIME:
        return new Time(data);
    case dataType::FLOAT:
        return new float(atof(data.c_str()));
    default:
        break;
    }
}

dataType checkDataType(string data) {
    if (data.at(0) == '\'') {
        if(data[1] >= '0' && data[1] <= '9') {
            if (data.length() == 12 && data[3] == '-' && data[6] == '-')
                return dataType::DATE;
            else if (data.length() == 10 && data[3] == ':' && data[6] == ':')
                return dataType::TIME;
        }
        else if (data.length() == 3 && data[2] == '\'')
            return dataType::CHAR;
        else if(data[data.length()-1] == '\'')
            return dataType::TEXT;
    }
    if (data[0] >= '0' && data[0] <= '9') {
        for (int i = 1; i < data.size(); i++)
            if (data[i] == '.')
                return dataType::FLOAT;
        return dataType::INT;
    }
    return dataType::DATAINVAL;
}

dataType findType(string type) {
    if (type.compare("INT") == 0)
        return dataType::INT;
    else if (type.compare("TEXT") == 0)
        return dataType::TEXT;
    else if (type.compare("FLOAT") == 0)
        return dataType::FLOAT;
    else if (type.compare("DATE") == 0)
        return dataType::DATE;
    else if (type.compare("TIME") == 0)
        return dataType::TIME;
    else if (type.compare("CHAR") == 0)
        return dataType::CHAR;
    return dataType::DATAINVAL;
}

parsedCondition::parsedCondition(string attrName, string value) {
    this->attrName = attrName;
    this->value = value;
}

Parser::Parser(Database* db) {
    this->db = db;
}

bool Parser::checkTable(string table){
    return this->db->exists(table);
}

bool Parser::checkAttribute(string attribute, string table){
    if (checkTable(table))
        if (this->db->getTable(table)->exists(attribute))
            return true;
    return false;
}

bool Parser::checkAttrTemp(string attribute, string table){
    if(this->temporary->name.compare(table) == 0)
        return this->temporary->exists(attribute); 
    return false;
}

bool checkBond(string bond){
    if(bond.compare("not null") == 0 || bond.compare("auto_increment") == 0 )
       return true;
    else return false;
}

//Salta gli spazi
int Parser::skipSpace(string s, int p){
    while(p < s.length() &&  s[p] == ' ' || 
          s[p] == '\t' || s[p] == '\n')
        p++;
    return p;
}


int Parser::parseCondition(string query, int p, string tab){
    string peek;
    string attribute = "";
    p = skipSpace(query, p);
    while(p < query.length() && query[p] != ' ' && query[p] != '\n' &&
    query[p] != '\t' && query[p] != '=' && query[p] != ';'){
        attribute = attribute + query[p]; 
        p++;
    }
    p = skipSpace(query, p);
    if (checkAttribute(attribute, tab)) {
        //Aggiungi attributo e tab alla struttura vincolo
        if(query[p] == '='){
            p = skipSpace(query, p+1);
            bool text = false;
            peek = "";
            if(query[p] == '\''){ peek = "\'"; p++; text = true;}
            while(p < query.length() && ((query[p] != ';' && query[p] != ',' &&
                    query[p] != 'o' && query[p] != 'w') || text)){
                if(text){
                    if(query[p] == '\\'){
                        p++;
                        peek = peek + query[p];
                    }else{ 
                        peek = peek + query[p];}
                    if(query[p-1] != '\\' && peek[peek.length() - 2] != '\\'){
                        if(query[p] == '\''){
                            p++;
                            text = false;
                            break;
                        }
                    }
                }else{ peek = peek + query[p]; }
                    p++;
            }
            //Aggiungi valore peek (ovvero il valore) alla struttura vincolo
            parsedConditions.push_back(parsedCondition(attribute, peek ));
            return p;
        }else {cout<<"ERROR: bad condition "<<endl; return -1;}
    }else{cout<<"ERROR: bad attribute '"+peek+"' in table '"+tab+"'"<<endl;return -1;}
}


bool Parser::parseCreate(string query){
    dataType datatype;
    bool success = false;
    bool notNull = false;
    bool autoincrement = false;
    bool checkPK = false;
    string peek, error, newTab, attribute, type;
    error = "ERROR: ";
    int p = skipSpace(query, 0);
    int state = 0;
    if(5 < query.length()&&  
      (peek = tolower(query.substr(p, 5))).compare("table") != 0){
        cout<<error<<"sintax error, expected 'table' after 'create'"<<endl;
        return false;
    }
    p = skipSpace(query, p + 5);
    while(p < query.length() && query[p] != ' ' && query[p] != '\n' &&
    query[p] != '\t' && query[p] != '('){
        newTab = newTab + query[p]; 
        p++;
    }
    if(checkTable(newTab)){
        cout<< "ERROR: Duplicate name for table: "<<newTab<<endl;
        return false;
    }
    this->temporary = new Table(newTab);
    p = skipSpace(query, p);
    while(p < query.length() && !success && state != -1){
        switch(state){
            // (
            case 0:
                if(query[p] == '('){ state = 1; p++; }
                else state = -1;
            break;
            // Nome attributo
            case 1:
                peek = "";
                while(p < query.length() && query[p] != ' ' 
                && query[p] != '\n' && query[p] != '\t'){
                    attribute = attribute + query[p]; 
                    p++;
                }
                if(!(checkAttrTemp(attribute, newTab)) ){ 
                    // Attributo inserito nella tabella provvisoria 
                    state = 2;  
                }else
                {
                    state = -1;
                    error = error + "Uncorrect name for attribute '" + peek +"'\n";   
                }        
            break;
            // Tipo
            case 2:
                type = "";
                while(p < query.length() && query[p] != ' ' 
                && query[p] != '\n' && query[p] != '\t' && query[p] != ','){
                    type = type + query[p]; 
                    p++;
                }   
                if((datatype = findType(type)) != dataType::DATAINVAL){ 
                    // Tipo inserito nella tabella provvisoria
                    state = 3;
                }else{
                    state = -1;
                    error = error + "Types '"+ peek + "' doesn't exists";   
                }
            break;

            //Vincolo
            case 3:
                if(query[p] == ','){ p++; state = 4; }
                else{
                    while(p < query.length() && query[p] != ' ' && query[p] != ')'
                    && query[p] != '\n' && query[p] != '\t' && query[p] != ','){
                        peek = peek + query[p]; 
                        p++;
                    } 
                    if((tolower(peek)).compare("not") == 0){
                        peek = peek + " ";
                    }else if(checkBond(peek)){ 
                        if((tolower(peek)).compare("not null") == 0)
                            notNull = true;
                        if(peek.compare("auto_increment") == 0)
                            autoincrement = true;
                        peek.clear();
                        // Vincolo associato all'attributo nella tabella provvisoria  
                    }else{
                        state = -1;
                        error = error + "Bond '"+ peek + "' doesn't exists";   
                    }
                }
            break;
            // Primary Key
            case 4:
                if(peek != ""){error = error + "Bond '"+ peek + "' doesn't exists"; state = -1; break;}
                this->col = new Column(datatype, attribute,false,false);
                this->col->setNotNull(notNull);
                this->col->setAutoIncrement(autoincrement);
                this->temporary->addColumn(col);
                attribute.clear();
                while(p < query.length() && query[p] != ' ' &&  
                query[p] != '\n' && query[p] != '\t'){
                    attribute = attribute + query[p]; 
                    p++;
                }
                if((tolower(attribute)).compare("primary") == 0){
                    p = skipSpace(query, p);
                    peek = tolower(query.substr(p,3));
                    if(peek.compare("key") == 0){
                        peek = "";
                        p = skipSpace(query, p + 3);
                        if(query[p] == '('){
                            p++;
                            peek = "";
                            while(p < query.length() && query[p] != ' ' 
                                 && query[p] != '\n' && query[p] != '\t' && query[p] != ')'){
                                peek = peek + query[p]; 
                                p++;
                            }
                            p = skipSpace(query, p);
                            if(query[p] == ')'){
                                if((checkAttrTemp(peek, newTab)) && !checkPK){
                                    checkPK = true;
                                    //Aggiungi il vincolo primary key
                                    this->temporary->getColumn(peek)->isPrimaryKey = checkPK;
                                    p = skipSpace(query, p + 1);
                                    if(query[p] == ','){
                                        p = skipSpace(query, p+1);
                                        state = 5;
                                    }else{
                                        state = 6;}  
                                }else{error = error + "attribute '"+peek+"' can't be primary key"; state = -1;}
                            }else{error = error + "sintax error, expected ')' after attribute"; state = -1;}                               
                        }else{error = error + "sintax error, expected '(' after 'primary key'"; state = -1;}
                    }else{error = error + "sintax error, expected 'key' after 'primary'"; state = -1;}
                }else{
                     if(!(checkAttrTemp(attribute, newTab))){ 
                        // Attributo inserito nella tabella provvisoria 
                        state = 2;  
                    }else{
                        state = -1;
                        error = error + "Uncorrect name for attribute '"+ peek +"'";   
                    }
                }
            break;
            //Foreign Key
            case 5:
               peek = tolower(query.substr(p, 7));
               if(peek.compare("foreign") == 0){
                   p = skipSpace(query, p + 7);
                   peek = tolower(query.substr(p,3));
                   if(peek.compare("key") == 0){
                        p = skipSpace(query, p + 3);
                        if(query[p] == '('){
                           p = skipSpace(query, p + 1);
                           attribute.clear();
                           while(p < query.length() && query[p] != ' ' 
                                 && query[p] != '\n' && query[p] != '\t' && query[p] != ')'){
                                attribute = attribute + query[p]; 
                                p++;
                            }
                            p = skipSpace(query, p);
                            if((checkAttrTemp(attribute, newTab)) && query[p] == ')'){
                                p = skipSpace(query, p + 1);
                                peek = tolower(query.substr(p, 10));
                                if(peek.compare("references") == 0){
                                    p = skipSpace(query, p + 10);
                                    peek = "";
                                    while(p < query.length() && query[p] != ' ' &&
                                    query[p] != '\n' && query[p] != '\t' && query[p] != '('){
                                        peek = peek + query[p]; 
                                        p++;
                                    }
                                    string refTable = peek;
                                    p = skipSpace(query,p);
                                    if(checkTable(refTable) && query[p] == '('){
                                        p = skipSpace(query, p + 1);
                                        peek ="";
                                        while(p < query.length() && query[p] != ' ' &&
                                        query[p] != '\n' && query[p] != '\t' && query[p] != ')'){
                                            peek = peek + query[p]; 
                                            p++;
                                        }
                                        p = skipSpace(query,p);
                                        if(checkAttribute(peek, refTable) && query[p] == ')'){
                                            p = skipSpace(query, p + 1);
                                            //Aggiungi vincolo d'integrità referenziale
        
                                            this->tempkeys.push_back(ExternKey(newTab, refTable, attribute, peek));
                                            
                                            if(query[p] == ','){
                                                p++;
                                                state = 5;
                                            }else{
                                                state = 6;}
                                        }else{error = error + "attribute '" + peek + "' in table '"+refTable+"' can't be refered"; state = -1;} 
                                    }else{error = error + "table '" +refTable +"' doesn't exists"; state = -1;} 
                                }else{error = error + "sintax error, expected keyword 'references' instead of '"+peek+"'"; state = -1;}
                            }else{error = error + "sintax error, uncorrect name for attribute"; state = -1;}
                       }else{error = error + "sintax error, expected '(' after key"; state = -1;}
                   }else{error = error + " expected 'key' after foreign"; state = -1;}
                }else{error = error + "expected 'foreign'"; state = -1;}
            break;
            // );
            case 6:
                 if(query[p] == ')'){
                    p = skipSpace(query,p+1);
                    if(query[p] == ';' && p == (query.length() - 1)){
                        success = true;
                    }else{error = error + "expected ';' at the end of query"; state = -1;}
                }else{error = error + "expected ')' to close the query"; state = -1;}
            break;
        }
        p = skipSpace(query, p);
    }
    if(success){
        // Aggiungi struttura provvisoria al database
        this->db->create(this->temporary);
        for (int i = 0; i < this->tempkeys.size(); i++) {
            this->db->addReference(this->tempkeys.at(i));
        }
        cout<<"SUCCESS: table '"<<newTab<<"' adds in database"<<endl;
        return true;
    }else{
        // Ignora la struttura provvisoria
        if(error.compare("ERROR: ") == 0) 
            cout<<error<<"sintax error"<<endl;
        else
            cout<<error<<endl;    
        return false;
    }
}


bool Parser::parseDrop(string query){
    int p = skipSpace(query,0);
    string peek, dropTab;
    dropTab = "";
    if(p + 5 < query.length() &&
        (peek = tolower(query.substr(p, 5))).compare("table") == 0){
        p = skipSpace(query, p + 5);
        while(p < query.length() && query[p] != ' ' && query[p] != '\n' &&
        query[p] != '\t' && query[p] != ';'){
            dropTab = dropTab + query[p]; 
            p++;
        }
        p = skipSpace(query, p);
        if(checkTable(dropTab)){
            if(p == (query.length()-1) && query[p] == ';'){
                
                // Cancella la tabella
                bool success = this->db->drop(dropTab);
                
                cout << "SUCCESS: table '" << dropTab << "' erased from database" << endl;

                return success;
            }else{cout<<"ERROR: unexpected element before ';"<<endl; return false;}
        }else{cout<<"ERROR: table '"<<dropTab<<"' doesn't exists"<<endl; return false;}
    }else{cout<<"ERROR: expected 'table' after drop"<<endl; return false;}
}




bool Parser::parseTruncate(string query){
    int p = skipSpace(query, 0);
    string peek, trunTab;
    trunTab = "";
    if(p + 5 < query.length() &&
        (peek = tolower(query.substr(p, 5))).compare("table") == 0){
        p = skipSpace(query, p + 5);
        while(p < query.length() && query[p] != ' ' && query[p] != '\n' &&
        query[p] != '\t' && query[p] != ';'){
            trunTab = trunTab + query[p]; 
            p++;
        }
        p = skipSpace(query, p);
        if(checkTable(trunTab)){
            if(p == (query.length()-1) && query[p] == ';'){
            //Svuota la tabella
            this->db->truncate(trunTab);
            cout<<"SUCCESS: now table '"<<trunTab<<"' is empty "<<endl;
            return true;
            }else{cout<<"ERROR: unexpected element before ';"<<endl; return false;}
        }else{cout<<"ERROR: table '"<<trunTab<<"' doesn't exists"<<endl; return false;}
    }else{cout<<"ERROR: expected 'table' after 'truncate'"<<endl; return false;} 
}


bool Parser::parseInsert(string query){
    vector<string> attrs;
    vector<string> values;
    int p = skipSpace(query, 0);
    string peek, tab;
    tab = "";
    if(p + 5 < query.length() &&
      (peek = tolower(query.substr(p, 5))).compare("into ") == 0){
          p = skipSpace(query, p + 5);
          while(p < query.length() && query[p] != ' ' && query[p] != '\n' &&
          query[p] != '\t' && query[p] != '('){
            tab = tab + query[p]; 
            p++;
        }
        p = skipSpace(query, p);
        if(checkTable(tab)){
            if(query[p] == '('){
                p = skipSpace(query, p+1);
                while(p < query.length() && query[p] != ')'){
                    peek = "";
                    while(p < query.length() && query[p] != ' ' && query[p] != '\n' &&
                    query[p] != '\t' && query[p] != ',' && query[p] != ')'){
                        peek = peek + query[p]; 
                        p++;
                    }
                    if(checkAttribute(peek, tab)){
                        // Aggiungi attributo alla coda di inserimento
                        attrs.push_back(peek);
                    }else{cout<<"bad attribute: '"+peek+"' in table '"+tab+"'";return false;}
                    p = skipSpace(query, p);
                    if(query[p] == ','){ p = skipSpace(query, p+1); }
                }
                p = skipSpace(query, p + 1);
                    if(p + 6 < query.length() &&
                    (peek = tolower(query.substr(p, 6))).compare("values") == 0){
                        p = skipSpace(query, p + 6);
                        if(query[p] == '('){
                            bool text = false;
                            while(p < query.length() && query[p] != ')'){
                                peek = "";
                                p = skipSpace(query, p+1);
                                if(query[p] == '\''){ peek = "\'"; p++; text = true;}
                                while(p < query.length() && ((query[p] != ',' && query[p] != ')') || text)){
                                    if(text){
                                        if(query[p] == '\\'){
                                            p++;
                                            peek = peek + query[p];
                                        }else{ 
                                            peek = peek + query[p];}
                                        if(query[p-1] != '\\' && peek[peek.length() - 2] != '\\'){
                                            if(query[p] == '\''){
                                                text = false;
                                                break;
                                            }
                                        }
                                    }else{ peek = peek + query[p]; }
                                        p++;
                                }
                                if(peek != ""){ 
                                    /*Inserisci peek in coda di valorizzazione;*/
                                    values.push_back(peek);
                                }     
                            }
                            if(query[p] == ')'){
                                p = skipSpace(query, p+1);
                                if(p == (query.length() - 1) && query[p] == ';'){
                                    // Procedimento di inserimento con relativi check
                                    if(attrs.size() == values.size()){
                                        vector<Attribute*> attrib;
                                        for (int i = 0; i < values.size(); i++) {
                                            dataType type = checkDataType(values.at(i));
                                            if (type == dataType::DATAINVAL) {
                                                cout<<"ERROR: invalid type for "<<values.at(i)<<endl;
                                                return false;
                                            }
                                            void* value = retrieveValue(type, values.at(i));
                                            attrib.push_back(new Attribute(type,value));
                                        }
                                        bool success = this->db->getTable(tab)->addRow(attrs, attrib);
                                        cout<<"SUCCESS: row successfully insert in "<<tab<<endl;
                                        return success;
                                    }else{cout<<"ERROR: failed correspondence between lists of attributes and values"<<endl; return false;}
                                }else{cout<<"ERROR: unexpected element before ;"<<endl; return false;}
                            }else{cout<<"ERROR: expected ')' to close the list of values"<<endl; return false;}
                        }else{cout<<"ERROR: expected '(' after values"<<endl; return false;}
                    }else{cout<<"ERROR: expected 'values' after list of attributes"<<endl; return false;}
            }else{cout<<"ERROR: expected '(' after '"+tab+"'"<<endl; return false;}
        }else{cout<<"ERROR: table '"<<tab<<"' doesn't exists"<<endl; return false;}
    }else{cout<<"ERROR: expected 'into' after 'insert'"<<endl; return false;}

}




bool Parser::parseDelete(string query){
    int p = skipSpace(query, 0);
    string peek, tab;
    tab = "";
    if(p + 5 < query.length() &&
      (peek = tolower(query.substr(p, 5))).compare("from ") == 0){
       p = skipSpace(query, p + 5);
          while(p < query.length() && query[p] != ' ' && query[p] != '\n' &&
          query[p] != '\t'){
            tab = tab + query[p]; 
            p++;
        }
        p = skipSpace(query, p);
        if(checkTable(tab)){
            if(p + 6 < query.length() &&
            (peek = tolower(query.substr(p, 6))).compare("where ") == 0){
                p = skipSpace(query, p+6);
                if((p = parseCondition(query, p, tab)) == -1){return false;}
                p = skipSpace(query, p);
                if(p == (query.length()-1) && query[p] == ';'){
                    //Esegui cancellazione
                    dataType type = checkDataType((parsedConditions[0].value));
                    if (type == dataType::DATAINVAL) {
                        cout<<"ERROR: invalid type for "<<parsedConditions[0].value <<endl;
                        return false;
                    }
                    void* value = retrieveValue(type, parsedConditions[0].value);
                    this->db->Delete(tab,parsedConditions[0].attrName, conditions::EQUAL,value);
                    cout<<"SUCCESS"<<endl;
                    return true;
                }else{cout<<"ERROR: unexpected element before ;"<<endl; return false;}
            }else{cout<<"ERROR: expected 'where' after table"<<endl; return false;}
        }else{cout<<"ERROR: table '"+tab+"' doesn't exits"<<endl; return false;}
    }else{cout<<"ERROR: expected 'from' after 'delete'"<<endl; return false;}

}




bool Parser::parseUpdate(string query){
    int p = skipSpace(query, 0);
    string peek, tab;
    tab = "";
    while(p < query.length() && query[p] != ' ' && query[p] != '\n' &&
          query[p] != '\t' && query[p] != '('){
            tab = tab + query[p]; 
            p++;
        }
    p = skipSpace(query, p);
    if(checkTable(tab)){
        if(p + 3 < query.length() &&
            (peek = tolower(query.substr(p, 3))).compare("set") == 0){
            p = skipSpace(query,p+3);
            bool endList = false;
            while(p < query.length() && !endList){
                if((p = parseCondition(query, p, tab)) == -1) return false;
                p = skipSpace(query, p);
                if(query[p] != ','){
                    endList = true;
                }else{p++;}
                p = skipSpace(query,p);
            }
            if(p + 5 < query.length() &&
            (peek = tolower(query.substr(p, 5))).compare("where") == 0){
                p = skipSpace(query, p+5);
                if((p = parseCondition(query, p, tab)) == -1) return false;
                p = skipSpace(query, p);
                if(p == (query.length()-1) && query[p] == ';'){
                    // Aggiorna la tabella    
                    int attrList = parsedConditions.size() -1;                
                    string referenceColumn = parsedConditions[attrList].attrName;
                    void* referenceValue = retrieveValue(checkDataType(parsedConditions[attrList].value), parsedConditions[attrList].value);
                    for(int i = 0 ; i < attrList; i++){
                        dataType type = checkDataType(parsedConditions[i].value);
                        void* value = retrieveValue(type, parsedConditions[i].value);
                        this->db->update(tab, parsedConditions[i].attrName, referenceColumn, conditions::EQUAL, referenceValue,type,value);
                    }
                    cout<<"SUCCESS: Update completed"<<endl;
                    return true;
                }else{cout<<"ERROR: unexpected element before ;"<<endl; return false;}
            }else{cout<<"ERROR: expected 'where' after update list instead of "<<peek<<endl; return false;}
        }else{cout<<"ERROR: expected 'set' after table"<<endl; return false;}
    }else{cout<<"ERROR: table '"<<tab<<"' doesn't exists"<<endl; return false;}
}




bool Parser::parseSelect(string query){
    int p = skipSpace(query, 0);
    string peek, tab;
    bool stampAll = false;
    bool endList = false;
    bool where = false;
    bool order = false;
    bool asc = false;
    tab = "";
    peek = "";
    if(query[p] != '*'){
        while(p < query.length() && !endList){
            peek = "";
            while(p < query.length() && query[p] != ' ' && query[p] != '\n' &&
                query[p] != '\t' && query[p] != ','){
                    peek = peek + query[p]; 
                    p++;
                }
            // Aggiungi peek alla coda di attributi da stampare
            this->selected.push_back(peek);
            p = skipSpace(query, p);
            if(query[p] != ','){
                endList = true;
            }else{p++;}
            p = skipSpace(query,p);
        }
    }else{ stampAll = true; p = skipSpace(query, p+1);}
    if(p + 4 < query.length() &&
        (peek = tolower(query.substr(p, 4))).compare("from") == 0){ 
        p = skipSpace(query, p+4);
        while(p < query.length() && query[p] != ' ' && query[p] != '\n' &&
            query[p] != '\t' && query[p] != ';'){
                tab = tab + query[p]; 
                p++;
            }
        p = skipSpace(query, p);
        if(checkTable(tab)){
            for(int i = 0; i < this->selected.size(); i++){
                if(!checkAttribute(selected[i], tab)){
                    cout<<"ERROR: Attribute "<<selected[i]<<" not found"<<endl;
                    return false;
                }
            }
            if(p == (query.length()-1) && query[p] == ';'){
                //procede a stampa senza condizioni
                if(stampAll){
                    cout << this->db->getTable(tab)->toString() << endl;
                }else{
                    cout << this->db->getTable(tab)->project(selected)->toString() << endl;
                }
                return true;
            }else{
                // Where condition
                if(query[p] == 'w'){
                    where = true;
                    if(p + 5 < query.length() &&
                    (peek = tolower(query.substr(p, 5))).compare("where") == 0){ 
                        p = skipSpace(query, p+5); 
                        if((p = parseCondition(query, p, tab)) == -1) return false;
                        p = skipSpace(query, p);
                        if(p == (query.length()-1) && query[p] == ';'){
                            string referenceColumn = this->parsedConditions[0].attrName;
                            void* data = retrieveValue(checkDataType(this->parsedConditions[0].value), this->parsedConditions[0].value);
                            if(stampAll){
                                cout<<this->db->getTable(tab)->select(referenceColumn, conditions::EQUAL, data)->toString() << endl;
                            }else{
                                cout << this->db->getTable(tab)->select(referenceColumn, conditions::EQUAL, data)->project(this->selected)->toString() << endl;
                            }
                            return true;
                            }
                    }else{cout<<"ERROR: expected 'where' after table"<<endl; return false;}
                }
                // Order by condition
                if(query[p] == 'o'){
                    order = true;
                    if(p + 5 < query.length() &&
                    (peek = tolower(query.substr(p, 5))).compare("order") == 0){
                        p = skipSpace(query, p+5);
                        if(p + 2 < query.length() &&
                        (peek = tolower(query.substr(p, 2))).compare("by") == 0){
                            p = skipSpace(query, p+2);
                            peek = "";
                            while(p < query.length() && query[p] != ' ' && query[p] != '\n' &&
                            query[p] != '\t' && query[p] != ';'){
                                peek = peek + query[p]; 
                                p++;
                            }
                            p = skipSpace(query, p);
                            if(checkAttribute(peek, tab)){
                                if((p + 3 < query.length()) && (tolower(query.substr(p,3)) == "asc")){ p = skipSpace(query, p+3); asc = true;}
                                else if((p + 4 < query.length()) && (tolower(query.substr(p,4)) == "desc")){p = skipSpace(query, p+4); asc = false;}
                                else{cout<<"ERROR: bad type of order: "<<endl; return false;} 

                                if(p == (query.length()-1) && query[p] == ';'){
                                    //procede a stampa con condizione e ordinamento
                                   
                                    string referenceColumn = this->parsedConditions[0].attrName;
                                    void* data = retrieveValue(checkDataType(this->parsedConditions[0].value), this->parsedConditions[0].value);
                                    if (stampAll) {
                                        this->db->getTable(tab)->sort(peek, !asc);
                                        cout << this->db->getTable(tab)->select(referenceColumn, conditions::EQUAL, data)->toString() << endl;
                                        this->db->getTable(tab)->resetAttributePositions();
                                    }
                                    else {
                                        this->db->getTable(tab)->sort(peek, !asc);
                                        cout << this->db->getTable(tab)->select(referenceColumn, conditions::EQUAL, data)->project(this->selected)->toString() << endl;
                                        this->db->getTable(tab)->resetAttributePositions();
                                    }
                                    return true;
                                }else{cout<<"ERROR: unexpected element before ;"<<endl; return false;}
                            }else{cout<<"ERROR: bad attribute: '"+peek+"' in table '"+tab+"'"; return false;}                            
                        }else{cout<<"ERROR: expected 'by' after order"<<endl; return false;}
                    }else{cout<<"ERROR: expected 'order' instead of '"<<peek<<"'"<<endl; return false;}
                }
                cout<<"ERROR: unexpected element after 'from' condition"<<endl; return false;
            }
        }else{cout<<"ERROR: table '"<<tab<<"' doesn't exits"<<endl; return false;}
    }else{cout<<"ERROR: expected 'from' after list of attributes instead of"<<peek<<endl; return false;}
}



bool Parser::parseQuit(string query){
    int p = skipSpace(query, 0);
    if((query.substr(p,2)).compare("()")== 0){
        p = skipSpace(query, p+2);
        if(p == (query.length()-1) && query[p] == ';'){
            cout<<"Saving database"<<endl;
            this->db->exit();
            cout<<"SUCCESS"<<endl;
            exit(1);
        }else{cout<<"ERROR: unexpected element before ';"<<endl;}
    }else{cout<<"ERROR: unexpected element after 'quit'"<<endl;}
}


bool Parser::scanner(string query)
{
    int op = -1;
    string subquery;
    if(4 < query.length() && op == -1){
        subquery = (tolower(query.substr(0,4)));
        if(subquery.compare("drop") == 0) op = 1;
        else if(subquery.compare("quit") == 0) op = 7;
        subquery = query.substr(4, query.length() - 4);
    }
    if(6 < query.length() && op == -1){
        subquery = (tolower(query.substr(0,6)));
        if(subquery.compare("create") == 0) op = 0;
        else if(subquery.compare("insert") == 0) op = 2;
        else if(subquery.compare("delete") == 0) op = 3;
        else if(subquery.compare("update")== 0) op = 5;
        else if(subquery.compare("select") == 0) op = 6;
        subquery = query.substr(6, query.length() - 6);
    }
    if(8 < query.length() && op == -1){
        subquery = (tolower(query.substr(0,8)));
        if(subquery.compare("truncate") == 0) op = 4;
        subquery = query.substr(8, query.length() - 8);
    }
    if(op == -1){ cout<<"ERROR: unknown operation for '"+subquery+"'"<<endl; return false;} 
    parsedConditions.clear();
    switch(op){
        case 0: return parseCreate(subquery); break;
        case 1: return parseDrop(subquery); break;
        case 2: return parseInsert(subquery); break;
        case 3: return parseDelete(subquery); break;
        case 4: return parseTruncate(subquery); break;
        case 5: return parseUpdate(subquery); break;
        case 6: return parseSelect(subquery); break;
        case 7: return parseQuit(subquery); break;
    }
}

