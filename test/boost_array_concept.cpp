// Copyright 2024 Alfredo A. Correa

// #if not defined(__clang_major__)
// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Walloc-zero"
// #endif

#include <boost/multi_array.hpp>

#include <boost/concept_check.hpp>

#include <boost/test/unit_test.hpp>

#include <multi/array.hpp>

BOOST_AUTO_TEST_CASE(concepts_boost_array) {
	using BMA = boost::multi_array<int, 2>;

	BOOST_CONCEPT_ASSERT((boost::multi_array_concepts::ConstMultiArrayConcept<BMA, 2>));
	BOOST_CONCEPT_ASSERT((boost::multi_array_concepts::MutableMultiArrayConcept<BMA, 2>));
}

namespace multi = boost::multi;

BOOST_AUTO_TEST_CASE(concepts_array) {
	using MA = multi::array<int, 2>;

	// BOOST_CONCEPT_ASSERT((boost::multi_array_concepts::ConstMultiArrayConcept<MA, 2>));
	// BOOST_CONCEPT_ASSERT((boost::multi_array_concepts::MutableMultiArrayConcept<MA, 2>));

	BOOST_CONCEPT_ASSERT((boost::Assignable<MA>));
	BOOST_CONCEPT_ASSERT((boost::SGIAssignable<MA>));
	BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<MA>));
	BOOST_CONCEPT_ASSERT((boost::CopyConstructible<MA>));
	BOOST_CONCEPT_ASSERT((boost::EqualityComparable<MA>));
	BOOST_CONCEPT_ASSERT((boost::LessThanComparable<MA>));

	BOOST_CONCEPT_ASSERT((boost::Generator<MA, boost::multi::subarray<int, 2>>));
	BOOST_CONCEPT_ASSERT((boost::UnaryFunction<MA, MA::reference, MA::index>));
	BOOST_CONCEPT_ASSERT((boost::BinaryFunction<MA, typename MA::element_ref, typename MA::index, typename MA::index>));

	// BOOST_CONCEPT_ASSERT((boost::AdaptableGenerator<MA, boost::multi::subarray<int, 2>>));  // needs result_type TODO(correaa) add to array?, should result_type be array<T, D-1>? or subarray?
	// BOOST_CONCEPT_ASSERT((boost::AdaptableUnaryFunction<MA, MA::reference, MA::index>));
	// BOOST_CONCEPT_ASSERT((boost::AdaptableBinaryFunction<MA, typename MA::element_ref, typename MA::index, typename MA::index>));

	BOOST_CONCEPT_ASSERT((boost::Container<MA>));
}

BOOST_AUTO_TEST_CASE(concepts_iterator) {
	using MAIt = multi::array<int, 2>::iterator;

	BOOST_CONCEPT_ASSERT((boost::Assignable<MAIt>));
	BOOST_CONCEPT_ASSERT((boost::SGIAssignable<MAIt>));
	BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<MAIt>));
	BOOST_CONCEPT_ASSERT((boost::CopyConstructible<MAIt>));
	BOOST_CONCEPT_ASSERT((boost::EqualityComparable<MAIt>));
	BOOST_CONCEPT_ASSERT((boost::LessThanComparable<MAIt>));

	BOOST_CONCEPT_ASSERT((boost::InputIterator<MAIt>));
	BOOST_CONCEPT_ASSERT((boost::OutputIterator<MAIt, MAIt::reference>));
		BOOST_CONCEPT_ASSERT((boost::OutputIterator<MAIt, MAIt::value_type>));

	BOOST_CONCEPT_ASSERT((boost::ForwardIterator<MAIt>));
	BOOST_CONCEPT_ASSERT((boost::Mutable_ForwardIterator<MAIt>));
	BOOST_CONCEPT_ASSERT((boost::BidirectionalIterator<MAIt>));
	BOOST_CONCEPT_ASSERT((boost::Mutable_BidirectionalIterator<MAIt>));
	BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<MAIt>));
	BOOST_CONCEPT_ASSERT((boost::Mutable_RandomAccessIterator<MAIt>));
}

BOOST_AUTO_TEST_CASE(concepts_const_iterator) {
	using MAIt = multi::array<int, 2>::const_iterator;

	BOOST_CONCEPT_ASSERT((boost::Assignable<MAIt>));
	BOOST_CONCEPT_ASSERT((boost::SGIAssignable<MAIt>));
	BOOST_CONCEPT_ASSERT((boost::DefaultConstructible<MAIt>));
	BOOST_CONCEPT_ASSERT((boost::CopyConstructible<MAIt>));
	BOOST_CONCEPT_ASSERT((boost::EqualityComparable<MAIt>));
	BOOST_CONCEPT_ASSERT((boost::LessThanComparable<MAIt>));

	BOOST_CONCEPT_ASSERT((boost::InputIterator<MAIt>));
	// BOOST_CONCEPT_ASSERT((boost::OutputIterator<MAIt, MAIt::reference>));
	//   BOOST_CONCEPT_ASSERT((boost::OutputIterator<MAIt, MAIt::value_type>));

	BOOST_CONCEPT_ASSERT((boost::ForwardIterator<MAIt>));
// 	BOOST_CONCEPT_ASSERT((boost::Mutable_ForwardIterator<MAIt>));
	BOOST_CONCEPT_ASSERT((boost::BidirectionalIterator<MAIt>));
// 	BOOST_CONCEPT_ASSERT((boost::Mutable_BidirectionalIterator<MAIt>));
	BOOST_CONCEPT_ASSERT((boost::RandomAccessIterator<MAIt>));
// 	BOOST_CONCEPT_ASSERT((boost::Mutable_RandomAccessIterator<MAIt>));
}

// #if defined(__clang_major__)
// #pragma GCC diagnostic pop
// #endif
