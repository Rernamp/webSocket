#pragma once

#include <array>
#include <cstring>

template<typename TypeContainer, std::size_t SizeContainer>
class Container {
public:
	Container() {
		_buffer.fill(0);
		reset();
	}

	void reset() {
		_startIndex = 0;
		_endIndex = 0;
		_freeSize = SizeContainer;
	}

	bool add(TypeContainer* data, std::size_t size) {
		bool result = checkFreeSize(size);

		if (result) {
			if ((_endIndex >= _startIndex) && (size > (SizeContainer - _endIndex))) {
				std::size_t partOfDataSize = SizeContainer - _endIndex;
				memcpy(&_buffer[_endIndex], data, partOfDataSize);
				_endIndex = 0;
				std::size_t secondPartSize = (size - partOfDataSize);
				memcpy(&_buffer[_endIndex], (data + secondPartSize), secondPartSize);
				_endIndex += secondPartSize;
				_endIndex %= SizeContainer;

			} else {
				memcpy(&_buffer[_endIndex], data, size);
				_endIndex += size;
				_endIndex %= SizeContainer;
			}
			_freeSize -= size;
		}

		return result;
	}

	TypeContainer* pointerForAdd(std::size_t& size) {
		TypeContainer* result = nullptr;
		if (checkFreeSize(size)) {
			if (size > (SizeContainer - _endIndex)) {
				size = SizeContainer - _endIndex;
			}
			result = &_buffer[_endIndex];
			_endIndex += size;
			_freeSize -= size;
		}
		return result;
	}

	bool get(TypeContainer* value, std::size_t size) {
		bool result = checkUsedSize(size);

		if (result) {
			if ((_startIndex < _endIndex) && (size > (SizeContainer - _startIndex))) {
				std::size_t sizeFirstPartOfData = SizeContainer - _startIndex;
				memcpy(value, &_buffer[_startIndex], sizeFirstPartOfData);
				_startIndex = 0;
				std::size_t sizeSecondPartOfData = size - sizeFirstPartOfData;

				memcpy(value, &_buffer[_startIndex], sizeSecondPartOfData);
				_startIndex += sizeSecondPartOfData;
				_startIndex %= SizeContainer;
			} else {
				memcpy(value, &_buffer[_startIndex], size);
				_startIndex += size;
				_startIndex %= SizeContainer;
			}
			_freeSize += size;
		}

		return result;
	}

	TypeContainer* pointerForGet(std::size_t& size) {
		TypeContainer* result = nullptr;
		if (checkUsedSize(size)) {
			result = &_buffer[_startIndex];
			if ((size > (SizeContainer - _startIndex))) {	// TODO chage if to ternar operator
				size = SizeContainer - _startIndex;
			}
			_startIndex += size;
			_startIndex %= SizeContainer;
			_freeSize += size;
		}

		return result;
	}

	std::size_t getUsedSize() {
		return (SizeContainer - _freeSize);
	}

	std::size_t getFreeSize() {
		return _freeSize;
	}

	std::size_t getSize() {
		return SizeContainer;
	}

private:

	bool checkFreeSize(std::size_t size) {
		return (size <= _freeSize);
	}
	bool checkUsedSize(std::size_t size) {
		return (size <= getUsedSize());
	}

	std::array<TypeContainer, SizeContainer> _buffer {};
	std::size_t _startIndex = 0;
	std::size_t _endIndex = 0;
	std::size_t _freeSize = SizeContainer;
};

