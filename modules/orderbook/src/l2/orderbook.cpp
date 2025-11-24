#include <l2/orderbook.hpp>

using namespace hft::core;

namespace hft::orderbook {

void OrderBook::add_bid_side(uint64_t price, uint64_t qty)
{
	auto *existing = m_bids_hash.lookup(price);
	if (existing)
	{
		existing->quantity = qty;
		return;
	}

	if (m_bid_count >= MAX_LEVELS)
	{
		if (ci_dllist_is_empty(&m_bids_list))
		{
			// Should not happen
			return;
		}

		const auto tail_price = get_bid_tail_price();
		if (price <= tail_price)
		{
			return;
		}

		// Remove tail
		auto *tail_link = ci_dllist_tail(&m_bids_list);
		auto *tail_level = container_of(tail_link, Level, link);
		ci_dllist_remove(tail_link);
		m_bids_hash.remove(tail_level->price);
		m_bids_pool.deallocate(tail_level);
	}

	auto *new_level = m_bids_pool.allocate();
	if (!new_level) [[unlikely]]
	{
		return;
	}

	new_level->price = price;
	new_level->quantity = qty;
	new_level->link = { nullptr, nullptr };
	insert_bid_sorted(new_level);
	m_bids_hash.insert(price, new_level);

	++m_bid_count;
}

void OrderBook::add_ask_side(uint64_t price, uint64_t qty)
{
	auto *existing = m_asks_hash.lookup(price);
	if (existing)
	{
		existing->quantity = qty;
		return;
	}

	if (m_ask_count >= MAX_LEVELS)
	{
		if (ci_dllist_is_empty(&m_asks_list))
		{
			// Should not happen
			return;
		}

		const auto tail_price = get_ask_tail_price();
		if (price >= tail_price)
		{
			return;
		}

		// Remove tail
		auto *tail_link = ci_dllist_tail(&m_asks_list);
		auto *tail_level = container_of(tail_link, Level, link);
		ci_dllist_remove(tail_link);
		m_asks_hash.remove(tail_level->price);
		m_asks_pool.deallocate(tail_level);
	}

	auto *new_level = m_asks_pool.allocate();
	if (!new_level) [[unlikely]]
	{
		return;
	}

	new_level->price = price;
	new_level->quantity = qty;
	new_level->link = { nullptr, nullptr };
	insert_ask_sorted(new_level);
	m_asks_hash.insert(price, new_level);

	++m_ask_count;
}

void OrderBook::update_bid_side(uint64_t price, uint64_t qty)
{
	auto *level = m_bids_hash.lookup(price);

	if (level)
	{
		if (qty == 0)
		{
			core::ci_dllist_remove(&level->link);
			m_bids_hash.remove(price);
			m_bids_pool.deallocate(level);
			--m_bid_count;
		}
		else
		{
			level->quantity = qty;
		}
		return;
	}

	if (qty == 0) [[unlikely]]
	{
		return;
	}
	/*Insert New*/
	add_bid_side(price, qty);
}

void OrderBook::update_ask_side(uint64_t price, uint64_t qty)
{
	auto *level = m_asks_hash.lookup(price);

	if (level)
	{
		if (qty == 0)
		{
			core::ci_dllist_remove(&level->link);
			m_asks_hash.remove(price);
			m_asks_pool.deallocate(level);
			--m_ask_count;
		}
		else
		{
			level->quantity = qty;
		}
		return;
	}

	if (qty == 0) [[unlikely]]
	{
		return;
	}
	/*Insert New*/
	add_ask_side(price, qty);
}

void OrderBook::insert_bid_sorted(Level *level)
{
	auto *link_pos = &level->link;
	auto *pos = &m_bids_list.l;

	ci_dllink *lnk;
	CI_DLLIST_FOR_EACH(lnk, &m_bids_list)
	{
		auto *cur = container_of(lnk, Level, link);
		if (cur->price < level->price)
		{
			break;
		}
		pos = lnk;
	}
	ci_dllist_insert_after(pos, link_pos);
}

void OrderBook::insert_ask_sorted(Level *level)
{
	auto *link_pos = &level->link;
	auto *pos = &m_asks_list.l;

	ci_dllink *lnk;
	CI_DLLIST_FOR_EACH(lnk, &m_asks_list)
	{
		auto *cur = container_of(lnk, Level, link);
		if (cur->price > level->price)
		{
			pos = lnk;
			break;
		}
	}
	ci_dllist_insert_before(pos, link_pos);
}

void OrderBook::clear_bid_side()
{
	ci_dllink *lnk;
	CI_DLLIST_FOR_EACH(lnk, &m_bids_list)
	{
		auto *level = container_of(lnk, Level, link);
		m_bids_hash.remove(level->price);
		m_bids_pool.deallocate(level);
	}
	m_bid_count = 0;
	ci_dllist_init(&m_bids_list);
}

void OrderBook::clear_ask_side()
{
	ci_dllink *lnk;
	CI_DLLIST_FOR_EACH(lnk, &m_asks_list)
	{
		auto *level = container_of(lnk, Level, link);
		m_asks_hash.remove(level->price);
		m_asks_pool.deallocate(level);
	}
	m_ask_count = 0;
	ci_dllist_init(&m_asks_list);
}

} // namespace hft::orderbook