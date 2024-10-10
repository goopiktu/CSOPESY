#pragma once
#include <string>
#include <ctime>
#include <iostream>
using namespace std;

enum Status {
    READY,
    WAITING,
    RUNNING,
    TERMINATED
};

class ProcessFactory
{
private:
    string name;
    string timeCreated;
    int lineOfInstruction;
    int totalLineOfInstruction;
    Status status;

public:
    // Constructor
    ProcessFactory(string name, int totalLineOfInstruction) {
        this->name = name;
        this->lineOfInstruction = 0; // Starts at 0 instructions executed
        this->totalLineOfInstruction = totalLineOfInstruction;
        this->status = READY;  // Process starts in READY state
        initializeTimeCreated();
    }

    // Getters
    string getName() const {
        return name;
    }

    string getTime() const {
        return timeCreated;
    }

    int getLineOfInstruction() const {
        return lineOfInstruction;
    }

    int getTotalLineOfInstruction() const {
        return totalLineOfInstruction;
    }

    Status getStatus() const {
        return status;
    }

    // Setters
    void setLineOfInstruction(int newLineOfInstruction) {
        this->lineOfInstruction = newLineOfInstruction;
    }

    void setStatus(Status newStatus) {
        this->status = newStatus;
    }

    // Private method to initialize the time when process was created
private:
    void initializeTimeCreated() {
        time_t now = time(0);
        tm localTime;
        localtime_s(&localTime, &now);

        char output[50];
        strftime(output, 50, "(%m/%d/%y %H:%M:%S %p)", &localTime);
        this->timeCreated = output;
    }
};
