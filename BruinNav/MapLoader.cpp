#include "provided.h"
#include <string>
#include <fstream>
#include <vector>
using namespace std;

class MapLoaderImpl
{
public:
	MapLoaderImpl();
	~MapLoaderImpl();
	bool load(string mapFile);
	size_t getNumSegments() const;
	bool getSegment(size_t segNum, StreetSegment& seg) const;
private:
	vector<StreetSegment> m_streets;
};

MapLoaderImpl::MapLoaderImpl()
{
}

MapLoaderImpl::~MapLoaderImpl()
{
}

bool MapLoaderImpl::load(string mapFile)
{
	ifstream Streets("mapdata.txt");
	if (!Streets)
		return false;									 // Loading failed
	string s;
	string lat;
	string lon;
	int numAtt = 0;
	while (getline(Streets, s)) {
		StreetSegment tempSS;
		tempSS.streetName = s;
		getline(Streets, s);
		int i = 0;
		while (s[i] != ',') {
			lat += s[i];
			i++;
		}
		i += 2;
		while (s[i] != ' ') {
			lon += s[i];
			i++;
		}
		GeoCoord coord1(lat, lon);
		lat = "";
		lon = "";
		i++;
		while (s[i] != ',') {
			lat += s[i];
			i++;
		}
		i++;
		while (i != s.size() - 1) {
			lon += s[i];
			i++;
		}
		lon += s[i];
		GeoCoord coord2(lat, lon);
		lat = "";
		lon = "";
		GeoSegment tempSeg(coord1, coord2);
		tempSS.segment = tempSeg;
		getline(Streets, s);
		numAtt = stoi(s);
		for (; numAtt > 0; numAtt--)
		{
			Attraction tempA;
			string aName;
			getline(Streets, s);
			int g = 0;
			while (true) {
				if (s[g] == '|')
					break;
				aName.push_back(s[g]);
				g++;
			}
			tempA.name = aName;
			g++;
			while (s[g] != ',') {
				lat += s[g];
				g++;
			}
			g += 2;
			while (g != s.size() - 1) {
				lon += s[g];
				g++;
			}
			lon += s[g];
			GeoCoord aCoord(lat, lon);
			lat = "";
			lon = "";
			tempA.geocoordinates = aCoord;
			tempSS.attractions.push_back(tempA);
		}
		m_streets.push_back(tempSS);
	}


	return true;
}

size_t MapLoaderImpl::getNumSegments() const
{
	return m_streets.size();
}

bool MapLoaderImpl::getSegment(size_t segNum, StreetSegment &seg) const
{
	if (segNum < 0 || segNum >= m_streets.size())
		return false;
	seg = m_streets[segNum];
	return true;
}

//******************** MapLoader functions ************************************

// These functions simply delegate to MapLoaderImpl's functions.
// You probably don't want to change any of this code.

MapLoader::MapLoader()
{
	m_impl = new MapLoaderImpl;
}

MapLoader::~MapLoader()
{
	delete m_impl;
}

bool MapLoader::load(string mapFile)
{
	return m_impl->load(mapFile);
}

size_t MapLoader::getNumSegments() const
{
	return m_impl->getNumSegments();
}

bool MapLoader::getSegment(size_t segNum, StreetSegment& seg) const
{
	return m_impl->getSegment(segNum, seg);
}
