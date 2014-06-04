// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2007-2012 Barend Gehrels, Amsterdam, the Netherlands.
// Copyright (c) 2008-2012 Bruno Lalande, Paris, France.
// Copyright (c) 2009-2012 Mateusz Loskot, London, UK.

// Parts of Boost.Geometry are redesigned from Geodan's Geographic Library
// (geolib/GGL), copyright (c) 1995-2010 Geodan, Amsterdam, the Netherlands.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_MULTI_ALGORITHMS_REVERSE_HPP
#define BOOST_GEOMETRY_MULTI_ALGORITHMS_REVERSE_HPP


#include <boost/range/metafunctions.hpp>

#include <boost/geometry/algorithms/reverse.hpp>

#include <boost/geometry/core/tags.hpp>
#include <boost/geometry/geometries/concepts/check.hpp>

#include <boost/geometry/multi/algorithms/detail/modify.hpp>

namespace boost { namespace geometry
{


#ifndef DOXYGEN_NO_DISPATCH
namespace dispatch
{


template <typename Geometry>
struct reverse<Geometry, multi_linestring_tag>
    : detail::multi_modify
        <
            Geometry,
            detail::reverse::range_reverse
        >
{};


template <typename Geometry>
struct reverse<Geometry, multi_polygon_tag>
    : detail::multi_modify
        <
            Geometry,
            detail::reverse::polygon_reverse
        >
{};


} // namespace dispatch
#endif // DOXYGEN_NO_DISPATCH


}} // namespace boost::geometry


#endif // BOOST_GEOMETRY_MULTI_ALGORITHMS_REVERSE_HPP
