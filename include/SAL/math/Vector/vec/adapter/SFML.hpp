#ifndef _SRYFV9234F098H467HS20923578HY3Y34N87MRHC8673H0978MYHFHGJ_
#define _SRYFV9234F098H467HS20923578HY3Y34N87MRHC8673H0978MYHFHGJ_

//============================================================================
// SFML sf::Vector2/3 adapter
//
// Mandatory traits (helpers, ctors, accessors etc.) are enforced via the
// VectorDataAdapter concept.
//============================================================================

#include "../base.hpp" // Just the concepts are enough here!

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

#ifdef VEC_DEBUG
# include "../io.hpp"
# ifdef VEC_CPP_IMPORT_STD
   import std;
#  include <assert.h> //! No <cassert> with import std! :-/
# else
#  include <iostream>
#  include <cassert>
# endif
#endif // VEC_DEBUG

namespace VEC_NAMESPACE
{
namespace adapter
{

template <unsigned Dim, Scalar NumT> requires (Dim == 2 || Dim == 3)
struct SFML : public std::conditional_t<Dim == 2, sf::Vector2<NumT>, sf::Vector3<NumT>>
{
	// Helper props used by the vector classes:
	static constinit inline const unsigned dim = Dim;
	using number_type = NumT;

	// Adapter core...
	using foreign_type = std::conditional_t<Dim == 2, sf::Vector2<NumT>, sf::Vector3<NumT>>;
	constexpr       foreign_type& foreign()       { return static_cast<      foreign_type&>(*this); }
	constexpr const foreign_type& foreign() const { return static_cast<const foreign_type&>(*this); }

	// Reuse the original ctors:
	using foreign_type::foreign_type;

	// Alas, the foreign copy ctor can't be reused (it's not inherited), so:
	constexpr SFML(const foreign_type& other) : foreign_type(other)
	{
#ifdef VEC_DEBUG
		std::cerr << "[Adapter created from native object ("<< other.x <<","<< other.y << ")]\n";
#endif
	}

	// The adapter's own copy ctor:
	constexpr SFML(const SFML& other)
#ifndef VEC_DEBUG
		= default;
#else
		: foreign_type(static_cast<foreign_type>(other)) 
	{
		if constexpr (Dim == 2) {
			std::cerr << "[Native object copied (by adapter copy ctor) from {"<< other.x <<","<< other.y << "}:\n";
			std::cerr << " x, y == "<< this->x <<", "<< this->y << "]\n";
		} else {
			std::cerr << "[Native object copied (by adapter copy ctor) from {"<< other.x <<","<< other.y <<","<< other.z << "}:\n";
			std::cerr << " x, y, z == "<< this->x <<", "<< this->y <<", "<< this->z << "]\n";
		}
	}
#endif

	// Comp-time accessors...
	template <unsigned i> requires (i < dim) constexpr
	      NumT& ref()         { if constexpr (i == 0) return this->x; else if constexpr (i == 1) return this->y; else if constexpr (i == 2) return this->z; }
	template <unsigned i> requires (i < dim) constexpr
	const NumT& ref() const   { if constexpr (i == 0) return this->x; else if constexpr (i == 1) return this->y; else if constexpr (i == 2) return this->z; }
	template <unsigned i> requires (i < dim) constexpr
	      NumT  get() const   { if constexpr (i == 0) return this->x; else if constexpr (i == 1) return this->y; else if constexpr (i == 2) return this->z; }
	template <unsigned i> requires (i < dim) constexpr
	auto&       set(NumT val) { if constexpr (i == 0) this->x = val;  else if constexpr (i == 1) this->y = val;  else if constexpr (i == 2) this->z = val; return *this; }

	// Run-time accessors...
	constexpr       NumT& ref(unsigned i)           { assert(i < dim);
		if constexpr (Dim == 2) return i ? this->y : this->x; else return i ? (i==2 ? this->z : this->y) : this->x; }

	constexpr const NumT& ref(unsigned i) const     { assert(i < dim);
		if constexpr (Dim == 2) return i ? this->y : this->x; else return i ? (i==2 ? this->z : this->y) : this->x; }

	constexpr       NumT  get(unsigned i) const     { assert(i < dim);
		if constexpr (Dim == 2) return i ? this->y : this->x; else return i ? (i==2 ? this->z : this->y) : this->x; }

	constexpr auto&       set(unsigned i, NumT val) { assert(i < dim);
		if constexpr (Dim == 2) { if (i==0) this->x = val; else if (i==1) this->y = val; }
		else                    { if (i==0) this->x = val; else if (i==1) this->y = val; else if (i==2) this->z = val; }
		return *this; }
};


} // namespace adapter
} // namespace VEC_NAMESPACE


#endif // _SRYFV9234F098H467HS20923578HY3Y34N87MRHC8673H0978MYHFHGJ_
