#ifdef COMPILATION_INSTRUCTIONS
(echo '#include"'$0'"'>$0.cpp)&&$CXX -std=c++14 -Ofast -Wall -Wextra -Wpedantic -Wfatal-errors -D_TEST_MULTI_ADAPTORS_BLAS_HERK $0.cpp -o $0x -lboost_unit_test_framework \
`pkg-config --cflags --libs blas` \
`#-Wl,-rpath,/usr/local/Wolfram/Mathematica/12.0/SystemFiles/Libraries/Linux-x86-64 -L/usr/local/Wolfram/Mathematica/12.0/SystemFiles/Libraries/Linux-x86-64 -lmkl_intel_ilp64 -lmkl_intel_thread -lmkl_core -liomp5` \
-lboost_timer &&$0x&& rm $0x $0.cpp; exit
#endif
// © Alfredo A. Correa 2019
#ifndef MULTI_ADAPTORS_BLAS_HERK_HPP
#define MULTI_ADAPTORS_BLAS_HERK_HPP

#include "../blas/core.hpp"
#include "../blas/copy.hpp" 
//#include "../blas/scal.hpp" 
#include "../blas/syrk.hpp" // fallback to real case

#include "../blas/side.hpp"
#include "../blas/filling.hpp"

#include "../blas/operations.hpp"

#include "../../config/NODISCARD.hpp"

//#include<iostream> //debug
//#include<type_traits> // void_t

namespace boost{
namespace multi{namespace blas{

template<class A> auto base_aux(A&& a, std::false_type){return base(a);}
template<class A> auto base_aux(A&& a, std::true_type){return underlying(base(a));}

using core::herk;

template<class AA, class BB, class A2D, class C2D, class = typename A2D::element_ptr>
auto herk_aux(filling c_side, AA alpha, A2D const& a, BB beta, C2D&& c, std::true_type)
->decltype(herk('\0', '\0', size(c), size(         a), alpha, base_aux(a, is_hermitized<A2D>{}), stride(rotated(a)), beta, base_aux(c, is_hermitized<std::decay_t<C2D>>{}), stride(c)), std::forward<C2D>(c))
{
	assert( size(a) == size(c) );
	assert( size(c) == size(rotated(c)) );
	if(size(c)==0) return std::forward<C2D>(c);
	if(is_hermitized<std::decay_t<C2D>>{}){
		herk_aux(flip(c_side), alpha, a, beta, hermitized(c), std::true_type{});
	}else{
		auto base_a = base_aux(a, is_hermitized<A2D>{});
		auto base_c = base_aux(c, is_hermitized<std::decay_t<C2D>>{});
		if(is_hermitized<A2D>{}){
			// if you get an error here might be due to lack of inclusion of a header file with the backend appropriate for your type of iterator
				 if(stride(a)==1 and stride(c)!=1) herk(c_side==filling::upper?'L':'U', 'N', size(c), size(rotated(a)), alpha, base_a, stride(rotated(a)), beta, base_c, stride(c));
			else if(stride(a)==1 and stride(c)==1) assert(0);
			else if(stride(a)!=1 and stride(c)==1) herk(c_side==filling::upper?'U':'L', 'C', size(c), size(rotated(a)), alpha, base_a, stride(        a ), beta, base_c, stride(rotated(c)));
			else if(stride(a)!=1 and stride(c)!=1) herk(c_side==filling::upper?'L':'U', 'C', size(c), size(rotated(a)), alpha, base_a, stride(        a ), beta, base_c, stride(        c ));
			else assert(0);
		}else{
				 if(stride(a)!=1 and stride(c)!=1) herk(c_side==filling::upper?'L':'U', 'C', size(c), size(rotated(a)), alpha, base_a, stride(        a ), beta, base_c, stride(c));
			else if(stride(a)!=1 and stride(c)==1){
				if(size(a)==1) herk(c_side==filling::upper?'L':'U', 'N', size(c), size(rotated(a)), alpha, base_a, stride(rotated(a)), beta, base_c, stride(rotated(c)));
				else assert(0);
			}
			else if(stride(a)==1 and stride(c)!=1) assert(0);//herk(c_side==filling::upper?'L':'U', 'N', size(c), size(rotated(a)), alpha, base_a, stride(rotated(a)), beta, base(c), stride(c));
			else if(stride(a)==1 and stride(c)==1) herk(c_side==filling::upper?'U':'L', 'N', size(c), size(rotated(a)), alpha, base_a, stride(rotated(a)), beta, base_c, stride(rotated(c)));
			else assert(0);
		}
	}
	return std::forward<C2D>(c);
}

template<class AA, class BB, class A2D, class C2D, class = typename A2D::element_ptr>
auto herk_aux(filling c_side, AA alpha, A2D const& a, BB beta, C2D&& c, std::false_type)
->decltype(syrk(c_side, alpha, a, beta, std::forward<C2D>(c))){
	return syrk(c_side, alpha, a, beta, std::forward<C2D>(c));}

template<class AA, class BB, class A2D, class C2D, class = typename A2D::element_ptr>
auto herk(filling c_side, AA alpha, A2D const& a, BB beta, C2D&& c)
->decltype(herk_aux(c_side, alpha, a, beta, std::forward<C2D>(c), is_complex_array<std::decay_t<C2D>>{})){
	return herk_aux(c_side, alpha, a, beta, std::forward<C2D>(c), is_complex_array<std::decay_t<C2D>>{});}

template<class AA, class A2D, class C2D, class = typename A2D::element_ptr>
auto herk(filling c_side, AA alpha, A2D const& a, C2D&& c)
->decltype(herk(c_side, alpha, a, 0., std::forward<C2D>(c))){
	return herk(c_side, alpha, a, 0., std::forward<C2D>(c));}

template<typename AA, class A2D, class C2D>
auto herk(AA alpha, A2D const& a, C2D&& c)
->decltype(herk(filling::lower, alpha, a, herk(filling::upper, alpha, a, std::forward<C2D>(c)))){
	return herk(filling::lower, alpha, a, herk(filling::upper, alpha, a, std::forward<C2D>(c)));}

template<class A2D, class C2D>
auto herk(A2D const& a, C2D&& c)
->decltype(herk(1., a, std::forward<C2D>(c))){
	return herk(1., a, std::forward<C2D>(c));}

template<class A2D, class C2D>
NODISCARD("when last argument is const")
auto herk(A2D const& a, C2D const& c)
->decltype(herk(1., a, decay(c))){
	return herk(1., a, decay(c));}

template<class AA, class A2D, class Ret = typename A2D::decay_type>
NODISCARD("when second argument is const")
auto herk(AA alpha, A2D const& a)
->std::decay_t<decltype(herk(alpha, a, Ret({size(a), size(a)}, get_allocator(a))))>{
	return herk(alpha, a, Ret({size(a), size(a)}, get_allocator(a)));
}

template<class T> struct numeric_limits : std::numeric_limits<T>{};
template<class T> struct numeric_limits<std::complex<T>> : std::numeric_limits<std::complex<T>>{
	static std::complex<T> quiet_NaN(){auto n=numeric_limits<T>::quiet_NaN(); return {n, n};}
};

template<class AA, class A2D, class Ret = typename A2D::decay_type>
NODISCARD("second argument is const")
auto herk(filling cs, AA alpha, A2D const& a)
->std::decay_t<
decltype(herk(cs, alpha, a, Ret({size(a), size(a)}, 0., get_allocator(a))))>{
	return herk(cs, alpha, a, Ret({size(a), size(a)},
#ifdef NDEBUG
		numeric_limits<typename Ret::element_type>::quiet_NaN(),
#endif	
		get_allocator(a)
	));
}

template<class A2D> auto herk(filling s, A2D const& a)
->decltype(herk(s, 1., a)){
	return herk(s, 1., a);}

template<class A2D> auto herk(A2D const& a)
->decltype(herk(1., a)){
	return herk(1., a);}

}}

}

#if _TEST_MULTI_ADAPTORS_BLAS_HERK

#define BOOST_TEST_MODULE "C++ Unit Tests for Multi cuBLAS herk"
#define BOOST_TEST_DYN_LINK
#include<boost/test/unit_test.hpp>

#include "../../array.hpp"
#include "../../adaptors/blas/gemm.hpp"

#include<iostream>

namespace multi = boost::multi;

template<class M> decltype(auto) print(M const& C){
	using std::cout;
	using boost::multi::size;
	for(int i = 0; i != size(C); ++i){
		for(int j = 0; j != size(C[i]); ++j) cout << C[i][j] << ' ';
		cout << std::endl;
	}
	return cout << std::endl;
}

using complex = std::complex<double>; complex const I{0, 1};

BOOST_AUTO_TEST_CASE(inq_case){
	using namespace multi::blas;
	multi::array<double, 2> A({4, 3});
	for(auto i : extension(A))
		for(auto j : extension(A[i]))
			A[i][j] = 20.*(i + 1)*sqrt(j);

	BOOST_REQUIRE( 
		herk(0.01, hermitized(A)) == gemm(0.01, transposed(A), A) 
	);
}

#if 1
BOOST_AUTO_TEST_CASE(multi_blas_herk_complex_identity){
	multi::array<complex, 2> const a = {
		{ 1. + 3.*I, 3.- 2.*I, 4.+ 1.*I},
		{ 9. + 1.*I, 7.- 8.*I, 1.- 3.*I}
	};
	using namespace multi::blas;
	{
		multi::array<complex, 2> c({2, 2}, 9999.);
		herk(filling::lower, 1., a, 0., c); // c†=c=aa†=(aa†)†, `c` in lower triangular
		BOOST_REQUIRE( c[1][0]==complex(50., -49.) );
		BOOST_REQUIRE( c[0][1]==9999. );
	}
	{
		multi::array<complex, 2> c({2, 2}, 9999.);
		herk(filling::lower, 1., a, 0., hermitized(c)); // c†=c=aa†=(aa†)†, `c` in lower triangular
		BOOST_REQUIRE( hermitized(c)[1][0]==complex(50., -49.) );
		BOOST_REQUIRE( hermitized(c)[0][1]==9999. );
	}
	{
		multi::array<complex, 2> c({2, 2}, 9999.);
	//	herk(filling::lower, 1., a, 0., transposed(c)); // c†=c=aa†=(aa†)†, `c` in lower triangular
	//	BOOST_REQUIRE( transposed(c)[1][0]==complex(50., -49.) );
	//	BOOST_REQUIRE( transposed(c)[0][1]==9999. );
	}
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
	//	herk(filling::lower, 1., transposed(a), 0., c); // c†=c=aT(aT)† not supported
	//	print(c);
	//	BOOST_REQUIRE( c[1][0]==complex(52., -90.) );
	//	BOOST_REQUIRE( c[0][1]==9999. );
	}
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
	//	herk(filling::lower, 1., transposed(a), 0., hermitized(c)); // c†=c=aT(aT)† not supported
	//	BOOST_REQUIRE( hermitized(c)[1][0]==complex(52., -90.) );
	//	BOOST_REQUIRE( hermitized(c)[0][1]==9999. );
	}
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
		herk(filling::lower, 1., transposed(a), 0., transposed(c)); // c†=c=aT(aT)† not supported
		BOOST_REQUIRE( transposed(c)[1][0]==complex(52., -90.) );
		BOOST_REQUIRE( transposed(c)[0][1]==9999. );
	}
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
		herk(filling::lower, 1., transposed(a), 0., hermitized(transposed(c))); // c†=c=aT(aT)† not supported
	//	print( hermitized(transposed(c)) );
		BOOST_REQUIRE( hermitized(transposed(c))[1][0]==complex(52., -90.) );
		BOOST_REQUIRE( hermitized(transposed(c))[0][1]==9999. );
	}
#if 0
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
		using namespace multi::blas;
		herk(filling::lower, 1., transposed(a), 0., c); // c†=c=aa†=(aa†)†, `c` in lower triangular
		print(c);
		BOOST_REQUIRE( c[1][0]==complex(50., -49.) );
		BOOST_REQUIRE( c[0][1]==9999. );
	}
	{
		multi::array<complex, 2> c({2, 2}, 9999.);
		using namespace multi::blas;
		herk(filling::upper, 1., a, 0., c); // c†=c=aa†=(aa†)†, `c` in upper triangular
		BOOST_REQUIRE( c[0][1]==complex(50., +49.) );
		BOOST_REQUIRE( c[1][0]==9999. );
	}
	{
		multi::array<complex, 2> c({2, 2}, 9999.);
		using namespace multi::blas;
		herk(1., a, c); // c†=c=aa†=(aa†)†, `c` in lower triangular
		BOOST_REQUIRE( c[0][1]==complex(50., +49.) );
		BOOST_REQUIRE( c[1][0]==complex(50., -49.) );
	}
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
		using namespace multi::blas;
		herk(filling::lower, 1., hermitized(a), 0., c); // c†=c=aa†=(aa†)†, `c` in lower triangular
		BOOST_REQUIRE( c[1][0]==complex(52., 90.) );
		BOOST_REQUIRE( c[0][1]==9999. );
	}
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
		using namespace multi::blas;
		herk(filling::lower, 1., transposed(a), 0., c); // c†=c=aa†=(aa†)†, `c` in lower triangular
		BOOST_REQUIRE( c[0][1]==9999. );
		BOOST_REQUIRE( c[1][0]==complex(52., 90.) );
	}
#endif
}

BOOST_AUTO_TEST_CASE(multi_blas_herk_complex_real_case){
	multi::array<complex, 2> const a = {
		{ 1., 3., 4.},
		{ 9., 7., 1.}
	};
	namespace blas = multi::blas;
	using blas::filling;
	using blas::transposed;
	using blas::hermitized;
	{
		multi::array<complex, 2> c({3, 3}, 9999.);

		herk(filling::lower, 1., hermitized(a), 0., c);//c†=c=a†a=(a†a)†, `c` in lower triangular
		BOOST_REQUIRE( c[2][1]==complex(19.,0.) );
		BOOST_REQUIRE( c[1][2]==9999. );
	}
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
		herk(filling::upper, 1., hermitized(a), 0., c);//c†=c=a†a=(a†a)†, `c` in lower triangular
		BOOST_REQUIRE( c[1][2]==complex(19.,0.) );
		BOOST_REQUIRE( c[2][1]==9999. );
	}
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
	//	herk(filling::upper, 1., hermitized(a), 0., transposed(c));//c†=c=a†a=(a†a)†, `c` in lower triangular
	//	print(transposed(c));
	//	BOOST_REQUIRE( c[1][2]==complex(19.,0.) );
	//	BOOST_REQUIRE( c[2][1]==9999. );
	}
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
		using blas::transposed;
	//	herk(filling::upper, 1., transposed(a), 0., c);//c_†=c_=a_†a_=(a_†a_)†, `c_` in lower triangular
	//	BOOST_REQUIRE( c[2][1] == 9999. );
	//	BOOST_REQUIRE( c[1][2] == 19. );
	}
}

BOOST_AUTO_TEST_CASE(multi_blas_herk_complex_basic_transparent_interface){
	multi::array<complex, 2> const a = {
		{ 1. + 3.*I, 3.- 2.*I, 4.+ 1.*I},
		{ 9. + 1.*I, 7.- 8.*I, 1.- 3.*I}
	};
	namespace blas = multi::blas;
	using blas::filling;
	using blas::hermitized;
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
		herk(filling::lower, 1., hermitized(a), 0., c); // c†=c=a†a=(a†a)†, information in `c` lower triangular
		BOOST_REQUIRE( c[2][1]==complex(41.,2.) );
		BOOST_REQUIRE( c[1][2]==9999. );
	}
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
		using multi::blas::herk;
		herk(filling::upper, 1., hermitized(a), 0., c); // c†=c=a†a=(a†a)†, `c` in upper triangular
		BOOST_REQUIRE( c[1][2]==complex(41., -2.) );
		BOOST_REQUIRE( c[2][1]==9999. );
	}
	{
		multi::array<complex, 2> c({2, 2}, 9999.);
		using multi::blas::herk;
		herk(filling::lower, 1., a, 0., c); // c†=c=aa†, `a` and `c` are c-ordering, information in c lower triangular
		BOOST_REQUIRE( c[1][0]==complex(50., -49.) );
		BOOST_REQUIRE( c[0][1]==9999. );
	}
	{
		multi::array<complex, 2> c({2, 2}, 9999.);
		using multi::blas::herk;
		herk(filling::upper, 1., a, 0., c); //c†=c=aa†, `c` in upper triangular
		BOOST_REQUIRE( c[0][1]==complex(50., 49.) );
		BOOST_REQUIRE( c[1][0]==9999. );
	}
}

BOOST_AUTO_TEST_CASE(multi_blas_herk_complex_basic_enum_interface){
	multi::array<complex, 2> const a = {
		{ 1. + 3.*I, 3.- 2.*I, 4.+ 1.*I},
		{ 9. + 1.*I, 7.- 8.*I, 1.- 3.*I}
	};
	namespace blas = multi::blas;
	using blas::filling;
	using blas::hermitized;
	using blas::transposed;
	{
	//	multi::array<complex, 2> c({2, 2}, 8888.);
	//	std::cerr << "here" << std::endl;
	//	herk(filling::lower, 1., hermitized(transposed(a)), 0., c); //c†=c=a†a=(a†a)†, `c` in lower triangular
	//	print(c) << std::endl;
	//	std::cerr << "there" << std::endl;
	//	BOOST_REQUIRE( c[0][1]==complex(41.,2.) );
	//	BOOST_REQUIRE( c[1][0]==8888. );
	}
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
		herk(filling::lower, 1., hermitized(a), 0., c); //c†=c=a†a=(a†a)†, `c` in lower triangular
		BOOST_REQUIRE( c[2][1]==complex(41.,2.) );
		BOOST_REQUIRE( c[1][2]==9999. );
	}
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
		using namespace multi::blas;
		herk(filling::upper, 1., hermitized(a), 0., c); //c†=c=a†a=(a†a)†, `c` in upper triangular
		BOOST_REQUIRE( c[1][2]==complex(41., -2.) );
		BOOST_REQUIRE( c[2][1]==9999. );
	}
	{
		multi::array<complex, 2> c({2, 2}, 9999.);
		using namespace multi::blas;
		herk(filling::lower, 1., a, 0., c); // c†=c=aa†, `c` in lower triangular
		BOOST_REQUIRE( c[1][0]==complex(50., -49.) );
		BOOST_REQUIRE( c[0][1]==9999. );
	}
	{
		multi::array<complex, 2> c({2, 2}, 9999.);
		using namespace multi::blas;
		herk(filling::upper, 1., a, 0., c); // c†=c=aa†, `c` in upper triangular
		BOOST_REQUIRE( c[0][1]==complex(50., 49.) );
		BOOST_REQUIRE( c[1][0]==9999. );
	}
}

BOOST_AUTO_TEST_CASE(multi_blas_herk_complex_basic_explicit_enum_interface){
	multi::array<complex, 2> const a = {
		{ 1. + 3.*I, 3.- 2.*I, 4.+ 1.*I},
		{ 9. + 1.*I, 7.- 8.*I, 1.- 3.*I}
	};
	using namespace multi::blas;
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
		herk(filling::lower, 1., hermitized(a), 0., c); // c†=c=a†a=(a†a)†, `c` in lower triangular
		BOOST_REQUIRE( c[2][1]==complex(41.,2.) );
		BOOST_REQUIRE( c[1][2]==9999. );
	}
	BOOST_REQUIRE( herk(hermitized(a)) == gemm(hermitized(a), a) );
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
	//	herk(filling::lower, 1., hermitized(a), 0., transposed(c)); // c†=c=a†a=(a†a)†, `c` in lower triangular
	//	print(transposed(c));
	//	BOOST_REQUIRE( c[2][1]==complex(41.,2.) );
	//	BOOST_REQUIRE( c[1][2]==9999. );
	}
	{
		multi::array<complex, 2> c({2, 2}, 9999.);
		herk(filling::lower, 1., hermitized(transposed(a)), 0., transposed(c)); // c†=c=a†a=(a†a)†, `c` in lower triangular
		BOOST_REQUIRE( transposed(c)[1][0]==complex(50.,+49.) );
		BOOST_REQUIRE( transposed(c)[0][1]==9999. );
	}
//	BOOST_REQUIRE( herk(hermitized(transposed(a))) == gemm(hermitized(transposed(a)), transposed(a)) );
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
		herk(filling::upper, 1., hermitized(a), 0., c); // c†=c=a†a=(a†a)†, `c` in upper triangular
		BOOST_REQUIRE( c[1][2]==complex(41., -2.) );
		BOOST_REQUIRE( c[2][1]==9999. );
		BOOST_REQUIRE( herk(hermitized(a)) == gemm(hermitized(a), a) );
	}
	{
		multi::array<complex, 2> c({2, 2}, 9999.);
		herk(filling::lower, 1., a, 0., c); // c†=c=aa†=(aa†)†, `c` in lower triangular
		BOOST_REQUIRE( c[1][0]==complex(50., -49.) );
		BOOST_REQUIRE( c[0][1]==9999. );
		BOOST_REQUIRE( herk(a) == gemm(a, hermitized(a)) );
	}
	{
		multi::array<complex, 2> c({2, 2}, 9999.);
		herk(filling::upper, 1., a, 0., c); // c†=c=aa†=(aa†)†, `c` in upper triangular
		BOOST_REQUIRE( c[0][1]==complex(50., 49.) );
		BOOST_REQUIRE( c[1][0]==9999. );
		BOOST_REQUIRE( herk(a) == gemm(a, hermitized(a)) );
	}
	{
		multi::array<complex, 2> c({2, 2}, 9999.);
		herk(filling::upper, 2., a, 0., c); // c†=c=aa†=(aa†)†, `c` in upper triangular
		BOOST_REQUIRE( c[0][1]==complex(100., 98.) );
		BOOST_REQUIRE( c[1][0]==9999. );
		BOOST_REQUIRE( herk(2., a) == gemm(2., a, hermitized(a)) );
	}
	{
		multi::array<complex, 2> c({2, 2}, 9999.);
		herk(filling::upper, 1., a, 0., c); // c†=c=aa†=(aa†)†, `c` in upper triangular
		BOOST_REQUIRE( c[0][1]==complex(50., 49.) );
		BOOST_REQUIRE( c[1][0]==9999. );
		BOOST_REQUIRE( herk(1., a) == gemm(1., a, hermitized(a)) );
	}
}

BOOST_AUTO_TEST_CASE(multi_blas_herk_complex_automatic_operator_interface){
	multi::array<complex, 2> const a = {
		{ 1. + 3.*I, 3.- 2.*I, 4.+ 1.*I},
		{ 9. + 1.*I, 7.- 8.*I, 1.- 3.*I}
	};
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
		namespace blas = multi::blas;
		using blas::filling;
		using blas::hermitized;
		herk(filling::lower, 1., hermitized(a), 0., c); // c=c†=a†a, `c` in lower triangular
		BOOST_REQUIRE( c[2][1]==complex(41., 2.) );
		BOOST_REQUIRE( c[1][2]==9999. );
	}
	{
		multi::array<complex, 2> c({2, 2}, 9999.);
		using multi:: blas::filling;
		herk(filling::lower, 1., a, 0., c); // c=c†=aa†, `c` in lower triangular
		BOOST_REQUIRE( c[1][0]==complex(50., -49.) );
		BOOST_REQUIRE( c[0][1]==9999. );
	}
	{
		multi::array<complex, 2> c({2, 2}, 9999.);
		using multi::blas::herk;
		herk(1., a, c); // c=c†=aa†
		BOOST_REQUIRE( c[1][0]==complex(50., -49.) );
		BOOST_REQUIRE( c[0][1]==complex(50., +49.) );
	}
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
		namespace blas = multi::blas;
		using blas::filling;
		using blas::hermitized;
		herk(filling::lower, 1., hermitized(a), 0., c); // c=c†=a†a, `c` in lower triangular
		herk(filling::upper, 1., hermitized(a), 0., c);
		BOOST_REQUIRE( c[2][1]==complex(41., 2.) );
		BOOST_REQUIRE( c[1][2]==complex(41., -2.) );
	}
}

BOOST_AUTO_TEST_CASE(multi_blas_herk_complex_automatic_operator_interface_implicit_no_sum){
	multi::array<complex, 2> const a = {
		{ 1. + 3.*I, 3.- 2.*I, 4.+ 1.*I},
		{ 9. + 1.*I, 7.- 8.*I, 1.- 3.*I}
	};
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
		namespace blas = multi::blas;
		using blas::filling;
		using blas::hermitized;
		herk(filling::lower, 1., hermitized(a), c); // c=c†=a†a, `c` in lower triangular
		BOOST_REQUIRE( c[2][1]==complex(41., 2.) );
		BOOST_REQUIRE( c[1][2]==9999. );
	}
	{
		multi::array<complex, 2> c({2, 2}, 9999.);
		using multi::blas::filling;
		herk(filling::lower, 1., a, c); // c=c†=aa†, `c` in lower triangular
		BOOST_REQUIRE( c[1][0]==complex(50., -49.) );
		BOOST_REQUIRE( c[0][1]==9999. );
	}
}

BOOST_AUTO_TEST_CASE(multi_blas_herk_complex_automatic_ordering_and_symmetrization){

	multi::array<complex, 2> const a = {
		{ 1. + 3.*I, 3.- 2.*I, 4.+ 1.*I},
		{ 9. + 1.*I, 7.- 8.*I, 1.- 3.*I}
	};
	namespace blas = multi::blas;
	using blas::herk;
	using blas::hermitized;
	using blas::filling;
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
		herk(filling::upper, 1., hermitized(a), c); // c†=c=a†a
		BOOST_REQUIRE( c[2][1]==9999. );
		BOOST_REQUIRE( c[1][2]==complex(41., -2.) );
	}
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
		herk(1., hermitized(a), c); // c†=c=a†a
		BOOST_REQUIRE( c[2][1]==complex(41., +2.) );
		BOOST_REQUIRE( c[1][2]==complex(41., -2.) );
	}
	{
		multi::array<complex, 2> c({2, 2}, 9999.);
		herk(filling::upper, 1., a, c); // c†=c=aa† // c implicit hermitic in upper
		BOOST_REQUIRE( c[1][0] == 9999. );
		BOOST_REQUIRE( c[0][1] == complex(50., +49.) );
	}
	{
		multi::array<complex, 2> c({2, 2}, 9999.);
		herk(1., a, c); // c†=c=aa†
		BOOST_REQUIRE( c[1][0] == complex(50., -49.) );
		BOOST_REQUIRE( c[0][1] == complex(50., +49.) );
	}
	{
		multi::array<complex, 2> c = herk(filling::upper, 1., a); // c†=c=aa†
	//	BOOST_REQUIRE( c[1][0] == complex(50., -49.) );
		BOOST_REQUIRE( c[0][1] == complex(50., +49.) );
	}
	{
		using multi::blas::herk;
		using multi::blas::filling;
		multi::array<complex, 2> c = herk(1., a); // c†=c=aa†
		BOOST_REQUIRE( c[1][0] == complex(50., -49.) );
		BOOST_REQUIRE( c[0][1] == complex(50., +49.) );
	}
	{
		using multi::blas::herk;
		using multi::blas::hermitized;
		using multi::blas::filling;
		multi::array<complex, 2> c = herk(filling::upper, 1., hermitized(a)); // c†=c=a†a

		BOOST_REQUIRE( size(hermitized(a))==3 );		
	//	BOOST_REQUIRE( c[2][1] == complex(41., +2.) );
		BOOST_REQUIRE( c[1][2] == complex(41., -2.) );
	}
	{
		using multi::blas::herk;
		using multi::blas::filling;
		multi::array<complex, 2> c = herk(filling::upper, a); // c†=c=a†a
//		what(multi::pointer_traits<decltype(base(a))>::default_allocator_of(base(a)));
	//	BOOST_REQUIRE( c[1][0] == complex(50., -49.) );
		BOOST_REQUIRE( c[0][1] == complex(50., +49.) );
	}
	{
		using multi::blas::herk;
		using multi::blas::hermitized;
		using multi::blas::filling;
		multi::array<complex, 2> c = herk(filling::upper, hermitized(a)); // c†=c=a†a
	//	BOOST_REQUIRE( c[2][1] == complex(41., +2.) );
		BOOST_REQUIRE( c[1][2] == complex(41., -2.) );
	}
}

BOOST_AUTO_TEST_CASE(multi_blas_herk_complex_size1_real_case){
	multi::array<complex, 2> const a = {
		{1., 3., 4.}
	};
	using namespace multi::blas;
	{
		multi::array<complex, 2> c({1, 1}, 9999.);
		herk(filling::upper, 1., a, c); // c†=c=aa†
		BOOST_TEST( c[0][0] == 26. );
	}
	BOOST_TEST( herk(a) == gemm(a, hermitized(a)) );
}

BOOST_AUTO_TEST_CASE(multi_blas_herk_complex_size1){
	multi::array<complex, 2> const a = {
		{1. + 4.*I, 3. + 2.*I, 4. - 1.*I}
	};
	using namespace multi::blas;
	{
		multi::array<complex, 2> c({1, 1}, 9999.);
		herk(filling::upper, 1., a, c); // c†=c=aa†
		BOOST_TEST( c[0][0] == 47. );
	}
}

BOOST_AUTO_TEST_CASE(multi_blas_herk_complex_size0){
	multi::array<complex, 2> const a;
	using namespace multi::blas;
	{
		multi::array<complex, 2> c;
		herk(filling::upper, 1., a, c); // c†=c=aa†
	//	BOOST_TEST( c[0][0] == 47. );
	}
}


BOOST_AUTO_TEST_CASE(multi_blas_herk_complex_automatic_ordering_and_symmetrization_real_case){

	multi::array<complex, 2> const a = {
		{ 1., 3., 4.},
		{ 9., 7., 1.}
	};
	using namespace multi::blas;
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
		herk(filling::upper, 1., hermitized(a), c); // c†=c=a†a
	//	BOOST_REQUIRE( c[2][1]==19. );
		BOOST_REQUIRE( c[1][2]==19. );
	}
	{
		multi::array<complex, 2> c({2, 2}, 9999.);
		herk(filling::upper, 1., a, c); // c†=c=aa†
	//	BOOST_REQUIRE( c[1][0] == 34. );
		BOOST_REQUIRE( c[0][1] == 34. );
	}
	{
		multi::array<complex, 2> c = herk(filling::upper, 1., a); // c†=c=aa†
	//	BOOST_REQUIRE( c[1][0] == 34. );
		BOOST_REQUIRE( c[0][1] == 34. );
	}
	{
		multi::array<complex, 2> c = herk(filling::upper, 1., hermitized(a)); // c†=c=a†a
		BOOST_REQUIRE( size(hermitized(a))==3 );		
	//	BOOST_REQUIRE( c[2][1]==19. );
		BOOST_REQUIRE( c[1][2]==19. );
	}
	{
		multi::array<complex, 2> c = herk(filling::upper, a); // c†=c=a†a
	//	BOOST_REQUIRE( c[1][0] == 34. );
		BOOST_REQUIRE( c[0][1] == 34. );
	}
	{
		multi::array<complex, 2> c = herk(filling::upper, hermitized(a)); // c†=c=a†a
	//	BOOST_REQUIRE( c[2][1]==19. );
		BOOST_REQUIRE( c[1][2]==19. );
	}
}


BOOST_AUTO_TEST_CASE(multi_blas_herk_real_automatic_ordering_and_symmetrization_real_case){

	multi::array<double, 2> const a = {
		{ 1., 3., 4.},
		{ 9., 7., 1.}
	};
	{
		multi::array<double, 2> c({3, 3}, 9999.);
		using multi::blas::hermitized;
		using multi::blas::herk;
		using multi::blas::filling;
	//	herk(filling::upper, 1., hermitized(a), c); // c†=c=a†a
	//	BOOST_REQUIRE( c[2][1]==19. );
	//	BOOST_REQUIRE( c[1][2]==19. );
	}
	{
		multi::array<double, 2> c({2, 2}, 9999.);
		using multi::blas::herk;
		using multi::blas::filling;
		herk(filling::upper, 1., a, c); // c†=c=aa†
	//	BOOST_REQUIRE( c[1][0] == 34. );
		BOOST_REQUIRE( c[0][1] == 34. );
	}
	{
		multi::array<double, 2> c({2, 2}, 9999.);
		using multi::blas::herk;
		using multi::blas::filling;
		herk(filling::upper, 1., a, c); // c†=c=aa†
	//	BOOST_REQUIRE( c[1][0] == 34. );
		BOOST_REQUIRE( c[0][1] == 34. );
	}
	{
		using multi::blas::herk;
		using multi::blas::filling;
		multi::array<double, 2> c = herk(filling::upper, 1., a); // c†=c=aa†
	//	BOOST_REQUIRE( c[1][0] == 34. );
		BOOST_REQUIRE( c[0][1] == 34. );
	}
	{
		using multi::blas::herk;
		multi::array<complex, 2> c = herk(a); // c†=c=a†a
		BOOST_REQUIRE( c[1][0] == 34. );
		BOOST_REQUIRE( c[0][1] == 34. );
	}
	{
		using multi::blas::herk;
		using multi::blas::hermitized;
		multi::array<complex, 2> c = herk(hermitized(a)); // c†=c=a†a
		BOOST_REQUIRE( c[2][1]==19. );
		BOOST_REQUIRE( c[1][2]==19. );
	}
}

BOOST_AUTO_TEST_CASE(multi_blas_herk_real_case){
	multi::array<double, 2> const a = {
		{ 1., 3., 4.},
		{ 9., 7., 1.}
	};
	using multi::blas::filling;
	{
		static_assert( not boost::multi::blas::is_complex_array<multi::array<double, 2>>{} , "!");
		multi::array<double, 2> c({2, 2}, 9999.);
		syrk(filling::lower, 1., a, 0., c);//c†=c=aa†=(aa†)†, `c` in lower triangular
	}
	{
		multi::array<double, 2> c({2, 2}, 9999.);
		herk(filling::lower, 1., a, 0., c);//c†=c=aa†=(aa†)†, `c` in lower triangular
	}
	{
		static_assert( not boost::multi::blas::is_complex_array<multi::array<double, 2>>{} , "!");
		multi::array<double, 2> c = herk(filling::upper, a);//c†=c=aa†=(aa†)†, `c` in lower triangular
	}
}

BOOST_AUTO_TEST_CASE(multi_blas_herk_complex_real_case_1d){
	multi::array<complex, 2> const a = {
		{ 1., 3., 4.},
	};
	namespace blas = multi::blas;
	using blas::filling;
	using blas::transposed;
	using blas::hermitized;
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
		herk(filling::lower, 1., hermitized(a), 0., c);//c†=c=a†a=(a†a)†, `c` in lower triangular
		print(c);
		BOOST_REQUIRE( c[2][1]==complex(12.,0.) );
		BOOST_REQUIRE( c[1][2]==9999. );
	}
	{
		multi::array<complex, 2> c({3, 3}, 9999.);
		herk(2., hermitized(a), c);//c†=c=a†a=(a†a)†, `c` in lower triangular

		BOOST_REQUIRE( c[2][1]==complex(24.,0.) );
		BOOST_REQUIRE( c[1][2]==complex(24.,0.) );
		multi::array<complex, 2> c_gemm({3, 3});
	//	gemm(2., hermitized(a), a, c_gemm);
	}
}
#endif

#if 0
BOOST_AUTO_TEST_CASE(multi_blas_herk_complex_timing){
	multi::array<complex, 2> const a({4000, 4000}); std::iota(data_elements(a), data_elements(a) + num_elements(a), 0.2);
	multi::array<complex, 2> c({4000, 4000}, 9999.);
	boost::timer::auto_cpu_timer t;
	using multi::blas::herk;
	using multi::blas::hermitized;
	using multi::blas::filling;
	herk(filling::upper, 1., hermitized(a), c); // c†=c=a†a
}
#endif

#endif
#endif

