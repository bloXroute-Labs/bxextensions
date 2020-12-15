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
		explicit const_iterator(const uint8_t *ptr): _ptr(ptr){}
		const_iterator(const const_iterator& other):
			_ptr(other._ptr)
		{
		}

		const_iterator operator++(){
			const_iterator tmp(*this);
			++_ptr;
			return tmp;
		}
		const const_iterator operator++(int) {
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
	explicit BufferView(
			const std::vector<uint8_t>& buffer,
			size_t length = 0,
			size_t from = 0
	);
	BufferView(const BufferView& src, size_t length, size_t from);
	BufferView(const BufferView&);
	BufferView(BufferView&&) noexcept;
	virtual ~BufferView();

	static BufferView empty();

	BufferView& operator =(const BufferView& other);
	BufferView& operator =(BufferView&& other) noexcept;
	explicit operator bool() const;
	const uint8_t& operator[](size_t idx) const;
	const uint8_t& at(size_t idx) const;
	const char* char_array() const;
	const unsigned char* byte_array() const;
    std::vector<uint8_t> vector(size_t, size_t) const;

	const_iterator begin() const;
	const_iterator end() const;

	size_t size() const;

	void* ptr();

protected:
	void _set_buffer(const uint8_t* buffer, size_t size);
private:
	const uint8_t* _buffer;
	size_t _from, _size;
};

} // common
} // utils



#endif /* UTILS_COMMON_BUFFER_VIEW_H_ */
