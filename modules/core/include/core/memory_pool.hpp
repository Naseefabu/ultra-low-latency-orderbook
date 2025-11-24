#pragma once

#include <core/sllist.hpp>

namespace hft::core {

/*Fixed Intrusive Memory pool with O(1) allocation and deallocation*/
template<typename T, std::size_t N>
class MemoryPool
{
	static_assert(N > 0 && N <= 64, "Pool size must be between 1 and 64");

public:
	MemoryPool() noexcept: m_storage {}, m_free_list {}
	{
		m_free_list.reserve(N);
		// Push all pointers to free list
		for (auto &obj : m_storage)
		{
			m_free_list.push_back(&obj);
		}
	}

	MemoryPool(const MemoryPool &) = delete;

	MemoryPool &operator=(const MemoryPool &) = delete;

	MemoryPool(MemoryPool &&) = delete;

	MemoryPool &operator=(MemoryPool &&) = delete;

	auto allocate() noexcept -> T *
	{
		if (m_free_list.empty())
		{
			return nullptr;
		}
		T *obj = m_free_list.back();
		m_free_list.pop_back();
		return obj;
	}

	void deallocate(T *obj) noexcept
	{
		if (!obj) [[unlikely]]
		{
			return;
		}
		m_free_list.push_back(obj);
	}

	[[nodiscard]] auto empty() const noexcept -> bool
	{
		return m_free_list.empty();
	}

	/*Trivial getter*/
	[[nodiscard]] auto capacity() const noexcept -> std::size_t
	{
		return N;
	}

	/*Trivial getter*/
	[[nodiscard]] auto &get_storage() noexcept
	{
		return m_storage;
	}

public:
	std::array<T, N> m_storage;

	std::vector<T *> m_free_list;
};

} // namespace hft::core