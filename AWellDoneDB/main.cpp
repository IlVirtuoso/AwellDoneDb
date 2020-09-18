#include <iostream>
#include "include/Database.hpp"
#include "include/XmlParser.hpp"
#include "include/SQLParser.hpp"
#include <fstream>

using namespace std;
using namespace WellDoneDB;

std::vector<Type*> generateInt(){
    vector<Type*> integers;
    for(int i = 0 ; i < 1000; i++){
        integers.push_back(new Integer(rand()%10000));
    }
    return integers;
}

vector<Type*> generateDate(){
    vector<Type*> dates;
    for(int i = 0 ; i < 1000; i++){
        dates.push_back(new Date{rand()%27 + 1,rand()%11 + 1,rand()%4000});
    }
    return dates;
}

vector<Type*> generateTime(){
    vector<Type*> time;
    for(int i = 0 ; i < 100; i++){
        time.push_back(new Time(rand()%24,rand()%60,rand()%60,rand()%9999));
    }
    return time;
}

int test() {
    Database db("dance");
    db.createTable("id");
    db["id"]->createColumn("ids", Types::INT);
    db["id"]->createColumn("dates", Types::DATE);
    for (int i = 0; i < 1000; i++) {
        db["id"]->addRow({ generateInt()[rand() % 100],generateDate()[rand() % 100] }, { "ids","dates" });
    }
    Selection sel1(*db["id"]->get("ids"), Conditions::LESSEQTHAN, (Type*)new Integer(100));
    Selection sel2(*db["id"]->get("dates"), Conditions::GREATEREQTHAN, new Date("20-11-1997",Date::DD_MM_YYYY));
    cout << db["id"]->select(sel1 || sel2)->sort("ids",true)->toString() << endl;
    return 1;
}

int main(int argc, char* argv[]) {
    std::string command = "";
    Database* db = new Database("default");
    db->loadXml();
    while (true) {
        cout << std::endl;
        cout << "<WellDoneDB>";
        getline(cin, command);
        if (command == "QUIT" || command == "quit" || command == "quit()" || command == "QUIT()") {
            db->save();
            cout << "saving and exit program" << endl;
            exit(1);
        }
        else if (command != "") {
            SQLParser parser(command, db);
        }
    }
}
