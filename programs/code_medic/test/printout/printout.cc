#include <iostream>

using std::ostream;
using std::cout;
using std::endl;

struct Point
{
	double x,y;
};

ostream& operator<<(ostream& output, const Point& pt)
{
	output << pt.x << ' ' << pt.y;
	return output;
}

int main()
{
	Point point = {3,5};
	Point peakPt = {5.2,-7.5};
	double maxgrad = 18.345;

	while (1)
	{
		point.x++;
		peakPt.y++;
		maxgrad+=0.1;
		cout << "GetPeakNearPoint: point = " << point << ", peak = " << peakPt << ", grad = " << maxgrad << endl;
		cout << "MriImage::GetEpiPeak: intenThresh=" << point << ", minThesh=" << maxgrad << endl;
		cout << "MriImage::fitLVEpi: point " << peakPt << endl;
	}

	return 0;
}
