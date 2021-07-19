#include <iostream>

using std::std::ostream;
using std::std::cout;
using std::std::endl;

struct Point
{
	double x,y;
};

std::ostream& operator<<(std::ostream& output, const Point& pt)
{
	output << pt.x << ' ' << pt.y;
	return output;
}

int main()
{
	Point point = {3,5};
	Point peakPt = {5.2,-7.5};
	double maxgrad = 18.345;

	while (true)
	{
		point.x++;
		peakPt.y++;
		maxgrad+=0.1;
		std::cout << "GetPeakNearPoint: point = " << point << ", peak = " << peakPt << ", grad = " << maxgrad << std::endl;
		std::cout << "MriImage::GetEpiPeak: intenThresh=" << point << ", minThesh=" << maxgrad << std::endl;
		std::cout << "MriImage::fitLVEpi: point " << peakPt << std::endl;
	}

	return 0;
}
