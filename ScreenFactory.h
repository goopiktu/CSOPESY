#pragma once
#include <string>
#include <ctime>
#include <iostream>
#include <stdlib.h> 
#include <time.h> 
using namespace std;

enum Status {
	READY,
	WAITING,
	RUNNING,
	TERMINATED
};

class ScreenFactory
{
	private:
		string name;
		string timeCreated;
		int lineOfInstruction;
		int totalLineofInstruction;
		Status status;
	
	public:
		ScreenFactory(string name, int total_instruction) {
			this->name = name;
			this->lineOfInstruction = 0;
			this->totalLineofInstruction = total_instruction;
			this->status = READY;
			initializeTimeCreated();
		}

		string getTime() {
			return timeCreated;
		}

		string getName() {
			return name;
		}

		Status getStatus() const {
			return status;
		}

		void setStatus(Status s) {
			status = s;
		}

		int getLineOfInstruction() {
			return lineOfInstruction;
		}

		int getTotalLineofInstruction() {
			return totalLineofInstruction;
		}

		void print(int core) {
			if (status == RUNNING) {
				if (lineOfInstruction < totalLineofInstruction) {
					//print thing
					time_t now = time(0);
					tm localTime;
					localtime_s(&localTime, &now);

					char output[50];

					strftime(output, 50, "(%m/%d/%y %H:%M:%S %p)", &localTime);

					//cout << output << " Core:" << core << " \"Hello world from " << name << "!\"\n";
					lineOfInstruction+=1;
				}
				else status = TERMINATED;
			}
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

