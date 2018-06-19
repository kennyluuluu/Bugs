#include "provided.h"
#include "MyMap.h"
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

class AttractionMapperImpl
{
public:
	AttractionMapperImpl();
	~AttractionMapperImpl();
	void init(const MapLoader& ml);
	bool getGeoCoord(string attraction, GeoCoord& gc) const;
private:
	MyMap<string, GeoCoord> attToCoord;
};

AttractionMapperImpl::AttractionMapperImpl()
{
}

AttractionMapperImpl::~AttractionMapperImpl()
{
}

void AttractionMapperImpl::init(const MapLoader& ml)
{
	StreetSegment a;
	for (size_t i = 0; i < ml.getNumSegments(); i++) {
		ml.getSegment(i, a);
		for (size_t j = 0; j < a.attractions.size(); j++) {
			string lowercased;
			for (size_t k = 0; k < a.attractions[j].name.size(); k++)
				lowercased += tolower(a.attractions[j].name[k]);
			attToCoord.associate(lowercased, a.attractions[j].geocoordinates);
		}
	}
	return;
}

bool AttractionMapperImpl::getGeoCoord(string attraction, GeoCoord& gc) const
{
	string lowercase;
	for (size_t i = 0; i < attraction.size(); i++)
		lowercase += tolower(attraction[i]);
	if (attToCoord.find(lowercase) != nullptr) {
		gc = *(attToCoord.find(lowercase));
		return true;
	}
	return false;
}

//******************** AttractionMapper functions *****************************

// These functions simply delegate to AttractionMapperImpl's functions.
// You probably don't want to change any of this code.

AttractionMapper::AttractionMapper()
{
	m_impl = new AttractionMapperImpl;
}

AttractionMapper::~AttractionMapper()
{
	delete m_impl;
}

void AttractionMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

bool AttractionMapper::getGeoCoord(string attraction, GeoCoord& gc) const
{
	return m_impl->getGeoCoord(attraction, gc);
}
