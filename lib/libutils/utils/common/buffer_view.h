#include <iostream>
#include <vector>
#include <cstdint>

#ifndef UTILS_COMMON_BUFFER_VIEW_H_
#define UTILS_COMMON_BUFFER_VIEW_H_

namespace utils {
namespace common {

class BufferView {
public:
	class const_iterator : std::iterator<
								std::input_iterator_tag,
								uint8_t
							>
	{
	public:
		const_iterator(const uint8_t *ptr): _ptr(ptr){}
		const_iterator(const const_iterator& other):
			_ptr(other._ptr)
		{
		}

		const_iterator operator++(){
			const_iterator tmp(*this);
			++_ptr;
			return tmp;
		}
		const_iterator operator++(int) {
			++_ptr;
			return *this;
		}
		const_iterator operator+(size_t from) {
			return const_iterator(_ptr + from);
		}
		const uint8_t& operator*(){return *_ptr;}
		const uint8_t* operator->(){return _ptr;}
		bool operator==(const const_iterator& rhs) {
			return _ptr == rhs._ptr;
		}
		bool operator!=(const const_iterator& rhs) {
			return _ptr != rhs._ptr;
		}

	private:
		const uint8_t *_ptr;
	};

	BufferView();
	BufferView(
			const uint8_t* buffer,
			size_t size,
			size_t from = 0
	);
	BufferView(
			const std::vector<uint8_t>& buffer,
			size_t length = 0,
			size_t from = 0
	);
	BufferView(const BufferView&) = default;
	virtual ~BufferView();

	static BufferView empty();

	BufferView& operator=(const BufferView& other);
	operator bool() const;
	const uint8_t& operator[](size_t idx) const;
	const uint8_t& at(size_t idx) const;
	const char* char_array(void) const;
	const unsigned char* byte_array(void) const;

	const_iterator begin(void) const;
	const_iterator end(void) const;

	size_t size(void) const;
protected:
	void _set_buffer(const uint8_t* buffer, size_t size);
private:
	const uint8_t* _buffer;
	size_t _size, _from;
};

} // common
} // utils



#endif /* UTILS_COMMON_BUFFER_VIEW_H_ */
