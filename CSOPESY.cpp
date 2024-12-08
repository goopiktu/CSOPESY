#include "ScreenFactory.h"
#include "ScreenManager.h"
#include "Config.h"
#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <thread>
#include <atomic>
#include <cstdlib>
#include "Memory.h"
#include "Command.h"
#include "InputHandler.h"

Command getCommandFromString(const std::string& commandStr) {
    static const std::unordered_map<std::string, Command> commandMap = {
        {"exit", Command::EXIT},
        {"initialize", Command::INITIALIZE},
        {"screen", Command::SCREEN},
        {"scheduler-test", Command::SCHEDULER_TEST},
        {"scheduler-stop", Command::SCHEDULER_STOP},
        {"clear", Command::CLEAR},
        {"report-util", Command::REPORT_UTIL},
        {"process-smi", Command::PROCESS_SMI},
        {"vmstat", Command::VMSTAT}
    };

    auto it = commandMap.find(commandStr);
    return it != commandMap.end() ? it->second : Command::INVALID;
}

std::string green = "\033[32m";
std::string reset = "\033[0m";
std::string yellow = "\033[38;5;229m";

int cpu_cycles = 0;
std::atomic<bool> running{ true };
std::atomic<bool> initialized(false);  // Track initialization status
Config* config = Config::getInstance();
ScreenManager* screens = nullptr; // Pointer to ScreenManager

void mainThread();
void cpuCycle();

std::thread main_worker(mainThread);
std::thread scheduler_test_thread;
std::thread cpu_cycle(cpuCycle);
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

IMemoryAllocator* memoryAllocator = nullptr;

void initializeScreens() {
    std::cout << "Initializing screens with " << config->getNumCPU() << " CPUs.\n"; // Debug output

    
    if (config->getMaxOverallMem() == config->getMemPerFrame()) {
        //flat memory allocator
        memoryAllocator = new FlatMemoryAllocator(config->getMaxOverallMem());
    }
    else {
        memoryAllocator = new PagingAllocator(config->getMaxOverallMem(), config->getMemPerFrame());
    }

    screens = new ScreenManager(*config, *memoryAllocator);
    if (screens) {
        std::cout << "ScreenManager initialized successfully.\n"; // Debug output
    }
    else {
        std::cout << "Failed to initialize ScreenManager.\n"; // Debug output
    }
}

void Screen(std::vector<std::string> inputBuffer) {
    if (inputBuffer.size() == 2 && inputBuffer[1] == "-ls") {
        screens->listScreens();
    }
    else if (inputBuffer.size() == 3) {
        std::string action = inputBuffer[1];
        std::string name = inputBuffer[2];

        if (action == "-r") {
            std::cout << "Restoring screen with name: " << name << "\n";
            if (!screens->sFind(name)) {
                std::cout << "Screen with the name: [" << name << "] does not exist.\n";
            }
            else {
                screens->displayScreen(name);
                screens->loopScreen(name);
                Clear();
            }
        }
        else if (action == "-s") {
            std::cout << "Creating a new screen with name: " << name << "\n";
            if (screens->sFind(name)) {
                std::cout << "Screen with the name: [" << name << "] already exists.\n";
            }
            else {
                screens->addScreen(name);
                screens->isInsideScreen(true);
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

void SchedulerTest(int batch_process_freq) {
    std::cout << "scheduler-test command recognized. Starting process generation.\n";
    if (!making_process.load()) {
        making_process.store(true);
        scheduler_test_thread = std::thread([=]() {
            int process_count = 0;
            while (making_process.load()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(batch_process_freq*config->getDelayPerExec()*10 + 1));
                std::string process_name = "Process_" + std::to_string(process_count++);
                screens->addScreen(process_name);
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
    if (screens) {
        screens->shutdown();
    }
    delete screens; // Clean up
    screens = nullptr;
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
    Clear();

    while (running) {
        // Get user input
        std::string input = InputHandler::getUserInput();
        std::vector<std::string> inputBuffer = InputHandler::splitInput(input);

        if (inputBuffer.empty()) continue;

        // Parse command
        Command command = getCommandFromString(inputBuffer[0]);

        // Check if initialization is required
        if (!initialized.load() && command != Command::INITIALIZE && command != Command::EXIT) {
            std::cout << "Error: The program must be initialized before using this command.\n";
            std::cout << "Type 'initialize' to configure the system, or 'exit' to quit.\n";
            continue;
        }

        // Handle commands
        switch (command) {
        case Command::EXIT:
            Exit();
            _Exit(0);
            break;

        case Command::INITIALIZE:
            std::cout << "Initializing...\n";
            try {
                config->loadConfig("config.txt");
                if (config->isInitialized()) {
                    initialized.store(true);
                    initializeScreens();
                    std::cout << "Configuration loaded successfully.\n";
                }
                else {
                    std::cout << "Failed to initialize configuration.\n";
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << "\n";
            }
            break;

        case Command::SCREEN:
            Screen(inputBuffer);
            break;

        case Command::SCHEDULER_TEST:
            SchedulerTest(config->getBatchProcessFreq());
            break;

        case Command::SCHEDULER_STOP:
            SchedulerStop();
            break;

        case Command::CLEAR:
            Clear();
            break;

        case Command::REPORT_UTIL:
            screens->report_util();
            break;
        case Command::PROCESS_SMI:
            screens->process_SMI();
            break;
        case Command::VMSTAT:
            screens->vmstat();
            break;
        case Command::INVALID:
        default:
            std::cout << "Invalid command. Please try again.\n";
            break;
        }
    }
}



void cpuCycle() {
    while (running) {
        cpu_cycles++;
        std::this_thread::sleep_for(std::chrono::milliseconds(config->getDelayPerExec()*100 + 1));
    }
}

int main() {
    std::thread cpu_cycle(cpuCycle); // Create CPU cycle thread
    main_worker.join();
    cpu_cycle.join(); // Wait for CPU cycle thread to finish

    return 0;
}
