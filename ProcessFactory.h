#pragma once
#include <string>
#include <ctime>
#include <iostream>
#include <stdlib.h> 
#include <time.h> 
using namespace std;

enum status {
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
	int totalLineofInstruction;
	enum status;

public:
	ProcessFactory(string name, int lineOfInstruction, int totalLineOfInstruction) {
		this->name = name;
		this->lineOfInstruction = lineOfInstruction;
		this->totalLineofInstruction = totalLineOfInstruction;
		initializeTimeCreated();
	}

	string getTime() {
		return timeCreated;
	}

	string getName() {
		return name;
	}

	void setLineOfInstruction(int newLineOfInstruction) {
		this->lineOfInstruction = newLineOfInstruction;
	}

	int getLineOfInstruction() {
		return lineOfInstruction;
	}

	int getTotalLineofInstruction() {
		return totalLineofInstruction;
	}


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

