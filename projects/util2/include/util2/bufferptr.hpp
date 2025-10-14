#ifndef __UTIL2_BUFFER_POINTER_MANAGER__
#define __UTIL2_BUFFER_POINTER_MANAGER__
#include "util2/C/base_type.h"
#include "ifcrash.hpp"
#include "util2/C/macro.h"


namespace util2 {


template <
    typename T,
    bool deallocate_on_destroy,
    class Deallocator>
class _buffer_pointer_manager {
public:
	using class_type = _buffer_pointer_manager<T, deallocate_on_destroy, Deallocator>;

public:
	class_type&  operator=(class_type const& other) = delete;
	class_type&& operator=(class_type && 	 other) = delete;


	void create(T* buffer, u32 maximal_size) {
		m_buffer = __scast(T*, buffer);
		m_size   = maximal_size;
		return;
	}

	void create(class_type const& other) {
		m_buffer = malloc(other.m_size);
		m_size = other.m_size;
		copyFrom(other.m_buffer);
		return;
	}

	void destroy() {
		if constexpr (deallocate_on_destroy) {
			Deallocator()(m_buffer);
		}
		m_buffer = __rcast(T*, DEFAULT64);
		m_size   = DEFAULT32;
		return;
	}


	void copyFrom(T const* other) {
		ifcrashfmt_debug(m_buffer == __rcast(T*, DEFAULT64) || size == DEFAULT32, 
			"util2::_buffer_pointer_manager::copy() -> create() was not called, internal members are invalid\n"
		)
		for(u32 i = 0; i < m_size; ++i) {
			m_buffer[i] = other[i];
		}
		return;
	}


	void moveTo(class_type& to) {
		/* Transfer ownership of pointers */
		to.m_buffer = m_buffer;
		to.m_size   = m_size;

		m_buffer = __rcast(T*, DEFAULT64);
		m_size   = DEFAULT32;
		return;
	}


	auto operator[](u32 index) -> T& {
		ifcrashfmt_debug(index >= m_size, 
            "Out-of-bounds memory access, %u >= %u (idx, buf_size)", 
            index, 
            m_size
        );
		return m_buffer[index]; /* NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic) */
	}

	auto operator[](u32 index) const -> const T& {
		ifcrashfmt_debug(index >= m_size, 
            "Out-of-bounds memory access, %u >= %u (idx, buf_size)", 
            index, 
            m_size
        );
		return m_buffer[index]; /* NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic) */
	}

	auto data() const -> T* {
		return m_buffer;
	}

	[[nodiscard]] auto size() const -> u32 {
		return m_size;
	}

	/* NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic) */
	auto begin() -> T* {
		return &m_buffer[0];
	}

	auto end() -> T* {
		return &m_buffer[m_size];
	}

	auto begin() const -> const T* {
		return &m_buffer[0];
	}

	auto end() const -> const T* {
		return &m_buffer[m_size];
	}

	/* NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic) */

private:
	T*  m_buffer = __rcast(T*, DEFAULT64);
	u32 m_size   = DEFAULT32;
};


template<typename T> 
class NewDeleteDeallocator {
	void operator()(T* ptr) {
		delete[] ptr;
		return;
	}
};


template<typename T> 
class MallocFreeDeallocator {
	void operator()(T* ptr) {
		free(ptr);
		return;
	}
};



template <typename T>
using BufferPointer = _buffer_pointer_manager<T, false, void>;

template <typename T, class DeallocatorClass>
using ManagedBufferPointer = _buffer_pointer_manager<T, true, DeallocatorClass>;

template<typename T>
using ManagedMallocPointer = _buffer_pointer_manager<T, true, MallocFreeDeallocator<T>>;

template<typename T>
using ManagedNewPointer = _buffer_pointer_manager<T, true, NewDeleteDeallocator<T>>;


} /* namespace util2 */


#endif /* __UTIL2_BUFFER_POINTER_MANAGER__ */