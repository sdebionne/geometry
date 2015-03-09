// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2008-2015 Bruno Lalande, Paris, France.
// Copyright (c) 2007-2015 Barend Gehrels, Amsterdam, the Netherlands.
// Copyright (c) 2009-2015 Mateusz Loskot, London, UK.
// Copyright (c) 2015 Samuel Debionne, Grenoble, France.

// This file was modified by Oracle on 2015.
// Modifications copyright (c) 2015, Oracle and/or its affiliates.

// Contributed and/or modified by Menelaos Karavelas, on behalf of Oracle

// Parts of Boost.Geometry are redesigned from Geodan's Geographic Library
// (geolib/GGL), copyright (c) 1995-2010 Geodan, Amsterdam, the Netherlands.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_GEOMETRY_ALGORITHMS_NOT_IMPLEMENTED_HPP
#define BOOST_GEOMETRY_ALGORITHMS_NOT_IMPLEMENTED_HPP


#include <exception>
#include <string>

#include <boost/mpl/assert.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/identity.hpp>

#include <boost/type_traits/is_base_of.hpp>

#include <boost/geometry/core/tags.hpp>


namespace boost { namespace geometry
{


namespace info
{
    struct UNRECOGNIZED_GEOMETRY_TYPE {};
    struct POINT {};
    struct LINESTRING {};
    struct POLYGON {};
    struct RING {};
    struct BOX {};
    struct SEGMENT {};
    struct MULTI_POINT {};
    struct MULTI_LINESTRING {};
    struct MULTI_POLYGON {};
    struct GEOMETRY_COLLECTION {};
    template <size_t D> struct DIMENSION {};
}


/// A base class for every algorithm tags
/// Derived class must implement as static function
/// const char* what();
/// that return a textual information on the algorithm.
///
/// Example :
///
/// struct within_tag : algorithm_tag
/// {
///     static const char* what() { return "within"; }
/// };
struct algorithm_tag {};

/// Exception class thrown when a functionnality is not implemented
/// for the given argument types
struct not_implemented_runtime_error : std::runtime_error
{
    explicit not_implemented_runtime_error(const std::string& what)
        : std::runtime_error(what) {}
    explicit not_implemented_runtime_error(const char* what)
        : std::runtime_error(what) {}
};


#ifndef BOOST_GEOMETRY_IMPLEMENTATION_STATUS_BUILD
# define BOOST_GEOMETRY_IMPLEMENTATION_STATUS_BUILD 0
#endif

/// Use this policy or BOOST_GEOMETRY_NYI_RUNTIME_ERROR to
/// specify whether the algorithm should report invalid parameter
/// type at compile (false) or runtime (true)
template <class T>
struct not_implemented_policy :
#if BOOST_GEOMETRY_IMPLEMENTATION_STATUS_BUILD
    boost::mpl::true_
#else
    boost::mpl::false_
#endif
{};


namespace nyi {

struct not_implemented_tag {};


/// A fake result type used to 'implement' archetypes of not implemented
/// algorithm / strategies...
struct not_implemented_result
{
    //TODO add support for compilers that do not support variadic template
    template <typename... Args>
    not_implemented_result(Args...) {}

    template <typename T>
    operator T()
    {
        // This will never actually happen - we just need to write something
        // that hopefully all compilers are happy with (ideally without warnings)
        // and doesn't assume anything about T
        BOOST_ASSERT(false);
        
        T* t = NULL;
        return *t;
    }
};


/// Archetype of a result_handler
struct not_implemented_result_handler
{
    typedef not_implemented_result result_type;

    //TODO add support for compilers that do not support variadic template
    template <typename... Args>
    not_implemented_result_handler(Args...) {}

    not_implemented_result handle() const
    {
        return not_implemented_result();
    }
};

/// Archetype of an algorithm / strategy
/// The Tag template parameter enables to specify the runtime / compile time
/// policy on a per alogorithm basis and gives information on the not implemented
/// algorithms.
/// 
/// Compile time : sqtatic assert (default)
/// Runtime time : throw not_implemented_runtime_error
template
<
    typename Tag,
    typename Term1,
    typename Term2,
    typename Term3
>
struct not_implemented_error
{
    /// Use not_implemented_policy to decide whether static assert or not
    BOOST_MPL_ASSERT_MSG
        (
            not_implemented_policy<Tag>::value,
            THIS_OPERATION_IS_NOT_OR_NOT_YET_IMPLEMENTED,
            (
                types<Term1, Term2, Term3>
            )
        );

    //TODO add support for compilers that do not support variadic template
    template <typename... Args>
    static not_implemented_result apply(Args...)
    {
        std::string what(Tag::what());
        what += " is not implemented for the given parameters";
        throw not_implemented_runtime_error(what);
    }
};

/// Legacy not_implemented_error
template
<
    typename Term1,
    typename Term2,
    typename Term3
>
struct not_implemented_error<void, Term1, Term2, Term3>
{
    BOOST_MPL_ASSERT_MSG
        (
            BOOST_GEOMETRY_IMPLEMENTATION_STATUS_BUILD,
            THIS_OPERATION_IS_NOT_OR_NOT_YET_IMPLEMENTED,
            (
                types<Term1, Term2, Term3>
            )
        );

    //TODO add support for compilers that do not support variadic template
    template <typename... Args>
    static not_implemented_result apply(Args...)
    {
        throw not_implemented_runtime_error();
    }
};

template <typename Tag>
struct tag_to_term
{
    typedef Tag type;
};

template <> struct tag_to_term<geometry_not_recognized_tag> { typedef info::UNRECOGNIZED_GEOMETRY_TYPE type; };
template <> struct tag_to_term<point_tag>                   { typedef info::POINT type; };
template <> struct tag_to_term<linestring_tag>              { typedef info::LINESTRING type; };
template <> struct tag_to_term<polygon_tag>                 { typedef info::POLYGON type; };
template <> struct tag_to_term<ring_tag>                    { typedef info::RING type; };
template <> struct tag_to_term<box_tag>                     { typedef info::BOX type; };
template <> struct tag_to_term<segment_tag>                 { typedef info::SEGMENT type; };
template <> struct tag_to_term<multi_point_tag>             { typedef info::MULTI_POINT type; };
template <> struct tag_to_term<multi_linestring_tag>        { typedef info::MULTI_LINESTRING type; };
template <> struct tag_to_term<multi_polygon_tag>           { typedef info::MULTI_POLYGON type; };
template <> struct tag_to_term<geometry_collection_tag>     { typedef info::GEOMETRY_COLLECTION type; };
template <int D> struct tag_to_term<boost::mpl::int_<D> >   { typedef info::DIMENSION<D> type; };


} // namespace nyi


template
<
    typename Term1 = void,
    typename Term2 = void,
    typename Term3 = void,
    typename Term4 = void
>
struct not_implemented
    : nyi::not_implemented_tag,
      /// For backward compability
      /// Switch between the old and the new not_implemented_error implementation
      /// depend on the first template parameter (whether is an algorithm tag or not)
      boost::mpl::if_
      <
        boost::is_base_of<algorithm_tag, Term1>,
        nyi::not_implemented_error
        <
            Term1,
            typename boost::mpl::identity<typename nyi::tag_to_term<Term2>::type>::type,
            typename boost::mpl::identity<typename nyi::tag_to_term<Term3>::type>::type,
            typename boost::mpl::identity<typename nyi::tag_to_term<Term4>::type>::type
        >,
        nyi::not_implemented_error
        <
            void,
            typename boost::mpl::identity<typename nyi::tag_to_term<Term1>::type>::type,
            typename boost::mpl::identity<typename nyi::tag_to_term<Term2>::type>::type,
            typename boost::mpl::identity<typename nyi::tag_to_term<Term3>::type>::type
        >
      >::type
{};


}} // namespace boost::geometry


/// Register an algorithm (i.e. define an algorithm tag)
#define BOOST_GEOMETRY_REGISTER_ALGORITHM(Algo) \
namespace boost { namespace geometry { \
    struct Algo##_tag : algorithm_tag{ \
        static const char* what() { return #Algo; } \
    }; \
}}

/// Enable not implemented runtime errors for the given algorithm
#define BOOST_GEOMETRY_NYI_RUNTIME_ERROR(Tag) \
namespace boost { namespace geometry { \
    template<> \
    struct boost::geometry::not_implemented_policy<Tag> \
        : boost::mpl::true_{}; \
}}


#endif // BOOST_GEOMETRY_ALGORITHMS_NOT_IMPLEMENTED_HPP
