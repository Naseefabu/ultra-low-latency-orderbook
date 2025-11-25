add_library_module(orderbook l2/hashtable.cpp l2/orderbook.cpp)

target_link_libraries(orderbook PUBLIC core)

if(ENABLE_UNIT_TESTING)
    add_test_executable(orderbook orderbook.cpp)
endif()
