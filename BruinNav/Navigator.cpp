#include "provided.h"
#include "support.h"
#include "MyMap.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <queue>
#include <list>
#include <stack>
using namespace std;

class NavigatorImpl
{
public:
	NavigatorImpl();
	~NavigatorImpl();
	bool loadMapData(string mapFile);
	NavResult navigate(string start, string end, vector<NavSegment>& directions) const;
private:
	MapLoader ml;
	AttractionMapper am;
	SegmentMapper sm;
};

NavigatorImpl::NavigatorImpl()
{
}

NavigatorImpl::~NavigatorImpl()
{
}

bool NavigatorImpl::loadMapData(string mapFile)
{
	if (ml.load(mapFile)) {
		am.init(ml);
		sm.init(ml);
		return true;
	}
	return false;
}

NavResult NavigatorImpl::navigate(string start, string end, vector<NavSegment> &directions) const
{
	GeoCoord sc;
	GeoCoord ec;
	if (!am.getGeoCoord(start, sc))
		return NAV_BAD_SOURCE;
	if (!am.getGeoCoord(end, ec))
		return NAV_BAD_DESTINATION;
	MyMap<GeoCoord, GeoCoord> parent;
	MyMap<GeoCoord, string> coordToStreetName;
	MyMap<GeoCoord, GeoSegment> coordToSeg;
	vector<GeoCoord> open;
	open.push_back(sc);
	vector<GeoCoord> closed;
	stack<NavSegment> navS;

	vector<StreetSegment> beginningStreet = sm.getSegments(sc);
	coordToStreetName.associate(sc, beginningStreet[0].streetName);
	coordToSeg.associate(sc, GeoSegment(sc, sc));

	for (size_t j = 0; j < beginningStreet[0].attractions.size(); j++) {
		if (beginningStreet[0].attractions[j].geocoordinates == ec) {
			directions.clear();
			GeoSegment x(sc, ec);
			directions.push_back(NavSegment(angleToDirection(angleOfLine(x)), beginningStreet[0].streetName, distanceEarthMiles(sc, ec), x));
			return NAV_SUCCESS;
		}
	}

	vector<StreetSegment> endingStreet = sm.getSegments(ec);
	coordToStreetName.associate(ec, endingStreet[0].streetName);

	while (open.size() > 0) {
		size_t optimal = 0;
		GeoCoord opGC;
		for (size_t i = 0; i < open.size(); i++) {
			optimal = (costFunction(sc,ec, open[optimal]) <= costFunction(sc,ec, open[i])) ? optimal : i;
		}
		opGC = open[optimal];
		closed.push_back(opGC);
		vector<GeoCoord>::iterator it = open.begin();

		open.erase(it + optimal);

		if (!navS.empty())
			NavSegment opGCnav = navS.top();

		vector<StreetSegment> successors = sm.getSegments(opGC);
		for (size_t i = 0; i < successors.size(); i++) {
			for (size_t j = 0; j < successors[i].attractions.size(); j++) {
				if (successors[i].attractions[j].geocoordinates == ec) {
					parent.associate(ec, opGC);
					GeoSegment lastSeg = GeoSegment(opGC, ec);
					coordToSeg.associate(ec, lastSeg);
					GeoCoord currentc = *parent.find(ec);

					double lastAngle = angleOfLine(lastSeg);
					string lastDir = angleToDirection(lastAngle);
					navS.push(NavSegment(lastDir, *coordToStreetName.find(ec), distanceEarthMiles(opGC, ec), lastSeg));
					double prevNavSegAngle = 0;
					while (!(*(parent.find(currentc)) == sc)) {
						NavSegment prevNavSeg = navS.top();
						GeoCoord myParent = *parent.find(currentc);
						GeoSegment temp = *coordToSeg.find(currentc);
						double tempAngle = angleOfLine(temp);
						string tempDir = angleToDirection(tempAngle);
						if (prevNavSeg.m_streetName != *coordToStreetName.find(currentc)) {
							string whichWay;
							double angleBetween = angleBetween2Lines(temp, prevNavSeg.m_geoSegment);
							if (angleBetween <= 180)
								whichWay = "left";
							else
								whichWay = "right";
							NavSegment turn(whichWay, prevNavSeg.m_streetName);
							navS.push(turn);
						}
						NavSegment proceed(tempDir, *coordToStreetName.find(currentc), distanceEarthMiles(myParent,currentc), temp);
						navS.push(proceed);
						currentc = myParent;
						prevNavSegAngle = tempAngle;
					}
					
					NavSegment prevNavSeg = navS.top();
					GeoSegment temp(sc, prevNavSeg.m_geoSegment.start);
					double tempAngle = angleOfLine(temp);
					string tempDir = angleToDirection(tempAngle);
					if (prevNavSeg.m_streetName != *coordToStreetName.find(sc)) {
						string whichWay;
						double angleBetween = angleBetween2Lines(temp, prevNavSeg.m_geoSegment);
						if (angleBetween <= 180)
							whichWay = "left";
						else
							whichWay = "right";
						NavSegment turn(whichWay, prevNavSeg.m_streetName);
						navS.push(turn);
					}
					navS.push(NavSegment(tempDir, beginningStreet[0].streetName, distanceEarthMiles(sc, prevNavSeg.m_geoSegment.start), temp));

					directions.clear();
					while (!navS.empty()) {
						directions.push_back(navS.top());
						navS.pop();
					}

					return NAV_SUCCESS;
				}
			}
			bool test = true;
			for (size_t k = 0; k < open.size(); k++) {
				if (open[k] == successors[i].segment.start)
					test = false;
			}
			for (size_t l = 0; l < closed.size(); l++) {
				if (closed[l] == successors[i].segment.start)
					test = false;
			}
			if (test) {
				open.push_back(successors[i].segment.start);
				GeoSegment t(opGC, successors[i].segment.start);
				coordToStreetName.associate(successors[i].segment.start, successors[i].streetName);
				coordToSeg.associate(successors[i].segment.start, t);
				parent.associate(successors[i].segment.start, opGC);
			}
			bool test2 = true;
			for (size_t k = 0; k < open.size(); k++) {
				if (open[k] == successors[i].segment.end)
					test2 = false;
			}
			for (size_t l = 0; l < closed.size(); l++) {
				if (closed[l] == successors[i].segment.end)
					test2 = false;
			}
			if (test2) {
				open.push_back(successors[i].segment.end);
				GeoSegment t(opGC, successors[i].segment.end);
				coordToStreetName.associate(successors[i].segment.end, successors[i].streetName);
				coordToSeg.associate(successors[i].segment.end, t);
				parent.associate(successors[i].segment.end, opGC);
			}
		}
	}


	return NAV_NO_ROUTE;
}


//******************** Navigator functions ************************************

// These functions simply delegate to NavigatorImpl's functions.
// You probably don't want to change any of this code.

Navigator::Navigator()
{
	m_impl = new NavigatorImpl;
}

Navigator::~Navigator()
{
	delete m_impl;
}

bool Navigator::loadMapData(string mapFile)
{
	return m_impl->loadMapData(mapFile);
}

NavResult Navigator::navigate(string start, string end, vector<NavSegment>& directions) const
{
	return m_impl->navigate(start, end, directions);
}
