#pragma once

#include <core/dllist.hpp>

namespace hft::orderbook {

struct Level
{
	uint64_t price {};

	uint64_t quantity {};

	core::ci_dllink link;
};

class L2HashTable
{
public:
	static constexpr size_t ENTRIES_SHIFT = 10;

	static constexpr size_t ENTRIES = 1ULL << ENTRIES_SHIFT;

	static constexpr size_t ENTRIES_MASK = ENTRIES - 1;

	// --- State Markers for uint64_t price field ---
	// Price = 0 means the slot is not occupied and stops a search (TERMINAL)
	// Price = ~0ULL (all bits set) means the slot is not occupied but must be
	// traversed (TOMBSTONE).
	static constexpr uint64_t TABLE_TERMINAL_ID = 0ULL;

	static constexpr uint64_t TABLE_TOMBSTONE_ID = ~0ULL;

	struct L2Entry
	{
		uint64_t price {};

		Level *ptr = nullptr;

		uint16_t route_count {};
	};

	L2HashTable() = default;

	L2HashTable(const L2HashTable &) = delete;

	L2HashTable &operator=(const L2HashTable &) = delete;

	[[nodiscard]] auto lookup(uint64_t price) const -> Level *;

	auto insert(uint64_t price, Level *level) -> bool;

	auto remove(uint64_t price) -> bool;

	void reset()
	{
		m_table.fill(L2Entry {});
	}

private:
	[[nodiscard]] auto hash1(uint64_t key) const -> size_t
	{
		return static_cast<size_t>(key) & ENTRIES_MASK;
	}

	[[nodiscard]] auto hash2(uint64_t key) const -> size_t
	{
		key = key * 0x9ddfea08eb382d69ULL;
		key ^= key >> 33;
		key *= 0xc4ceb9fe1a85ec53ULL;
		key ^= key >> 33;
		auto h = static_cast<size_t>(key) & ENTRIES_MASK;
		return (h | 1); // Ensure odd for double hashing
	}

	[[nodiscard]] auto matches(const L2Entry &entry, uint64_t key) const -> bool
	{
		return entry.price == key;
	}

	[[nodiscard]] auto is_occupied(const L2Entry &entry) const noexcept -> bool
	{
		return entry.price != TABLE_TERMINAL_ID && entry.price != TABLE_TOMBSTONE_ID;
	}

	[[nodiscard]] auto is_tombstone(const L2Entry &entry) const noexcept -> bool
	{
		return entry.price == TABLE_TOMBSTONE_ID;
	}

	std::array<L2Entry, ENTRIES> m_table {};
};

} // namespace hft::orderbook