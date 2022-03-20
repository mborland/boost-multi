// -*-indent-tabs-mode:t;c-basic-offset:4;tab-width:4;autowrap:nil;-*-
// Copyright 2018-2021 Alfredo A. Correa

#ifndef MULTI_LAYOUT_HPP
#define MULTI_LAYOUT_HPP

#define EXCLUDE_CPPCHECK

#include "index_range.hpp"

#include "../config/ASSERT.hpp"
#include "../config/NO_UNIQUE_ADDRESS.hpp"

#include "../detail/operators.hpp"

#include <tuple>        // for apply
#include <type_traits>  // for make_signed_t
#include <utility>      // for swap

namespace boost::multi {

namespace detail {

template<class To, class From, size_t... I>
constexpr auto to_tuple_impl(std::initializer_list<From> il, std::index_sequence<I...>/*012*/) {
	(void)il;
	return std::make_tuple(To{il.begin()[I]}...);
}

template<class To, class From, size_t... I>
constexpr auto to_tuple_impl(std::array<From, sizeof...(I)> arr, std::index_sequence<I...>/*012*/) {
	return std::make_tuple(To{std::get<I>(arr)}...);
}

template<class To, std::size_t N, class From>
constexpr auto to_tuple(std::array<From, N> arr) {
	return to_tuple_impl<To, From>(arr, std::make_index_sequence<N>());
}

template <class TT, class Tuple, std::size_t... I>
constexpr auto to_array_impl(
	Tuple&& t, std::index_sequence<I...> /*012*/
) -> std::array<TT, std::tuple_size<std::decay_t<Tuple>>{}> {
	return {static_cast<TT>(std::get<I>(std::forward<Tuple>(t)))...};
}

template<class T = void, class Tuple, class TT = std::conditional_t<std::is_same<T, void>{}, std::decay_t<decltype(std::get<0>(std::decay_t<Tuple>{}))>, T> >
constexpr auto to_array(Tuple&& t) -> std::array<TT, std::tuple_size<std::decay_t<Tuple>>{}> {
	return to_array_impl<TT>(
		std::forward<Tuple>(t),
		std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>{}>{}
	);
}

template <class Tuple, std::size_t... Ns>
constexpr auto tuple_tail_impl(Tuple&& t, std::index_sequence<Ns...> /*012*/) {
	(void)t;  // workaround bug warning in nvcc
	return std::forward_as_tuple(std::forward<decltype(std::get<Ns + 1>(t))>(std::get<Ns + 1>(t))...);
}

template<class Tuple>
constexpr auto tuple_tail(Tuple&& t)
->decltype(tuple_tail_impl(t, std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>{} - 1>())) {
	return tuple_tail_impl(t, std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>{} - 1>()); }

}  // end namespace detail

template<dimensionality_type D, typename SSize=multi::size_type> struct layout_t;

#ifdef EXCLUDE_CPPCHECK  // TODO(correaa) there is code in this that makes cppcheck crash, narrow it down with ifdef/endif

template<dimensionality_type D>
struct extensions_t {
	using base_ = std::decay_t<decltype(std::tuple_cat(std::make_tuple(std::declval<index_extension>()), std::declval<typename extensions_t<D-1>::base_>()))>;

 private:
	base_ impl_;

 public:
	static constexpr dimensionality_type dimensionality = D;

	extensions_t() = default;
	using nelems_type = multi::index;

	template<class T = void, std::enable_if_t<sizeof(T*) and D == 1, int> = 0>
	// cppcheck-suppress noExplicitConstructor ; to allow passing tuple<int, int> // NOLINTNEXTLINE(runtime/explicit)
	constexpr extensions_t(multi::size_t i) : extensions_t{index_extension{i}} {}  // NOLINT(google-explicit-constructor,hicpp-explicit-conversions) : allow terse syntax

	template<class T = void, std::enable_if_t<sizeof(T*) and D == 1, int> = 0>
	// cppcheck-suppress noExplicitConstructor ; to allow passing tuple<int, int> // NOLINTNEXTLINE(runtime/explicit)
	constexpr extensions_t(index_extension e1) : impl_{e1} {}  // NOLINT(google-explicit-constructor,hicpp-explicit-conversions) : allow terse syntax

	template<class T = void, std::enable_if_t<sizeof(T*) and D == 2, int> = 0>
	constexpr extensions_t(index_extension e1, index_extension e2) : impl_{e1, e2} {}

	template<class T = void, std::enable_if_t<sizeof(T*) and D == 3, int> = 0>
	constexpr extensions_t(index_extension e1, index_extension e2, index_extension e3) : impl_{e1, e2, e3} {}

	template<class T = void, std::enable_if_t<sizeof(T*) and D == 4, int> = 0>
	constexpr extensions_t(index_extension e1, index_extension e2, index_extension e3, index_extension e4) : impl_{e1, e2, e3, e4} {}

	template<class T = void, std::enable_if_t<sizeof(T*) and D == 5, int> = 0>
	constexpr extensions_t(index_extension e1, index_extension e2, index_extension e3, index_extension e4, index_extension e5) : impl_{e1, e2, e3, e4, e5} {}

	template<class T = void, std::enable_if_t<sizeof(T*) and D == 6, int> = 0>
	constexpr extensions_t(index_extension e1, index_extension e2, index_extension e3, index_extension e4, index_extension e5, index_extension e6) : impl_{e1, e2, e3, e4, e5, e6} {}

	template<class T1, class T = void, class = decltype(base_{std::tuple<T1>{}}), std::enable_if_t<sizeof(T*) and D == 1, int> = 0>
	// cppcheck-suppress noExplicitConstructor ; to allow passing tuple<int, int> // NOLINTNEXTLINE(runtime/explicit)
	constexpr extensions_t(std::tuple<T1> e) : impl_{std::move(e)} {} // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)

	template<class T1, class T2, class T = void, class = decltype(base_{std::tuple<T1, T2>{}}), std::enable_if_t<sizeof(T*) and D == 2, int> = 0>
	// cppcheck-suppress noExplicitConstructor ; to allow passing tuple<int, int> // NOLINTNEXTLINE(runtime/explicit)
	constexpr extensions_t(std::tuple<T1, T2> e) : impl_{std::move(e)} {} // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)

	template<class T1, class T2, class T3, class T = void, class = decltype(base_{std::tuple<T1, T2, T3>{}}), std::enable_if_t<sizeof(T*) and D == 3, int> = 0>
	// cppcheck-suppress noExplicitConstructor ; to allow passing tuple<int, int> // NOLINTNEXTLINE(runtime/explicit)
	constexpr extensions_t(std::tuple<T1, T2, T3> e) : impl_{std::move(e)} {} // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)

	template<class T1, class T2, class T3, class T4, class T = void, class = decltype(base_{std::tuple<T1, T2, T3, T4>{}}), std::enable_if_t<sizeof(T*) and D == 4, int> = 0>
	// cppcheck-suppress noExplicitConstructor ; to allow passing tuple<int, int> // NOLINTNEXTLINE(runtime/explicit)
	constexpr extensions_t(std::tuple<T1, T2, T3, T4> e) : impl_{std::move(e)} {} // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)

	template<class... Ts>
	constexpr explicit extensions_t(std::tuple<Ts...> const& t)
	: extensions_t(t, std::make_index_sequence<static_cast<std::size_t>(D)>()) {}

	constexpr extensions_t(index_extension const& ie, typename layout_t<D-1>::extensions_type const& other)
	: extensions_t(std::tuple_cat(std::make_tuple(ie), other.base())) {}

	constexpr auto base()            const&    -> base_ const& {return impl_;}

	friend constexpr auto operator*(index_extension const& ie, extensions_t const& self) -> extensions_t<D + 1> {
		return extensions_t<D + 1>{std::tuple_cat(std::make_tuple(ie), self.base())};
	}

	friend auto operator==(extensions_t const& s, extensions_t const& o) {return s.impl_ == o.impl_;}
	friend auto operator!=(extensions_t const& s, extensions_t const& o) {return s.impl_ != o.impl_;}

//	auto operator!=(extensions_t const& other) const -> bool {return impl_ != other.impl_;}

	using indices_type = decltype(std::tuple_cat(std::make_tuple(multi::index{}), typename extensions_t<D-1>::indices_type{}));

	template<class Tuple = typename detail::repeat<multi::index, D, std::tuple>::type>
	[[nodiscard]] constexpr auto from_linear(nelems_type const& n) const {
		auto const sub_extensions = extensions_t<D-1>{detail::tuple_tail(this->base())};
		auto const sub_num_elements = sub_extensions.num_elements();
		assert( sub_num_elements != 0 );
		return std::apply(
			[](auto const&... e){return Tuple{e...};},
			std::tuple_cat(std::make_tuple(n/sub_num_elements), sub_extensions.from_linear(n%sub_num_elements))
		);
	}

	friend constexpr auto operator%(nelems_type n, extensions_t const& s) {return s.from_linear(n);}

	constexpr explicit operator bool() const {return not layout_t<D>{*this}.empty();}

	template<class... Indices>
	constexpr auto to_linear(index i, Indices... is) const {
		auto const sub_extensions = extensions_t<D-1>{detail::tuple_tail(this->base())};
		return i*sub_extensions.num_elements() + sub_extensions.to_linear(is...);
	}
	template<class... Indices>
	constexpr auto operator()(index i, Indices... is) const {return to_linear(i, is...);}

	template<class... Indices>
	constexpr auto next_canonical(index& i, Indices&... is) const -> bool {
		if(extensions_t<D-1>{detail::tuple_tail(this->base())}.next_canonical(is...)) {++i;}
		if(i == std::get<0>(impl_).last()) {
			i = std::get<0>(impl_).first();
			return true;
		}
		return false;
	}
	template<class... Indices>
	constexpr auto prev_canonical(index& i, Indices&... is) const -> bool {
		if(extensions_t<D-1>{detail::tuple_tail(this->base())}.prev_canonical(is...)) {--i;}
		if(i <  std::get<0>(impl_).first()) {
			i = std::get<0>(impl_).back();
			return true;
		}
		return false;
	}

 private:
	template<class Archive, std::size_t... I>
	void serialize_impl(Archive& ar, std::index_sequence<I...> /*012*/) {
		(void)std::initializer_list<unsigned>{(ar & multi::archive_traits<Archive>::make_nvp("extension", std::get<I>(impl_)) , 0U)...};
	//	(void)std::initializer_list<unsigned>{(ar & boost::serialization::          make_nvp("extension", std::get<I>(impl_)) , 0U)...};
	//	(void)std::initializer_list<unsigned>{(ar & cereal::                        make_nvp("extension", std::get<I>(impl_)) , 0U)...};
	//	(void)std::initializer_list<unsigned>{(ar &                                                       std::get<I>(impl_)  , 0U)...};
	}

 public:
	template<class Archive>
	void serialize(Archive& ar, const unsigned int /*version*/) {//, unsigned /*version*/) {
		serialize_impl(ar, std::make_index_sequence<static_cast<std::size_t>(D)>());
	}

 private:
	template<class Array, std::size_t... I, typename = decltype(base_{std::get<I>(std::declval<Array const&>())...})>
	constexpr extensions_t(Array const& t, std::index_sequence<I...> /*012*/) : impl_ {std::get<I>(t)...} {}

	static constexpr auto multiply_fold() -> size_type {return static_cast<size_type>(1);}
	static constexpr auto multiply_fold(size_type const& a0) -> size_type {return static_cast<size_type>(a0);}
	template<class...As> static constexpr auto multiply_fold(size_type const& a0, As const&...as) -> size_type {return static_cast<size_type>(a0)*static_cast<size_type>(multiply_fold(as...));}

	template<std::size_t... I> constexpr auto num_elements_impl(std::index_sequence<I...> /*012*/) const -> size_type {return static_cast<size_type>(multiply_fold(static_cast<size_type>(std::get<I>(impl_).size())...));}

 public:
	constexpr auto num_elements() const -> size_type {
		return static_cast<size_type>(num_elements_impl(std::make_index_sequence<static_cast<std::size_t>(D)>()));
	}
	friend constexpr auto intersection(extensions_t const& x1, extensions_t const& x2) -> extensions_t{
		return extensions_t{
			std::tuple_cat(
				std::tuple<index_extension>{intersection(std::get<0>(x1.impl_), std::get<0>(x2.impl_))},
				intersection( extensions_t<D-1>{detail::tuple_tail(x1.base())}, extensions_t<D-1>{detail::tuple_tail(x2.base())} ).base()
			)
		};
	}
};

template<> struct extensions_t<0> {
	using base_ = std::tuple<>;

 private:
	base_ impl_;

 public:
	static constexpr dimensionality_type dimensionality = 0;  // TODO(correaa): consider deprecation

	using rank = std::integral_constant<dimensionality_type, 0>;

	using nelems_type = index;
//	using std::tuple<>::tuple;

	explicit extensions_t(std::tuple<> const& t) : impl_{t} {}

	extensions_t() = default;

	constexpr auto base() const -> base_ const& {return impl_;}

	template<class Archive> void serialize(Archive&/*ar*/, unsigned /*version*/) {}

	static constexpr auto num_elements() /*const*/ -> size_type {return 1;}

	using indices_type = std::tuple<>;

	template<class Tuple = typename detail::repeat<multi::index, 0, std::tuple>::type>
	[[nodiscard]] static constexpr auto from_linear(nelems_type const& n) /*const*/ -> indices_type {
		assert(n == 0); (void)n;  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay) : constexpr function
		return Tuple{};
	}
	friend constexpr auto operator%(nelems_type const& n, extensions_t const& /*s*/) -> std::tuple<> {return /*s.*/from_linear(n);}

	static constexpr auto to_linear() /*const*/ -> difference_type {return 0;}
	constexpr auto operator()() const {return to_linear();}

	static constexpr auto next_canonical() /*const*/ -> bool {return true;}
	static constexpr auto prev_canonical() /*const*/ -> bool {return true;}

	friend constexpr auto intersection(extensions_t const& /*x1*/, extensions_t const& /*x2*/) -> extensions_t {return {};}

	constexpr auto operator==(extensions_t const& /*other*/) const {return true ;}
	constexpr auto operator!=(extensions_t const& /*other*/) const {return false;}
};

template<> struct extensions_t<1> {
	using base_ = std::tuple<multi::index_extension>;

 private:
	base_ impl_;

 public:
	static constexpr auto dimensionality = 1;  // TODO(correaa): consider deprecation

	using nelems_type = index;

	// seems to be needed by icpc 20.x
	// cppcheck-suppress noExplicitConstructor ; to allow terse syntax (compatible with std::vector(int) constructor
	constexpr extensions_t(multi::size_t size) : impl_{multi::index_extension{0, size}} {}  // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)

	template<class T1>
	// cppcheck-suppress noExplicitConstructor ; to allow passing tuple<int, int> // NOLINTNEXTLINE(runtime/explicit)
	constexpr extensions_t(std::tuple<T1> e) : impl_{std::move(e)} {} // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)

	// cppcheck-suppress noExplicitConstructor ; to allow passing tuple<int, int> // NOLINTNEXTLINE(runtime/explicit)
	constexpr extensions_t(multi::index_extension e1) : impl_{e1} {}  // NOLINT(google-explicit-constructor,hicpp-explicit-conversions) : allow terse syntax
	constexpr explicit extensions_t(base_ t) : impl_{std::move(t)} {}

	extensions_t() = default;
	constexpr auto base() const -> base_ const& {return impl_;}

	auto operator==(extensions_t const& other) const -> bool {return impl_ == other.impl_;}
	auto operator!=(extensions_t const& other) const -> bool {return impl_ != other.impl_;}

	constexpr auto num_elements() const -> size_type {return std::get<0>(impl_).size();}

	using indices_type = decltype(std::make_tuple(multi::index{}));

	template<class Tuple = typename detail::repeat<multi::index, 1, std::tuple>::type>
	[[nodiscard]] constexpr auto from_linear(nelems_type const& n) const {
	//	assert(n <= num_elements());  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay) : normal in constexpr function
	//	return std::make_tuple(n);
	//  return std::tuple<multi::index>{n};
		return Tuple{n};
	}

	friend constexpr auto operator%(nelems_type n, extensions_t const& s) -> std::tuple<multi::index> {return s.from_linear(n);}

	static constexpr auto to_linear(index const& i) -> difference_type  /*const*/ {return i;}
	constexpr auto operator()(index const& i) const -> difference_type {return to_linear(i);}

	template<class... Indices>
	constexpr auto next_canonical(index& i) const -> bool {
		++i;
		if(i == std::get<0>(impl_).last()) {
			i = std::get<0>(impl_).first();
			return true;
		}
		return false;
	}
	constexpr auto prev_canonical(index& i) const -> bool {
		--i;
		if(i == std::get<0>(impl_).first() - 1) {
			i = std::get<0>(impl_).back();
			return true;
		}
		return false;
	}

	friend auto intersection(extensions_t const& x1, extensions_t const& x2){
		return extensions_t({ intersection(std::get<0>(x1.impl_), std::get<0>(x2.impl_)) });
	}
	template<class Ar>
	void serialize(Ar& ar, unsigned /*version*/) {
		auto& extension_ = std::get<0>(impl_);
		ar & multi::archive_traits<Ar>::make_nvp("extension", extension_);
	//	ar & boost::serialization::     make_nvp("extension", extension);
	//	ar & cereal::                   make_nvp("extension", extension);
	//	ar &                                                  extension ;
	}
};

#endif  // EXCLUDE_CPPCHECK

template<dimensionality_type D> using iextensions = extensions_t<D>;

template<boost::multi::dimensionality_type D>
constexpr auto array_size_impl(const boost::multi::extensions_t<D>&)
	-> std::integral_constant<std::size_t, static_cast<std::size_t>(D)>;

}  // end namespace boost::multi

namespace std {  // NOLINT(cert-dcl58-cpp) : to implement structured bindings

    template<boost::multi::dimensionality_type D>
    struct tuple_size<boost::multi::extensions_t<D>> : std::integral_constant<std::size_t, static_cast<std::size_t>(D)> {};

	template<std::size_t Index, boost::multi::dimensionality_type D>
	constexpr auto get(boost::multi::extensions_t<D> const& self) -> auto const& {
		return std::get<Index>(self.base());
	}

}  // end namespace std

namespace boost::multi {

struct monostate : equality_comparable<monostate> {
	friend constexpr auto operator==(monostate const& /*self*/, monostate const& /*other*/) {return true;}
};

template<typename SSize>
struct layout_t<0, SSize>
: multi::equality_comparable<layout_t<0, SSize> >
{
	using dimensionality_type = multi::dimensionality_type;
	using rank = std::integral_constant<dimensionality_type, 0>;

	using size_type       = SSize;
	using difference_type = std::make_signed_t<size_type>;
	using index           = difference_type;
	using index_extension = multi::index_extension;
	using index_range     = multi::range<index>;

	using sub_type    = monostate;
	using stride_type = monostate;
	using offset_type = index;
	using nelems_type = index;

	using strides_type  = std::tuple<>;
	using offsets_type  = std::tuple<>;
	using nelemss_type  = std::tuple<>;

	using extensions_type = extensions_t<rank::value>;
	using sizes_type      = std::tuple<>;

	static constexpr dimensionality_type rank_v = rank::value;
	static constexpr dimensionality_type dimensionality = rank_v;  // TODO(correaa) : consider deprecation

	friend constexpr auto dimensionality(layout_t const&/*self*/) {return rank_v;}

 private:
	sub_type    sub_    = {};  // TODO(correaa)  use  [[no_unique_address]]
	stride_type stride_ = {};  // TODO(correaa)  use  [[no_unique_address]]
	offset_type offset_ =  0;
	nelems_type nelems_ =  1;  // TODO(correaa) : or std::numeric_limits<nelems_type>::max(); ?

	template<dimensionality_type, typename> friend struct layout_t;

 public:
	layout_t() = default;
	constexpr explicit layout_t(extensions_type const& /*nil*/) {}

	constexpr layout_t(sub_type sub, stride_type stride, offset_type offset, nelems_type nelems)  // NOLINT(bugprone-easily-swappable-parameters)
	: sub_{sub}, stride_{stride}, offset_{offset}, nelems_{nelems} {}

	[[nodiscard]] constexpr auto extensions()        const     {return extensions_type{};}
	friend        constexpr auto extensions(layout_t const& s) {return s.extensions();}

	[[nodiscard]] constexpr auto num_elements()        const     {return nelems_;}
	friend        constexpr auto num_elements(layout_t const& s) {return s.num_elements();}

	[[nodiscard]] constexpr auto empty()        const     {return nelems_ == 0;}
	friend        constexpr auto empty(layout_t const& s) {return s.empty();}

	[[nodiscard]] constexpr auto sizes()        const     {return std::tuple<>{};}
	friend        constexpr auto sizes(layout_t const& s) {return s.sizes();}

	[[nodiscard]] auto strides() const {return strides_type{};}
	[[nodiscard]] auto offsets() const {return offsets_type{};}
	[[nodiscard]] auto nelemss() const {return nelemss_type{};}

	constexpr auto operator()() const {return offset_;}
	explicit operator offset_type() const {return offset_;}

	constexpr auto base_size() const -> size_type   {return 0;}
	constexpr auto origin()    const -> offset_type {return 0;}

	constexpr auto reverse()          -> layout_t& {return *this;}
	constexpr auto scale(size_type /*s*/) const {return *this;}

//	friend constexpr auto operator!=(layout_t const& s, layout_t const& o) {return not(s == o);}
	friend constexpr auto operator==(layout_t const& s, layout_t const& o) {
		return s.sub_ == o.sub_ and s.stride_ == o.stride_ and s.offset_ == o.offset_ and s.nelems_ == o.nelems_;
	}

	constexpr auto   rotate() -> layout_t& {return *this;}
	constexpr auto unrotate() -> layout_t& {return *this;}

	constexpr auto hull_size() const -> size_type {return num_elements();}
};

#if 0
template<typename SSize>
struct layout_t<1, SSize>
: multi::equality_comparable2<layout_t<1, SSize>, void> { static constexpr multi::dimensionality_type D = 1;  // TODO(correaa) check equality_comparable2, not working
	using dimensionality_type = multi::dimensionality_type;
	using sub_type = layout_t<D - 1, SSize>;
	using size_type = SSize;
	using index = multi::index;
	using difference_type = std::make_signed_t<size_type>;
	using index_extension = multi::index_extension;
	using index_range = multi::range<index>;
	using stride_type = index;
	using offset_type = index;
	using nelems_type = index;

	static constexpr dimensionality_type dimensionality = D;  // TODO(correaa): consider deprecation

	static constexpr dimensionality_type rank_v = D;
	using rank = std::integral_constant<dimensionality_type, rank_v>;

	friend constexpr auto dimensionality(layout_t const& /*self*/) {return D;}

 private:
	MULTI_NO_UNIQUE_ADDRESS
	sub_type    sub_       ;
	stride_type stride_ = 1;  // TODO(correaa) std::numeric_limits<stride_type>::max()?
	offset_type offset_ = 0;
	nelems_type nelems_ = 0;

	template<dimensionality_type, typename> friend struct layout_t;

 public:
	using extensions_type = extensions_t<D>;
	using strides_type    = decltype(tuple_cat(std::make_tuple(std::declval<index    >()), std::declval<typename sub_type::strides_type>()));
	using sizes_type      = decltype(tuple_cat(std::make_tuple(std::declval<size_type>()), std::declval<typename sub_type::sizes_type  >()));

	layout_t() = default;

	constexpr layout_t(sub_type sub, stride_type stride, offset_type offset, nelems_type nelems)  // NOLINT(bugprone-easily-swappable-parameters)
	: sub_{sub}, stride_{stride}, offset_{offset}, nelems_{nelems} {}

	constexpr explicit layout_t(extensions_type const& x)
	: sub_{std::apply([](auto... e){return multi::extensions_t<D - 1>{e...};}, detail::tail(x.base()))}
//  , stride_{1}  // sub_.size()*sub_.stride()}
	, offset_{std::get<0>(x.base()).first()*stride_}
	, nelems_{std::get<0>(x.base()).size()*(sub().num_elements())} {}

 private:
	constexpr auto at(index i) const {
		auto ret = sub_;
		ret.offset_ += offset_ + i*stride_;
		return ret();
	}

 public:
	constexpr auto operator[](index i) const {return at(i);}

	constexpr auto operator()()        const {return *this;}
	constexpr auto operator()(index i) const {return at(i);}
	template<class... Is>
	constexpr auto operator()(index i, Is... is) const {
		return operator[](i)(is...);
	}

	constexpr auto origin() const {return sub_.origin() - offset_;}

	       constexpr auto sub()             &    -> sub_type      & {return sub_;}
	       constexpr auto sub()        const&    -> sub_type const& {return sub_;}
	friend constexpr auto sub(layout_t const& s) -> sub_type const& {return s.sub();}

	       constexpr auto offset()        const& -> offset_type {return offset_;}
	friend constexpr auto offset(layout_t const& s) {return s.offset();}
	       constexpr auto offset(dimensionality_type d) const -> offset_type {
		assert(d < D); (void)d;  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay) : normal in a constexpr function
		return offset_;
	}
	constexpr auto offsets() const {return tuple_cat(std::make_tuple(offset()), sub_.offsets());}

	       constexpr auto nelems()             &    -> nelems_type      & {return   nelems_;}
	       constexpr auto nelems()        const&    -> nelems_type const& {return   nelems_;}
	friend constexpr auto nelems(layout_t const& s) -> nelems_type const& {return s.nelems();}

	constexpr auto nelems(dimensionality_type d) const {
		assert(d < D); (void)d;  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay) : normal in constexpr function
		return nelems_;
	}

	friend constexpr auto size(layout_t const& s) -> size_type {return s.size();}
	       constexpr auto size()        const     -> size_type {
		if(nelems_ == 0) {return 0;}  // TODO(correaa) perhaps initializing stride_ to max removes this if-statement
		MULTI_ACCESS_ASSERT(stride_);  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay) : normal in constexpr function
		return nelems_/stride_;
	}
//	constexpr auto size(dimensionality_type d) const -> size_type {  // TODO(correaa) remove?
//		assert( d == 0 ); (void)d;  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay) : normal in constexpr function
//		return nelems_/stride_;
//	}

	constexpr auto reindex(index i) -> layout_t& {offset_ = i*stride_; return *this;}
	template<class... Is>
	constexpr auto reindex(index i, Is... is) -> layout_t& {reindex(i).rotate().reindex(is...).unrotate(); return *this;}

	constexpr auto base_size() const {using std::max; return max(nelems_, sub_.base_size());}

	       constexpr auto num_elements()        const&    -> size_type {return size()*sub_.num_elements();}
	friend constexpr auto num_elements(layout_t const& s) -> size_type {return s.num_elements();}

	constexpr auto stride()      & -> stride_type      & {return stride_;}
	constexpr auto stride() const& -> stride_type const& {return stride_;}

	       constexpr auto strides()        const&    {return tuple_cat(std::make_tuple(stride()), sub_.strides());}
	friend constexpr auto strides(layout_t const& s) {return s.strides();}
	friend constexpr auto stride(layout_t const& s) -> index {return s.stride();}

	       constexpr auto is_empty()        const     {return nelems_ == 0;}
	friend constexpr auto is_empty(layout_t const& s) {return s.is_empty();}

	[[nodiscard]]
	constexpr auto    empty()        const {return is_empty();}

	constexpr auto sizes()   const {return tuple_cat(std::make_tuple(size())  , sub_.sizes()  );}
	constexpr auto nelemss() const {return tuple_cat(std::make_tuple(nelems()), sub_.nelemss());}

	       constexpr auto is_compact()        const&    {return base_size() == num_elements();}
	friend constexpr auto is_compact(layout_t const& s) {return s.is_compact();}

	friend constexpr auto extension(layout_t const& s) -> index_extension {return s.extension();}
	       constexpr auto extension()        const&    -> index_extension {
		if(nelems_ == 0) {return {};}
		assert(stride_);  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay) : normal in a constexpr function
		return {offset_/stride_, (offset_ + nelems_)/stride_};
	}

	       constexpr auto extensions()        const     -> extensions_type {return extensions_type{tuple_cat(std::make_tuple(extension()), sub_.extensions().base())};}
	friend constexpr auto extensions(layout_t const& s) -> extensions_type {return s.extensions();}

	[[deprecated("use get<d>(m.extensions()")]] constexpr auto extension(dimensionality_type d) const {return std::apply([](auto... e){return std::array<size_type, D>{e...};}, extensions().base()).at(static_cast<std::size_t>(d));}
	[[deprecated("use get<d>(m.strides())  ")]] constexpr auto stride   (dimensionality_type d) const {return std::apply([](auto... e){return std::array<size_type, D>{e...};}, strides()          ).at(static_cast<std::size_t>(d));}
	[[deprecated("use get<d>(m.sizes())    ")]] constexpr auto size     (dimensionality_type d) const {return std::apply([](auto... e){return std::array<size_type, D>{e...};}, sizes()            ).at(static_cast<std::size_t>(d));}

	friend constexpr auto operator!=(layout_t const& self, layout_t const& other) {return not(self == other);}
	friend constexpr auto operator==(layout_t const& self, layout_t const& other) {
		return self.sub_==other.sub_ and self.stride_==other.stride_ and self.offset_==other.offset_ and self.nelems_==other.nelems_;
	}

	constexpr auto reverse() -> layout_t& {
		unrotate();
		sub_.reverse();
		return *this;
	}

	constexpr auto   rotate() -> layout_t& {
		if constexpr(D > 1) {
			transpose(); sub_.rotate();
		}
		return *this;
	}
	constexpr auto unrotate() -> layout_t& {
		if constexpr(D > 1) {
			sub_.unrotate(); transpose();
		}
		return *this;
	}

	constexpr auto transpose() -> layout_t& {
		if constexpr(D > 1) {
			using std::swap;
			swap(stride_, sub_.stride_);
			swap(offset_, sub_.offset_);
			swap(nelems_, sub_.nelems_);
		}
		return *this;
	}

	constexpr auto scale(size_type s) const {
		return layout_t{sub_.scale(s), stride_*s, offset_*s, nelems_*s};
	}
};
#endif

template<dimensionality_type D, typename SSize>
struct layout_t
: multi::equality_comparable<layout_t<D, SSize>>
{
	using dimensionality_type = multi::dimensionality_type;
	using rank = std::integral_constant<dimensionality_type, D>;

	using sub_type        = layout_t<D-1>;
	using size_type       = SSize;
	using difference_type = std::make_signed_t<size_type>;
	using index           = difference_type;

	using index_extension = multi::index_extension;
	using index_range = multi::range<index>;
	using stride_type = index;
	using offset_type = index;
	using nelems_type = index;

	using strides_type    = decltype(tuple_cat(std::make_tuple(std::declval<index      >()), std::declval<typename sub_type::strides_type>()));
	using offsets_type    = decltype(tuple_cat(std::make_tuple(std::declval<offset_type>()), std::declval<typename sub_type::offsets_type>()));
	using nelemss_type    = decltype(tuple_cat(std::make_tuple(std::declval<nelems_type>()), std::declval<typename sub_type::nelemss_type>()));

	using extensions_type = extensions_t<rank::value>;
	using sizes_type      = decltype(tuple_cat(std::make_tuple(std::declval<size_type  >()), std::declval<typename sub_type::sizes_type  >()));

	static constexpr dimensionality_type rank_v = rank::value;
	static constexpr dimensionality_type dimensionality = rank_v;  // TODO(correaa): consider deprecation

	friend constexpr auto dimensionality(layout_t const& /*self*/) {return rank_v;}

 private:
	sub_type    sub_    = {};
	stride_type stride_ =  1;  // or std::numeric_limits<stride_type>::max()?
	offset_type offset_ =  0;
	nelems_type nelems_ =  0;

	template<dimensionality_type, typename> friend struct layout_t;

 public:
	layout_t() = default;
	constexpr explicit layout_t(extensions_type const& x)
	: sub_(std::apply([](auto... e){return multi::extensions_t<D-1>{e...};}, detail::tail(x.base())))
	, stride_{sub_.num_elements()}  // {sub_.size()*sub_.stride()}
	, offset_{std::get<0>(x.base()).first()*stride_}
	, nelems_{std::get<0>(x.base()).size()*(sub().num_elements())} {}

	constexpr layout_t(sub_type sub, stride_type stride, offset_type offset, nelems_type nelems)  // NOLINT(bugprone-easily-swappable-parameters)
	: sub_{sub}, stride_{stride}, offset_{offset}, nelems_{nelems} {}

	constexpr auto origin() const {return sub_.origin() - offset_;}

 private:
	constexpr auto at_aux(index i) const {
		return sub_type{sub_.sub_, sub_.stride_, sub_.offset_ + offset_ + i*stride_, sub_.nelems_}();
	//	auto ret = sub_;
	//	ret.offset_ += offset_ + i*stride_;
	//	return ret();
	}

 public:
	constexpr auto operator[](index i) const {return at_aux(i);}

	template<typename... Indexes>
	constexpr auto operator()(index i, Indexes... idxs) const {return operator[](i)(idxs...);}
	constexpr auto operator()(index i)                  const {return at_aux(i);}
	constexpr auto operator()()                         const {return *this;}

	       constexpr auto sub()             &    -> sub_type      & {return sub_;}
	       constexpr auto sub()        const&    -> sub_type const& {return sub_;}
	friend constexpr auto sub(layout_t const& s) -> sub_type const& {return s.sub();}

	       constexpr auto nelems()             &    -> nelems_type      & {return   nelems_;}
	       constexpr auto nelems()        const&    -> nelems_type const& {return   nelems_;}
	friend constexpr auto nelems(layout_t const& s) -> nelems_type const& {return s.nelems();}

	constexpr auto nelems(dimensionality_type d) const {return (d!=0)?sub_.nelems(d-1):nelems_;}

//  friend constexpr auto operator!=(layout_t const& s, layout_t const& o) {return not(s == o);}
	friend constexpr auto operator==(layout_t const& s, layout_t const& o) -> bool {
		return s.sub_ == o.sub_ and s.stride_ == o.stride_ and s.offset_ == o.offset_ and s.nelems_ == o.nelems_;
	}

	constexpr auto reindex(index i) -> layout_t& {offset_ = i*stride_; return *this;}
	template<class... Idx>
	constexpr auto reindex(index i, Idx... is) -> layout_t& {reindex(i).rotate().reindex(is...).unrotate(); return *this;}

	       constexpr auto num_elements()        const&    -> size_type {return size()*sub_.num_elements();}
	friend constexpr auto num_elements(layout_t const& s) -> size_type {return s.num_elements();}

	       constexpr auto is_empty()        const     {return nelems_ == 0;}
	friend constexpr auto is_empty(layout_t const& s) {return s.is_empty();}

	constexpr auto    empty()        const {return is_empty();}

	friend constexpr auto size(layout_t const& l) -> size_type {return l.size();}
	       constexpr auto size()        const&    -> size_type {
		if(nelems_ == 0) {return 0;}
		MULTI_ACCESS_ASSERT(stride_);  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay) : normal in a constexpr function
		return nelems_/stride_;
	}

//	constexpr auto size(dimensionality_type d) const -> size_type {return (d!=0)?sub_.size(d-1):size();}

	constexpr auto stride()      & -> stride_type      & {return stride_;}
	constexpr auto stride() const& -> stride_type const& {return stride_;}

	friend constexpr auto stride(layout_t const& s) -> index {return s.stride();}

	       constexpr auto strides()        const&    -> strides_type {return tuple_cat(std::make_tuple(stride()), sub_.strides());}
	friend constexpr auto strides(layout_t const& s) -> strides_type {return s.strides();}

	constexpr auto offset(dimensionality_type d) const -> index {return (d!=0)?sub_.offset(d-1):offset_;}
	       constexpr auto offset() const -> index {return offset_;}
	friend constexpr auto offset(layout_t const& self) -> index {return self.offset();}
	constexpr auto offsets() const {return tuple_cat(std::make_tuple(offset()), sub_.offsets());}
	constexpr auto nelemss() const {return tuple_cat(std::make_tuple(nelems()), sub_.nelemss());}

	constexpr auto base_size() const {using std::max; return max(nelems_, sub_.base_size());}

	       constexpr auto is_compact()        const&    {return base_size() == num_elements();}
	friend constexpr auto is_compact(layout_t const& s) {return s.is_compact();}

	       constexpr auto shape()        const&    -> decltype(auto) {return   sizes();}
	friend constexpr auto shape(layout_t const& s) -> decltype(auto) {return s.shape();}

	constexpr auto sizes() const {return tuple_cat(std::make_tuple(size()), sub_.sizes());}
	template<class T = void>
	constexpr auto sizes_as() const {return detail::to_array<T>(sizes());}

	friend        constexpr auto extension(layout_t const& s) {return s.extension();}
	[[nodiscard]] constexpr auto extension()        const     {
		if(nelems_ == 0) {return index_extension{};}
		assert(stride_ != 0);  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay) : normal in a constexpr function
		assert(offset_ % stride_ == 0);
		assert(nelems_ % stride_ == 0);
		return index_extension{offset_/stride_, (offset_ + nelems_)/stride_};
	}

//	constexpr auto extension_aux() const -> index_extension {
//		assert(stride_ != 0 and nelems_%stride_ == 0);
//		return {offset_/stride_, (offset_ + nelems_)/stride_};
//	}

	constexpr auto extensions() const {return extensions_type{tuple_cat(std::make_tuple(extension()), sub_.extensions().base())};}
	friend constexpr auto extensions(layout_t const& self) -> extensions_type {return self.extensions();}

	[[deprecated("use get<d>(m.extensions()")]] constexpr auto extension(dimensionality_type d) const {return std::apply([](auto... e) {return std::array<index_extension, static_cast<std::size_t>(D)>{e...};}, extensions().base()).at(static_cast<std::size_t>(d));}
	[[deprecated("use get<d>(m.strides())  ")]] constexpr auto stride   (dimensionality_type d) const {return std::apply([](auto... e) {return std::array<stride_type    , static_cast<std::size_t>(D)>{e...};}, strides   ()       ).at(static_cast<std::size_t>(d));}
	[[deprecated("use get<d>(m.sizes())    ")]] constexpr auto size     (dimensionality_type d) const {return std::apply([](auto... e) {return std::array<size_type      , static_cast<std::size_t>(D)>{e...};}, sizes     ()       ).at(static_cast<std::size_t>(d));}

	template<typename Size>
	constexpr auto partition(Size const& s) -> layout_t& {
		using std::swap;
		stride_ *= s;
		nelems_ *= s;
		sub_.partition(s);
		return *this;
	}
//	template<class T = void, std::enable_if_t<(D > 1) and sizeof(T*), int> = 0>
	constexpr auto transpose() -> layout_t& {
		using std::swap;
		swap(stride_, sub_.stride_);
		swap(offset_, sub_.offset_);
		swap(nelems_, sub_.nelems_);
		return *this;
	}
	constexpr auto reverse() -> layout_t& {
		unrotate();
		sub_.reverse();
		return *this;
	}

	constexpr auto   rotate() -> layout_t& {if constexpr(D > 1) {transpose(); sub_.  rotate();} return *this;}
	constexpr auto unrotate() -> layout_t& {if constexpr(D > 1) {sub_.unrotate(); transpose();} return *this;}

	constexpr auto hull_size() const -> size_type {
		if(is_empty()) {return 0;}
		return std::abs(size()*stride())>std::abs(sub_.hull_size())?size()*stride():sub_.hull_size();
	}

//	constexpr auto   rotate(dimensionality_type r) -> layout_t& {
//		assert( r >= 0 );  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay) : normal in a constexpr function
//		while(r != 0) {rotate(); --r;}
//	//  if(r >= 0) {
//	//  } else {
//	//  	return rotate(D - r);
//	//  }
//		return *this;
//	}

//	constexpr auto unrotate(dimensionality_type r) -> layout_t& {
//		assert( r >= 0 );  // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay) : normal in a constexpr function
//		while(r != 0) {unrotate(); --r;}
//	//  if(r >= 0) {
//	//  } else {
//	//  	return unrotate(D-r);
//	//  }
//		return *this;
//	}

	constexpr auto scale(size_type s) const {
		return layout_t{sub_.scale(s), stride_*s, offset_*s, nelems_*s};
	}
};

inline constexpr auto
operator*(layout_t<0>::index_extension const& ie, layout_t<0>::extensions_type const& /*zero*/)
-> typename layout_t<1>::extensions_type {
	return typename layout_t<1>::extensions_type{std::make_tuple(ie)};
}

inline constexpr auto operator*(extensions_t<1> const& ie, extensions_t<1> const& self) {
	return extensions_t<2>({std::get<0>(ie), std::get<0>(self)});
}

template<class T, class Layout>
constexpr auto sizes_as(Layout const& self)
->decltype(self.template sizes_as<T>()) {
	return self.template sizes_as<T>(); }

}  // end namespace boost::multi

namespace std {
	template<> struct tuple_size<boost::multi::extensions_t<0>> : std::integral_constant<boost::multi::dimensionality_type, 0> {};
	template<> struct tuple_size<boost::multi::extensions_t<1>> : std::integral_constant<boost::multi::dimensionality_type, 1> {};
	template<> struct tuple_size<boost::multi::extensions_t<2>> : std::integral_constant<boost::multi::dimensionality_type, 2> {};
	template<> struct tuple_size<boost::multi::extensions_t<3>> : std::integral_constant<boost::multi::dimensionality_type, 3> {};
	template<> struct tuple_size<boost::multi::extensions_t<4>> : std::integral_constant<boost::multi::dimensionality_type, 4> {};
}  // end namespace std

#endif
