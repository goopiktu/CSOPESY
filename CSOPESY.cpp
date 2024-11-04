// CSOPESY.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "ScreenFactory.h"
#include "ScreenManager.h"
#include "Config.h"
#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <thread>
#include <atomic>

std::string green = "\033[32m";
std::string reset = "\033[0m";
std::string yellow = "\033[38;5;229m";

int cpu_cycles = 0;
bool running = true;
std::atomic<bool> initialized(false);  // Track initialization status
Config* config = Config::getInstance();
void mainThread();
void cpuCycle();

std::thread main_worker(mainThread);
std::thread scheduler_test_thread;
std::thread cpu_cycle(cpuCycle);
ScreenManager screens(4); // Initialize ScreenManager with 4 cores
std::atomic<bool> making_process(false);

// Clear screen function
void Clear() {
    system("CLS");
    std::cout << R"(
         ___________ ____  ____  _____________  __
        / ____/ ___// __ \/ __ \/ ____/ ___/\ \/ /
       / /    \__ \/ / / / /_/ / __ / \__ \  \  /
      / /___ ___/ / /_/ / ____/ /___ ___/ /  / /
      \____//____/\____/_/   /_____//____/  /_/                                                                                                                      
    )" << std::endl;

    std::cout << green << "Hello, Welcome to CSOPESY commandline!\n" << reset;
    std::cout << yellow << "Type 'exit' to quit, 'clear' to clear the screen\n" << reset;
}

void Screen(std::vector<std::string> inputBuffer) {
    if (inputBuffer.size() == 2 && inputBuffer[1] == "-ls") {
        screens.listScreens();
    }
    else if (inputBuffer.size() == 3) {
        std::string action = inputBuffer[1];
        std::string name = inputBuffer[2];

        if (action == "-r") {
            std::cout << "Restoring screen with name: " << name << "\n";
            if (!screens.sFind(name)) {
                std::cout << "Screen with the name: [" << name << "] does not exist.\n";
            }
            else {
                screens.displayScreen(name);
                screens.loopScreen(name);
                Clear();
            }
        }
        else if (action == "-s") {
            std::cout << "Creating a new screen with name: " << name << "\n";
            if (screens.sFind(name)) {
                std::cout << "Screen with the name: [" << name << "] already exists.\n";
            }
            else {
                screens.isInsideScreen(true);
            }
        }
        else {
            std::cout << "Unknown screen command action: " << action << "\n";
        }
    }
    else {
        std::cout << "Invalid screen command. Please provide -r or -s and a name.\n";
    }
}

void SchedulerTest(int batch_process_freq, int min_ins, int max_ins) {
    std::cout << "scheduler-test command recognized. Starting process generation.\n";
    if (!making_process.load()) {
        making_process.store(true);
        scheduler_test_thread = std::thread([=]() {
            int process_count = 0;
            while (making_process.load()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(batch_process_freq));
                std::string process_name = "Process_" + std::to_string(process_count++);
                screens.addScreen(process_name, min_ins, max_ins);
            }
            });
    }
    else {
        std::cout << "Scheduler is already running.\n";
    }
}

void SchedulerStop() {
    std::cout << "scheduler-stop command recognized. Stopping process generation.\n";
    making_process.store(false);
    if (scheduler_test_thread.joinable()) {
        scheduler_test_thread.join();
    }
}

void Exit() {
    screens.shutdown();
    std::cout << "Exiting program.\n";
    running = false;
}

std::vector<std::string> split_sentence(std::string sen) {
    std::stringstream ss(sen);
    std::string word;
    std::vector<std::string> words;
    while (ss >> word) {
        words.push_back(word);
    }
    return words;
}

void mainThread() {
    std::string inputBufferB;
    std::vector<std::string> inputBuffer;
    bool input_done = false;
    Clear();

    while (running) {
        inputBuffer.clear();
        inputBufferB.clear();
        input_done = false;

        std::cout << "Enter a command: ";
        while (!input_done) {
            if (_kbhit()) {
                char ch = _getch();
                switch (ch) {
                case 8: // backspace
                    if (inputBufferB.size() > 0) {
                        inputBufferB.pop_back();
                        std::cout << "\b \b";
                    }
                    break;
                case 13: // enter
                    std::cout << "\n";
                    input_done = true;
                    break;
                default:
                    if (ch >= 32 && ch <= 126) {
                        inputBufferB.push_back(ch);
                        std::cout << ch;
                    }
                    break;
                }
            }
        }

        inputBuffer = split_sentence(inputBufferB);
        if (inputBuffer.size() <= 0) continue;

        std::string firstInput = inputBuffer[0];
        Config* config = Config::getInstance(); // Access singleton instance

        if (firstInput == "exit") {
            Exit();
        }
        else if (firstInput == "initialize") {
            try {
                config->loadConfig("config.txt"); // Load configuration file
                if (config->isInitialized()) {
                    initialized.store(true);
                    std::cout << "Configuration loaded successfully.\n";
                }
                else {
                    std::cout << "Failed to initialize configuration.\n";
                }
            }
            catch (const ConfigException& e) {
                std::cerr << "Configuration error: " << e.what() << "\n";
            }
        }
        else if (initialized.load()) {
            if (firstInput == "screen") {
                Screen(inputBuffer);
            }
            else if (firstInput == "scheduler-test") {
                SchedulerTest(config->getBatchProcessFreq(), config->getMinIns(), config->getMaxIns());
            }
            else if (firstInput == "scheduler-stop") {
                SchedulerStop();
            }
            else if (firstInput == "clear") {
                Clear();
            }
            else {
                std::cout << firstInput << " is not a recognized command.\n";
            }
        }
        else {
            std::cout << "You must initialize first.\n";
        }
    }
}

void cpuCycle() {
    Config* config = Config::getInstance(); // Access singleton instance
    while (running) {
        cpu_cycles++;
        std::this_thread::sleep_for(std::chrono::milliseconds(config->getDelayPerExec()));
    }
}

int main() {
    cpu_cycle.join();
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
