// Boost.Geometry (aka GGL, Generic Geometry Library)
//
// Copyright Barend Gehrels 2007-2009, Geodan, Amsterdam, the Netherlands.
// Copyright Bruno Lalande 2008, 2009
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_ALGORITHMS_EQUALS_HPP
#define BOOST_GEOMETRY_ALGORITHMS_EQUALS_HPP

/*!
\defgroup equals equals: detect if two geometries are spatially equal
\details Equals returns true if geometries are spatially equal. Spatially equal
means including the same point-set. A linestring can be spatially equal to
another linestring, even if both do not have the same amount of points.
A polygon can be spatially equal to a multi-polygon (which then has
only one element).

\par Geometries:
- \b point + \b point
- \b box + \b box

\note There is a functor \ref compare "compare" as well, which can be used
for std:: library compatibility.

*/


#include <cstddef>
#include <deque>
#include <vector>

#include <boost/mpl/if.hpp>
#include <boost/static_assert.hpp>
#include <boost/range/functions.hpp>
#include <boost/range/metafunctions.hpp>

#include <boost/geometry/core/access.hpp>
#include <boost/geometry/core/coordinate_dimension.hpp>
#include <boost/geometry/core/is_multi.hpp>
#include <boost/geometry/core/reverse_dispatch.hpp>

#include <boost/geometry/geometries/concepts/check.hpp>

#include <boost/geometry/algorithms/detail/disjoint.hpp>
#include <boost/geometry/algorithms/detail/not.hpp>

// For trivial checks
#include <boost/geometry/algorithms/area.hpp>
#include <boost/geometry/algorithms/length.hpp>
#include <boost/geometry/util/math.hpp>
#include <boost/geometry/util/select_coordinate_type.hpp>
#include <boost/geometry/util/select_most_precise.hpp>

#include <boost/geometry/algorithms/detail/equals/collect_vectors.hpp>


namespace boost { namespace geometry
{

#ifndef DOXYGEN_NO_DETAIL
namespace detail { namespace equals
{


template
<
    typename Box1,
    typename Box2,
    std::size_t Dimension,
    std::size_t DimensionCount
>
struct box_box
{
    static inline bool apply(Box1 const& box1, Box2 const& box2)
    {
        if (!geometry::math::equals(get<min_corner, Dimension>(box1), get<min_corner, Dimension>(box2))
            || !geometry::math::equals(get<max_corner, Dimension>(box1), get<max_corner, Dimension>(box2)))
        {
            return false;
        }
        return box_box<Box1, Box2, Dimension + 1, DimensionCount>::apply(box1, box2);
    }
};

template <typename Box1, typename Box2, std::size_t DimensionCount>
struct box_box<Box1, Box2, DimensionCount, DimensionCount>
{
    static inline bool apply(Box1 const& , Box2 const& )
    {
        return true;
    }
};


struct area_check
{
    template <typename Geometry1, typename Geometry2>
    static inline bool apply(Geometry1 const& geometry1, Geometry2 const& geometry2)
    {
        return geometry::math::equals(
                geometry::area(geometry1),
                geometry::area(geometry1));
    }
};


struct length_check
{
    template <typename Geometry1, typename Geometry2>
    static inline bool apply(Geometry1 const& geometry1, Geometry2 const& geometry2)
    {
        return geometry::math::equals(
                geometry::length(geometry1),
                geometry::length(geometry1));
    }
};


template <typename Geometry1, typename Geometry2, typename TrivialCheck>
struct equals_by_collection
{
    static inline bool apply(Geometry1 const& geometry1, Geometry2 const& geometry2)
    {
        if (! TrivialCheck::apply(geometry1, geometry2))
        {
            return false;
        }

        typedef typename geometry::select_most_precise
            <
                typename select_coordinate_type
                    <
                        Geometry1, Geometry2
                    >::type,
                double
            >::type calculation_type;

        typedef std::vector<collected_vector<calculation_type> > v;
        v c1, c2;

        geometry::collect_vectors(c1, geometry1);
        geometry::collect_vectors(c2, geometry2);

        if (boost::size(c1) != boost::size(c2))
        {
            return false;
        }

        // Check where direction is NOT changing

        std::sort(c1.begin(), c1.end());
        std::sort(c2.begin(), c2.end());

        // Just check if these vectors are equal.
        return c1.size() == c2.size()
            && std::equal(c1.begin(), c1.end(), c2.begin());

    }
};


}} // namespace detail::equals
#endif // DOXYGEN_NO_DETAIL


#ifndef DOXYGEN_NO_DISPATCH
namespace dispatch
{

template
<
    typename Tag1, typename Tag2,
    bool IsMulti1, bool IsMulti2,
    typename Geometry1,
    typename Geometry2,
    std::size_t DimensionCount
>
struct equals
{};


template <typename P1, typename P2, std::size_t DimensionCount>
struct equals<point_tag, point_tag, false, false, P1, P2, DimensionCount>
    : geometry::detail::not_
        <
            P1,
            P2,
            detail::disjoint::point_point<P1, P2, 0, DimensionCount>
        >
{};


template <typename Box1, typename Box2, std::size_t DimensionCount>
struct equals<box_tag, box_tag, false, false, Box1, Box2, DimensionCount>
    : detail::equals::box_box<Box1, Box2, 0, DimensionCount>
{};


template <typename Ring1, typename Ring2>
struct equals<ring_tag, ring_tag, false, false, Ring1, Ring2, 2>
    : detail::equals::equals_by_collection
        <
            Ring1, Ring2,
            detail::equals::area_check
        >
{};


template <typename Polygon1, typename Polygon2>
struct equals<polygon_tag, polygon_tag, false, false, Polygon1, Polygon2, 2>
    : detail::equals::equals_by_collection
        <
            Polygon1, Polygon2,
            detail::equals::area_check
        >
{};


template <typename LineString1, typename LineString2>
struct equals<linestring_tag, linestring_tag, false, false, LineString1, LineString2, 2>
    : detail::equals::equals_by_collection
        <
            LineString1, LineString2,
            detail::equals::length_check
        >
{};


template <typename Polygon, typename Ring>
struct equals<polygon_tag, ring_tag, false, false, Polygon, Ring, 2>
    : detail::equals::equals_by_collection
        <
            Polygon, Ring,
            detail::equals::area_check
        >
{};


template <typename Ring, typename Box>
struct equals<ring_tag, box_tag, false, false, Ring, Box, 2>
    : detail::equals::equals_by_collection
        <
            Ring, Box,
            detail::equals::area_check
        >
{};


template <typename Polygon, typename Box>
struct equals<polygon_tag, box_tag, false, false, Polygon, Box, 2>
    : detail::equals::equals_by_collection
        <
            Polygon, Box,
            detail::equals::area_check
        >
{};


template
<
    typename Tag1, typename Tag2,
    bool IsMulti1, bool IsMulti2,
    typename Geometry1,
    typename Geometry2,
    std::size_t DimensionCount
>
struct equals_reversed
{
    static inline bool apply(Geometry1 const& g1, Geometry2 const& g2)
    {
        return equals
            <
                Tag2, Tag1,
                IsMulti2, IsMulti1,
                Geometry2, Geometry1,
                DimensionCount
            >::apply(g2, g1);
    }
};


} // namespace dispatch
#endif // DOXYGEN_NO_DISPATCH


/*!
    \brief Detect if two geometries are spatially equal
    \ingroup equals
    \tparam Geometry1 first geometry type
    \tparam Geometry2 second geometry type
    \param geometry1 first geometry
    \param geometry2 second geometry
    \return true if geometries are spatially equal, else false
 */
template <typename Geometry1, typename Geometry2>
inline bool equals(Geometry1 const& geometry1, Geometry2 const& geometry2)
{
    concept::check_concepts_and_equal_dimensions
        <
            Geometry1 const,
            Geometry2 const
        >();

    return boost::mpl::if_c
        <
            reverse_dispatch<Geometry1, Geometry2>::type::value,
            dispatch::equals_reversed
            <
                typename tag<Geometry1>::type,
                typename tag<Geometry2>::type,
                is_multi<Geometry1>::type::value,
                is_multi<Geometry2>::type::value,
                Geometry1,
                Geometry2,
                dimension<Geometry1>::type::value
            >,
            dispatch::equals
            <
                typename tag<Geometry1>::type,
                typename tag<Geometry2>::type,
                is_multi<Geometry1>::type::value,
                is_multi<Geometry2>::type::value,
                Geometry1,
                Geometry2,
                dimension<Geometry1>::type::value
            >
        >::type::apply(geometry1, geometry2);
}


}} // namespace boost::geometry


#endif // BOOST_GEOMETRY_ALGORITHMS_EQUALS_HPP

