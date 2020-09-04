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

int main(int argc, char * argv[]){
    Database* db = new Database();
    db->createTable("times");
    db->get("times")->createColumn("id", Types::INT);
    db->get("times")->createColumn("date", Types::DATE);
    for (int i = 0; i < 100; i++) {
        db->get("times")->addRow({ generateInt()[0], generateDate()[0] }, { "id","date" });
    }
    SQLParser parser("SELECT * FROM times WHERE id BETWEEN 100 AND 1000 ORDER BY id ASC;",db);
} 