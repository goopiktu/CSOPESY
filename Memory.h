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