// CSOPESY.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "ScreenFactory.h"
#include "ScreenManager.h"
#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <sstream>
struct Config {
    int num_cpu;
    std::string scheduler; 
    int quantum_cycles;
    int batch_process_freq;
    int min_ins;
    int max_ins;
    int delay_per_exec;
};
Config config;

string green = "\033[32m";
string reset = "\033[0m";
string yellow = "\033[38;5;229m";

int cpu_cycles = 0;

bool running = true;
bool initialized = false;

void mainThread();

std::thread main_worker(mainThread);
std::thread scheduler_test_thread;
ScreenManager screens(4); //initialize screenmanager with 4 cores


atomic<bool> making_process(false);

void Initialize() {
    std::cout << "initialize command recognized. Loading configuration.\n";

    std::ifstream MyReadFile("config.txt");
    if (!MyReadFile) {
        std::cerr << "Error: Could not open config.txt file.\n";
        return;
    }

    std::string line;
    while (std::getline(MyReadFile, line)) {
        std::istringstream iss(line);
        std::string key;
        int value;

        iss >> key;

        if (key == "num-cpu") {
            iss >> value;
            config.num_cpu = value;
        }
        else if (key == "scheduler") {
            std::getline(iss, config.scheduler);
            config.scheduler.erase(0, config.scheduler.find_first_not_of(" \t\""));
            config.scheduler.erase(config.scheduler.find_last_not_of(" \t\"") + 1); 
        }
        else if (key == "quantum-cycles") {
            iss >> value;
            config.quantum_cycles = value;
        }
        else if (key == "batch-process-freq") {
            iss >> value;
            config.batch_process_freq = value;
        }
        else if (key == "min-ins") {
            iss >> value;
            config.min_ins = value;
        }
        else if (key == "max-ins") {
            iss >> value;
            config.max_ins = value;
        }
        else if (key == "delay-per-exec") {
            iss >> value;
            config.delay_per_exec = value;
        }
        else {
            std::cout << "Unknown config parameter: " << key << "\n";
        }
    }

    MyReadFile.close();

    // Display loaded configuration
    std::cout << "Configuration Loaded:\n";
    std::cout << "num-cpu: " << config.num_cpu << "\n";
    std::cout << "scheduler: " << config.scheduler << "\n";
    std::cout << "quantum-cycles: " << config.quantum_cycles << "\n";
    std::cout << "batch-process-freq: " << config.batch_process_freq << "\n";
    std::cout << "min-ins: " << config.min_ins << "\n";
    std::cout << "max-ins: " << config.max_ins << "\n";
    std::cout << "delay-per-exec: " << config.delay_per_exec << "\n";
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
                screens.addScreen(name, config.min_ins, config.max_ins);
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



void SchedulerTest(int batch_process_freq, int min_ins, int max_ins) {
    std::cout << "scheduler-test command recognized. Starting process generation.\n";

    // Check if the process generation is already running
    if (!making_process.load()) {
        making_process.store(true); // Set the flag to true to indicate that process generation is active

        // Create a single thread for generating processes
        scheduler_test_thread = std::thread([=]() {
            int process_count = 0; // Count of processes generated

            while (making_process.load()) { // Loop while process generation is active
                std::this_thread::sleep_for(std::chrono::milliseconds(batch_process_freq)); // Wait for the specified frequency

                // Create a unique name for each process
                std::string process_name = "Process_" + std::to_string(process_count++);
                screens.addScreen(process_name, min_ins, max_ins); // Add new process to the ScreenManager
                //std::cout << "Generated process: " << process_name << "\n"; // Log the generated process
            }
            });
    }
    else {
        std::cout << "Scheduler is already running.\n"; // Inform if process generation is already active
    }
}

void SchedulerStop() {
    std::cout << "scheduler-stop command recognized. Stopping process generation.\n";

    // Signal the thread to stop and join it
    making_process.store(false); // Set the flag to false to stop process generation

    if (scheduler_test_thread.joinable()) {
        scheduler_test_thread.join(); // Wait for the thread to finish
    }
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
            initialized = true; 
        }
        else if (initialized) {  
            if (firstInput == "screen") {
                Screen(inputBuffer);
            }
            else if (firstInput == "scheduler-test") {
                SchedulerTest(config.batch_process_freq, config.min_ins, config.max_ins);
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
            else {
                cout << firstInput << " is not a recognized command. Please try again.\n";
            }
        }
        else {
            cout << "You must initialize first.\n";  
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
