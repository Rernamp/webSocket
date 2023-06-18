#pragma once

#include <cstdint>
#include <cstddef>
#include <type_traits>
#include <new>
#include <algorithm>
#include <initializer_list>

namespace UDA {

template<class _ValueType, size_t _Size>
class StaticVector {
public:
	typedef size_t size_type;
	typedef _ValueType value_type;
	typedef value_type& reference;
	typedef const value_type& const_reference;

	StaticVector()
		: _size(0) {
		static_assert(_Size > 0, "Invalid array size");
	}

	template<typename... Values>
	StaticVector(const Values&... values):_size(0){
		static_assert(sizeof...(Values) <= _Size, "Not enough capacity");
		(push_back(values) , ...);
	}

	template<size_t Size>
	StaticVector(_ValueType const (&values)[Size]):_size(0){
		static_assert(Size <= _Size, "Not enough capacity");
		for(size_t i = 0; i < Size; ++i){
			push_back(values[i]);
		}
	}

	~StaticVector() {
	    clear();
	}

	size_type size() const {
		return _size;
	}

	constexpr size_type capacity() const {
		return _Size;
	}

	bool empty() const {
		return _size == 0;
	}

	bool full() const {
		return _size == _Size;
	}

	void clear() {
	    for(auto it = begin(); it != end(); ++it) {
	        it->~value_type();
	    }
	    _size = 0;
	}

	value_type* begin() {
		return reinterpret_cast<value_type*>(&_buffer[0]);
	}

	const value_type* begin() const {
		return  reinterpret_cast<const value_type*>(&_buffer[0]);
	}

	value_type* end() {
		return reinterpret_cast<value_type*>(&_buffer[_size]);
	}

	const value_type* end() const {
		return reinterpret_cast<const value_type*>(&_buffer[_size]);
	}

	//Calling this function on an empty container causes undefined behavior.
	reference back() {
		return *reinterpret_cast<value_type*>(&_buffer[_size - 1]);
	}

	//Calling this function on an empty container causes undefined behavior.
	const_reference back() const {
		return *reinterpret_cast<const value_type*>(&_buffer[_size - 1]);
	}

	template<typename ...Args>
	void emplace_back(Args&&... args) {
		if(!full()) {
			::new(&_buffer[_size++]) value_type(std::forward<Args>(args)...);
		}
	}

	void push_back(const_reference value) {
		if (!full()) {
			::new((void*)&_buffer[_size++]) value_type(value);
		}
	}

	void erase(const size_type first, const size_type count) {
		size_type last = first + count;
		if ((last <= first) || (last > _size)) {
			return;
		}

		//move items
		auto writePtr = reinterpret_cast<value_type*>(&_buffer[first]);
		for(size_type i = last; i < _size; ++i){
			*writePtr = std::move(*reinterpret_cast<value_type*>(&_buffer[i]));
			++writePtr;
		}

		//destroy tail
		for(size_type i = first; i < last; ++i){
			writePtr->~value_type();
			++writePtr;
		}

		_size -= (last - first);
	}

	void erase(const value_type& value) {
		for (size_type i = 0; i < _size; ++i) {
			if (_buffer[i] == value) {
				erase(i, 1);
				return;
			}
		}
	}

	void erase(const value_type* itemPtr) {
		if(itemPtr < begin()){
			return;
		}
		size_type id = itemPtr - begin();
		erase(id, 1);
	}

	void pop_back() {
		if (!empty()) {
			reinterpret_cast<value_type*>(&_buffer[--_size])->~value_type();
		}
	}

	reference operator[](size_type i) {
		return *reinterpret_cast<value_type*>(&_buffer[i]);
	}

	const_reference operator[](size_type i) const {
		return *reinterpret_cast<const value_type*>(&_buffer[i]);
	}

	bool contains(const_reference value) {
		for (size_type i = 0; i < _size; ++i) {
			if (_buffer[i] == value) {
				return true;
			}
		}
		return false;
	}

	void resize(size_type newSize) {
		if (newSize > _Size) {
			newSize = _Size;
		}

		for(size_type i = _size; i < newSize; ++i){
			::new(reinterpret_cast<value_type*>(&_buffer[i])) value_type;
		}

		for(size_type i = newSize; i < _size; ++i){
			reinterpret_cast<value_type*>(&_buffer[i])->~value_type();
		}

		_size = newSize;
	}

private:
	typename std::aligned_storage<sizeof(value_type), alignof(_ValueType)>::type _buffer[_Size];
	size_type _size;
};

}


