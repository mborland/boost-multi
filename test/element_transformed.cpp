// Copyright 2022-2024 Alfredo A. Correa
// Copyright 2024 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#if defined(__clang__)
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wold-style-cast"
	#pragma clang diagnostic ignored "-Wundef"
	#pragma clang diagnostic ignored "-Wconversion"
	#pragma clang diagnostic ignored "-Wsign-conversion"
	#pragma clang diagnostic ignored "-Wignored-qualifiers"
#elif defined(__GNUC__)
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wold-style-cast"
	#pragma GCC diagnostic ignored "-Wundef"
	#pragma GCC diagnostic ignored "-Wconversion"
	#pragma GCC diagnostic ignored "-Wsign-conversion"
#elif defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable : 4244)
#endif

#ifndef BOOST_TEST_MODULE
	#define BOOST_TEST_MAIN
#endif

#include <boost/test/unit_test.hpp>

#if defined(__clang__)
	#pragma clang diagnostic pop
#elif defined(__GNUC__)
	#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
	#pragma warning(pop)
#endif

#include <boost/multi/array.hpp>  // for transform_ptr, array, subarray

#include <complex>      // for complex, operator*, operator+
#include <numeric>      // for inner_product
// #include <type_traits>  // for declval  // in utility
#include <utility>      // for declval, forward
#include <vector>       // for vector

namespace multi = boost::multi;

BOOST_AUTO_TEST_CASE(element_transformed_1D_conj_using_function_reference) {
	using complex = std::complex<double>;
	auto const I  = complex{ 0.0, 1.0 };  // NOLINT(readability-identifier-length) I imaginary unit

	multi::array<complex, 1> arr = { 1.0 + 2.0 * I, 3.0 + 4.0 * I };

	constexpr auto conj = static_cast<complex (&)(complex const&)>(std::conj<double>);

	auto const& conjd_arr = arr.element_transformed(conj);
	BOOST_REQUIRE( conjd_arr[0] == conj(arr[0]) );
	BOOST_REQUIRE( conjd_arr[1] == conj(arr[1]) );

	//  Ac[0] = 5. + 4.*I;  // this doesn't compile, good!
	BOOST_REQUIRE( conjd_arr[0] == 1.0 - 2.0*I );

	BOOST_TEST_REQUIRE( real(std::inner_product(arr.begin(), arr.end(), conjd_arr.begin(), complex{0.0, 0.0})) == std::norm(arr[0]) + std::norm(arr[1]) );
	BOOST_REQUIRE_CLOSE(imag(std::inner_product(arr.begin(), arr.end(), conjd_arr.begin(), complex{ 0.0, 0.0 })), 0.0, 1E-6);

	BOOST_TEST_REQUIRE( std::inner_product(arr.begin(), arr.end(), conjd_arr.begin(), complex{0.0, 0.0}) == std::norm(arr[0]) + std::norm(arr[1]) );
}

BOOST_AUTO_TEST_CASE(element_transformed_1D_conj_using_lambda) {
	using complex = std::complex<double>;
	auto const I  = complex{ 0.0, 1.0 };  // NOLINT(readability-identifier-length) I imaginary unit

	multi::array<complex, 1> arr = { 1.0 + 2.0 * I, 3.0 + 4.0 * I };

	// g++ -std=20 needs the transformation (lambda) to be noexcept
	auto const& conjd_arr = arr.element_transformed([](auto const& cee) noexcept { return std::conj(cee); });
	BOOST_REQUIRE( conjd_arr[0] == std::conj(arr[0]) );
	BOOST_REQUIRE( conjd_arr[1] == std::conj(arr[1]) );

	//  Ac[0] = 5. + 4.*I;  // this doesn't compile, good!
	BOOST_REQUIRE( conjd_arr[0] == 1.0 - 2.0*I );
}

BOOST_AUTO_TEST_CASE(element_transformed_1D_conj_using_lambda_with_const_return) {
	using complex = std::complex<double>;
	auto const I  = complex{ 0.0, 1.0 };  // NOLINT(readability-identifier-length) I imaginary unit

	multi::array<complex, 1> arr = { 1.0 + 2.0 * I, 3.0 + 4.0 * I };

	// g++ -std=20 needs the transformation (lambda) to be noexcept
	// NOLINTNEXTLINE(readability-const-return-type) a way to disable assignment
	auto&& conjd_arr = arr.element_transformed([](auto const& cee) noexcept -> auto const { return std::conj(cee); });  // `const` allows this idiom. it needs -Wno-nonportable-cfstrings and -Wignored-qualifiers in clang
	BOOST_REQUIRE( conjd_arr[0] == std::conj(arr[0]) );
	BOOST_REQUIRE( conjd_arr[1] == std::conj(arr[1]) );

	// conjd_arr[0] = 5.0 + 4.0*I;  // this doesn't compile, good! otherwise it would be misleading (see above)
	BOOST_REQUIRE( conjd_arr[0] == 1.0 - 2.0*I );
}

template<typename ComplexRef> struct Conjd;  // NOLINT(readability-identifier-naming) for testing

struct Conj_t {  // NOLINT(readability-identifier-naming) for testing
	template<class ComplexRef> constexpr auto operator()(ComplexRef&& zee) const { return Conjd<decltype(zee)>{ std::forward<ComplexRef>(zee) }; }
	template<class T> constexpr auto          operator()(Conjd<T> const&) const = delete;
	template<class T> constexpr auto          operator()(Conjd<T>&&) const      = delete;
	template<class T> constexpr auto          operator()(Conjd<T>&) const       = delete;
};
inline constexpr Conj_t Conj;

template<typename ComplexRef>
struct Conjd {  // NOLINT(readability-identifier-naming) for testing
	using decay_type = decltype(+std::declval<ComplexRef>());

	// explicit constexpr operator decay_type() const { return std::conj(c_); }  // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)  // NOSONAR(cpp:S1709)

	friend constexpr auto operator==(decay_type const& other, Conjd const& self) -> bool { return std::conj(self.c_) == other; }
	friend constexpr auto operator!=(decay_type const& other, Conjd const& self) -> bool { return std::conj(self.c_) != other; }

	friend constexpr auto operator==(Conjd const& self, decay_type const& other) -> bool { return other == std::conj(self.c_); }
	friend constexpr auto operator!=(Conjd const& self, decay_type const& other) -> bool { return other != std::conj(self.c_); }

	friend constexpr auto operator==(Conjd const& self, Conjd const& other) -> bool { return other.c_ == self.c_; }
	friend constexpr auto operator!=(Conjd const& self, Conjd const& other) -> bool { return other.c_ != self.c_; }

	constexpr auto operator=(decay_type const& other) && -> Conjd& {
		c_ = std::conj(other);
		return *this;
	}

 private:
	constexpr explicit Conjd(ComplexRef& cee) : c_{ cee } {}
	ComplexRef& c_;  // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members) can be a reference
	friend decltype(Conj);
};

BOOST_AUTO_TEST_CASE(element_transformed_1D_conj_using_proxy) {
	using complex = std::complex<double>;
	auto const I  = complex{ 0.0, 1.0 };  // NOLINT(readability-identifier-length) I imaginary unit

	multi::array<complex, 1> const arr = { 1.0 + 2.0 * I, 3.0 + 4.0 * I };

	auto const& conj_arr = arr.element_transformed(Conj);
	BOOST_REQUIRE( std::conj(arr[0]) == conj_arr[0] );
	BOOST_REQUIRE( std::conj(arr[1]) == conj_arr[1] );

	//  Ac[0] = 5. + 4.*I;  // not allowed, compile error, Ac is const
	BOOST_REQUIRE( conj_arr[0] == 1.0 - 2.0*I );
}

BOOST_AUTO_TEST_CASE(element_transformed_1D_conj_using_mutable_proxy) {
	using complex = std::complex<double>;
	auto const I  = complex{ 0.0, 1.0 };  // NOLINT(readability-identifier-length) I imaginary unit

	multi::array<complex, 1> arr = { 1.0 + 2.0 * I, 3.0 + 4.0 * I };

	auto&& conj_arr = arr.element_transformed(Conj);  // NOLINT(readability-const-return-type) to disable assignment

	BOOST_REQUIRE( std::conj(arr[0]) == conj_arr[0] );
	BOOST_REQUIRE( std::conj(arr[1]) == conj_arr[1] );

	conj_arr[0] = 5.0 + 4.0 * I;
	BOOST_REQUIRE( conj_arr[0] == 5.0 + 4.0*I );
	BOOST_REQUIRE(  arr[0] == 5.0 - 4.0*I );
}

BOOST_AUTO_TEST_CASE(transform_ptr_single_value) {
	using complex = std::complex<double>;
	auto const I  = complex{ 0.0, 1.0 };  // NOLINT(readability-identifier-length) I imaginary unit

	complex cee = 1.0 + 2.0 * I;

	// NOLINTNEXTLINE(readability-const-return-type,clang-diagnostic-ignored-qualifiers) to prevent assignment
	constexpr auto conj_ro = [](auto const& zee) noexcept { return std::conj(zee); };  // g++ -std=20 needs the transformation (lambda) to be noexcept

	multi::transform_ptr<complex, decltype(conj_ro), complex*> const conjd_ceeP{ &cee, conj_ro };
	BOOST_REQUIRE( *conjd_ceeP == std::conj(1.0 + 2.0*I) );
}

BOOST_AUTO_TEST_CASE(transform_ptr_1D_array) {
	using complex = std::complex<double>;
	auto const I  = complex{ 0.0, 1.0 };  // NOLINT(readability-identifier-length) I imaginary unit

	multi::array<complex, 1> arr = { 1.0 + 2.0 * I, 3.0 + 4.0 * I };

	// NOLINT(readability-const-return-type,clang-diagnostic-ignored-qualifiers) to prevent assignment
	constexpr auto conj_ro = [](auto const& zee) noexcept { return std::conj(zee); };  // g++ -std=20 needs the transformation (lambda) to be noexcept

	auto const& conjd_arr = arr.element_transformed(conj_ro);
	BOOST_REQUIRE( conjd_arr[0] == conj_ro(arr[0]) );
	BOOST_REQUIRE( conjd_arr[1] == conj_ro(arr[1]) );

	//  Ac[0] = 5. + 4.i;  // doesn't compile thanks to the `auto const` in the `conj` def
}

BOOST_AUTO_TEST_CASE(arthur_odwyer_array_transform_int) {
	struct S {  // NOLINT(readability-identifier-naming)
		int a;
		int b;
	};

	multi::array<S, 1> arr({ 2 }, S{});
	auto&&             ref = arr.element_transformed(&S::a);
	ref[0]                 = 990;

	BOOST_REQUIRE( arr[0].a == 990 );

	auto const& cref = arr.element_transformed(&S::a);
	BOOST_REQUIRE( cref[0] == 990 );
	//  cr[0] = 99.;  // compile error "assignment of read-only location"
}

BOOST_AUTO_TEST_CASE(arthur_odwyer_array_transform_int_array) {
	struct S {                                                                                                     // NOLINT(readability-identifier-naming)
		int a[10];  // NOLINT(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays) testing
		int b;
	};

	multi::array<S, 1> vec({ 2 }, S{});

	auto&& ref = vec.element_transformed(&S::a);

	ref[0][1] = 990;

	BOOST_REQUIRE( ref[0][1] == 990 );
	BOOST_REQUIRE( vec[0].a[1] == 990 );

	auto const& cref = vec.element_transformed(&S::a);
	BOOST_REQUIRE( cref[0][1] == 990 );
	//  cref[0][1] = 990;  // compile error "assignment of read-only location"
}

BOOST_AUTO_TEST_CASE(indirect_transformed) {
	std::vector<int> vec = { 00, 11, 22, 33, 44, 55 };  // std::vector NOLINT(fuchsia-default-arguments-calls)

	using index_t = std::vector<int>::size_type;

	multi::array<index_t, 1> const arr = { 4, 3, 2, 1, 0 };

	auto&& indirect_v = arr.element_transformed([&vec](index_t idx) noexcept -> int& { return vec[idx]; });

	BOOST_REQUIRE(  indirect_v[1] ==  vec[3] );
	BOOST_REQUIRE( &indirect_v[1] == &vec[3] );

	indirect_v[1] = 990;
	BOOST_REQUIRE(  vec[3] ==  990 );

	//  for(auto&& elem : indirect_v) {elem = 88.;}
	//  std::fill(indirect_v.begin(), indirect_v.end(), 88.0);

#if !defined(_MSC_VER)
	indirect_v.fill(880);
	BOOST_REQUIRE(  vec[3] ==  880 );

	auto const& const_indirect_v = indirect_v;
	(void)const_indirect_v;
	// const_indirect_v[1] = 9990;  // does not compile, good!
	BOOST_REQUIRE(const_indirect_v[3] ==  880);
#endif
}

BOOST_AUTO_TEST_CASE(indirect_transformed_carray) {
	// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays) testing legacy types
	int carr[5][3] = {
		{ 00,  10,  20},
		{100, 110, 120},
		{200, 210, 220},
		{300, 310, 320},
		{400, 410, 420},
	};

	using index_t = std::vector<int>::size_type;

	multi::array<index_t, 1> const arr = { 4, 3, 2, 1, 0 };

	// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
	auto&& indirect_v = arr.element_transformed([&carr](index_t idx) noexcept -> int(&)[3] { return carr[idx]; });

	BOOST_REQUIRE( &indirect_v[1][2] ==  &carr[3][2] );
	BOOST_REQUIRE(  indirect_v[1][2] ==  320 );

	indirect_v[1][2] = 111110;
	BOOST_TEST   (  indirect_v[1][2] ==  111110 );

	auto const& const_indirect_v = indirect_v;

	BOOST_REQUIRE(  const_indirect_v[1][2] ==  111110 );  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic) testing legacy type
	                                                    //  const_indirect_v[1][2] = 999.;  // doesn't compile, good!
}
