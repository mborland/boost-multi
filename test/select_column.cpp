// Copyright 2018-2024 Alfredo A. Correa
// Copyright 2024 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/multi/array.hpp>

// Suppress warnings from boost.test
#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wold-style-cast"
#  pragma clang diagnostic ignored "-Wundef"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wsign-conversion"
#  pragma clang diagnostic ignored "-Wfloat-equal"
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wundef"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wsign-conversion"
#  pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

#ifndef BOOST_TEST_MODULE
#  define BOOST_TEST_MAIN
#endif

#include <boost/test/unit_test.hpp>

namespace multi = boost::multi;

BOOST_AUTO_TEST_CASE(multi_array_range_section_1D) {
	multi::array<double, 1> arr = {00.0, 01.0, 02.0}; (void)arr;
	BOOST_REQUIRE( arr == arr(multi::ALL) );
	BOOST_REQUIRE( size(arr( 1 <= multi::ALL )) == 2 );
	BOOST_REQUIRE( arr( 1 <= multi::ALL )[0] == 1.0 );
	BOOST_REQUIRE( size(arr( multi::ALL < 2 )) == 2 );
	BOOST_REQUIRE( arr( multi::ALL < 2 )[1] == 1.0 );
}

BOOST_AUTO_TEST_CASE(multi_array_range_section_part1) {
	multi::array<double, 2> arr = {
		{00.0, 01.0, 02.0},
		{10.0, 11.0, 12.0},
		{20.0, 21.0, 22.0},
		{30.0, 31.0, 32.0},
	};

	using multi::_;
	using multi::U;

	BOOST_REQUIRE( size( arr(      multi::ALL     , 2) ) == size(arr) );
	BOOST_REQUIRE( size( arr(      multi::_       , 2) ) == size(arr) );
	BOOST_REQUIRE( size( arr(     *multi::_       , 2) ) == size(arr) );
	BOOST_REQUIRE( size( arr(      multi::U       , 2) ) == size(arr) );

	BOOST_REQUIRE( size( arr(      multi::ALL     , 2) ) == 4 );
	BOOST_REQUIRE( size( arr(      multi::ALL < 2 , 2) ) == 2 );
	BOOST_REQUIRE( size( arr( 1 <= multi::ALL     , 2) ) == 3 );
	BOOST_REQUIRE( size( arr( 1 <= multi::ALL < 3 , 2) ) == 2 );  // NOLINT(bugprone-chained-comparison)

	BOOST_REQUIRE( size( arr(      multi::_       , 2) ) == 4 );
	BOOST_REQUIRE( size( arr(      multi::_   < 2 , 2) ) == 2 );
	BOOST_REQUIRE( size( arr( 1 <= multi::_       , 2) ) == 3 );
	BOOST_REQUIRE( size( arr( 1 <= multi::_   < 3 , 2) ) == 2 );  // NOLINT(bugprone-chained-comparison)

	BOOST_REQUIRE( size( arr(             _       , 2) ) == 4 );
	BOOST_REQUIRE( size( arr(             _ < 2   , 2) ) == 2 );
	BOOST_REQUIRE( size( arr( 1 <=        _       , 2) ) == 3 );
	BOOST_REQUIRE( size( arr( 1 <=        _ < 3   , 2) ) == 2 );  // NOLINT(bugprone-chained-comparison)

	// BOOST_REQUIRE( size( arr(             _._       , 2) ) == 4 );
	// BOOST_REQUIRE( size( arr(             _._ < 2   , 2) ) == 2 );
	// BOOST_REQUIRE( size( arr( 1 <=        _._       , 2) ) == 3 );
	// BOOST_REQUIRE( size( arr( 1 <=        _._ < 3   , 2) ) == 2 );  // NOLINT(bugprone-chained-comparison)

	// BOOST_REQUIRE( size( arr(           _._._       , 2) ) == 4 );
	// BOOST_REQUIRE( size( arr(           _._._ < 2   , 2) ) == 2 );
	// BOOST_REQUIRE( size( arr( 1 <=      _._._       , 2) ) == 3 );
	// BOOST_REQUIRE( size( arr( 1 <=      _._._ < 3   , 2) ) == 2 );  // NOLINT(bugprone-chained-comparison)
}

BOOST_AUTO_TEST_CASE(multi_array_range_section_part2) {
	multi::array<double, 2> arr = {
		{00.0, 01.0, 02.0},
		{10.0, 11.0, 12.0},
		{20.0, 21.0, 22.0},
		{30.0, 31.0, 32.0},
	};

	BOOST_REQUIRE( size( arr(arr.extension(), 2) ) == size(arr) );

	auto&& col2( arr(arr.extension(), 2) );  // select column #2
	// same as arr(extesion(arr), 2)
	// same as arr(arr.extension(0), 2);
	// same as rotated(arr)[2];
//  BOOST_REQUIRE( col2.size(0) == size(arr) );

	BOOST_REQUIRE( dimensionality(col2) == 1 );
	BOOST_REQUIRE( size(col2) == size(arr) );
	BOOST_REQUIRE( col2.size() == size(arr) );
	BOOST_REQUIRE( col2.stride() == 3 );
	BOOST_REQUIRE( col2[0] == 02. );
	BOOST_REQUIRE( col2[1] == 12. );
	BOOST_REQUIRE(( col2 == multi::array<double, 1>{02.0, 12.0, 22.0, 32.0} ));
	BOOST_REQUIRE(( col2 == multi::array<double, 1>(rotated(arr)[2]) ));
	BOOST_REQUIRE(( col2 == rotated(arr)[2] ));
	BOOST_REQUIRE(( col2 == arr(arr.extension(), 2) ));
}

BOOST_AUTO_TEST_CASE(multi_array_range_section_syntax) {
	multi::array<double, 2> arr = {
		{00.0, 01.0, 02.0},
		{10.0, 11.0, 12.0},
		{20.0, 21.0, 22.0},
		{30.0, 31.0, 32.0},
	};

	using multi::_;
	BOOST_REQUIRE( size( arr(       _       , 2) ) == size(arr) );
	BOOST_REQUIRE( size( arr(      *_       , 2) ) == size(arr) );

	BOOST_REQUIRE( size( arr(      (_)      , 2) ) == size(arr) );

	using multi::U;
	BOOST_REQUIRE( size( arr(       U       , 2) ) == size(arr) );
	BOOST_REQUIRE( size( arr(       U       , 2) ) == size(arr) );

	BOOST_REQUIRE( size( arr(       U       , 2) ) == size(arr) );

	using multi::V;
	BOOST_REQUIRE( size( arr(       V       , 2) ) == size(arr) );
	BOOST_REQUIRE( size( arr(       V       , 2) ) == size(arr) );

	BOOST_REQUIRE( size( arr(       V       , 2) ) == size(arr) );

//  using multi::A;
//  BOOST_REQUIRE( size( arr(       arr       , 2) ) == size(arr) );
//  BOOST_REQUIRE( size( arr(       arr       , 2) ) == size(arr) );

//  BOOST_REQUIRE( size( arr(       arr       , 2) ) == size(arr) );

	BOOST_REQUIRE( size( arr(       _  < 2  , 2) ) == 2 );
	BOOST_REQUIRE( size( arr(      *_  < 2  , 2) ) == 2 );
	BOOST_REQUIRE( size( arr(       U  < 2  , 2) ) == 2 );

	BOOST_REQUIRE( size( arr( 1 <=  _       , 2) ) == 3 );
	BOOST_REQUIRE( size( arr( 1 <= *_       , 2) ) == 3 );
	BOOST_REQUIRE( size( arr( 1 <=  U       , 2) ) == 3 );

	BOOST_REQUIRE( size( arr( 1 <=  _ < 3   , 2) ) == 2 );  // NOLINT(bugprone-chained-comparison)
	BOOST_REQUIRE( size( arr( 1 <= *_ < 3   , 2) ) == 2 );  // NOLINT(bugprone-chained-comparison)
	BOOST_REQUIRE( size( arr( 1 <=  U < 3   , 2) ) == 2 );  // NOLINT(bugprone-chained-comparison)

	BOOST_REQUIRE( size( arr(      *_ < 2   , 2) ) == 2 );
	BOOST_REQUIRE( size( arr(       U < 2   , 2) ) == 2 );
}
