# Installation Guide & Instructions
Clone the git repository in Microsoft Visual Studio 2020.

Before running the program, configure the `config.txt` file in the root folder to your desired simulation parameters. The available parameters are as follows:

- `num-cpu`: Number of CPUs available. The range is [1, 128].
- `scheduler`: The scheduler algorithm: “fcfs” or “rr”.
- `quantum-cycles`: The time slice is given for each processor if a round-robin scheduler is used. Has no effect on other schedulers. The range is [1, 2^32].
- `batch-process-freq`: The frequency of generating processes in the “scheduler-test” command in CPU cycles. The range is [1, 2^32 ]. If one, a new process is generated at the end of each CPU cycle.
- `min-ins`: The minimum instructions/command per process. The range is [1,2^32].
- `max-ins`: The maximum instructions/command per process. The range is [1,2^32].
- `delays-per-exec`: Delay before executing the next instruction in CPU cycles. The delay is a “busy-waiting” scheme wherein the process remains in the CPU. The range is [0, 2^32 ]. If zero, each instruction is executed per CPU cycle.
- `max-overall-mem`: Maximum memory available in KB.
- `mem-per-frame`: The size of memory in KB per frame. This is also the memory size per page. The total number of frames is equal to max-overall-mem / mem-per-frame. If max-overall-mem = mem-per-frame, then the emulator will use a flat memory allocator.
- `min-mem-per-proc`: Memory required for each process.
- `max-mem-per-proc`: Let P be the number of pages required by a process and M is the rolled value between min-mem-per-proc and max-mem-proc. P can be computed as M/ mem-per-frame.

Once ready,  enter the `initialize` command to prepare the simulation.

## Commands
The program uses a text-based input system. Simply type in the command and hit enter to confirm.

The following commands are available to the user:
- `exit`: safely closes the application.
- `initialize`: reads the config file and prepares the CPU and memory.
- `screen -s [screen_name]`: creates a new process with screen_name. screen_name must be unique.
- `screen -r [screen_name]`: restores a new process with screen_name. screen_name must be unique.
- `screen -ls`: displays CPU info and the currently running and finished processes.
- `scheduler-test`: Begins the scheduler test, creating a new process every timespan based on batch-process-freq.
- `scheduler-stop`: Stops the scheduler test.
- `clear`: Clears the screen.
- `report-util`: Similar to screen -ls, but saves to the [`report.txt`](report.txt) file.
- `process-smi`: displays memory info.
- `vmstat`: displays a more detailed view of memory info.

#### Tip:  The up and down arrow keys can be used as shorthand for `screen -ls` and `process-smi`, respectively.

## Files
The following files are described below:
- [`Source.cpp`](Source.cpp): Contains the main function thread, command recognition, and initialization functions.
- [`ScreenFactory.h`](ScreenFactory.h): Contains the ScreenFactory class that represents processes in the simulation.
- [`ScreenManager.h`](ScreenManager.h): Contains the ScreenManager class that handles the CPU scheduling and Core threads.
- [`InputHandler.h`](InputHandler.h): Handles keyboard input and is called by the main thread to get the inputted command.
- [`Memory.h`](Memory.h): Contains both memory allocator classes. Paging and Flat Memory.
- [`Command.h`](Command.h): Contains the ENUM class holding the list of the valid commands.
- [`backingStore.txt`](backingStore.txt): The simulated backing store that contains a list of all processes currently in the backing store, each process on its own line.
- [`report.txt`](report.txt): File outputed by the system when running the `report-util` command.
- [`output`](output): Folder containing files written to by each process.

