#ifdef COMPILATION_INSTRUCTIONS
nvcc -x cu --expt-relaxed-constexpr`#$CXX -Wall -Wextra -Wpedantic -Wfatal-errors` -Wno-deprecated-declarations $0 -o $0x -lcudart -lcublas -lboost_unit_test_framework \
`pkg-config --libs blas`&&$0x&&rm $0x;exit
#endif
// © Alfredo A. Correa 2019

#define BOOST_TEST_MODULE "C++ Unit Tests for Multi cuBLAS herk"
#define BOOST_TEST_DYN_LINK
#include<boost/test/unit_test.hpp>

#include "../../../adaptors/cuda.hpp" // multi::cuda ns

#include "../../../adaptors/blas/cuda.hpp"
#include "../../../adaptors/blas/herk.hpp"

#include "../../../array.hpp"

BOOST_AUTO_TEST_CASE(multi_blas_cuda_herk){
	namespace multi = boost::multi;
	namespace cuda = multi::cuda;

	using complex = std::complex<double>;
	constexpr complex I{0, 1};

	multi::array<complex, 2> const a = {
		{ 1. + 3.*I, 3.- 2.*I, 4.+ 1.*I},
		{ 9. + 1.*I, 7.- 8.*I, 1.- 3.*I}
	};
	{
		multi::array<complex, 2> c({2, 2}, 9999.);
		using multi::blas::herk;
		herk(1., a, c);
		BOOST_REQUIRE( c[1][0] == complex(50., -49.) );
		BOOST_REQUIRE( c[0][1] == complex(50., +49.) );

		multi::array<complex, 2> const c_copy = herk(1., a);
		BOOST_REQUIRE( c == c_copy );
	}
	{
		cuda::array<complex, 2> const acu = a; BOOST_REQUIRE(a == acu);
		cuda::array<complex, 2> ccu({2, 2}, 9999.);
		using multi::blas::herk;
		herk(1., acu, ccu);
		BOOST_REQUIRE( ccu[1][0] == complex(50., -49.) );
		BOOST_REQUIRE( ccu[0][1] == complex(50., +49.) );

		cuda::array<complex, 2> const ccu_copy = herk(1., acu);
		BOOST_REQUIRE( herk(1., acu) == ccu );
	}
	{
		cuda::managed::array<complex, 2> const amcu = a; BOOST_REQUIRE(a == amcu);
		cuda::managed::array<complex, 2> cmcu({2, 2}, 9999.);
		using multi::blas::herk;
		herk(1., amcu, cmcu);
		BOOST_REQUIRE( cmcu[1][0] == complex(50., -49.) );
		BOOST_REQUIRE( cmcu[0][1] == complex(50., +49.) );

		cuda::managed::array<complex, 2> const cmcu_copy = herk(1., amcu);
		BOOST_REQUIRE( cmcu_copy == cmcu );
	}
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
		using multi::blas::herk;
		using multi::blas::hermitized;
		herk(1., hermitized(a), c);
		BOOST_REQUIRE( c[2][1] == complex(41, +2) );
		BOOST_REQUIRE( c[1][2] == complex(41, -2) );

		multi::array<complex, 2> const c_copy = herk(1., hermitized(a));
		BOOST_REQUIRE( c_copy == c );
	}
	{
		cuda::array<complex, 2> const acu = a; BOOST_REQUIRE(a == acu);
		cuda::array<complex, 2> ccu({3, 3}, 9999.);
		using multi::blas::herk;
		using multi::blas::hermitized;
		herk(1., hermitized(acu), ccu);
		BOOST_REQUIRE( ccu[2][1] == complex(41, +2) );
		BOOST_REQUIRE( ccu[1][2] == complex(41, -2) );

		cuda::array<complex, 2> const ccu_copy = herk(1., hermitized(acu));
		BOOST_REQUIRE( ccu_copy == ccu );
	}
}

