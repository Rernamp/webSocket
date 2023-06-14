#pragma once

#include <cstdint>
#include <cstddef>
#include <cstring>

namespace UDA {

	template<size_t Capacity = 1024>
	class Fifo {
	public:

		size_t push(const uint8_t* data, size_t size) {
			if(_size + size > Capacity) {
				size = Capacity - _size;
			}
			if(size != 0) {
				redirectIn(data, size);
			}
			return size;
		}

		size_t pop(uint8_t * data, size_t size) {
			size_t realSize = size;
			if(realSize > _size) {
				realSize = _size;
			}
			if(realSize) {
				redirectOut(data, realSize);
			}
			return realSize;
		}

		size_t size() const {
			return _size;
		}

		size_t getFreeSize() const {
			return Capacity - _size;
		}

		void clear() {
			_size = 0;
			_entry = 0;
			_exit = 0;
		}

	private:
		uint8_t _buffer[Capacity];
		size_t	_entry = 0;
		size_t	_exit = 0;
		size_t 	_size = 0;

		void redirectOut(uint8_t * data, size_t size) {
			size_t end = _exit + size;
			if(end >= Capacity) {
				size_t countCopyFirst = Capacity - _exit;
				size_t countCopySecond = size - countCopyFirst;
				memcpy(data, _buffer + _exit, countCopyFirst);
				memcpy(data + countCopyFirst, _buffer + 0, countCopySecond);
				_exit = countCopySecond;
			}
			else {
				memcpy(data, _buffer + _exit, size);
				_exit = (_exit + size) % Capacity;
			}
			_size -= size;
		}
		void redirectIn(const uint8_t * data, size_t size) {
			size_t end = _entry + size;
			if(end >= Capacity) {
				size_t countCopyFirst = Capacity - _entry;
				size_t countCopySecond = size - countCopyFirst;
				memcpy(_buffer + _entry, data, countCopyFirst);
				memcpy(_buffer + 0, data + countCopyFirst, countCopySecond);
			}
			else {
				memcpy(_buffer + _entry, data, size);
			}
			_entry = end % Capacity;
			_size += size;
		}
	};

}

