#pragma once

#include <array>
#include <cstring>

template<typename TypeContainer, std::size_t SizeContainer>
class Container {
public:
	Container() {
		_buffer.fill(0);
	}

	bool add(TypeContainer* data, std::size_t size) {
		bool result = false;

		if (size <= _freeSize) {
			memcpy(&_buffer[_lastIndex], data, size);
			fill(size);
			result = true;
		}

		return result;
	}

	bool fill(std::size_t size) {
		bool result = false;

		if (size <= _freeSize) {
			_lastIndex += size;
			_freeSize -= size;
			result = true;
		}

		return result;
	}

	TypeContainer* data() {	// rename this method // it`s lies
		return & _buffer[_lastIndex];
	}

	TypeContainer get() {
		++_freeSize;
		return _buffer[_lastIndex--];
	}

	std::size_t getSize() {
		return SizeContainer;
	}

	std::size_t getFreeSize() {
		return _freeSize;
	}
private:
	std::array<TypeContainer, SizeContainer> _buffer {};
	std::size_t _lastIndex = 0;
	std::size_t _freeSize = _buffer.size() - _lastIndex;
};

