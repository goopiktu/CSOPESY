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
#include "Config.h"

class ScreenManager {

	private: 
		unordered_map<string, ScreenFactory*> screens;

		queue<ScreenFactory*> ready_queue;

		std::vector <string> running_queue;
		std::vector <thread> core_threads;
		std::thread manager_thread;
		int cores;
		bool insideScreen = false;

		atomic<bool> running = true;

		size_t cpu_cycles = 0;
		size_t active_cpu_cycles = 0;

		// MUTEX LOCKS
		std::mutex screens_mutex;
		std::mutex ready_queue_mutex;
		std::mutex running_queue_mutex; 

		int count = 0;
		int delay = 0;
		int timeslice = 0;

		IMemoryAllocator& memoryAllocator;
		int mem_per_frame = 0;

		size_t min_mem = 0;
		size_t max_mem = 0;

		size_t min_ins = 0;
		size_t max_ins = 0;

		size_t curr_id = 0;

		ofstream backingStore;

	public:
		/*void shutdown() {
			running = false;
		}*/

		ScreenManager(Config& config, IMemoryAllocator& memoryAllocator) : memoryAllocator(memoryAllocator) {
			
			backingStore.open("backingStore.txt");
			backingStore << "";//initialize backing store
			backingStore.close();

			this->delay = config.getDelayPerExec();
			this->timeslice = config.getQuantumCycles();
			this->mem_per_frame = config.getMemPerFrame();
			this->max_mem = config.getMaxMemPerProc();
			this->min_mem = config.getMinMemPerProc();

			this->max_ins = config.getMaxIns();
			this->min_ins = config.getMinIns();

			this->cores = config.getNumCPU();


			for (int i = 0; i < cores; i++) {
				running_queue.push_back("");
			}

			manager_thread = std::thread(&ScreenManager::managerJob, this);
			//manager.detach(); // Detach the manager thread to let it run independently

			/*--- Initialize Cores ---*/
			for (int i = 0; i < cores; i++) {
				if (config.getSchedulerType() == "rr") {
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


		void addScreen(string name) {
			ScreenFactory* screen = new ScreenFactory(name, min_ins, max_ins, min_mem, max_mem, curr_id);
			{
				std::lock_guard<std::mutex> lock(screens_mutex);
				screens[name] = screen;
			}
			{
				std::lock_guard<std::mutex> lock(ready_queue_mutex);
				ready_queue.push(screen);
			}
			curr_id += 1;
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
			std::lock_guard<std::mutex> lock(running_queue_mutex);
			int cpu_usage_count = 0;
			for (int i = 0; i < cores; i++) {
				if (screens.find(running_queue[i]) == screens.end()) {
					continue;
				}

				if (screens[running_queue[i]]->getStatus() == RUNNING) {
					cpu_usage_count++;
				}
			}

			cout << "CPU: " << cpu_usage_count * 100 / cores  <<"%"<< endl;

			cout << "--------------------------------------\n";
			cout << "Running processes: \n";

			for (int i = 0; i < cores; i++) {
				if (screens.find(running_queue[i]) == screens.end()) {
					continue;
				}

				ScreenFactory* s = screens[running_queue[i]];
				if (s->getStatus() == RUNNING) {
					cout << s->getName() << "\t" << s->getTime() << "\tCore:" << i << "\t" << s->getLineOfInstruction() << " / " << s->getTotalLineofInstruction() << "\n";
				}
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
			std::lock_guard<std::mutex> lock(running_queue_mutex);
			ofstream file = ofstream("report.txt");

			int cpu_usage_count = 0;
			for (int i = 0; i < cores; i++) {
				if (screens.find(running_queue[i]) == screens.end()) {
					continue;
				}

				if (screens[running_queue[i]]->getStatus() == RUNNING) {
					cpu_usage_count++;
				}
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

		void process_SMI() {
			std::lock_guard<std::mutex> lock(running_queue_mutex);

			cout << "--------------------------------------------\n";
			cout << "| PROCESS-SMI V01.00 Driver Version: 01.00 |\n";
			cout << "--------------------------------------------\n";

			int cpu_usage_count = 0;
			for (int i = 0; i < cores; i++) {
				if (screens.find(running_queue[i]) == screens.end()) {
					continue;
				}

				if (screens[running_queue[i]]->getStatus() == RUNNING) {
					cpu_usage_count++;
				}
				
			}

			cout << "CPU-Util:\t" << cpu_usage_count * 100 / cores << "%" << endl;
			cout << "Memory Usage:\t" << memoryAllocator.getAllocatedSize() << "KB / " << memoryAllocator.getMaximumSize() << "KB\n";
			cout << "Memory Util:\t" << memoryAllocator.getAllocatedSize()*100/memoryAllocator.getMaximumSize() << "%\n";
			
			cout << "============================================\n";
			cout << "Running processes and memory usage: \n";
			cout << "--------------------------------------------\n";

			for (int i = 0; i < cores; i++) {
				if (screens.find(running_queue[i]) == screens.end()) {
					continue;
				}

				ScreenFactory* s = screens[running_queue[i]];
				if (s->getStatus() == RUNNING) {
					cout << s->getName() << "\t" << s->getMemoryRequired() << "KB\n";
				}
				
				
			}

			cout << "--------------------------------------------\n";

		}

		void vmstat() {
			cout << "Total Memory:\t" << memoryAllocator.getMaximumSize() << " KB\n";
			cout << "Used Memory:\t" << memoryAllocator.getAllocatedSize() << " KB\n";
			cout << "Free Memory:\t" << memoryAllocator.getMaximumSize() - memoryAllocator.getAllocatedSize() << " KB\n";

			cout << "Idle CPU Ticks:\t" << cpu_cycles - active_cpu_cycles << "\n";
			cout << "Active CPU Ticks:\t" << active_cpu_cycles << "\n";
			cout << "Total CPU Ticks:\t" << cpu_cycles << "\n";


			cout << "# Paged-In:\t" << memoryAllocator.getFrameIn() << "\n";
			cout << "# Paged-Out:\t" << memoryAllocator.getMaxFrames() - memoryAllocator.getFrameIn() << "\n";
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

				Sleep(delay*10+1);
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

					if (!ready_queue.empty() && (screens[screen_name]->getStatus() != TERMINATED)) {
						{	// Change status to ready 
							std::lock_guard<std::mutex> lock(screens_mutex);
							screens[screen_name]->setStatus(WAITING);
						}

					}
					
					counter = 0;
					continue;
				} // ENDIF

				if (screens[screen_name]->getStatus() == RUNNING) {
					screens[screen_name]->print(i);
				}
				
				counter++;
				
				
				Sleep(delay * 10 + 1);
			}
		}

		std::string findFirst() {
			std::string next_up = "";
			
			if (!ready_queue.empty()) {
				std::lock_guard<std::mutex> lock(ready_queue_mutex);
				ScreenFactory* process = ready_queue.front();
				ready_queue.pop();
				next_up = process->getName();
			}
			return next_up;
		}

		bool removeOldest() {
			//find oldest screen
			size_t lowest_cycle = 10000000000000;
			ScreenFactory* oldest_screen = nullptr;

			for (auto& s : screens) {
				if (s.second->getStatus() == WAITING && s.second->getMemState() == IN_MEMORY){
					if (s.second->getLastTimeMem() < lowest_cycle) {
						lowest_cycle = s.second->getLastTimeMem();
						oldest_screen = s.second;
					}
				}
			}
			if (oldest_screen == nullptr) return false;

			oldest_screen->setMemState(IN_BACKING_STORE);
			//write to file
			backingStore.open("backingStore.txt",fstream::app);
			backingStore << oldest_screen->getName() << endl;
			backingStore.close();

			memoryAllocator.deallocate(oldest_screen->getID());
			return true;
		}

		void managerJob() {
			while (running) {
				bool ran = false;
				for (int i = 0; i < cores; i++) {
					std::string next_up;
					{
						// Lock the mutexes for screens and running_queue
						std::unique_lock<std::mutex> lock_screens(screens_mutex, std::defer_lock);
						std::unique_lock<std::mutex> lock_running(running_queue_mutex, std::defer_lock);
						std::lock(lock_screens, lock_running); // Lock both mutexes
						if (screens.find(running_queue[i]) == screens.end() || screens[running_queue[i]]->getStatus() != RUNNING) { 
							//dealloc finished process
							if (screens.find(running_queue[i]) != screens.end()) { //if something in CPU
								
								if (screens[running_queue[i]]->getStatus() == TERMINATED) {
									size_t id = screens[running_queue[i]]->getID();
									memoryAllocator.deallocate(id);
									screens[running_queue[i]]->setMemState(NOT_ALLOCATED);
								}

								if (screens[running_queue[i]]->getStatus() == WAITING) {
									ready_queue.push(screens[running_queue[i]]);
									running_queue[i] = "";
								}
							}
							
							{
								// find if smthn is ready
								next_up = findFirst();
							}
							if (next_up == "") {
								running_queue[i] = ""; //nothing ready, so just clear it.
								continue;
							};

							if (screens[next_up]->getStatus() == WAITING && screens[next_up]->getMemState() == IN_MEMORY) {
								//if process has already been memallocated / from waiting
								screens[next_up]->setStatus(RUNNING);
								running_queue[i] = next_up;
								
								continue;
							}

							//allocate memory
							try_to_allocate_again:
							if (memoryAllocator.allocate(screens[next_up]->getMemoryRequired(), screens[next_up]->getID())) {
								//theres memory!
								if (screens[next_up]->getMemState() == IN_BACKING_STORE) {
									//TODO: remove from backing store
									ifstream file("backingStore.txt");
									string str;
									string file_contents;

									while (std::getline(file, str))
									{
										//dont include the one we want to remove
										if (str != screens[next_up]->getName()) { 
											file_contents += str;
											file_contents.push_back('\n');
										}
									}
									file.close();

									//override backing store with new list
									backingStore.open("backingStore.txt");
									backingStore << file_contents;
									backingStore.close();

								}

								screens[next_up]->setStatus(RUNNING);
								screens[next_up]->setMemState(IN_MEMORY);
								screens[next_up]->setLastTimeMem(cpu_cycles); //to know which is the oldest

								running_queue[i] = next_up;
							}
							else {
								if (removeOldest()) {
									//something in memory was successfully removed
									goto try_to_allocate_again;
								}
								else {
									//nothing left to remove
									screens[next_up]->setStatus(READY);
									ready_queue.push(screens[next_up]);
								}
							}

							continue;
						} // ENDIF
						else { //will run if at least one cpu is running 
							ran = true;
						}
					} //END MUTEX LOCK
				}// ENDFORLOOP
				
				//listScreens();

				
				Sleep(delay * 10 + 1);
				cpu_cycles++;
				if (ran)active_cpu_cycles++;
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