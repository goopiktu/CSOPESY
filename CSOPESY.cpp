// CSOPESY.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "ScreenFactory.h"
#include "ScreenManager.h"
#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <sstream>

using namespace std;

string green = "\033[32m";
string reset = "\033[0m";
string yellow = "\033[38;5;229m";

bool running = true;

void mainThread();

std::thread main_worker(mainThread);
ScreenManager screens(4); //initialize screenmanager with 4 cores

void Initialize() {
    cout << "initialize command recognized. Doing something.\n";

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
}

void Screen(vector<string> inputBuffer) {
    //cout << "screen command recognized. Doing something.\n";
    if (inputBuffer.size() == 2 && inputBuffer[1] == "-ls") {
        //helper function to see all the screens
        screens.listScreens();
    }
    else if (inputBuffer.size() == 3) {
        string action = inputBuffer[1]; 
        string name = inputBuffer[2]; 

        if (action == "-r") {
            cout << "Restoring screen with name: " << name << "\n";
            if (!screens.sFind(name)) {
                cout << "screen with the following name: [" << name << "] does not exist.\n";
            }
            else {
                screens.displayScreen(name);
                screens.loopScreen(name);

                //This stuff happens after exit
                Clear();
            }

        }
        else if (action == "-s") {
            cout << "Creating a new screen with name: " << name << "\n";
            if (screens.sFind(name)) {
                cout << "screen with the following name: [" << name << "] already exists.\n";
            }
            else {
                screens.isInsideScreen(true);
                screens.addScreen(name, 100);
                //screens.displayScreen(name);
                //screens.loopScreen(name);

                //This stuff happens after exit
                //Clear();
            }
            
        }
        else {
            cout << "Unknown screen command action: " << action << "\n";
        }
    }
    else {
        cout << "Invalid screen command. Please provide -r or -s and a name.\n";
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



void Exit() {
    screens.shutdown();
    cout << "exit command recognized. Doing something.\n";
    running = false;
}

vector<string> split_sentence(string sen) {

    // Create a stringstream object
    stringstream ss(sen);

    // Variable to hold each word
    string word;

    // Vector to store the words
    vector<string> words;

    // Extract words from the sentence
    while (ss >> word) {

        // Add the word to the vector
        words.push_back(word);
    }

    return words;
}


void mainThread() {

    std::string inputBufferB;
    vector<string> inputBuffer;
    string input;
    bool input_done = false;
    Clear();

    while (running) {
        inputBuffer.clear();
        inputBufferB.clear();
        input_done = false;

        cout << "Enter a command: ";

        while (!input_done) {
            if (_kbhit()) {
                char ch = _getch();

                switch (ch) {
                case 8: //bs
                    if (inputBufferB.size() > 0) {
                        inputBufferB.pop_back();
                        cout << char(8) << " " << char(8);
                    }
                    break;
                case 13: //enter
                    cout << "\n";
                    input_done = true;
                    break;
                default:
                    if (ch >= 32 && ch <= 126) {
                        inputBufferB.push_back(ch);
                        cout << ch;
                    }
                    
                    break;
                }
                
            }
        }

        inputBuffer = split_sentence(inputBufferB);

        if (inputBuffer.size() <= 0) continue; //nothing was entered.

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




int main()
{
    main_worker.join();
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
