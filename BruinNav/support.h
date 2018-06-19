#ifndef SUPPORT_H
#define SUPPORT_H

#include "provided.h"
#include <string>
#include <iostream>

inline std::string angleToDirection(const double &a) {
	if (a >= 0 && a <= 22.5)
		return "east";
	if (a > 22.5 && a <= 67.5)
		return "northeast";
	if (a > 67.5 && a <= 112.5)
		return "north";
	if (a > 112.5 && a <= 157.5)
		return "northwest";
	if (a > 157.5 && a <= 202.5)
		return "west";
	if (a > 202.5 && a <= 247.5)
		return "southwest";
	if (a > 247.5 && a <= 292.5)
		return "south";
	if (a > 292.5 && a <= 337.5)
		return "southeast";
	if (a > 337.5 && a < 360)
		return "east";
	return "error";
}

inline double costFunction(const GeoCoord &start, const GeoCoord &end, const GeoCoord &pos) {
	return distanceEarthMiles(start, end) + distanceEarthMiles(pos, end);
}

inline bool operator< (const GeoCoord  &c1, const GeoCoord &c2) {
	if (c1.latitude < c2.latitude)
		return true;
	if (c1.latitude > c2.latitude)
		return false;
	else {
		if (c1.longitude < c2.longitude)
			return true;
		if (c1.longitude > c2.longitude)
			return false;
	}
	return false;
}


inline bool operator== (const GeoCoord  &c1, const GeoCoord &c2) {
	return (c1.latitude == c2.latitude && c1.longitude == c2.longitude);
}

inline bool operator< (const NavSegment &ns1, const NavSegment &ns2) {
	if (ns1.m_distance < ns2.m_distance)
		return true;
	else return false;
}

inline bool operator== (const NavSegment &ns1, const NavSegment &ns2) {
	return (ns1.m_distance == ns2.m_distance);
}

#endif
