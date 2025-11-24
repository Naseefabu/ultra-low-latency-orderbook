#pragma once

#ifndef TESTING
	#define MOCKABLE

	#define TEST_DEFAULT_COMPARISON_OP(CLASS_NAME)

#else
	#define MOCKABLE virtual

    // NOLINTNEXTLINE
	#define TEST_DEFAULT_COMPARISON_OP(CLASS_NAME) \
		friend auto operator==(const CLASS_NAME &lhs, const CLASS_NAME &rhs)->bool = default;

#endif
