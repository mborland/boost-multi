// -*-indent-tabs-mode:t;c-basic-offset:4;tab-width:4;autowrap:nil;-*-
// Copyright 2019-2022 Alfredo A. Correa

#define BOOST_TEST_MODULE "C++ Unit Tests for Multi pmr allocators"
#include<boost/test/unit_test.hpp>

#include "multi/array.hpp"

#include <memory_resource>  // for polymorphic memory resource, monotonic buffer
#include <numeric>

namespace multi = boost::multi;

BOOST_AUTO_TEST_CASE(pmr_partially_formed) {
	{
		char buffer[] = "0123456789012345678901234567890123456789012345678901234567890123456789";  // NOLINT(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays) use raw memory

		std::pmr::monotonic_buffer_resource mbr{std::data(buffer), std::size(buffer)};
		static_assert( std::size(buffer) > 6*sizeof(double) );

		multi::array<double, 2, std::pmr::polymorphic_allocator<double>> A({2, 3}, &mbr);  // NOLINT(readability-identifier-length)
		BOOST_TEST( buffer[ 0] == '0' );  // buffer is intact when initializing without value
		BOOST_TEST( buffer[13] == '3' );

		BOOST_TEST( A.num_elements() == 2*3 );
	//  BOOST_TEST( A[0][0] != 0. );
	//  BOOST_TEST( A[1][2] != 0. );
	}
	{
		char buffer[] = "0123456789012345678901234567890123456789012345678901234567890123456789";  // NOLINT(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays) use raw memory

		std::pmr::monotonic_buffer_resource mbr{std::data(buffer), std::size(buffer)};
		static_assert( std::size(buffer) > 6*sizeof(double) );

		multi::array<double, 2, std::pmr::polymorphic_allocator<double>> A({2, 3}, 0., &mbr);  // NOLINT(readability-identifier-length)
	//  BOOST_TEST( buffer[ 0] != '0' );  // buffer not is intact when initializing with value
	//  BOOST_TEST( buffer[13] != '3' );

		BOOST_TEST( A[0][0] == 0. );
		BOOST_TEST( A[1][2] == 0. );
	}
	{
		char buffer[] = "0123456789012345678901234567890123456789012345678901234567890123456789";  // NOLINT(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays) use raw memory

		std::pmr::monotonic_buffer_resource mbr{std::data(buffer), std::size(buffer)};
		static_assert( std::size(buffer) > 6*sizeof(double) );

		multi::array<double, 2, std::pmr::polymorphic_allocator<double>> A({2, 3}, {}, &mbr);  // NOLINT(readability-identifier-length)
	//  BOOST_TEST( buffer[ 0] != '0' );  // buffer not is intact when initializing with value
	//  BOOST_TEST( buffer[13] != '3' );

		BOOST_TEST( A[0][0] == double{} );
		BOOST_TEST( A[1][2] == double{} );
	}
	{
		char buffer[] = "0123456789012345678901234567890123456789012345678901234567890123456789";  // NOLINT(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays) use raw memory

		std::pmr::monotonic_buffer_resource mbr{std::data(buffer), std::size(buffer)};
		static_assert( std::size(buffer) > 6*sizeof(double) );

		multi::array<double, 2, std::pmr::polymorphic_allocator<double>> A({2, 3}, 666., &mbr);  // NOLINT(readability-identifier-length)
	//  BOOST_TEST( buffer[ 0] != '0' );  // buffer not is intact when initializing with value
	//  BOOST_TEST( buffer[13] != '3' );

		BOOST_TEST( A[0][0] == 666. );
		BOOST_TEST( A[1][2] == 666. );
	}
}

BOOST_AUTO_TEST_CASE(pmr_benchmark) {

//  auto* resp = std::pmr::unsynchronized_pool_resource(std::pmr::get_default_resource());
	auto* resp = std::pmr::get_default_resource();

	auto count = 500;
	auto start_time = std::chrono::high_resolution_clock::now();

	int64_t acc = 0;
	for(int64_t i = 0; i != count; ++i) {
		multi::pmr::array<int64_t, 2> arr({1000 - i%10, 1000 + i%10}, resp);
		std::fill_n(arr.data_elements(), arr.num_elements(), 1.);
		acc += std::accumulate(arr.data_elements(), arr.data_elements() + arr.num_elements(), 0L);
	}
	auto time = std::chrono::high_resolution_clock::now() - start_time;
	std::cout<< time.count() / count <<"          "<< acc <<std::endl;
}
