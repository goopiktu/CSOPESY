#pragma once
#include <string>
#include <vector>
#include <unordered_map>

class IMemoryAllocator {
public:
	virtual void* allocate(size_t size) = 0;
	virtual void deallocate(void* ptr, size_t size) = 0;
	virtual size_t ptr_to_index(void* ptr) = 0;
	virtual std::string visualizeMemory() = 0;

	virtual size_t getMaximumSize() = 0;
	virtual size_t getAllocatedSize() = 0;

};

class FlatMemoryAllocator : public IMemoryAllocator {
private:
	size_t maximumSize;
	size_t allocatedSize;
	std::vector<char> memory;
	std::vector<bool> allocationMap;
	//std::unordered_map<size_t, bool> allocationMap;

	

	void initializeMemory(size_t maximumSize) {
		for (size_t i = 0; i < maximumSize; i++) {
			memory.push_back('.');
			allocationMap.push_back(false);
		}
		//std::fill(memory.begin(), memory.end(), '.'); // '.' means unallocated memory
		//std::fill(allocationMap.begin(), allocationMap.end(), false);
	}

	bool canAllocateAt(size_t index, size_t size) const {
		return (index + size <= maximumSize);
	}

	void allocateAt(size_t index, size_t size) {
		for (size_t i = index; i < index + size; i++) {
			allocationMap[i] = true;
		}
		allocatedSize += size;
	}

	void deallocateAt(size_t index, size_t size) {
		allocationMap[index] = false;
		allocatedSize -= size;
	}

public:
	size_t getMaximumSize() override { return maximumSize; }
	size_t getAllocatedSize() override { return allocatedSize; }

	FlatMemoryAllocator(size_t maximumSize) : maximumSize(maximumSize), allocatedSize(0) {
		initializeMemory(maximumSize);
	}

	~FlatMemoryAllocator() {
		memory.clear();
	}

	void* allocate(size_t size) override {

		for (size_t i = 0; i < maximumSize - size + 1; ++i) {
			if (!allocationMap[i] && canAllocateAt(i, size)) {
				allocateAt(i, size);
				return &memory[i];
			}
		}

		return nullptr;
	}

	size_t ptr_to_index(void* ptr) override {
		return static_cast<char*>(ptr) - &memory[0];
	}

	void deallocate(void* ptr, size_t size) override {
		size_t index = static_cast<char*>(ptr) - &memory[0];
		if (allocationMap[index]) {
			deallocateAt(index, size);
			
		}
	}

	std::string visualizeMemory() override {
		return std::string(memory.begin(), memory.end());
	}
};


class PagingAllocator : public IMemoryAllocator {
private:
	size_t maxMemorySize;
	size_t frameSize;
	size_t numFrames;
	std::vector<size_t> memory;                         // Memory as frames
	std::unordered_map<size_t, std::vector<size_t>> processFrameMap; // Tracks frames per process
	std::vector<size_t> freeFrameList;                  // List of free frames

	size_t allocateFrames(size_t numFrames, size_t processId) {
		if (numFrames > freeFrameList.size()) {
			throw std::runtime_error("Not enough free frames.");
		}

		std::vector<size_t> allocatedFrames;
		for (size_t i = 0; i < numFrames; ++i) {
			size_t frameIndex = freeFrameList.back();
			freeFrameList.pop_back();
			allocatedFrames.push_back(frameIndex);
			memory[frameIndex] = processId; // Mark frame as occupied
		}

		processFrameMap[processId] = allocatedFrames;
		return allocatedFrames[0];
	}

	void deallocateFrames(size_t processId) {
		if (processFrameMap.find(processId) == processFrameMap.end()) {
			throw std::runtime_error("Process ID not found.");
		}

		for (size_t frameIndex : processFrameMap[processId]) {
			memory[frameIndex] = 0;
			freeFrameList.push_back(frameIndex);
		}

		processFrameMap.erase(processId);
	}

public:
	PagingAllocator(size_t maxMemorySize, size_t frameSize)
		: maxMemorySize(maxMemorySize), frameSize(frameSize),
		numFrames(maxMemorySize / frameSize), memory(numFrames, 0) {
		for (size_t i = 0; i < numFrames; ++i) {
			freeFrameList.push_back(i);
		}
	}

	void* allocate(size_t size) override {
		size_t numFramesNeeded = (size + frameSize - 1) / frameSize;
		size_t processId = size; 
		return reinterpret_cast<void*>(allocateFrames(numFramesNeeded, processId));
	}

	void deallocate(void* ptr, size_t size) override {
		size_t processId = size; 
		deallocateFrames(processId);
	}

	size_t getMaximumSize() override {
		return maxMemorySize;
	}

	size_t getAllocatedSize() override {
		return (numFrames - freeFrameList.size()) * frameSize;
	}
};
