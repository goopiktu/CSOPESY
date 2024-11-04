#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <queue> 
#include "ScreenFactory.h"
#include <thread>
#include <Windows.h>

class ScreenManager {

	private: 
		unordered_map<string, ScreenFactory*> screens;

		queue<ScreenFactory*> ready_queue;

		std::vector <string> running_queue;
		std::vector <thread> core_threads;
		int cores;
		bool insideScreen;

		atomic<bool> running = true;

		int cpu_cycles = 0;

		// MUTEX LOCKS
		std::mutex screens_mutex;
		std::mutex ready_queue_mutex;
		std::mutex running_queue_mutex; 

		int count = 0;
		int delay = 0;
		int timeslice = 0;

	public:
		void shutdown() {
			running = false;
		}

		ScreenManager(int cores, int delay, int timeslice) : cores(cores), insideScreen(false) {
			delay = delay;

			for (int i = 0; i < cores; i++) {
				running_queue.push_back("");
			}

			std::thread manager(&ScreenManager::managerJob, this);
			manager.detach(); // Detach the manager thread to let it run independently

			/*--- Initialize Cores ---*/
			for (int i = 0; i < cores; i++) {
				core_threads.push_back(std::thread(&ScreenManager::coreJob, this, i));
			}
		}

		void addScreen(string name, int min_ins, int max_ins) {
			ScreenFactory* screen = new ScreenFactory(name, min_ins, max_ins);
			{
				std::lock_guard<std::mutex> lock(screens_mutex);
				screens[name] = screen;
			}
			{
				std::lock_guard<std::mutex> lock(ready_queue_mutex);
				ready_queue.push(screen);
			}
			//cout << "Screen '" << name << "' created." << endl;
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
					count++;
				}
				
			}
			cout << count << "??????";
			count = 0;
			cout << "--------------------------------------\n";


			for (auto& s : screens) {
				cout << "Process Name: " << s.second->getName() << ", Status: " << s.second->getStatus() << endl;
			}
		}

		void coreJob(int i) {
			int delay = this->delay;
			while (running) {
				std::string screen_name;
				{
					std::lock_guard<std::mutex> lock(running_queue_mutex);
					screen_name = running_queue[i];
				}

				if (screens.find(screen_name) != screens.end()) {
					// Process exists in screens and hasn't been terminated
					screens[screen_name]->print(i);
				}

				Sleep(delay); // Adjust this as needed
			}
		}

		void coreJob_RR(int i) {
			int time_slice = this->timeslice;
			int counter = 0;
			int delay = this->delay;
			
			while (running) {
				std::string screen_name;
				{
					std::lock_guard<std::mutex> lock(running_queue_mutex);
					screen_name = running_queue[i]; 
				}

				{
					// Key is not present
					std::lock_guard<std::mutex> lock(screens_mutex);
					if (screens.find(screen_name) == screens.end()) continue;

					// Process is done
					/*std::lock_guard<std::mutex> lock(screens_mutex);*/
					if (screens[screen_name]->getStatus() == TERMINATED) {
						counter = 0;
						continue;
					}
				}

				// Current process has reached allotted time slice
				if (counter >= time_slice) {

					{ // If no current process in ready_queue, continue process
						std::string next_up = findFirst();
						if (next_up == "") {
							counter = 0;
							continue;
						}
					}
					
					{	// Change status to ready 
						std::lock_guard<std::mutex> lock(screens_mutex);
						screens[screen_name]->setStatus(READY);
					}
					counter = 0;

					{	// Clear current core process
						std::lock_guard<std::mutex> lock(running_queue_mutex);
						running_queue[i] = "";
					}

					{	// Requeue process
						std::lock_guard<std::mutex> lock(ready_queue_mutex);
						ready_queue.push(screens[screen_name]);
					}
					continue;
				} // ENDIF

				screens[screen_name]->print(i);
				counter++;
				Sleep(delay);
			}
		}

		std::string findFirst() {
			std::string next_up = "";
			std::lock_guard<std::mutex> lock(ready_queue_mutex);

			if (!ready_queue.empty()) {
				ScreenFactory* process = ready_queue.front();
				ready_queue.pop();
				next_up = process->getName();
			}
			return next_up;
		}

		void managerJob() {
			while (running) {
				
				for (int i = 0; i < cores; i++) {
					std::string next_up;
					{
						// Lock the mutexes for screens and running_queue
						std::unique_lock<std::mutex> lock_screens(screens_mutex, std::defer_lock);
						std::unique_lock<std::mutex> lock_running(running_queue_mutex, std::defer_lock);
						std::lock(lock_screens, lock_running); // Lock both mutexes
						if (screens.find(running_queue[i]) == screens.end() || screens[running_queue[i]]->getStatus() == TERMINATED) {
							//screen doesnt exist yet or is terminated 

							{
								// find if smthn is ready
								next_up = findFirst();
							}
							if (next_up == "") continue;

							screens[next_up]->setStatus(RUNNING);
							running_queue[i] = next_up;
							continue;
						} // ENDIF
					} //END MUTEX LOCK
				}// ENDFORLOOP

			/*	std::cout << "RQ: " << ready_queue.size(); */
				Sleep(delay);
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