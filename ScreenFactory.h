#pragma once
#include <string>
#include <ctime>
#include <iostream>
#include <stdlib.h> 
#include <time.h> 
#include <fstream>

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

		ofstream file;
	
	public:
		ScreenFactory(string name, int min_ins, int max_ins) {
			this->name = name;
			this->lineOfInstruction = 0;

			setTotalLineofInstruction(min_ins, max_ins);
		
			this->status = READY;
			this->file = ofstream("output/" + name + ".txt");

			file << "Process name: " << name << "\nLogs:\n";
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

					file << output << " Core:" << core << " \"Hello world from " << name << "!\"\n";
					lineOfInstruction+=1;
				}
				else {
					this->status = TERMINATED;
					file.close();
				}
			}
		}

		void setTotalLineofInstruction(int min_ins, int max_ins) {
			int range = max_ins - min_ins + 1;
			int num = rand() % range + min_ins;
			this->totalLineofInstruction = num;
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

