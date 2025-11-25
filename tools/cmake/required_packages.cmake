if(ENABLE_UNIT_TESTING)
    find_package(GTest REQUIRED)
endif()

find_package(spdlog REQUIRED)

include(GoogleTest)
