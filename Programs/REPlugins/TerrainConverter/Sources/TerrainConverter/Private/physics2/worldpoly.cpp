/******************************************************************************
BigWorld Technology 
Copyright BigWorld Pty, Ltd.
All Rights Reserved. Commercial in confidence.

WARNING: This computer program is protected by copyright law and international
treaties. Unauthorized use, reproduction or distribution of this program, or
any portion of this program, may result in the imposition of civil and
criminal penalties as provided by law.
******************************************************************************/

/**
 *	@file
 */



#include "worldpoly.hpp"

//#include "cstdmf/debug.hpp"
//#include "cstdmf/vectornodest.hpp"
//#include "math/vector3.hpp"

#ifndef CODE_INLINE
#include "worldpoly.ipp"
#endif

// -----------------------------------------------------------------------------
// Section: WorldPolygon
// -----------------------------------------------------------------------------

/**
 *	@internal
 *	This is a simple helper function used by splitPoly. It returns whether or
 *	not two points are too close.
 */
inline static bool tooClose(const DAVA::Vector3& p1, const DAVA::Vector3& p2)
{
    const float MIN_DIST = 0.001f; // A millimetre
    const float MIN_DIST2 = MIN_DIST * MIN_DIST;

    return (p1 - p2).SquareLength() <= MIN_DIST2;
}

/**
 *	@internal
 *	This is a simple helper function used by splitPoly. It adds a point to the
 *	polygon if it is not too close to the previous point.
 */
inline static int addToPoly(WorldPolygon& poly, const DAVA::Vector3& point)
{
    if (poly.empty() || !tooClose(poly.back(), point))
    {
        poly.push_back(point);

        return 0;
    }

    return 1;
}

/**
 *	@internal
 *	This is a simple helper function used by splitPoly. It removes some
 *	unnecessary points.
 */
inline static void compressPoly(WorldPolygon& poly)
{
    if (poly.size() < 3)
    {
        poly.clear();
        return;
    }

    if (tooClose(poly.front(), poly.back()))
    {
        poly.pop_back();
    }

    if (poly.size() < 3)
    {
        poly.clear();
    }
}

/**
 *	This method splits this polygon into the two polygons based on the input
 *	plane.
 *
 *	@param planeEq		The partitioning plane.
 *	@param frontPoly	The polygon in front of the partitioning plane is put in
 *						here.
 *	@param backPoly		The polygon behind the partitioning plane is put in
 *						here.
 */
void WorldPolygon::split(const PlaneEq& planeEq,
                         WorldPolygon& frontPoly,
                         WorldPolygon& backPoly) const
{
    using namespace DAVA;

    frontPoly.clear();
    backPoly.clear();

    if (this->empty())
        return;

    DVASSERT(this->size() >= 3);

    const Vector3* pPrevPoint = &this->back();
    float prevDist = planeEq.distanceTo(*pPrevPoint);
    bool wasInFront = (prevDist > 0.f);
    int compressions = 0;

    WorldPolygon::const_iterator iter = this->begin();

    while (iter != this->end())
    {
        float currDist = planeEq.distanceTo(*iter);
        bool isInFront = (currDist > 0.f);

        if (isInFront != wasInFront)
        {
            float ratio = fabsf(prevDist / (currDist - prevDist));
            Vector3 cutPoint =
            (*pPrevPoint) + ratio * (*iter - *pPrevPoint);

            compressions += addToPoly(frontPoly, cutPoint);
            compressions += addToPoly(backPoly, cutPoint);

            DVASSERT(fabs(planeEq.distanceTo(cutPoint)) < 0.01f);
        }

        if (isInFront)
        {
            compressions += addToPoly(frontPoly, *iter);
        }
        else
        {
            compressions += addToPoly(backPoly, *iter);
        }

        wasInFront = isInFront;
        pPrevPoint = &(*iter);
        prevDist = currDist;

        iter++;
    }

    //	DVASSERT( this->size() + 4 == frontPoly.size() + backPoly.size() + compressions );

    compressPoly(frontPoly);
    compressPoly(backPoly);
}

/**
 *	This method chops this polygon by the input plane. Only the part of the
 *	polygon in the half-space in front of the plane equation is kept.
 *
 *	@return true if the chopped poly is valid, false if not
 */
// bool WorldPolygon::chop( const PlaneEq & planeEq )
// {
// 	// Early out and clear the poly if we have less than 3 verts
// 	if (size() < 3)
// 	{
// 		clear();
// 		return false;
// 	}
//
// 	// Temporary vector for the chopped points
// 	static VectorNoDestructor< Vector3 > newPoints;
// 	newPoints.clear();
//
// 	// Calculate the first distance to the plane, as we recycle this
// 	float lastDist = planeEq.distanceTo( this->front() );
//
// 	// Iterate over all the points in the polygon
// 	for (uint32 i = 0; i < this->size(); i++)
// 	{
// 		// Grab the last point, if it's in front of the plane
// 		// add it to our list
// 		const Vector3& lastPoint = (*this)[i];
// 		if (lastDist > 0.f)
// 		{
// 			newPoints.push_back( lastPoint );
// 		}
//
// 		// Grab the current point and calculate the distance to the plane
// 		const Vector3& point = (*this)[(i + 1) % size()];
// 		float dist = planeEq.distanceTo( point );
//
// 		// If the current and the last point lie on different sides of the plane,
// 		// create a new point at the intersection point
// 		if ((dist >= 0.f && lastDist < 0.f) ||
// 			(dist < 0.f && lastDist >= 0.f))
// 		{
// 			newPoints.push_back( lastPoint + ( point - lastPoint ) *
// 				(- lastDist / (dist - lastDist) ) );
// 		}
// 		lastDist = dist;
// 	}
//
// 	// Make sure we have enough room for the points
// 	this->resize( newPoints.size() );
// 	iterator dit = begin();
//
// 	// Iterate over all the new points and copy them to the poly list
// 	for (uint32 i = 0; i < newPoints.size(); i++)
// 	{
// 		// If the point is too close to the next point in the list
// 		// don't add it to the list
// 		if (!tooClose(newPoints[i], newPoints[(i+1) % newPoints.size()]))
// 		{
// 			*(dit++) = newPoints[i];
// 		}
// 	}
//
// 	// resize the vector to only contain points that have actually been
// 	// copied across
// 	resize( dit - begin() );
//
// 	return size() >= 3;
// }

// worldtri.cpp
