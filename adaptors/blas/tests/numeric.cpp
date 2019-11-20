#ifdef COMPILATION_INSTRUCTIONS
clang++ -Wall -Wextra -Wpedantic $0 -o $0x `pkg-config --cflags --libs blas` -Wno-deprecated-declarations -lboost_unit_test_framework -lcudart -lcublas &&$0x&&rm $0x;exit
#endif

#include "../../blas.hpp"
#include "../../blas/cuda.hpp"

#include "../../blas/numeric.hpp"
#include "../../../adaptors/cuda.hpp"

#include<complex>
#include<cassert>

using std::cout;
namespace multi = boost::multi;

#define BOOST_TEST_MODULE "C++ Unit Tests for Multi BLAS numeric"
#define BOOST_TEST_DYN_LINK
#include<boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(multi_adaptors_blas_test_numeric_imag){
	using complex = std::complex<double>;
	constexpr complex I{0, 1};
	multi::array<complex, 1> a = { 1. + 2.*I, 3. + 5.*I, 9. + 2.*I };
	using multi::blas::imag;
	BOOST_REQUIRE( imag(a)[2] == 2. );

	multi::member_array_cast<double>(multi::reinterpret_array_cast<multi::blas::Complex_<double>>(a), &multi::blas::Complex_<double>::imag); 
}

namespace cuda = multi::cuda;
using complex = std::complex<double>;
constexpr complex I{0, 1};

BOOST_AUTO_TEST_CASE(multi_adaptors_blas_test_numeric_imag_cuda){
	cuda::array<complex, 1> a = { 1. + 2.*I, 3. + 5.*I, 9. + 2.*I };
	using multi::blas::imag;
	imag(a);
	BOOST_REQUIRE( imag(a)[2] == 2. );
}

BOOST_AUTO_TEST_CASE(multi_adaptors_blas_test_numeric_imag_cuda_managed){
	cuda::managed::array<complex, 1> a = { 1. + 2.*I, 3. + 5.*I, 9. + 2.*I };
	using multi::blas::imag;
	imag(a);
	BOOST_REQUIRE( imag(a)[2] == 2. );
}

