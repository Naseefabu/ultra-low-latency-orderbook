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

		entry.route_count++;

		if (!is_occupied(entry))
		{
			entry.price = price;
			entry.ptr = level;
			return true;
		}

		if (matches(entry, price))
		{
			throw std::runtime_error("Adding a duplicate entry is illegal");
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

		assert(entry.route_count > 0 && "Route count must be > 0 when traversing a populated slot");
		entry.route_count--;

		if (is_occupied(entry))
		{
			if (matches(entry, price))
			{
				if (entry.route_count == 0)
				{
					entry.price = TABLE_TERMINAL_ID;
				}
				else
				{
					// If other entries depend on this slot, mark as TOMBSTONE
					entry.price = TABLE_TOMBSTONE_ID;
				}
				entry.ptr = nullptr;
				return true;
			}
		}

		index = (index + hash2(price)) & ENTRIES_MASK;
	} while (index != first_index);

	return false;
}

} // namespace hft::orderbook