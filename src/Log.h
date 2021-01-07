#pragma once
#include <iostream>
using namespace std;

class Log {
public:
    Log() : cost_time(0.0), search_count(0) {};

    void printLog() {
        cout << "Total time cost: " << cost_time << " ms.\n";
        cout << "Total search count: " << search_count << " times.\n\n";

    }

    double cost_time;
    int search_count;

}; 