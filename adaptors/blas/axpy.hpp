#ifdef COMPILATION// -*-indent-tabs-mode:t;c-basic-offset:4;tab-width:4;-*-
$CXX $0 -o $0x `pkg-config --libs blas` -lboost_unit_test_framework&&$0x&&rm $0x;exit
#endif
// © Alfredo Correa 2019-2020

#ifndef MULTI_ADAPTORS_BLAS_AXPY_HPP
#define MULTI_ADAPTORS_BLAS_AXPY_HPP

#include "../blas/core.hpp"
#include "../../config/NODISCARD.hpp"
#include "../../config/DELETE.hpp"

namespace boost{
namespace multi{namespace blas{

using core::axpy;

template<class T, class It1, class Size, class OutIt>
auto axpy_n(T alpha, It1 first, Size n, OutIt d_first)
{	return axpy(n, alpha, base(first), stride(first), base(d_first), stride(d_first)), d_first + n;}

template<class T, class It1, class OutIt>
auto axpy(T alpha, It1 first, It1 last, OutIt d_first)
->decltype(axpy_n(alpha, first, std::distance(first, last), d_first)){
	return axpy_n(alpha, first, std::distance(first, last), d_first);}

template<class T, class X1D, class Y1D, DELETE((not std::is_assignable<Y1D&&, X1D>{}))>
auto axpy(T alpha, X1D const& x, Y1D&& y)
->decltype(axpy(size(x), alpha, base(x), stride(x), base(y), stride(y)), std::forward<Y1D>(y)){assert(size(x)==size(y));
	return axpy(size(x), alpha, base(x), stride(x), base(y), stride(y)), std::forward<Y1D>(y);}

template<class T, class X1D, class Y1D>
NODISCARD("because input is read-only")
auto axpy(T alpha, X1D const& x, Y1D const& y)->std::decay_t<
decltype(axpy(alpha, x, typename Y1D::decay_type{y})){
return   axpy(alpha, x, typename Y1D::decay_type{y});}

template<class X1D, class Y1D, DELETE((not std::is_assignable<Y1D&&, X1D>{}))>
Y1D&& axpy(X1D const& x, Y1D&& y){return axpy(+1., x, std::forward<Y1D>(y));}

template<class T, class X1D, class Y1D>
NODISCARD("because input is read-only")
auto axpy(X1D const& x, Y1D const& y)->std::decay_t<
decltype(axpy(x, typename Y1D::decay_type{y}))>{
return   axpy(x, typename Y1D::decay_type{y}));}

}}

}

#if not __INCLUDE_LEVEL__ // _TEST_MULTI_ADAPTORS_BLAS_AXPY

#define BOOST_TEST_MODULE "C++ Unit Tests for Multi.BLAS axpy"
#define BOOST_TEST_DYN_LINK
#include<boost/test/unit_test.hpp>
//#include<boost/test/floating_point_comparison.hpp>

#include "../../array.hpp"
#include "../../utility.hpp"

#include<complex>
#include<cassert>
#include<iostream>
#include<numeric>
#include<algorithm>

#include "../blas/numeric.hpp"

using std::cout;
namespace multi = boost::multi;
namespace blas = multi::blas;

BOOST_AUTO_TEST_CASE(multi_blas_axpy_double){
	multi::array<double, 2> const cA = {
		{1.,  2.,  3.,  4.},
		{5.,  6.,  7.,  8.},
		{9., 10., 11., 12.}
	};
	multi::array<double, 2> A = cA;
	multi::array<double, 1> const b = cA[2];

	blas::axpy(2., b, A[1]); // y = a*x + y, y+= a*x
	BOOST_REQUIRE( A[1][2] == 2.*b[2] + cA[1][2] );

	using complex = std::complex<double>; complex const I = {0, 1};
	multi::array<complex, 1> AC = {1. + 2.*I, 3. + 4.*I, 4. - 8.*I};
	multi::array<complex, 1> BC(size(AC), complex{0.});
//	blas::axpy(+1., begin(blas::real(AC)), end(blas::real(AC)), begin(blas::real(BC)));
//	blas::axpy(-1., begin(blas::imag(AC)), end(blas::imag(AC)), begin(blas::imag(BC)));
	
	blas::axpy(+1., blas::real(AC), blas::real(BC));
	blas::axpy(-1., blas::imag(AC), blas::imag(BC));

	BOOST_REQUIRE( BC[2] == std::conj(AC[2]) );
}

BOOST_AUTO_TEST_CASE(multi_blas_axpy_double_const){
	using complex = std::complex<double>;
	multi::array<complex, 1> const x = {00., 01., 02.};
	multi::array<complex, 1> const y = {10., 11., 12., 13.};
	static_assert( not std::is_assignable<decltype(y({0, 3})), decltype(x)>{}, "!" );

	multi::array<complex, 1> const y_cpy = blas::axpy( 4., x, y({0, 3}) );
	BOOST_REQUIRE( y_cpy[1] == 4.*01. + 11. );
	
	multi::array<complex, 1> y_mut = {10., 11., 12., 13.};
	blas::axpy( 4., x, y_mut({0, 3}) );
	BOOST_REQUIRE( y_mut[1] == 4.*01. + 11. );
}

#endif
#endif

