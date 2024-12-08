#pragma once
#include <unordered_map>
enum class Command {
    EXIT,
    INITIALIZE,
    SCREEN,
    SCHEDULER_TEST,
    SCHEDULER_STOP,
    CLEAR,
    REPORT_UTIL,
    PROCESS_SMI,
    VMSTAT,
    INVALID // For unrecognized commands
};





