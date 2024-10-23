#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "ScreenFactory.h"
#include <thread>
#include <Windows.h>

class ScreenManager {
	// might have to put limiter on how many screens are allowed to be in screens
	private: 
		unordered_map<string, ScreenFactory*> screens;

		std::vector <string> running_queue;
		std::vector <thread> core_threads;
		int cores;
		bool insideScreen;

		bool running = true;

	public:
		void shutdown() {
			running = false;
		}

		ScreenManager(int cores) {
			this->cores = cores;

			for (int i = 0; i < cores; i++) {
				running_queue.push_back("");
			}

			std::thread manager(&ScreenManager::managerJob, this);

			/*--- Initialize Cores ---*/
			for (int i = 0; i < cores; i++) {
				core_threads.push_back(std::thread(&ScreenManager::coreJob, this, i));
			};


			/*--- Execute Cores ---*/
			manager.join();

			for (int i = 0; i < cores; i++) {
				core_threads[i].join();
			};
		}

		void addScreen(string name, int instruction_count) {
			ScreenFactory* screen = new ScreenFactory(name, instruction_count);
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
			cout << "--------------------------------------\n";
			cout << "Running processes: \n";

			for (int i = 0; i < cores; i++) {
				if (screens.find(running_queue[i]) == screens.end()) {
					continue;
				}

				ScreenFactory* s = screens[running_queue[i]];
				if (s->getStatus() == RUNNING) {
					cout << s->getName() << "\t" << s->getTime() << "\tCore:"<<i<<"\t" << s->getLineOfInstruction() << " / " << s->getTotalLineofInstruction() << "\n";

				}
			}

			cout << "\nFinished processes: \n";
			for (auto& s : screens) {
				if (s.second->getStatus() == TERMINATED) {
					cout << s.second->getName() << "\t" << s.second->getTime() << "\tFinished\t" << s.second->getLineOfInstruction() << " / " << s.second->getTotalLineofInstruction() << "\n";
				}
			}
			cout << "--------------------------------------\n";
		}

		void coreJob(int i) {
			while (running) {
				string screen_name = running_queue[i];
				
				// Key is not present
				if (screens.find(screen_name) == screens.end()) continue;

				screens[screen_name]->print(i);
				Sleep(1000/60);
			}
			
		}

		string findFirst() {
			//return the first 
			string next_up = "";

			for (auto& s : screens) {
				if (s.second->getStatus() == READY) {
					//cout << s.second->getStatus();
					next_up = s.second->getName();
					break;
				}
			}

			return next_up;
		}

		void managerJob() {
			while (running) {
				
				for (int i = 0; i < cores; i++) {
					if (screens.find(running_queue[i]) == screens.end() || screens[running_queue[i]]->getStatus() == TERMINATED) {
						//screen doesnt exist yet or is terminated 

						//find if smthn is resdy
						string next_up = findFirst();
						if (next_up == "") continue;

						screens[next_up]->setStatus(RUNNING);
						running_queue[i] = next_up;
						continue;
					}
					
				}

				Sleep(1000/60);
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