#include "provided.h"
#include "MyMap.h"
#include <vector>
using namespace std;

class SegmentMapperImpl
{
public:
	SegmentMapperImpl();
	~SegmentMapperImpl();
	void init(const MapLoader& ml);
	vector<StreetSegment> getSegments(const GeoCoord& gc) const;
private:
	MyMap<GeoCoord,vector<StreetSegment>> map;
};

SegmentMapperImpl::SegmentMapperImpl()
{
}

SegmentMapperImpl::~SegmentMapperImpl()
{
}

void SegmentMapperImpl::init(const MapLoader& ml)
{
	StreetSegment a;
	for (size_t i = 0; i < ml.getNumSegments(); i++) {									// associate ALL geocoords with empty vector
		vector<StreetSegment> v;
		ml.getSegment(i, a);
		v.push_back(a);
		if (map.find(a.segment.start) == nullptr)
			map.associate(a.segment.start, v);											//map.associate(a.segment.start, v);
		else {
			vector<StreetSegment> t = *map.find(a.segment.start);
			t.push_back(a);
			map.associate(a.segment.start, t);
		}

		if (map.find(a.segment.end) == nullptr)							//map.associate(a.segment.end, v);
			map.associate(a.segment.end, v);
		else {
			vector<StreetSegment> t = *map.find(a.segment.end);
			t.push_back(a);
			map.associate(a.segment.end, t);
		}

		for (size_t j = 0; j < a.attractions.size(); j++) {
			map.associate(a.attractions[j].geocoordinates, v);				//map.associate(a.attractions[j].geocoordinates, v);
		}
	}
}

vector<StreetSegment> SegmentMapperImpl::getSegments(const GeoCoord& gc) const
{
	return *(map.find(gc));
}

//******************** SegmentMapper functions ********************************

// These functions simply delegate to SegmentMapperImpl's functions.
// You probably don't want to change any of this code.

SegmentMapper::SegmentMapper()
{
	m_impl = new SegmentMapperImpl;
}

SegmentMapper::~SegmentMapper()
{
	delete m_impl;
}

void SegmentMapper::init(const MapLoader& ml)
{
	m_impl->init(ml);
}

vector<StreetSegment> SegmentMapper::getSegments(const GeoCoord& gc) const
{
	return m_impl->getSegments(gc);
}
