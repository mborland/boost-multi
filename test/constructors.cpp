// -*-indent-tabs-mode:t;c-basic-offset:4;tab-width:4;autowrap:nil;-*-
// Copyright 2019-2023 Alfredo A. Correa

#include<boost/test/unit_test.hpp>

#include <multi/array.hpp>

#include<complex>

namespace multi = boost::multi;

struct multiplies_bind1st{
	using complex = std::complex<double>;
	explicit multiplies_bind1st(multi::array<complex, 2>&& marr) : m_(std::move(marr)) {}  // this produces a bug in nvcc11.0
 private:
	multi::array<complex, 2> m_;
};

BOOST_AUTO_TEST_CASE(multi_construct_1d) {
	multi::static_array<double, 1> arr(multi::extensions_t<1>{multi::iextension{10}}, 1.0);
//  multi::static_array<double, 1> arr(multi::array<double, 1>::extensions_type{10}, 1.);
	BOOST_REQUIRE( size(arr) == 10 );
	BOOST_REQUIRE( arr[1] == 1.0 );
}

BOOST_AUTO_TEST_CASE(multi_constructors_inqnvcc_bug) {
	using complex = std::complex<double>;

	multi::array<complex, 2> marr({10, 10});
	multiplies_bind1st(std::move(marr));
}

BOOST_AUTO_TEST_CASE(multi_constructors_1d) {
	{
		multi::array<double, 1> const arr(multi::extensions_t<1>{multi::iextension{10}});
		BOOST_REQUIRE( size(arr)==10 );
	}
	{
		multi::array<double, 1> arr(multi::extensions_t<1>{multi::iextension{10}}, double{});
		BOOST_REQUIRE( size(arr)==10 );
		BOOST_REQUIRE( arr[5]== double{} );
	}
	{
		multi::array<double, 1> arr(multi::extensions_t<1>{multi::iextension{10}}, double{});
		BOOST_REQUIRE( size(arr)==10 );
		BOOST_REQUIRE( arr[5]== double{} );
	}
	#if defined(__cpp_deduction_guides) and not defined(__NVCC__)
	{
		multi::array arr(multi::extensions_t<1>{{0, 10}}, double{});
		BOOST_REQUIRE( size(arr)==10 );
		BOOST_REQUIRE( arr[5]== double{} );
	}
	{
		multi::array arr({{0, 10}}, double{});
		BOOST_REQUIRE( size(arr)==10 );
		BOOST_REQUIRE( arr[5]== double{} );
	}
	{
		multi::array arr({10}, double{});
		BOOST_REQUIRE( size(arr)==10 );
		BOOST_REQUIRE( arr[5]== double{} );
	}
	{
		multi::array arr(10, double{});
		BOOST_REQUIRE( size(arr)==10 );
		BOOST_REQUIRE( arr[5]== double{} );
	}
	#endif
}

BOOST_AUTO_TEST_CASE(multi_constructors_2d_ctad) {
#if defined(__cpp_deduction_guides) and not defined(__NVCC__)
	multi::array arr({10, 20}, double{});
	BOOST_REQUIRE( size(arr)==10 );
	BOOST_REQUIRE( arr[5][6] == double{} );
#endif
}

BOOST_AUTO_TEST_CASE(multi_constructors) {
{
// multi::array<double, 1> arr({10}); assert(size(A)==1); // warning in clang
}{
// multi::array<double, 1> arr({10}, double{}); assert(size(arr)==10); // warning in clang
}{
// multi::array<double, 1> arr({10}, double{}); assert(size(arr)==10); // warning in clang
}{
// multi::array<double, 1> arr({10}, 0.); assert(size(arr)==10); // warning in clang
}{
// multi::array<double, 1> arr({10}, {}); assert(size(arr)==10); // error ambiguous
}{ multi::array<int        , 1> arr = {10}    ; BOOST_REQUIRE( size(arr)==1 and arr[0]==10 );
}{ multi::array<std::size_t, 1> arr = {10}    ; BOOST_REQUIRE( size(arr)==1 and arr[0]==10 );
}{ multi::array<double     , 1> arr = {10}    ; BOOST_REQUIRE( size(arr)==1 and arr[0]==10 );
}{ multi::array<int        , 1> arr({10})     ; BOOST_REQUIRE( size(arr)==1 and arr[0]==10 );
}{ multi::array<std::size_t, 1> arr({10})     ; BOOST_REQUIRE( size(arr)==1 and arr[0]==10 );
}{ multi::array<double     , 1> arr({10})     ; BOOST_REQUIRE( size(arr)==1 and arr[0]==10 );
//}{ multi::array<std::size_t, 1> arr({{10}})   ; assert( size(arr)==1 and arr[0]==10 );  // clang warns about double bracked
//}{ multi::array<int        , 1> arr({{10}})   ; assert( size(arr)==1 and arr[0]==10 );  // clang warns about double bracked
//}{ multi::array<double     , 1> arr({{10}})   ; assert( size(arr)==1 and arr[0]==10 );  // clang warns about double bracked
}
{
	multi::array<std::size_t, 1> const arr({0, 10})  ; BOOST_REQUIRE( size(arr)==2 );
}
{
	multi::array<int        , 1> const arr({0, 10})  ; BOOST_REQUIRE( size(arr)==2 );
}
{
	multi::array<double     , 1> const arr({0, 10})  ; BOOST_REQUIRE( size(arr)==2 );
}
{
	using T = multi::array<std::string, 3>;

	static_assert( std::is_nothrow_destructible_v<T> );
	static_assert( std::is_default_constructible_v<T> );
	static_assert( std::is_nothrow_default_constructible_v<T> );

	static_assert( std::is_copy_constructible_v<T> );
	static_assert( std::is_copy_assignable_v<T> );

	// static_assert( std::is_nothrow_copy_constructible_v<T> );
	// static_assert( std::is_nothrow_copy_assignable_v<T> );

	static_assert( std::is_move_constructible_v<T> );
	static_assert( std::is_move_assignable_v<T> );

	static_assert( std::is_nothrow_move_constructible_v<T> );
	static_assert( std::is_nothrow_move_assignable_v<T> );
}
}
