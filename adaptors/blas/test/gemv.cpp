// -*-indent-tabs-mode:t;c-basic-offset:4;tab-width:4;-*-
// © Alfredo A. Correa 2020-2021

#define BOOST_TEST_MODULE "C++ Unit Tests for Multi BLAS gemv"
#define BOOST_TEST_DYN_LINK
#include<boost/test/unit_test.hpp>

#include "config.hpp"

#include "../../../adaptors/blas/gemv.hpp"
#include "../../../array.hpp"

#include "../../../array.hpp"
#include "../../../utility.hpp"

#include "../../blas/dot.hpp"
#include "../../blas/axpy.hpp"
#include "../../blas/nrm2.hpp"
#include "../../blas/gemm.hpp"

#include<random>

namespace utf = boost::unit_test;
namespace tt = boost::test_tools;

namespace multi = boost::multi;
namespace blas = multi::blas;

template<class T> void what(T&&) = delete;

template<class M, class VI, class VO>
void MV(M const& m, VI const& x, VO&& y){
	std::transform(
		begin(m), end(m), begin(y),
		[&x](auto&& row){return std::inner_product(begin(row), end(row), begin(x), 0.);}
	);
}

BOOST_AUTO_TEST_CASE(multi_blas_gemv, *utf::tolerance(0.0001)){

	multi::array<double, 2> const M = {
		{ 9., 24., 30., 9.},
		{ 4., 10., 12., 7.},
		{14., 16., 36., 1.}
	};
	multi::array<double, 1> const v = {1.1, 2.1, 3.1, 4.1};
	{
		multi::array<double, 1>       w(size(M));
		blas::gemv_n(1., begin(M), size(M), begin(v), 0., begin(w));
		BOOST_TEST( w[1] == 91.3 );
		BOOST_TEST( w[2] == +blas::dot(M[2], v) );
	}
	{
		multi::array<double, 1>       w(size(M));
		multi::array<double, 2> const MT = ~M;
		blas::gemv_n(1., begin(~MT), size(~MT), begin(v), 0., begin(w));
		BOOST_TEST( w[1] == 91.3 );
		BOOST_TEST( w[2] == +blas::dot(M[2], v) );
	}
	{
		multi::array<double, 1> w(size(M));
		auto mv = blas::gemv(1., M, v);
		copy_n(mv.begin(), mv.size(), w.begin());
		BOOST_TEST( w[1] == 91.3 );
		
		multi::array<double, 1> w2(size(M));
		MV(M, v, w2);
		BOOST_TEST( w2 == w );
	}
	{
		multi::array<double, 1> w(size(M));
		w = blas::gemv(1., M, v);
		BOOST_TEST( w[1] == 91.3 );
	}
	{
		multi::array<double, 1> w = blas::gemv(1., M, v);
		BOOST_TEST( w[1] == 91.3 );
	}
	{
		multi::array<double, 1> w(size(M), 0.);
		w += blas::gemv(1., M, v);
		BOOST_TEST( w[1] == 91.3 );
	}
	{
		multi::array<double, 1> w = {4., 5., 6.};
		blas::gemv(1.1, M, v, 1., w); // y = a*M*x + b*y
		BOOST_TEST( w[1] == 105.43 );
	}
}

BOOST_AUTO_TEST_CASE(multi_blas_gemv_real, *utf::tolerance(0.0001)){
	namespace blas = multi::blas;

	using std::abs;
	multi::array<double, 2> const M = {
		{ 9., 24., 30., 9.},
		{ 4., 10., 12., 7.},
		{14., 16., 36., 1.}
	};
	multi::array<double, 1> const X = {1.1, 2.1, 3.1, 4.1};
	{
		multi::array<double, 1> Y = {4.,5.,6.};
		double const a = 1.1, b = 1.2;
		blas::gemv(a, M, X, b, Y); // y = a*M*x + b*y

		multi::array<double, 1> const Y3 = {214.02, 106.43, 188.37};
		BOOST_REQUIRE( abs(Y[1] - Y3[1]) < 2e-14 );
	}
	{
		auto Y = +blas::gemv(1., M, X);
		BOOST_TEST( Y[0] == +blas::dot(M[0], X) );
		BOOST_TEST( Y[1] == +blas::dot(M[1], X) );
		BOOST_TEST( Y[2] == +blas::dot(M[2], X) );
	}
	{
		multi::array<double, 1> const a = {1., 2., 3.};
		multi::array<double, 1> const b = {4., 5., 6.};
		multi::array<double, 1> const dot = blas::gemv(1., multi::array<double, 2>({a}), b);
		BOOST_REQUIRE( dot[0] == blas::dot(a, b) );
	}
	{
		multi::array<double, 2> const MT = ~M;
		using namespace blas::operators;
		BOOST_TEST_REQUIRE( (((~+~M)%X - M%X)^2) == 0., boost::test_tools::tolerance(1e-13) );
	}
}

BOOST_AUTO_TEST_CASE(multi_blas_gemv_real_complex){
	namespace blas = multi::blas;
	using complex = std::complex<double>; //#define I *std::complex<double>(0, 1)
	using std::abs;
	multi::array<complex, 2> const M = {
		{ 9., 24., 30., 9.},
		{ 4., 10., 12., 7.},
		{14., 16., 36., 1.}
	};
	multi::array<complex, 1> const X = {1.1, 2.1, 3.1, 4.1};
	{
		multi::array<complex, 1> Y = {4., 5., 6.};
		double const a = 1.1, b = 1.2;
		blas::gemv(a, M, X, b, Y); // y = a*M*x + b*y
		
		multi::array<complex, 1> const Y3 = {214.02, 106.43, 188.37};
		
		using namespace blas::operators;
		BOOST_TEST_REQUIRE( ((Y - Y3)^2)  == 0. , boost::test_tools::tolerance(1e-13) );
	}
}

#if CUDA_FOUND
#include<thrust/complex.h>
BOOST_AUTO_TEST_CASE(multi_blas_gemv_real_complex_thrust){
	namespace blas = multi::blas;
	using complex = thrust::complex<double>; //#define I *std::complex<double>(0, 1)
	using std::abs;
	multi::array<complex, 2> const M = {
		{ 9., 24., 30., 9.},
		{ 4., 10., 12., 7.},
		{14., 16., 36., 1.}
	};
	multi::array<complex, 1> const X = {1.1, 2.1, 3.1, 4.1};
	{
		multi::array<complex, 1> Y = {4., 5., 6.};
		double const a = 1.1, b = 1.2;
		blas::gemv(a, M, X, b, Y); // y = a*M*x + b*y
		
		multi::array<complex, 1> const Y3 = {214.02, 106.43, 188.37};
	}
	{
		multi::array<complex, 1> Y = {4., 5., 6.};
		blas::gemv(1.1, M, X, 1., Y); // y = a*M*x + b*y
		BOOST_TEST( Y[1] == 105.43 );
	}

}
#endif

BOOST_AUTO_TEST_CASE(multi_blas_gemv_complex){
	
	namespace blas = multi::blas;
	using complex = std::complex<double>; std::complex<double> const I{0, 1};
	
	using std::abs;
	multi::array<complex, 2> const M = {{2. + 3.*I, 2. + 1.*I, 1. + 2.*I}, {4. + 2.*I, 2. + 4.*I, 3. + 1.*I}, 
 {7. + 1.*I, 1. + 5.*I, 0. + 3.*I}};
	multi::array<complex, 1> const X = {1. + 2.*I, 2. + 1.*I, 9. + 2.*I};
	BOOST_REQUIRE(( +blas::gemv(1., M, X) == multi::array<complex, 1>{4. + 31.*I, 25. + 35.*I, -4. + 53.*I} ));
	
	auto MT = +~M;
	BOOST_REQUIRE(( +blas::gemv(1., ~MT, X) == multi::array<complex, 1>{4. + 31.*I, 25. + 35.*I, -4. + 53.*I} ));
	
//	auto MH = +*~M;
	BOOST_REQUIRE( +blas::gemv(1., ~M, X) == (multi::array<complex, 1>{63. + 38.*I, -1. + 62.*I, -4. + 36.*I}) );
	BOOST_REQUIRE( +blas::gemv(1., ~M, X) == +blas::gemv(1., MT, X) );// == multi::array<complex, 1>{4. + 31.*I, 25. + 35.*I, -4. + 53.*I} ));
	
//	BOOST_REQUIRE( +blas::gemv(1., *M, X) == (multi::array<complex, 1>{26. - 15.*I, 45. - 3.*I, 22. - 23.*I}) );
//	BOOST_REQUIRE( +blas::gemv(1., ~*M, X) == (multi::array<complex, 1>{83. + 6.*I, 31. - 46.*I, 18. - 26.*I}) ); // not supported by blas

}

BOOST_AUTO_TEST_CASE(multi_blas_gemv_temporary){

	using complex = std::complex<double>;
	
	multi::array<complex, 2> const A = {
		{1., 0., 0.}, 
		{0., 1., 0.},
		{0., 0., 1.}
	};
	
	auto const B = []{
		multi::array<complex, 2> _({3, 3});
		auto rand = [d=std::normal_distribution<>{}, g=std::mt19937{}]()mutable{return complex{d(g), d(g)};};
		std::generate(_.elements().begin(), _.elements().end(), rand);
		return _;
	}();
	
//	BOOST_TEST( (+std::gemv(A, (~B)[0])[1] == ~
//	
//	namespace blas = multi::blas;
//	
//	using namespace blas::operators;
//	BOOST_TEST( ( (A%(~B)[0] - ( ~(A*B)  )[0])^2) == 0. );
//	BOOST_TEST( ( (A%(~B)[0] - ((~B)*(~A))[0])^2) == 0. );
}

BOOST_AUTO_TEST_CASE(multi_blas_gemv_context, *utf::tolerance(0.0001)){

	multi::array<double, 2> const M = {
		{ 9., 24., 30., 9.},
		{ 4., 10., 12., 7.},
		{14., 16., 36., 1.}
	};
	multi::array<double, 1> const v = {1.1, 2.1, 3.1, 4.1};
	
	blas::context ctxt;
	{
		multi::array<double, 1>       w(size(M));
		blas::gemv_n(ctxt, 1., begin(M), size(M), begin(v), 0., begin(w));
		BOOST_TEST( w[1] == 91.3 );
		BOOST_TEST( w[2] == +blas::dot(M[2], v) );
	}
	{
		multi::array<double, 1>       w(size(M));
		multi::array<double, 2> const MT = ~M;
		blas::gemv_n(ctxt, 1., begin(~MT), size(~MT), begin(v), 0., begin(w));
		BOOST_TEST( w[1] == 91.3 );
		BOOST_TEST( w[2] == +blas::dot(M[2], v));
	}
	{
		multi::array<double, 1> w(size(M));
		auto&& mv = blas::gemv(ctxt, 1., M, v);
		copy_n(mv.begin(), mv.size(), w.begin());
		BOOST_TEST( w[1] == 91.3 );
	}
	{
		multi::array<double, 1> w(size(M));
		w = blas::gemv(ctxt, 1., M, v);
		BOOST_TEST( w[1] == 91.3 );
	}
	{
		multi::array<double, 1> w = blas::gemv(ctxt, 1., M, v);
		BOOST_TEST( w[1] == 91.3 );
	}
	{
		multi::array<double, 1> w(size(M), 0.);
		w += blas::gemv(ctxt, 1., M, v);
		BOOST_TEST( w[1] == 91.3 );
	}
	{
		multi::array<double, 1> w = {4., 5., 6.};
		w += blas::gemv(ctxt, 1.1, M, v);
		BOOST_TEST( w[1] == 105.43 );
	}

}

