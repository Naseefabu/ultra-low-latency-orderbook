#include <core/dllist.hpp>
#include <core/memory_pool.hpp>
#include <l2/hashtable.hpp>

namespace hft::orderbook {

class OrderBook
{
public:
	static constexpr size_t MAX_LEVELS = 5;

	static constexpr size_t POOL_SIZE = MAX_LEVELS + 5;

	OrderBook()
	{
		ci_dllist_init(&m_bids_list);
		ci_dllist_init(&m_asks_list);
	}

	OrderBook(const OrderBook &) = delete;

	OrderBook(OrderBook &&) = delete;

	auto operator=(const OrderBook &) -> OrderBook & = delete;

	auto operator=(OrderBook &&) -> OrderBook & = delete;

	~OrderBook()
	{
		clear_bid_side();
		clear_ask_side();
	}

	void add_bid_side(uint64_t price, uint64_t qty);

	void add_ask_side(uint64_t price, uint64_t qty);

	void update_bid_side(uint64_t price, uint64_t qty);

	void update_ask_side(uint64_t price, uint64_t qty);

	void insert_bid_sorted(Level *level);

	void insert_ask_sorted(Level *level);

	void clear_bid_side();

	void clear_ask_side();

	[[nodiscard]] auto get_bid_tail_price() const -> uint64_t
	{
		using namespace core;

		auto *tail_link = ci_dllist_tail(&m_bids_list);
		auto *tail_level = container_of(tail_link, Level, link);

		return tail_level->price;
	}

	[[nodiscard]] auto get_ask_tail_price() const -> uint64_t
	{
		using namespace core;

		auto *tail_link = ci_dllist_tail(&m_asks_list);
		auto *tail_level = container_of(tail_link, Level, link);

		return tail_level->price;
	}

	/*Trivial getter.*/
	[[nodiscard]] auto get_bids_hash_table() const -> const L2HashTable &
	{
		return m_bids_hash;
	}

	[[nodiscard]] auto get_asks_hash_table() const -> const L2HashTable &
	{
		return m_asks_hash;
	}

	/*Trivial getter.*/
	[[nodiscard]] auto get_bid_count() const -> size_t
	{
		return m_bid_count;
	}

	/*Trivial getter.*/
	[[nodiscard]] auto get_ask_count() const -> size_t
	{
		return m_ask_count;
	}

	/*Trivial getter.*/
	[[nodiscard]] auto get_bids_list() const noexcept -> const core::ci_dllist *
	{
		return &m_bids_list;
	}

	/*Trivial getter.*/
	[[nodiscard]] auto get_asks_list() const noexcept -> const core::ci_dllist *
	{
		return &m_asks_list;
	}

private:
	size_t m_bid_count {};

	size_t m_ask_count {};

	core::ci_dllist m_bids_list {};

	core::ci_dllist m_asks_list {};

	L2HashTable m_bids_hash {};

	L2HashTable m_asks_hash {};

	core::MemoryPool<Level, POOL_SIZE> m_bids_pool {};

	core::MemoryPool<Level, POOL_SIZE> m_asks_pool {};
};

} // namespace hft::orderbook