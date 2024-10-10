#include <thread>
#include <atomic>
#include "processFactory.h"

class Core {
private:
    int coreID;
    std::thread coreThread;
    std::atomic<bool> running;

public:
    Core(int id) : coreID(id), running(false) {}

    // Start core and assign it a process
    void start(ProcessFactory* process) {
        running = true;
        coreThread = std::thread(&Core::runProcess, this, process);
    }

    // Core running the assigned process
    void runProcess(ProcessFactory* process) {
        process->setStatus(RUNNING);
        while (process->getLineOfInstruction() < process->getTotalLineOfInstruction()) {
            process->setLineOfInstruction(process->getLineOfInstruction() + 1);
            std::cout << "Core " << coreID << " executing process: " << process->getName()
                << " (" << process->getLineOfInstruction()
                << "/" << process->getTotalLineOfInstruction() << ")" << std::endl;
        }
        process->setStatus(TERMINATED);
        std::cout << "Core " << coreID << " completed process: " << process->getName() << std::endl;
        running = false;
    }

    bool isRunning() const {
        return running;
    }

    // Join the thread when the process is done
    void join() {
        if (coreThread.joinable()) {
            coreThread.join();
        }
    }
};
