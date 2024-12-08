#pragma once
#include <string>
#include <vector>
#include <unordered_map>

class IMemoryAllocator {
public:
	virtual bool allocate(size_t size, int id) = 0;
	virtual void deallocate(int id) = 0;

	virtual size_t getMaximumSize() = 0;
	virtual size_t getAllocatedSize() = 0;

	virtual size_t getMaxFrames() = 0;
	virtual size_t getFrameIn() = 0;

};

class FlatMemoryAllocator : public IMemoryAllocator {
private:
	size_t maximumSize;
	size_t allocatedSize = 0;
	std::vector<int> allocationMap;
	//std::unordered_map<size_t, bool> allocationMap;

	void initializeMemory() {
		for (size_t i = 0; i < maximumSize; i++) {
			allocationMap.push_back(-1);
		}
	}

	bool canAllocateAt(size_t index, size_t size) {
		
		for (size_t i = index; i < index + size; i++) {
			if (allocationMap[i] != -1) return false;
		}
		return true;
	}

	void allocateAt(size_t index, size_t size, int id) {
		for (size_t i = index; i < index + size; i++) {
			allocationMap[i] = id;
		}
		allocatedSize += size;
	}

	

public:
	size_t getMaximumSize() override { return maximumSize; }
	size_t getAllocatedSize() override { return allocatedSize; }

	size_t getMaxFrames() override { return 0; }
	size_t getFrameIn() override { return 0; }

	FlatMemoryAllocator(size_t maximumSize) : maximumSize(maximumSize){
		initializeMemory();
	}

	~FlatMemoryAllocator() {
		allocationMap.clear();
	}

	bool allocate(size_t size, int id) override {

		for (int i = 0; i < maximumSize - size + 1; ++i) {
			if (canAllocateAt(i, size)) {
				allocateAt(i, size, id);
				return true;
			}
		}

		return false;
	}

	void deallocate(int id) override{
		for (size_t i = 0; i < maximumSize; i++) {
			if (allocationMap[i] == id) {
				allocationMap[i] = -1;
				allocatedSize--;
			}
		}
	}
};


class PagingAllocator : public IMemoryAllocator {
private:
	size_t maximumSize;
	size_t frameSize;
	size_t num_frames;

	std::vector<int> allocationMap;
	std::queue<int> freeFrameList;

	void initializeMemory() {
		for (size_t i = 0; i < num_frames; i++) {
			allocationMap.push_back(-1);
			freeFrameList.push(i);
		}
	}

	bool canAllocateAt(size_t numFramesNeeded) {
		
		return (freeFrameList.size() >= numFramesNeeded);
	}

	void allocateAt(size_t numFramesNeeded, int id) {
		for (size_t i = 0; i < numFramesNeeded; i++) {
			int index = freeFrameList.front();
			freeFrameList.pop();
			allocationMap[index] = id;
		}
	}



public:
	size_t getMaximumSize() override { return maximumSize; }
	size_t getAllocatedSize() override { return frameSize * (num_frames-freeFrameList.size()); }

	size_t getMaxFrames() override { return num_frames; }
	size_t getFrameIn() override { return (num_frames - freeFrameList.size()); }

	PagingAllocator(size_t maximumSize, size_t frameSize) : maximumSize(maximumSize), frameSize(frameSize), num_frames(maximumSize/ frameSize){
		initializeMemory();
	}

	~PagingAllocator() {
		allocationMap.clear();
	}

	bool allocate(size_t size, int id) override {
		int frames_needed = 1 + ((size - 1) / frameSize);

		if (canAllocateAt(frames_needed)) {
			allocateAt(frames_needed, id);
			return true;
		}

		return false;
	}

	void deallocate(int id) override {
		for (size_t i = 0; i < num_frames; i++) {
			if (allocationMap[i] == id) {
				allocationMap[i] = -1;
				freeFrameList.push(i); //return index to freeFrameList
			}
		}
	}
};
