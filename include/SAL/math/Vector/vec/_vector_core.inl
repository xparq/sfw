#ifndef _BCVVWOF0389Y7G9U8EVFYTHU985C809FV6MU0E985HR7UIB_
#define _BCVVWOF0389Y7G9U8EVFYTHU985C809FV6MU0E985HR7UIB_

//============================================================================
// Impl...
//============================================================================

#ifdef VEC_CPP_IMPORT_STD
  import std;
# include <assert.h> //! No <cassert> with import std!
#else
# include <type_traits> // is_integral
//# include <concepts>    // is_same
# include <utility>     // forward, decay, integer_sequence (not index_seq.: it'd use size_t -> overkill)
# include <cmath>       // sqrt, round
# include <limits>      // epsilon
# include <cassert>
#endif


namespace VEC_NAMESPACE {


// Helpers...
//----------------------------------------------------------------------------
/*
template <UniformVectorData VData, typename F> // https://www.fluentcpp.com/2021/03/05/stdindex_sequence-and-its-improvement-in-c20/
constexpr decltype(auto) for_each_get(VData&& v, F&& f)
{
    return constexpr [] <unsigned... I> (VData&& v, F&& f, std::index_sequence<I...>)
    {
        (f(v.get<I>()), ...);
        return f;
    }(std::forward<VData>(v), std::forward<F>(f), std::make_index_sequence<VData::dim);
}
*/


//----------------------------------------------------------------------------
// Member functions...
//
// See #8 for details (rationale, alternatives, limitations etc.)!
//----------------------------------------------------------------------------

#define Template_Self template <unsigned Dim, Scalar NumT, template <unsigned,typename> class A>
#define Self Vector<Dim, NumT, A>

#define _VEC_MEMBER_FN_(RetT, Name) Template_Self \
	constexpr RetT Self::Name


#define _VEC_MEMBER_OPEQ_VECT_DEF_(Op) \
	_VEC_MEMBER_FN_(Self&, operator Op) (const Self& other_v) { \
		[]<unsigned... I>(auto& v, const auto& _other_v, std::integer_sequence<unsigned, I...>) \
		constexpr { ((v.template ref<I>() Op _other_v.template get<I>()), ...); \
		}(*this, other_v, std::make_integer_sequence<unsigned, Self::dim>{}); \
		return *this; \
	}

#define _VEC_MEMBER_OPEQ_SCALAR_DEF_(Op) \
	_VEC_MEMBER_FN_(Self&, operator Op) (number_type n) { \
		[]<unsigned... I>(auto& v, auto _n, std::integer_sequence<unsigned, I...>) \
		constexpr { ((v.template ref<I>() Op _n), ...); \
		}(*this, n, std::make_integer_sequence<unsigned, Self::dim>{}); \
		return *this; \
	}

// Loop helpers:
#define _VEC_FOR_EACH_COORD_(Expr) \
	[]<unsigned... I>(auto& v, std::integer_sequence<unsigned, I...>) \
	constexpr { Expr; } (*this, std::make_integer_sequence<unsigned, Self::dim>{})

#define _VEC_FOR_EACH_COORD_CONST_(Expr) \
	[]<unsigned... I>(const auto& v, std::integer_sequence<unsigned, I...>) \
	constexpr { Expr; } (*this, std::make_integer_sequence<unsigned, Self::dim>{})


_VEC_MEMBER_OPEQ_VECT_DEF_(+=)
_VEC_MEMBER_OPEQ_VECT_DEF_(-=)
_VEC_MEMBER_OPEQ_VECT_DEF_(*=)
_VEC_MEMBER_OPEQ_VECT_DEF_(/=)

_VEC_MEMBER_OPEQ_SCALAR_DEF_(+=)
_VEC_MEMBER_OPEQ_SCALAR_DEF_(-=)
_VEC_MEMBER_OPEQ_SCALAR_DEF_(*=)
_VEC_MEMBER_OPEQ_SCALAR_DEF_(/=)

_VEC_MEMBER_FN_(, operator bool) () const // No ret. type for conv. ops!
{
	return _VEC_FOR_EACH_COORD_CONST_(
		return ((bool)v.template get<I>() || ...);
	);
}


_VEC_MEMBER_FN_(auto, length_squared) () const
{
	return _VEC_FOR_EACH_COORD_CONST_(
		return ((v.template get<I>() * v.template get<I>()) + ...)
	);
}

_VEC_MEMBER_FN_(auto, length) () const
{
	return std::sqrt(length_squared());
}

_VEC_MEMBER_FN_(Self&, normalize) ()
{
	static_assert(!std::is_integral_v<typename Self::number_type>, "Normalizing integer vectors is unsupported!");

#ifdef VEC_NORMALIZE_NO_DIV0_CHECK
	return *this /= length();
#else
	if (auto len = length())
		*this /= length();
	return *this;
#endif
}

_VEC_MEMBER_FN_(Self, normalized) () const
{
	return Self(*this).normalize();
}

_VEC_MEMBER_FN_(Self&, round) ()
{
	if constexpr (!std::is_integral_v<typename Self::number_type>) {
		_VEC_FOR_EACH_COORD_(
			((v.template ref<I>() = std::round(v.template get<I>())), ...)
		);
	}
	return *this;
}

_VEC_MEMBER_FN_(Self, rounded) () const
{
	return Self(*this).round();
}


#undef _VEC_FOR_EACH_COORD_CONST_
#undef _VEC_FOR_EACH_COORD_
#undef _VEC_MEMBER_OPEQ_SCALAR_DEF_
#undef _VEC_MEMBER_OPEQ_VECT_DEF_
#undef _VEC_MEMBER_FN_
#undef Self
#undef Template_Self


//----------------------------------------------------------------------------
// Free functions...
//
// See #8 for details (rationale, alternatives, limitations etc.)!
//----------------------------------------------------------------------------

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define _VEC_STANDALONE_BIVECT_OP_DEF_(Op) _VEC_STANDALONE_BIVECT_FN_(operator Op) { \
		using ResultT = V1; \
		return []<unsigned... I>(const auto& _v1, const auto& _v2, std::integer_sequence<unsigned, I...>) \
		constexpr { \
			return ResultT(_v1.template get<I>() Op _v2.template get<I>()...); \
		}(v1, v2, std::make_integer_sequence<unsigned, V1::dim>{}); \
	}
#define _VEC_STANDALONE_VECT_SCALAR_OP_DEF_(Op) _VEC_STANDALONE_VECT_SCALAR_FN_(operator Op) \
                                                _VEC_STANDALONE_VECT_SCALAR_OP_IMPL_(Op)
#define _VEC_STANDALONE_SCALAR_VECT_OP_DEF_(Op) _VEC_STANDALONE_SCALAR_VECT_FN_(operator Op) \
                                                _VEC_STANDALONE_VECT_SCALAR_OP_IMPL_(Op)
#define _VEC_STANDALONE_VECT_SCALAR_OP_IMPL_(Op) { \
		return []<unsigned... I>(const auto& _v, auto _n, std::integer_sequence<unsigned, I...>) \
		constexpr { return V(_v.template get<I>() Op _n...); \
		}(v, n, std::make_integer_sequence<unsigned, V::dim>{}); \
	}

_VEC_STANDALONE_BIVECT_FN_(operator ==) {
// -> #60: Compilations have been ridden with landmines
//         without finally having this explicitly defined.
	using Common = std::common_type<typename V1::number_type, typename V2::number_type>;
	static_assert(!std::is_void_v<Common>, "Trying to compare incompatible number types!");
	return []<unsigned... I>(const auto& _v1, const auto& _v2, std::integer_sequence<unsigned, I...>)
	constexpr {
		if constexpr (std::is_integral_v<typename Common::type>) {
			return ((_v1.template get<I>() == _v2.template get<I>()) && ...);
		} else {
			//static_assert(false, "Attempting `a == b` with floating-point type(s)!");
			constexpr auto Epsilon = std::numeric_limits<typename Common::type>::epsilon(); //!! Common::type is not the correct choice, though! :-/
			return ((std::abs(_v1.template get<I>() - _v2.template get<I>()) <= Epsilon) && ...);
		}
	}(v1, v2, std::make_integer_sequence<unsigned, V1::dim>{});
}

_VEC_STANDALONE_BIVECT_OP_DEF_(+)
_VEC_STANDALONE_BIVECT_OP_DEF_(-)
_VEC_STANDALONE_BIVECT_OP_DEF_(*)
_VEC_STANDALONE_BIVECT_OP_DEF_(/)

_VEC_STANDALONE_VECT_SCALAR_OP_DEF_(+)
_VEC_STANDALONE_VECT_SCALAR_OP_DEF_(*)
_VEC_STANDALONE_SCALAR_VECT_OP_DEF_(+)
_VEC_STANDALONE_SCALAR_VECT_OP_DEF_(*)

_VEC_STANDALONE_VECT_SCALAR_OP_DEF_(-)
_VEC_STANDALONE_VECT_SCALAR_OP_DEF_(/)

_VEC_STANDALONE_BIVECT_FN_(dot) {
	return []<unsigned... I>(const auto& _v1, const auto& _v2, std::integer_sequence<unsigned, I...>)
	constexpr {
		return ((_v1.template get<I>() * _v2.template get<I>()) + ...);
	}(v1, v2, std::make_integer_sequence<unsigned, V1::dim>{});
}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#undef _VEC_STANDALONE_VECT_SCALAR_OP_IMPL_
#undef _VEC_STANDALONE_SCALAR_VECT_OP_DEF_
#undef _VEC_STANDALONE_VECT_SCALAR_OP_DEF_
#undef _VEC_STANDALONE_BIVECT_OP_DEF_


} // namespace VEC_NAMESPACE


#endif // _BCVVWOF0389Y7G9U8EVFYTHU985C809FV6MU0E985HR7UIB_
