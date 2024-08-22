#ifndef _SDF98R5YHG39P58EHUIC3J45WP09R68MYJ98TFMYJBUIO6T4J_
#define _SDF98R5YHG39P58EHUIC3J45WP09R68MYJ98TFMYJBUIO6T4J_


#define VEC_NAMESPACE vec

//#define VEC_NO_IOS // Use before including io.hpp!
//#define VEC_NORMALIZE_NO_DIV0_CHECK
//#define VEC_STRICT_DIRECT_COORDS //!! TBD!
//#define VEC_NO_DIRECT_COORDS //!! Get rid of this; avoid bifurcating the interface!!!
//#define VEC_IMPLICIT_NUM_CONV

#ifdef VEC_CPP_IMPORT_STD
  import std;
# include <assert.h> //! No <cassert> with import std! :-/
#else
# include <utility> // declval
# include <type_traits>
# include <concepts> //!!?? Use is_same_v instead (if possible), if <concepts> is heavier; <type_traits> would be needed later anyway!
# include <cassert>
#endif


namespace VEC_NAMESPACE {


//============================================================================
// Generic helpers...
//============================================================================

// Utility concept to check if T is constructible with N # of ArgT params.:
//!! -> #23, #34, https://github.com/x1ab/flexivec/issues/34#issuecomment-2278866381
template<class T, typename ArgT, unsigned... Is>
constexpr bool is_constructible_helper(std::integer_sequence<unsigned, Is...>) {
	return std::is_constructible_v<T, decltype((void(Is), std::declval<ArgT>()))...>;
}

template<class T, unsigned N, typename ArgT>
concept HasNArgCtor = is_constructible_helper<T, ArgT>(std::make_integer_sequence<unsigned, N>{});

template <class V>
concept copyable_WORKAROUND_ = std::is_copy_constructible_v<V> && std::is_copy_assignable_v<V>;
//!! Because std::copyable didn't fit, failing for different obscure reasons across compilers... :-/
//!! (MSVC: expression not const, GCC: is_constructible/is_convertible depends on itself... Fun.)
//!!	std::copyable<V> // Includes copy_constructible, and movable etc.


//============================================================================
// VECTOR-RELATED CONCEPTS, helpers etc...
//============================================================================

//!! C++ helper traits (split out of the vector class hierarchy only to avoid
//!! a circular dependency problem when instantiating the CRTP base!)
//!! BUT: STILL CAN'T BE USED for the adapters themselves (instantiated WHILE
//!! DEFINING Vector...), due to another "incomplete type" (circ. ref.) error!
template <class V> struct Vector_traits; // Will contain specialized traits for every vector/backendv. type


template <typename N>
concept Scalar = !std::same_as<N, bool>
	&& requires(N n, N m) {
		// Exclude something that only the vec:: vectors have:
		//! Congrats for the amazingly fucked-up `requires { requires !requires{} }` syntax, BTW!... ;-p
		requires !requires { N::dim; }; //!! and/or: n.template get<1>();
	}; // See #12 for details (incl. why not using std-based rules)!


template <class V>
concept HasDirectCoords = // Implying dim >= 2 (as usual):
	   requires(V t) { ++t.x; }
	&& requires(V t) { t.y; };

template <class V>
concept HasCoordFuncs = // Implying dim >= 2 (as usual):
	   requires(V t) { { t.x() }; }
	&& requires(V t) { { t.y() }; };

// Covers both .x and .x():
template <class V>
concept HasCoordAccess = // Implying dim >= 2 (as usual):
	   requires(V t) { { t.x } -> std::same_as<void>; }
	&& requires(V t) { { t.y } -> std::same_as<void>; };

/*!!?? -> #39 etc.
// Classes (structs) with direct/named coord. access (.x, .y, ...)
//
//!! This is not the right minimum for vectors! Too strict:
//!! excludes anything with constructors (e.g. sf::Vector2)!
//!!
//!! Also: Checking for construction in a derived concept with {x, y, ...} or
//!! (x, y, ...) would be pointless:
//!! a) {...} is there for aggregates already (albeit the stupid C++ rule of
//!!    mandating an extra empty {} if there's a(n empty) base would make it
//!!    useless for anything non-trivial...), and
//!! b) the point is unification anyway...
//!!
//!!?? namespace internal {  <-- the ?? is for "...but maybe still useful publicly?"
//
template <class V, unsigned Dim>
concept VectorAggregate = requires { std::is_aggregate_v<V>; }
&& (Dim >= 2 && Dim <= 4 ? requires(V v) {
		{ v.x };// -> std::same_as<typename V::number_type&>;
		{ v.y };// -> std::same_as<typename V::number_type&>;
	} : true)
&& (Dim >= 3 && Dim <= 4 ? requires(V v) {
		{ v.z };// -> std::same_as<typename V::number_type&>;
	} : true)
&& (Dim == 4 ? requires(V v) {
		{ v.w };// -> std::same_as<typename V::number_type&>;
	} : true);
//!!?? } // namespace internal
??!!*/


// Generic ("trivial") vector data (struct)
//----------------------------------------------------------------------------
template <class V>
concept UniformVectorData =
//	copyable_WORKAROUND_<V> && // #52: Disabled for being arbitrarily restrictive.
	requires {
		typename V::number_type;
//!!NOPE:	{ V::dim } -> std::same_as<unsigned>;
//!!NOPE:	{ V::dim } -> std::same_as<const unsigned>;
//!!NOPE:	{ std::decay_t<decltype(V::dim)> } -> std::same_as<unsigned>;
//!!NOPE:	{ std::decay_t<decltype((V::dim))> } -> std::same_as<unsigned>;
		{ V::dim } -> std::same_as<const unsigned&>; //!! I don't want to talk about it... :-o
	}
	//!! Can't put this here, as this would break "atomic constraint must be constant expr" in adapter.hpp...:
	//!!&& std::copyable<V> // Includes copy_constructible!
	&& (V::dim >= 2)
	&& requires(typename V::number_type n, V v, const V cv) {

		// Accessors
		// - Templated:
		requires requires(V v, unsigned i, V::number_type val) {
			//!! Well, yep... Sorry to say, but C++ has cancer:
			{ []<unsigned I>(      V&  v) -> decltype( v.template ref<I>()) {} };
			{ []<unsigned I>(const V& cv) -> decltype(cv.template ref<I>()) {} };
			{ []<unsigned I>(      V&  v) -> decltype( v.template get<I>()) {} };
			{ []<unsigned I>(V& v, V::number_type val) -> decltype(v.template set<I>(val)) {} };
		};
		// - Runtime:
		requires requires(V v, const V cv, unsigned i, V::number_type val) {
			{  v.ref(i) } -> std::same_as<      typename V::number_type&>;
			{ cv.ref(i) } -> std::same_as<const typename V::number_type&>;
			{  v.get(i) } -> std::same_as<      typename V::number_type>;
			{  v.set(i, val) };//!! -> std::same_as<...Not quite: V&>;
	//!!??		{ v[i] } -> std::same_as<typename V::number_type&>; //!! Would need to be spec'd for direct named-coords.!
		};
	};


// Data "plugin" for our own vector class (not adapters)
//----------------------------------------------------------------------------
template <class V>
concept InternalVectorData = UniformVectorData<V>
	&& std::is_trivially_default_constructible_v<V>
	&& std::is_trivially_copyable_v<V>
	//!!?? && std::is_standard_layout_v<V>
;


// "Live" vector (class)
//----------------------------------------------------------------------------
//!!
//!! Split this into a generic part shared by both the "app vector" and the "vector adapter",
//!! and a specific part for app-vectors only!
//!!
template <class V>
concept VectorClass =
	UniformVectorData<V> //!! NOT anything stricter here though, like InternalVectorData<V>!
	&& std::copyable<V> // Includes copy_constructible!
	&& requires {
		typename V::number_type;
		{ Vector_traits<V>::dim };// -> std::convertible_to<unsigned>;
	}
	&& HasNArgCtor<V, V::dim, typename V::number_type>
	&& requires(typename V::number_type n) {
		requires Vector_traits<V>::dim >= 2;

		// Named coord. access at low dimensions, if enabled
#ifndef VEC_NO_DIRECT_COORDS
			requires (Vector_traits<V>::dim == 2 ? requires(V v) {
				{ v.x };// -> std::same_as<typename V::number_type&>;
				{ v.y };// -> std::same_as<typename V::number_type&>;
			} : true);
			requires (Vector_traits<V>::dim == 3 ? requires(V v) {
				{ v.x };// -> std::same_as<typename V::number_type&>;
				{ v.y };// -> std::same_as<typename V::number_type&>;
				{ v.z };// -> std::same_as<typename V::number_type&>;
			} : true);
			requires (Vector_traits<V>::dim == 4 ? requires(V v) {
				{ v.x };// -> std::same_as<typename V::number_type&>;
				{ v.y };// -> std::same_as<typename V::number_type&>;
				{ v.z };// -> std::same_as<typename V::number_type&>;
				{ v.w };// -> std::same_as<typename V::number_type&>;
			} : true);
#endif // VEC_NO_DIRECT_COORDS
	};


// VectorData adapter
//
// Note: must also have proper vector ctor(s), not just accessors etc.!
//----------------------------------------------------------------------------
template <class A>
concept VectorDataAdapter = UniformVectorData<A>
	//!! With C++20 I still couldn't do this (due to interfering e.g. with a
	//!! supposedly unrelated issue of ctor inheritance in templated adapters):
	//!!&& HasNArgCtor<A, A::dim, typename A::number_type>
	&& requires(A a, const A ca) {
		typename A::foreign_type;
		{  a.foreign() } -> std::same_as<      typename A::foreign_type&>;
		{ ca.foreign() } -> std::same_as<const typename A::foreign_type&>;
		// Vector-specifics:
		typename A::number_type;
		{ A::dim };
	};


// Adapter-vector
//----------------------------------------------------------------------------
template <class V>
concept AdapterVector = UniformVectorData<V>
	&& requires {
		typename V::foreign_type; // Normally impl. as something like `using Adapter::foreign_type`.
		typename V::number_type;
		{ Vector_traits<V>::dim };
		requires Vector_traits<V>::dim >= 2;
	} && requires (V v, const V cv) {
		{  v.operator       typename V::foreign_type&() };
		{ cv.operator const typename V::foreign_type&() };
	};


} // namespace VEC_NAMESPACE


#endif // _SDF98R5YHG39P58EHUIC3J45WP09R68MYJ98TFMYJBUIO6T4J_
