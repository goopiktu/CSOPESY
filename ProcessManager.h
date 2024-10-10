#pragma once
#include <queue>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "processFactory.h"
class ScreenManager {
	// might have to put limiter on how many screens are allowed to be in screens
private:
	queue<ProcessFactory*> processess;
	//unordered_map<string, ScreenFactory*> screens;
	//bool insideScreen;

public:
	void addScreen(string name) {
		ScreenFactory* screen = new ScreenFactory(name);
		screens[name] = screen;
		cout << "Screen '" << name << "' created." << endl;
	}

	void displayScreen(string name) {
		ScreenFactory* screen = screens[name];
		system("CLS");
		cout << "Screen name: " << screen->getName() << "\n";
		cout << "Date created: " << screen->getTime() << "\n";
		cout << "Line of instruction / Total line of instruction: " << screen->getLineOfInstruction() << "/" << screen->getTotalLineofInstruction() << "\n";
	}

	bool sFind(string name) {
		auto it = screens.find(name);
		if (it != screens.end()) {
			return true;
		}
		else {
			return false;
		}
	}

	void isInsideScreen(bool screen) {
		this->insideScreen = screen;
	}

	void listScreens() {
		cout << "The following screens currently exist: \n";
		for (auto& s : screens) {
			cout << "- " << s.second->getName() << "\t\t\t" << s.second->getTime() << "\n";
		}
	}

	void loopScreen(string name) {
		ScreenFactory* screen = screens[name];
		vector<string> inputBuffer;
		string input;


		while (insideScreen) {
			inputBuffer.clear();

			cout << "In screen [" << screen->getName() << "] Enter a command: ";

			while (cin >> input) {
				inputBuffer.push_back(input);
				if (cin.peek() == '\n')
					break;
			}

			string firstInput = inputBuffer[0];

			if (firstInput == "exit") {
				return;
			}
			else {
				cout << "echo: ";
				for (const string& word : inputBuffer) {
					cout << word << " ";
				}
				cout << "\n";
			}

		}

	}



};