#pragma once
#include "glm/glm.hpp"

using namespace glm;
class GeomUtils
{
	public:

	static float Distance(const vec3 &p0, const vec3 &p1)
	{
		return length(p0-p1);
	}

	static vec3 NearestPointOnInfLine(const vec3 &rLine, const vec3 &nLine, const vec3 &point)
	{
		return rLine + nLine*dot(nLine, point-rLine);
	}

	static float PointToInfLineDistance(const vec3 &rLine, const vec3 &nLine, const vec3 &point)
	{
		return Distance( point, NearestPointOnInfLine( rLine, nLine, point));
	}

	static vec3 NearestPointOnFiniteLine(const vec3 &rLine0, const vec3 &rLine1, const vec3 &point)
	{
		vec3 nLine = rLine1 - rLine0;
		float dSq = dot(nLine, nLine);
		float l = dot(point - rLine0, rLine1 - rLine0);
		if (l < 0.0f) l = 0.0f;
		if (l > dSq) l = dSq;

		return rLine0 + l*nLine/dSq;
	}

	static float PointToFiniteLineDistance(const vec3 &rLine0, const vec3 &rLine1, const vec3 &point)
	{
		return Distance( point, NearestPointOnFiniteLine(rLine0, rLine1, point) );
	}

	static vec3 NearestPointOnPlane(const vec3& rPlane, const vec3& nPlane, const vec3& point)
	{
		return point - nPlane * dot(point - rPlane, nPlane);
	}

	static float PointToPlaneDistance(const vec3& rPlane, const vec3& nPlane, const vec3& point)
	{
		return Distance(point, NearestPointOnPlane(rPlane, nPlane, point));
	}

	static vec3 NearestPointOnQuad(const vec3& rQuad, const vec3& aQuad, const vec3& bQuad, const vec3& point)
	{
		// rQuad - corner of quad
		// aQuad, bQaud - vectors along perpendicular edges
		// point - point to be tested
		float la = length(aQuad);
		float lb = length(bQuad);
		vec3 aHat = aQuad / la;
		vec3 bHat = bQuad / lb;
		float x, y; // - x, y coords in the frame of the quad
		x = dot(point - rQuad, aHat);
		y = dot(point - rQuad, bHat);
		if (x < 0.0f) x = 0.0f;
		if (y < 0.0f) y = 0.0f;
		if (x > la) x = la;
		if (y > lb) y = lb;
		return rQuad + x * aHat + y * bHat;
	}

	static float PointToQuadDistance(const vec3& rQuad, const vec3& aQuad, const vec3& bQuad, const vec3& point)
	{
		return Distance(point, NearestPointOnQuad(rQuad, aQuad, bQuad, point));
	}
};
