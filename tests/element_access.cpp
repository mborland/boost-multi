#ifdef COMPILATION_INSTRUCTIONS
$CXX -Wall -Wextra -Wpedantic $0 -o $0x -lboost_unit_test_framework&&$0x&&rm $0x; exit
#endif

#define BOOST_TEST_MODULE "C++ Unit Tests for Multi element access"
#define BOOST_TEST_DYN_LINK
#include<boost/test/unit_test.hpp>

#include "../array.hpp"

#if __cpp_lib_apply>=201603
#include<tuple> // apply
#else
#include<experimental/tuple>
#endif

namespace multi = boost::multi;

BOOST_AUTO_TEST_CASE(multi_tests_element_access){
	multi::array<double, 2> m({3, 3}, {});
	std::array<int, 2> p = {1, 2};
	BOOST_REQUIRE( &m[p[0]][p[1]] == &m(p[0], p[1]) );
#if __cpp_lib_apply>=201603
	BOOST_REQUIRE( &m[p[0]][p[1]] == &std::apply(m, p) );
#else
	BOOST_REQUIRE( &m[p[0]][p[1]] == &std::experimental::apply(m, p) );
#endif
}

