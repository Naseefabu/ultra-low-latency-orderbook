#include <l2/hashtable.hpp>

namespace hft::orderbook {

auto L2HashTable::lookup(uint64_t price) const -> Level *
{
	const auto first_index = hash1(price);
	auto index = first_index;
	do
	{
		const auto &entry = m_table[index];
		if (is_occupied(entry))
		{
			if (matches(entry, price))
			{
				return entry.ptr;
			}
		}
		else if (!is_tombstone(entry))
		{
			return nullptr;
		}
		index = (index + hash2(price)) & ENTRIES_MASK;
	} while (index != first_index);

	return nullptr;
}

auto L2HashTable::insert(uint64_t price, Level *level) -> bool
{
	const auto first_index = hash1(price);
	auto index = first_index;
	do
	{
		auto &entry = m_table[index];
		if (is_occupied(entry))
		{
			if (matches(entry, price))
			{
				entry.ptr = level;
				return true;
			}
		}
		else
		{
			entry.price = price;
			entry.ptr = level;
			return true;
		}
		index = (index + hash2(price)) & ENTRIES_MASK;
	} while (index != first_index);

	return false; // Full (probe cycle exhausted)
}

auto L2HashTable::remove(uint64_t price) -> bool
{
	const auto first_index = hash1(price);
	auto index = first_index;
	do
	{
		auto &entry = m_table[index];
		if (is_occupied(entry))
		{
			if (matches(entry, price))
			{
				entry.price = ~0ULL;
				entry.ptr = nullptr;
				return true;
			}
		}
		else if (!is_tombstone(entry))
		{
			return false;
		}
		index = (index + hash2(price)) & ENTRIES_MASK;
	} while (index != first_index);

	return false;
}

} // namespace hft::orderbook