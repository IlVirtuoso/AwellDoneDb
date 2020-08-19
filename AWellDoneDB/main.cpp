#include <iostream>
#include "./include/AWellDoneDB.hpp"

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
    Column colTime("Times","site",Types::TIME,generateTime());
    Column colDate("Dates", "site", Types::DATE,generateDate());
    Time t1(10,01);
    Date d1("20-11-1997",Date::DD_MM_YYYY);
    Column selectedColTime = colTime.select(Conditions::GREATEREQTHAN,t1);
    Column selectedDate = colDate.select(selectedColTime.getPositions());
    cout<<selectedDate.toString() << selectedColTime.toString()<<endl;
      
}