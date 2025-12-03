#pragma once
#include <string>
#include <vector>

#define EVENTNAME 99

struct Task {
    char name[EVENTNAME];
    int duration;
    int done;
    int priority;
    int day;
    int month;
    int year;

};

struct SingleDate {
    int day;
    int month;
    int year;

};

    void mainMenu();
    SingleDate getTodaysDate();
    void showToDolist(SingleDate);
    void toDolist();



    Task readLineToTask();

    void writeTaskToLine(Task task);


