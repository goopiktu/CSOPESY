#include <queue>
#include <vector>
#include "core.h"

class ProcessManager {
private:
    std::queue<ProcessFactory*> processQueue; // Queue of processes
    std::vector<Core*> cores;                 // Vector of cores (threads)

public:
    // Constructor to initialize the cores
    ProcessManager(int numCores = 4) {
        // Create the specified number of cores
        for (int i = 0; i < numCores; ++i) {
            cores.push_back(new Core(i));
        }
    }

    // Destructor to clean up dynamically allocated cores
    ~ProcessManager() {
        for (Core* core : cores) {
            delete core;
        }
    }

    // Method to add a process to the queue
    void addProcess(ProcessFactory* process) {
        processQueue.push(process);
        std::cout << "Process '" << process->getName() << "' added to the queue." << std::endl;
    }

    // Dispatch processes to available cores
    void dispatchProcesses() {
        // Loop while there are processes in the queue or any core is still running
        while (!processQueue.empty() || anyCoreRunning()) {
            for (Core* core : cores) {
                // If the core is idle and there are processes in the queue, assign a process to the core
                if (!core->isRunning() && !processQueue.empty()) {
                    ProcessFactory* process = processQueue.front();
                    processQueue.pop();
                    core->start(process); // Assign process to core and start execution
                }
            }
        }

        // Ensure all threads finish their work by joining them
        for (Core* core : cores) {
            core->join();
        }
    }

    // Check if any core is still running a process
    bool anyCoreRunning() const {
        for (const Core* core : cores) {
            if (core->isRunning()) {
                return true;
            }
        }
        return false;
    }
};
