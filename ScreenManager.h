#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "ScreenFactory.h"
class ScreenManager {
	// might have to put limiter on how many screens are allowed to be in screens
	private: 
		unordered_map<string, ScreenFactory*> screens;
		bool insideScreen;

	public:
		void addScreen(string name) {
			ScreenFactory* screen = new ScreenFactory(name);
			screens[name] = screen;
			cout << "Screen '" << name << "' created." << endl;
		}

	public:
		void displayScreen(string name) {	
			ScreenFactory* screen = screens[name];
			cout << "Screen name: " << screen->getName() << "\n";
			cout << "Date created: " << screen->getTime() << "\n";
			cout << "Line of instruction / Total line of instruction: " << screen->getLineOfInstruction() << "/" << screen->getTotalLineofInstruction() << "\n";
		}

	public:
		bool sFind(string name) {
			auto it = screens.find(name);
			if (it != screens.end()) {
				return true;
			}
			else {
				return false; 
			}
		}

	public:
		void isInsideScreen(bool screen) {
			this->insideScreen = screen;
		}


	public:
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
					cout << "echo: " << firstInput << "\n";
				}

			}

		}

	

};