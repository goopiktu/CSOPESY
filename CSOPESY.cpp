// CSOPESY.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "ScreenFactory.h"
#include "ScreenManager.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

string green = "\033[32m";
string reset = "\033[0m";
string yellow = "\033[38;5;229m";
ScreenManager screens;

bool running = true;

void Initialize() {
    cout << "initialize command recognized. Doing something.\n";

}

void Screen(vector<string> inputBuffer) {
    //cout << "screen command recognized. Doing something.\n";
    if (inputBuffer.size() == 3) {
        string action = inputBuffer[1]; 
        string name = inputBuffer[2]; 

        if (action == "-r") {
            cout << "Restoring screen with name: " << name << "\n";
        }
        else if (action == "-s") {
            cout << "Creating a new screen with name: " << name << "\n";
            screens.addScreen(name);
            screens.displayScreen(name);
        }
        else {
            cout << "Unknown screen command action: " << action << "\n";
        }
        
    }
    else {
        cout << "hello" << "\n";
    }

    
    
}

void SchedulerTest() {
    cout << "scheduler-test command recognized. Doing something.\n";
}

void SchedulerStop() {
    cout << "scheduler-stop command recognized. Doing something.\n";
}

void ReportUtil() {
    cout << "report-util command recognized. Doing something.\n";
}

void Clear() {
    system("CLS");
    cout << R"(
         ___________ ____  ____  _____________  __
        / ____/ ___// __ \/ __ \/ ____/ ___/\ \/ /
       / /    \__ \/ / / / /_/ / __ / \__ \  \  /
      / /___ ___/ / /_/ / ____/ /___ ___/ /  / /
      \____//____/\____/_/   /_____//____/  /_/                                                                                                                      
    )" << std::endl;

    cout << green << "Hello, Welcome to CSOPESY commandline!\n" << reset;
    cout << yellow << "Type 'exit' to quit, 'clear' to clear the screen\n" << reset;
    cout << "report-util command recognized. Doing something.\n";
}

void Exit() {
    cout << "exit command recognized. Doing something.\n";
    running = false;
}

int main()
{
    
    vector<string> inputBuffer;
    string input;

    Clear();
    
    while (running) {
        inputBuffer.clear();

        cout << "Enter a command: ";

        //get input as a string vector delimited by ' '
        while (cin >> input) {
            inputBuffer.push_back(input);

            //break if command ended
            if (cin.peek() == '\n')
                break;
        }

        string firstInput = inputBuffer[0];

        //get the first as the command

        if (firstInput == "exit") {
            Exit();
        }
        else if (firstInput == "initialize") {
            Initialize();
        }
        else if (firstInput == "screen") {
            Screen(inputBuffer);
        }
        else if (firstInput == "scheduler-test") {
            SchedulerTest();
        }
        else if (firstInput == "scheduler-stop") {
            SchedulerStop();
        }
        else if (firstInput == "report-util") {
            ReportUtil();
        }
        else if (firstInput == "clear") {
            Clear();
        }
        else if (firstInput == "") {

        }
        else {
            cout << firstInput << " is not a unrecognized command. Please try again.\n";
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
