// -*-indent-tabs-mode:t;c-basic-offset:4;tab-width:4;autowrap:nil;-*-
// © Alfredo A. Correa 2019-2021

#ifndef MULTI_ADAPTORS_BLAS_DOT_HPP
#define MULTI_ADAPTORS_BLAS_DOT_HPP

#include "../blas/core.hpp"
#include "../blas/numeric.hpp" // is_complex
#include "../blas/operations.hpp" // blas::C

namespace boost{
namespace multi::blas{

using core::dot ;
using core::dotu;
using core::dotc;

template<class Context, class XIt, class Size, class YIt, class RPtr>
auto dot_n(Context&& ctxt, XIt x_first, Size count, YIt y_first, RPtr rp){
	if constexpr(is_complex<typename XIt::value_type>{}){
		;;;; if constexpr (!is_conjugated<XIt>{} and !is_conjugated<YIt>{}) std::forward<Context>(ctxt)->dotu(count,            base(x_first) , stride(x_first), base(y_first), stride(y_first), rp);
		else if constexpr (!is_conjugated<XIt>{} and  is_conjugated<YIt>{}) std::forward<Context>(ctxt)->dotc(count, underlying(base(y_first)), stride(y_first), base(x_first), stride(x_first), rp);
		else if constexpr ( is_conjugated<XIt>{} and !is_conjugated<YIt>{}) std::forward<Context>(ctxt)->dotc(count, underlying(base(x_first)), stride(x_first), base(y_first), stride(y_first), rp);
		else if constexpr ( is_conjugated<XIt>{} and  is_conjugated<YIt>{}) static_assert(!sizeof(XIt*), "not implemented in blas");
	}else{
                                                                            std::forward<Context>(ctxt)->dot (count,            base(x_first) , stride(x_first), base(y_first), stride(y_first), rp);
	}
	struct{XIt x_last; YIt y_last;} ret{x_first + count, y_first + count};
	return ret;
}

template<class XIt, class Size, class YIt, class RPtr>
auto dot_n(XIt x_first, Size count, YIt y_first, RPtr rp){//->decltype(dot_n(blas::context{}, x_first, count, y_first, rp)){
	if constexpr(is_conjugated<XIt>{}){
		auto ctxtp = blas::default_context_of(underlying(x_first.base()));
		return dot_n(ctxtp, x_first, count, y_first, rp);
	}else{
		auto ctxtp = blas::default_context_of(x_first.base());
		return dot_n(ctxtp, x_first, count, y_first, rp);
	}
}

template<class Context, class X1D, class Y1D, class R>
R&& dot(Context&& ctxt, X1D const& x, Y1D const& y, R&& r){
	assert( size(x) == size(y) );
	return blas::dot_n(std::forward<Context>(ctxt), begin(x), size(x), begin(y), &r), std::forward<R>(r);
}

template<class X1D, class Y1D, class R>
R&& dot(X1D const& x, Y1D const& y, R&& r){
	assert( size(x) == size(y) );
	if constexpr(is_conjugated<X1D>{}){
		auto ctxtp = blas::default_context_of(underlying(x.base()));
		return blas::dot(ctxtp, x, y, r);
	}else{
		auto ctxtp = blas::default_context_of(x.base());
		return blas::dot(ctxtp, x, y, r);
	}
}

template<class ContextPtr, class ItX, class Size, class ItY>
class dot_ptr{
	ContextPtr ctxt_;
	ItX  x_first_;
	Size count_;
	ItY  y_first_;
protected:
	dot_ptr(ContextPtr ctxt, ItX x_first, Size count, ItY y_first) : ctxt_{ctxt}, x_first_{x_first}, count_{count}, y_first_{y_first}{}
public:
	dot_ptr(dot_ptr const&) = default;
	template<class ItOut, class Size2>
	friend constexpr auto copy_n(dot_ptr first, [[maybe_unused]] Size2 count, ItOut d_first)
	->decltype(blas::dot_n(std::declval<ContextPtr>(), std::declval<ItX>(), Size{}      , std::declval<ItY>(), d_first), d_first + count){assert(count == 1);
		return blas::dot_n(first.ctxt_               , first.x_first_     , first.count_, first.y_first_     , d_first), d_first + count;}

	template<class ItOut, class Size2>
	friend constexpr auto uninitialized_copy_n(dot_ptr first, Size2 count, ItOut d_first)
	->decltype(blas::dot_n(std::declval<ContextPtr>(), std::declval<ItX>(), Size{}      , std::declval<ItY>(), d_first), d_first + count){assert(count == 1);
		return blas::dot_n(first.ctxt_            , first.x_first_     , first.count_, first.y_first_     , d_first), d_first + count;}
//	->decltype(copy_n(first, count, d_first)){ // nvcc is not detecting friend copy_n
//		return copy_n(first, count, d_first);}
};

template<class ContextPtr, class X, class Y, class Ptr = dot_ptr<ContextPtr, typename X::const_iterator, typename X::size_type, typename Y::const_iterator>>
struct dot_ref : private Ptr{
	dot_ref(dot_ref const&) = delete;
	using decay_type = decltype(typename X::value_type{}*typename Y::value_type{});
	dot_ref(ContextPtr ctxt, X const& x, Y const& y) : Ptr{ctxt, begin(x), size(x), begin(y)}{assert(size(x)==size(y));}
	constexpr Ptr const& operator&() const&{return *this;}
	decay_type decay() const{decay_type r; copy_n(operator&(), 1, &r); return r;}
	operator decay_type() const&{return decay();}
	friend auto operator*(decay_type const& lhs, dot_ref const& self){return lhs*self.decay();}
	decay_type operator+() const{return decay();}
	bool operator==(dot_ref const& other) const{return decay() == other.decay();}
	bool operator!=(dot_ref const& other) const{return decay() != other.decay();}
	template<class Other>
	auto operator==(Other const& other) const
	->decltype(decay()==other){
		return decay()==other;}
	template<class Other>
	auto operator!=(Other const& other) const
	->decltype(decay()!=other){
		return decay()!=other;}
};

template<class Context, class X, class Y> [[nodiscard]] 
dot_ref<Context, X, Y> dot(Context const& ctxt, X const& x, Y const& y){return {ctxt, x, y};}

//template<class X, class Y> [[nodiscard]] 
//dot_ref<blas::context, X, Y> dot(X const& x, Y const& y){return {blas::context{}, x, y};}

template<class X, class Y> [[nodiscard]]
auto dot(X const& x, Y const& y){
	if constexpr(is_conjugated<X>{}){
		auto ctxtp = blas::default_context_of(underlying(x.base()));
		return blas::dot(ctxtp, x, y);
	}else{
		auto ctxtp = blas::default_context_of(x.base());
		return blas::dot(ctxtp, x, y);
	}
}

namespace operators{
	template<class X1D, class Y1D> [[nodiscard]]
	auto operator,(X1D const& x, Y1D const& y)
	->decltype(dot(x, y)){
		return dot(x, y);}
}

}
}

#endif

