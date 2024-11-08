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
#include "Memory.h"

class ScreenManager {

	private: 
		unordered_map<string, ScreenFactory*> screens;

		queue<ScreenFactory*> ready_queue;

		std::vector <string> running_queue;
		std::vector <thread> core_threads;
		std::thread manager_thread;
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

		IMemoryAllocator& memoryAllocator;
		int mem_per_proc = 0;
		int mem_per_frame = 0;

	public:
		/*void shutdown() {
			running = false;
		}*/

		ScreenManager(int cores, int delay, int timeslice, int RR, IMemoryAllocator& memoryAllocator, int mem_per_proc, int mem_per_frame) : cores(cores), insideScreen(false), memoryAllocator(memoryAllocator) {
			this->delay = delay;
			this->timeslice = timeslice;
			this->mem_per_proc = mem_per_proc;
			this->mem_per_frame = mem_per_frame;


			for (int i = 0; i < cores; i++) {
				running_queue.push_back("");
			}

			manager_thread = std::thread(&ScreenManager::managerJob, this);
			//manager.detach(); // Detach the manager thread to let it run independently

			/*--- Initialize Cores ---*/
			for (int i = 0; i < cores; i++) {
				if (RR == 1) {
					core_threads.push_back(std::thread(&ScreenManager::coreJob_RR, this, i));
				}
				else {
					core_threads.push_back(std::thread(&ScreenManager::coreJob, this, i));
				}
				
			}
		}

		void shutdown() {
			running = false;  // Signal threads to stop

			// Join manager thread if joinable
			if (manager_thread.joinable()) {
				manager_thread.join();
			}

			// Join all core threads
			for (std::thread& t : core_threads) {
				if (t.joinable()) {
					t.join();
				}
			}
		}


		void addScreen(string name, int min_ins, int max_ins) {
			ScreenFactory* screen = new ScreenFactory(name, min_ins, max_ins, mem_per_proc);
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
			int cpu_usage_count = 0;
			for (int i = 0; i < cores; i++) {
				if (screens.find(running_queue[i]) == screens.end()) {
					continue;
				}

				cpu_usage_count++;
			}

			cout << "CPU: " << cpu_usage_count * 100 / cores  <<"%"<< endl;

			cout << "--------------------------------------\n";
			cout << "Running processes: \n";

			for (int i = 0; i < cores; i++) {
				if (screens.find(running_queue[i]) == screens.end()) {
					continue;
				}

				ScreenFactory* s = screens[running_queue[i]];
				
				cout << s->getName() << "\t" << s->getTime() << "\tCore:"<<i<<"\t" << s->getLineOfInstruction() << " / " << s->getTotalLineofInstruction() << "\n";

			}

			cout << "\nFinished processes: \n";
			for (auto& s : screens) {
				if (s.second->getStatus() == TERMINATED) {
					cout << s.second->getName() << "\t" << s.second->getTime() << "\tFinished\t" << s.second->getLineOfInstruction() << " / " << s.second->getTotalLineofInstruction() << "\n";
					count++;
				}
				
			}
			/*cout << count << "??????";
			count = 0;*/
			cout << "--------------------------------------\n";

		}

		void report_util() {
			ofstream file = ofstream("report.txt");

			int cpu_usage_count = 0;
			for (int i = 0; i < cores; i++) {
				if (screens.find(running_queue[i]) == screens.end()) {
					continue;
				}

				cpu_usage_count++;
			}

			file << "CPU: " << cpu_usage_count * 100 / cores << "%" << endl;

			file << "--------------------------------------\n";
			file << "Running processes: \n";

			for (int i = 0; i < cores; i++) {
				if (screens.find(running_queue[i]) == screens.end()) {
					continue;
				}

				ScreenFactory* s = screens[running_queue[i]];
				if (s->getStatus() == RUNNING) {
					file << s->getName() << "\t" << s->getTime() << "\tCore:" << i << "\t" << s->getLineOfInstruction() << " / " << s->getTotalLineofInstruction() << "\n";

				}
			}

			file << "\nFinished processes: \n";
			for (auto& s : screens) {
				if (s.second->getStatus() == TERMINATED) {
					file << s.second->getName() << "\t" << s.second->getTime() << "\tFinished\t" << s.second->getLineOfInstruction() << " / " << s.second->getTotalLineofInstruction() << "\n";
					count++;
				}

			}
			/*cout << count << "??????";
			count = 0;*/
			file << "--------------------------------------\n";

			file.close();

			cout << "Report successfully generated." << endl;
		}

		int quantum_time = 0;
		void printMemory() {
			ofstream file = ofstream("memlogs/memory_stamp_" + std::to_string(quantum_time) + ".txt");


			time_t now = time(0);
			tm localTime;
			localtime_s(&localTime, &now);

			char output[50];

			strftime(output, 50, "(%m/%d/%y %H:%M:%S %p)", &localTime);

			file << "Timestamp" << output << endl;
			
			file << "Number of processes in memory: " << memoryAllocator.getAllocatedSize() / mem_per_proc << endl;
			file << "Total external fragmentation in KB: " << memoryAllocator.getMaximumSize() - memoryAllocator.getAllocatedSize() << endl;

			file << "----end---- = " << memoryAllocator.getMaximumSize() << "\n\n";

			for (int i = 0; i < cores; i++) {
				if (screens.find(running_queue[i]) == screens.end()) {
					file << "---\nIDLE\n---\n\n";
					continue;
				}

				ScreenFactory* s = screens[running_queue[i]];
				
				size_t base = memoryAllocator.ptr_to_index(s->getMemoryAddress());
				file << (base + s->getMemoryRequired())-1 << "\n" << s->getName() << endl << base << "\n\n";
			}

			file << "---start--- = 0\n\n";
			quantum_time++;
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

				Sleep(delay*100+1); // Adjust this as needed
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

					if (ready_queue.empty()) {
						
						counter = 0;
						continue;
					}

					if (screens[screen_name]->getStatus() != TERMINATED) {
						{	// Change status to ready 
							std::lock_guard<std::mutex> lock(screens_mutex);
							screens[screen_name]->setStatus(WAITING);
						}

						{	// Requeue process
							std::lock_guard<std::mutex> lock(ready_queue_mutex);
							ready_queue.push(screens[screen_name]);
						}
					}
					
					counter = 0;
					continue;
				} // ENDIF

				if (screens[screen_name]->getStatus() == RUNNING) {
					screens[screen_name]->print(i);
				}
				
				counter++;
				
				
				Sleep(delay * 100 + 1);
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
				bool change = false;
				for (int i = 0; i < cores; i++) {
					std::string next_up;
					{
						// Lock the mutexes for screens and running_queue
						std::unique_lock<std::mutex> lock_screens(screens_mutex, std::defer_lock);
						std::unique_lock<std::mutex> lock_running(running_queue_mutex, std::defer_lock);
						std::lock(lock_screens, lock_running); // Lock both mutexes
						if (screens.find(running_queue[i]) == screens.end() || screens[running_queue[i]]->getStatus() != RUNNING) { 
							//dealloc finished process
							if (screens.find(running_queue[i]) != screens.end() && screens[running_queue[i]]->getStatus() == TERMINATED) {
								void* addr = screens[running_queue[i]]->getMemoryAddress();
								memoryAllocator.deallocate(addr, screens[running_queue[i]]->getMemoryRequired());
							}
							
							{
								// find if smthn is ready
								next_up = findFirst();
							}
							if (next_up == "") {
								running_queue[i] = "";
								continue;
							};

							change = true;
							
							if (screens[next_up]->getStatus() == WAITING) {
								//if process has already been allocated / from waiting
								screens[next_up]->setStatus(RUNNING);
								running_queue[i] = next_up;
								
								continue;
							}

							//allocate memory
							void* mem = memoryAllocator.allocate(screens[next_up]->getMemoryRequired());
							if (mem != nullptr) {
								//theres memory!
								screens[next_up]->setMemoryAddress(mem);
								screens[next_up]->setStatus(RUNNING);
								running_queue[i] = next_up;
							}
							else {
								//boo try again
								screens[next_up]->setStatus(READY);
								ready_queue.push(screens[next_up]);
							}

							continue;
						} // ENDIF
					} //END MUTEX LOCK
				}// ENDFORLOOP
				 
				if(change) printMemory();
				//listScreens();

			/*	std::cout << "RQ: " << ready_queue.size(); */
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