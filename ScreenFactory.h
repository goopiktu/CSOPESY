#pragma once
#include <string>
#include <ctime>
#include <iostream>
#include <stdlib.h> 
#include <time.h> 
using namespace std;


class ScreenFactory
{
	private:
		string name;
		string timeCreated;
		int lineOfInstruction;
		int totalLineofInstruction;
	
	public:
		ScreenFactory(string name) {
			this->name = name;
			this->lineOfInstruction = 0;
			this->totalLineofInstruction = 0;
			initializeTimeCreated();
		}

	public:
		string getTime() {
			return timeCreated;
		}

	public:
		string getName() {
			return name;
		}

	public:
		int getLineOfInstruction() {
			return lineOfInstruction;
		}

	public:
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

