#ifdef COMPILATION_INSTRUCTIONS
(echo '#include"'$0'"'>$0.cpp)&&$CXX -Wall -Wextra -Wpedantic -D_TEST_MULTI_ADAPTORS_BLAS_SCAL $0.cpp -o $0x `pkg-config blas --cflags --libs` &&$0x&&rm $0x $0.cpp; exit
#endif
// © Alfredo A. Correa 2019

#ifndef MULTI_ADAPTORS_BLAS_SCAL_HPP
#define MULTI_ADAPTORS_BLAS_SCAL_HPP

#include "../blas/core.hpp"

#include<cassert>

#if __cplusplus>=201703L and __has_cpp_attribute(nodiscard)>=201603
#define NODISCARD(MsG) [[nodiscard]]
#elif __has_cpp_attribute(gnu::warn_unused_result)
#define NODISCARD(MsG) [[gnu::warn_unused_result]]
#else
#define NODISCARD(MsG)
#endif

namespace boost{
namespace multi{
namespace blas{

template<typename T, class It, typename Size>
auto scal_n(T a, It first, Size count)
->decltype(scal(count, a, base(first), stride(first)), first + count){
	return scal(count, a, base(first), stride(first)), first + count;}

template<typename T, class It>
auto scal(T a, It f, It l)
->decltype(scal_n(a, f, std::distance(f, l))){ assert(stride(f) == stride(l));
	return scal_n(a, f, std::distance(f, l));}

template<typename T, class X1D>
auto scal(T a, X1D&& m)
->decltype(scal(a, begin(m), end(m)), std::forward<X1D>(m)){
	return scal(a, begin(m), end(m)), std::forward<X1D>(m);}

template<typename T, class X1D> NODISCARD("when second argument is const")
auto scal(T a, X1D const& m)->std::decay_t<decltype(scal(a, m.decay()))>{
	return scal(a, m.decay());
}

}}}

#if _TEST_MULTI_ADAPTORS_BLAS_SCAL

#include "../../array.hpp"

#include<cassert>

using std::cout;
namespace multi = boost::multi;

int main(){
	{
		multi::array<double, 2> A = {
			{1.,  2.,  3.,  4.},
			{5.,  6.,  7.,  8.},
			{9., 10., 11., 12.}
		};

		using multi::blas::scal;
		auto&& S = scal(2., rotated(A)[1]);

		assert( A[2][1] == 20. );
		assert( S[0] == 4. );
	}
	{
		multi::array<double, 2> const A = {
			{1.,  2.,  3.,  4.},
			{5.,  6.,  7.,  8.},
			{9., 10., 11., 12.}
		};
		using multi::blas::scal;
		auto rA1_scaled = scal(2., A[1]);
		assert( size(rA1_scaled) == 4 );
		assert( rA1_scaled[1] == 12. );
	}
	{
	//	multi::array<double, 1> b; // empty vector is ok
	//	using multi::blas::scal;
	//	scal(2., b);
	//	assert( size(b) == 0 );
	}
}

#endif
#endif

