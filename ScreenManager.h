#pragma once
#include <unordered_map>
#include "ScreenFactory.h"
class ScreenManager {
	
	private: 
		unordered_map<string, ScreenFactory*> screens;
	
	public:
		void addScreen(string name) {
			ScreenFactory* screen = new ScreenFactory(name);
			screens[name] = screen;
			cout << "Screen '" << name << "' created." << endl;
		}

	public:
		void displayScreen(string name) {	
			ScreenFactory* screen = screens[name];
			cout << screen->getName() << "\n";
			cout << screen->getTime() << "\n";
			cout << screen->getLineOfInstruction() << "/" << screen->getTotalLineofInstruction() << "\n";
		}

	

};