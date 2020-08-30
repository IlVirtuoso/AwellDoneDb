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
    Database db("hello za warudo!");
    db.createTable("times");
    db["times"]->createColumn("Time", Types::TIME);
    db["times"]->createColumn("Dates", Types::DATE);
    for (int i = 0; i < 100; i++) {
        db["times"]->addRow({ generateTime()[0],generateDate()[0] },db["times"]->getColNames());
    }
    Selection sel(*(*db["times"])["Time"], Conditions::GREATEREQTHAN, new Time("10:02"));
    Selection sel2(*(*db["times"])["Dates"], Conditions::LESSEQTHAN, new Date("20-11-1997", Date::DD_MM_YYYY));
    Selection between(*(*db["times"])["Time"], new Time("10:02"), new Time("13:04"));
    cout << db["times"]->project({ "Dates" })->select(between)->toString() << endl;
} 