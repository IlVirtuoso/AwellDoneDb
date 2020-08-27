#include <iostream>
#include "include/Database.hpp"

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
        dates.push_back(new Date{rand()%28,rand()%12,rand()%4000});
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
    VectorizedTable* t1 = new VectorizedTable("site");
    t1->createColumn("Date", Types::DATE);
    t1->createColumn("Time", Types::TIME);
    VectorizedTable* t2 = new VectorizedTable("ids");
    Database db("db");
    t2->createColumn("Date", Types::DATE);
    t2->createColumn("IDs", Types::INT);
    auto dates = generateDate();
    auto times = generateTime();
    auto ints = generateInt();
    for (int i = 0; i < 100; i++) {
        t1->addRow({ dates[rand() % 100],times[rand() % 100] }, { "Date","Time" });
        t2->addRow({ dates[rand() % 100], ints[rand() % 100] }, { "Date", "IDs" });
    }
    Selection sel(*t1->operator[]("Date"), Conditions::LESSTHAN, new Date("20-11-1997",Date::DD_MM_YYYY));
    Selection sel2(*t1->operator[]("Date"), Conditions::GREATHERTHAN, new Date("20-11-1997", Date::DD_MM_YYYY));
    t1->removeRow(3, false);
    cout << t1->toString() << endl;
}