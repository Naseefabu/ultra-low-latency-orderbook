#include <gtest/gtest.h>
#include <l2/orderbook.hpp>

using namespace hft::orderbook;
using namespace hft::core;

class FixedSizeL2OrderBookTest: public ::testing::Test
{
protected:
	OrderBook book;

	auto get_bid_levels() const -> std::vector<std::pair<uint64_t, uint64_t>>
	{
		std::vector<std::pair<uint64_t, uint64_t>> levels;

		const ci_dllink *lnk;
		CI_DLLIST_FOR_EACH_CONST(lnk, book.get_bids_list())
		{
			auto *level = container_of(lnk, Level, link);
			levels.emplace_back(level->price, level->quantity);
		}
		return levels;
	}

	auto get_ask_levels() const -> std::vector<std::pair<uint64_t, uint64_t>>
	{
		std::vector<std::pair<uint64_t, uint64_t>> levels;

		const ci_dllink *lnk;
		CI_DLLIST_FOR_EACH_CONST(lnk, book.get_asks_list())
		{
			auto *level = container_of(lnk, Level, link);
			levels.emplace_back(level->price, level->quantity);
		}
		return levels;
	}
};

// ==================== BID SIDE TESTS ====================

TEST_F(FixedSizeL2OrderBookTest, Bid_InsertSingle)
{
	book.add_bid_side(1000, 10);
	auto levels = get_bid_levels();
	ASSERT_EQ(levels.size(), 1);
	EXPECT_EQ(levels[0].first, 1000);
	EXPECT_EQ(levels[0].second, 10);
}

TEST_F(FixedSizeL2OrderBookTest, Bid_InsertMultiple_SortedDescending)
{
	book.add_bid_side(1000, 10);
	book.add_bid_side(1010, 20);
	book.add_bid_side(990, 30);
	book.add_bid_side(1020, 40);

	auto levels = get_bid_levels();
	ASSERT_EQ(levels.size(), 4);
	EXPECT_EQ(levels[0].first, 1020);
	EXPECT_EQ(levels[1].first, 1010);
	EXPECT_EQ(levels[2].first, 1000);
	EXPECT_EQ(levels[3].first, 990);
}

TEST_F(FixedSizeL2OrderBookTest, Bid_MaxLevelsReached_EvictWorst)
{
	// Fill to max
	book.add_bid_side(1000, 10);
	book.add_bid_side(1100, 20);
	book.add_bid_side(1200, 30);
	book.add_bid_side(1300, 40);
	book.add_bid_side(1400, 50); // Now at MAX_LEVELS = 5

	// This should evict the lowest bid (1000)
	book.add_bid_side(1250, 99);

	auto levels = get_bid_levels();
	ASSERT_EQ(levels.size(), 5);
	EXPECT_EQ(levels[0].first, 1400);
	EXPECT_EQ(levels[1].first, 1300);
	EXPECT_EQ(levels[2].first, 1250);
	EXPECT_EQ(levels[3].first, 1200);
	EXPECT_EQ(levels[4].first, 1100);

	// 1000 should be gone
	EXPECT_EQ(book.get_bids_hash_table().lookup(1000), nullptr);
}

TEST_F(FixedSizeL2OrderBookTest, Bid_EvictOnlyWhenNewIsBetter)
{
	book.add_bid_side(1000, 10);
	book.add_bid_side(1100, 20);
	book.add_bid_side(1200, 30);
	book.add_bid_side(1300, 40);
	book.add_bid_side(1400, 50);

	// This is worse than tail (1000), should NOT insert or evict
	book.add_bid_side(900, 999);

	auto levels = get_bid_levels();
	ASSERT_EQ(levels.size(), 5);
	EXPECT_EQ(levels[4].first, 1000); // tail still 1000
	EXPECT_EQ(book.get_bids_hash_table().lookup(900), nullptr);
}

TEST_F(FixedSizeL2OrderBookTest, Bid_UpdateExisting_IncreaseQty)
{
	book.add_bid_side(1000, 10);
	book.update_bid_side(1000, 50);

	auto levels = get_bid_levels();
	ASSERT_EQ(levels.size(), 1);
	EXPECT_EQ(levels[0].second, 50);
}

TEST_F(FixedSizeL2OrderBookTest, Bid_UpdateToZero_RemovesLevel)
{
	book.add_bid_side(1000, 10);
	book.add_bid_side(1100, 20);
	book.update_bid_side(1000, 0);

	auto levels = get_bid_levels();
	ASSERT_EQ(levels.size(), 1);
	EXPECT_EQ(levels[0].first, 1100);
	EXPECT_EQ(book.get_bids_hash_table().lookup(1000), nullptr);
}

// ==================== ASK SIDE TESTS ====================

TEST_F(FixedSizeL2OrderBookTest, Ask_InsertMultiple_SortedAscending)
{
	book.add_ask_side(1000, 10);
	book.add_ask_side(990, 20);
	book.add_ask_side(1010, 30);
	book.add_ask_side(980, 40);

	auto levels = get_ask_levels();
	ASSERT_EQ(levels.size(), 4);
	EXPECT_EQ(levels[0].first, 980);
	EXPECT_EQ(levels[1].first, 990);
	EXPECT_EQ(levels[2].first, 1000);
	EXPECT_EQ(levels[3].first, 1010);
}

TEST_F(FixedSizeL2OrderBookTest, Ask_MaxLevelsReached_EvictWorst)
{
	book.add_ask_side(1400, 10);
	book.add_ask_side(1300, 20);
	book.add_ask_side(1200, 30);
	book.add_ask_side(1100, 40);
	book.add_ask_side(1000, 50); // Lowest ask

	book.add_ask_side(1150, 99); // Better than 1400 → should evict 1400

	auto levels = get_ask_levels();
	ASSERT_EQ(levels.size(), 5);
	EXPECT_EQ(levels[0].first, 1000);
	EXPECT_EQ(levels[1].first, 1100);
	EXPECT_EQ(levels[2].first, 1150);
	EXPECT_EQ(levels[3].first, 1200);
	EXPECT_EQ(levels[4].first, 1300);

	EXPECT_EQ(book.get_asks_hash_table().lookup(1400), nullptr);
}

TEST_F(FixedSizeL2OrderBookTest, Ask_EvictOnlyWhenNewIsBetter)
{
	book.add_ask_side(1000, 10);
	book.add_ask_side(1100, 20);
	book.add_ask_side(1200, 30);
	book.add_ask_side(1300, 40);
	book.add_ask_side(1400, 50);

	// Worse than current best ask (1000), should not insert
	book.add_ask_side(1500, 999);

	auto levels = get_ask_levels();
	ASSERT_EQ(levels.size(), 5);
	EXPECT_EQ(levels[0].first, 1000); // best ask unchanged
	EXPECT_EQ(book.get_asks_hash_table().lookup(1500), nullptr);
}

TEST_F(FixedSizeL2OrderBookTest, Ask_UpdateToZero_RemovesLevel)
{
	book.add_ask_side(1000, 10);
	book.add_ask_side(1010, 20);
	book.update_ask_side(1000, 0);

	auto levels = get_ask_levels();
	ASSERT_EQ(levels.size(), 1);
	EXPECT_EQ(levels[0].first, 1010);
}

// ==================== MIXED & UPDATE TESTS ====================

TEST_F(FixedSizeL2OrderBookTest, Update_NonExistingWithQtyZero_DoesNothing)
{
	book.add_bid_side(1000, 10);
	book.update_bid_side(9999, 0); // Should not crash or add

	auto levels = get_bid_levels();
	EXPECT_EQ(levels.size(), 1);
	EXPECT_EQ(book.get_bids_hash_table().lookup(9999), nullptr);
}

TEST_F(FixedSizeL2OrderBookTest, Update_NonExistingWithPositiveQty_Inserts)
{
	book.update_bid_side(1050, 25); // No prior add

	auto levels = get_bid_levels();
	ASSERT_EQ(levels.size(), 1);
	EXPECT_EQ(levels[0].first, 1050);
	EXPECT_EQ(levels[0].second, 25);
}

TEST_F(FixedSizeL2OrderBookTest, FullBook_UpdateWorstToBetter_PromotesIt)
{
	// Fill bids
	book.add_bid_side(1000, 1);
	book.add_bid_side(1100, 1);
	book.add_bid_side(1200, 1);
	book.add_bid_side(1300, 1);
	book.add_bid_side(1400, 1); // Best

	// Update worst (1000) to be much better
	book.update_bid_side(1000, 10); // Still at 1000 → but wait, price didn't change!
	// Actually price is key — so to promote, we need to remove + reinsert at better price

	// Let's do proper promote via remove + insert new
	book.update_bid_side(1000, 0);  // remove old
	book.update_bid_side(1500, 99); // insert new best

	auto levels = get_bid_levels();
	ASSERT_EQ(levels.size(), 5);
	EXPECT_EQ(levels[0].first, 1500); // new best
	EXPECT_EQ(book.get_bids_hash_table().lookup(1000), nullptr);
}

// ==================== CLEAR TESTS ====================

TEST_F(FixedSizeL2OrderBookTest, ClearBidSide_RemovesAll)
{
	book.add_bid_side(1000, 10);
	book.add_bid_side(1100, 20);
	book.clear_bid_side();

	EXPECT_TRUE(ci_dllist_is_empty(book.get_bids_list()));
	EXPECT_EQ(book.get_bid_count(), 0);
	EXPECT_EQ(book.get_bids_hash_table().lookup(1000), nullptr);
	EXPECT_EQ(book.get_bids_hash_table().lookup(1100), nullptr);
}

TEST_F(FixedSizeL2OrderBookTest, Destructor_CleansUpProperly)
{
	{
		OrderBook local_book;
		local_book.add_bid_side(1000, 10);
		local_book.add_ask_side(1010, 20);
		// local_book destructs here
	}
	// If no leaks or double-free → test passes (valgrind would catch issues)
	SUCCEED();
}

// ==================== EDGE CASE: Exact Price Duplicate (Should Update) ====================

TEST_F(FixedSizeL2OrderBookTest, SamePriceBid_UpdateQuantity)
{
	book.add_bid_side(1000, 10);
	book.add_bid_side(1000, 25); // Should update existing

	auto levels = get_bid_levels();
	ASSERT_EQ(levels.size(), 1);
	EXPECT_EQ(levels[0].second, 25);
}

TEST_F(FixedSizeL2OrderBookTest, InsertIntoEmptyBookAfterClear)
{
	book.add_bid_side(1000, 10);
	book.clear_bid_side();
	book.add_bid_side(2000, 30);

	auto levels = get_bid_levels();
	ASSERT_EQ(levels.size(), 1);
	EXPECT_EQ(levels[0].first, 2000);
	EXPECT_EQ(levels[0].second, 30);
}