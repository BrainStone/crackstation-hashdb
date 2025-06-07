#ifndef CRACKSTATION_HASHDB_UNINITIALIZEDARRAY_H
#define CRACKSTATION_HASHDB_UNINITIALIZEDARRAY_H

#include <cstdint>

template<typename T>
class UninitializedArray {
	T* ptr;
	std::size_t size;
public:
	explicit UninitializedArray(std::size_t n) : size(n) {
		ptr = static_cast<T*>(::operator new(n * sizeof(T)));
	}
	~UninitializedArray() {
		::operator delete(ptr);
	}
	
	T* data() { return ptr; }
	std::size_t length() const { return size; }
	T& operator[](std::size_t i) { return ptr[i]; }
	
	// Iterator stuff
	using iterator = T*;
	using const_iterator = const T*;
	
	iterator begin() { return ptr; }
	iterator end() { return ptr + size; }
	const_iterator begin() const { return ptr; }
	const_iterator end() const { return ptr + size; }
	const_iterator cbegin() const { return ptr; }
	const_iterator cend() const { return ptr + size; }
};


#endif // CRACKSTATION_HASHDB_UNINITIALIZEDARRAY_H
