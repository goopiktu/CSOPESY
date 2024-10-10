#pragma once
#include <queue>
#include "processFactory.h"
#include <iostream>
using namespace std;
class ProcessManager {
private:
    queue<ProcessFactory*> processQueue;

public:
    // Add process to the queue
    void addProcess(ProcessFactory* process) {
        processQueue.push(process);
        cout << "Process '" << process->getName() << "' added to the queue." << endl;
    }

    // Run the first process in the queue
    void runProcess() {
        if (!processQueue.empty()) {
            ProcessFactory* currentProcess = processQueue.front();
            currentProcess->setStatus(RUNNING);

            // Simulate process execution
            while (currentProcess->getLineOfInstruction() < currentProcess->getTotalLineOfInstruction()) {
                currentProcess->setLineOfInstruction(currentProcess->getLineOfInstruction() + 1);
                cout << "Executing process: " << currentProcess->getName()
                    << " (" << currentProcess->getLineOfInstruction()
                    << "/" << currentProcess->getTotalLineOfInstruction() << ")" << endl;
            }

            currentProcess->setStatus(TERMINATED);
            cout << "Process " << currentProcess->getName() << " completed." << endl;

            // Remove the process from the queue
            processQueue.pop();
        }
        else {
            cout << "No processes to run." << endl;
        }
    }

    // Check if there are processes in the queue
    bool hasProcesses() {
        return !processQueue.empty();
    }
};
