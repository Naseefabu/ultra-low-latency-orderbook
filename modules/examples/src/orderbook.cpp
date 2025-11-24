#include <l2/orderbook.hpp>

using namespace hft::orderbook;

auto main() -> int
{
	OrderBook book;

	book.update_bid_side(10000, 150);
	book.update_bid_side(9999, 100);
	book.update_bid_side(9998, 100);
	book.update_bid_side(9997, 100);
	book.update_bid_side(9995, 100);
	book.update_bid_side(9996, 100);


	book.update_ask_side(10001, 200);
	book.update_ask_side(10002, 300);
	book.update_ask_side(10003, 300);
	book.update_ask_side(10004, 300);
	book.update_ask_side(10008, 300);
	book.update_ask_side(10006, 300);

	return 0;
}