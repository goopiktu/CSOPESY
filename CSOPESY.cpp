// CSOPESY.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
using namespace std;

void Initialize() {
    cout << "initializing...\n";
}

void Screen() {
    cout << "screen...\n";
}

void SchedulerTest() {
    cout << "scheduler-test...\n";
}

void SchedulerStop() {
    cout << "scheduler-stop...\n";
}

void ReportUtil() {
    cout << "report-util...\n";
}

int main()
{
    bool running = true;
    string input = "";

    string green = "\033[32m";
    string reset = "\033[0m";
    string yellow = "\033[38;5;229m";



    cout << R"(
         ___________ ____  ____  _____________  __
        / ____/ ___// __ \/ __ \/ ____/ ___/\ \/ /
       / /    \__ \/ / / / /_/ / __ / \__ \  \  /
      / /___ ___/ / /_/ / ____/ /___ ___/ /  / /
      \____//____/\____/_/   /_____//____/  /_/                                                                                                                      
    )" << std::endl;
    
    cout << green << "Hello, Welcome to CSOPESY commandline!\n" << reset;
    cout << yellow << "Type 'exit' to quit, 'clear' to clear the screen\n" << reset;
    while (running) {
        
        cout << "Enter a command: ";
        cin >> input;


        if (input == "exit") {
            running = false;
        }
        else if (input == "initialize") {
            Initialize();
        }
        else if (input == "screen") {
            Screen();
        }
        else if (input == "scheduler-test") {
            SchedulerTest();
        }
        else if (input == "scheduler-stop") {
            SchedulerStop();
        }
        else if (input == "report-util") {
            ReportUtil();
        }
        else if (input == "clear") {
            system("CLS");
            cout << "cleared!\n";
        }
        else {
            cout << "unrecognized command please try again.\n";
        }
           
    }
}




// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
