#if defined(_MSC_VER)  &&  !defined(_DEBUG)
#include <iostream>
#include <windows.h>
#include <conio.h>

struct KeepWindowOpenUntilDismissed
{
	~KeepWindowOpenUntilDismissed()
	{
		DWORD pids[1];
		if (GetConsoleProcessList(pids, 1) == 1)
		{
			std::cout << "Press any key to continue . . . ";
			_getch();
		}
	}
} keepWindowOpenUntilDismissed;
#endif


#include "provided.h"
#include "support.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

int main()
{
	Navigator N;
	vector<NavSegment> NN;
	N.loadMapData("mapdata.txt");
	cout << N.navigate("Diddy Riese", "Yamato", NN) << endl;
	double totaldist = 0;
	for (size_t i = 0; i < NN.size(); i++) {
		if (NN[i].m_command == NavSegment::PROCEED) {
			cout << "Proceed " << NN[i].m_distance << " miles " << NN[i].m_direction << " on " << NN[i].m_streetName << endl;
			totaldist += NN[i].m_distance;
		}
		else {
			cout << "Turn " << NN[i].m_direction << " onto " << NN[i].m_streetName << endl;
		}
	}
	cout << "distance traveled: " << totaldist << endl;
}









// This is the BruinNav main routine.  If the executable built from this file
// and the other .cpp files you write is named BruinNav (or BruinNav.exe on
// Windows), then you can run it with
//  ./BruinNav theMapDataFileName "Start Attraction" "End Attraction"
// to get the turn-by-turn instructions a user wants to see, or
//  ./BruinNav theMapDataFileName "Start Attraction" "End Attraction" -raw
// to see the sequence of NavSegments produced by Navigator::navigate()
// (Under Windows, say "BruinNav" instead of "./BruinNav")
// For example, with the mapdata.txt file we supplied you,
//  ./BruinNav mapdata.txt "Harvard-Westlake Middle School" "GreyStone Mansion"
// should produce something like
//   Routing...                     
//   You are starting at: Harvard-Westlake Middle School
//   Proceed 0.47 miles southeast on Brooklawn Drive
//   Turn right onto Angelo Drive
//   Proceed 0.43 miles east on Angelo Drive
//   Turn right onto Benedict Canyon Drive
//   Proceed 0.13 miles southeast on Benedict Canyon Drive
//   Turn left onto Hartford Way
//   Proceed 0.13 miles east on Hartford Way
//   Turn left onto Lexington Road
//   Proceed 0.63 miles east on Lexington Road
//   Turn right onto Alpine Drive
//   Proceed 0.07 miles southeast on Alpine Drive
//   Turn left onto West Sunset Boulevard
//   Proceed 0.20 miles northeast on West Sunset Boulevard
//   Turn left onto Mountain Drive
//   Proceed 0.15 miles northeast on Mountain Drive
//   Turn left onto Schuyler Road
//   Proceed 0.19 miles north on Schuyler Road
//   Turn right onto Stonewood Drive
//   Proceed 0.15 miles northeast on Stonewood Drive
//   You have reached your destination: GreyStone Mansion
//   Total travel distance: 2.5 miles
// and
//  ./BruinNav mapdata.txt "Harvard-Westlake Middle School" "GreyStone Mansion" -raw
// might produce 133 lines starting
//   Start: Harvard-Westlake Middle School
//   End:   GreyStone Mansion
//   34.0904161,-118.4344198 34.0905309,-118.4343340 northeast 0.0093 Brooklawn Drive
//   34.0905309,-118.4343340 34.0904815,-118.4341398 east 0.0116 Brooklawn Drive
//   34.0904815,-118.4341398 34.0903824,-118.4339467 southeast 0.0130 Brooklawn Drive
//   34.0903824,-118.4339467 34.0902310,-118.4337702 southeast 0.0145 Brooklawn Drive
//   34.0902310,-118.4337702 34.0900681,-118.4335630 southeast 0.0163 Brooklawn Drive
//   34.0900681,-118.4335630 34.0899633,-118.4334635 southeast 0.0092 Brooklawn Drive
//   34.0899633,-118.4334635 34.0897917,-118.4333739 southeast 0.0129 Brooklawn Drive
//   34.0897917,-118.4333739 34.0894654,-118.4333087 south 0.0229 Brooklawn Drive
// and ending
//   34.0904163,-118.4036377 34.0905573,-118.4036590 north 0.0098 Schuyler Road
//   34.0905573,-118.4036590 34.0908428,-118.4038080 northwest 0.0215 Schuyler Road
//   turn right Stonewood Drive
//   34.0908428,-118.4038080 34.0908832,-118.4036471 east 0.0096 Stonewood Drive
//   34.0908832,-118.4036471 34.0908732,-118.4034846 east 0.0093 Stonewood Drive
//   34.0908732,-118.4034846 34.0908807,-118.4033732 east 0.0064 Stonewood Drive
//   34.0908807,-118.4033732 34.0909505,-118.4031144 east 0.0156 Stonewood Drive
//   34.0909505,-118.4031144 34.0909630,-118.4030512 east 0.0037 Stonewood Drive
//   34.0909630,-118.4030512 34.0909256,-118.4029338 east 0.0072 Stonewood Drive
//   34.0909256,-118.4029338 34.0919749,-118.4018226 northeast 0.0964 Stonewood Drive
//
// If you build the program as is, you'll notice the turn-by-turn instructions
// say IN_SOME_DIRECTION instead of east or southwest or some actual direction.
// That's because of the template appearing a few lines below; read the comment 
// before it.
