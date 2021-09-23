/*********************************************************************************
 J2DCurveInfo.h

	Copyright @ 1998 by Glenn W. Bach.

 ********************************************************************************/

#ifndef _H_J2DCurveInfo
#define _H_J2DCurveInfo

#include <jTypes.h>

class JString;

// Do not change or reorder these types once they are created
// because they are stored in files.  It is safe to add more
// types, but do not remove any.

enum J2DSymbolType
{
	kCircle = 0,
	kDiamond,
	kSquare,
	kTriangle,
	kPlus,
	kCross,

	kSymbolCount
};

struct J2DCurveInfo
{
	bool			show;
	bool			lines;
	bool			symbols;
	bool			own;
	bool			xerrors;
	bool			yerrors;
	bool			protect;
	JIndex			color;
	J2DSymbolType	symbol;
	JString*		name;

	J2DCurveInfo(const bool s,
				const bool l,
				const bool sym,
				const bool o,
				const bool x,
				const bool y,
				const bool prot,
				const JIndex col,
				const J2DSymbolType symType,
				JString* n)
		:
		show(s), lines(l), symbols(sym),
		own(o), xerrors(x), yerrors(y),
		protect(prot), color(col),
		symbol(symType), name(n)
		{ };

	J2DCurveInfo()
		:
		show(true), lines(true), symbols(false),
		own(false), xerrors(true), yerrors(true),
		protect(false), color(1),
		symbol(kCircle), name(nullptr)
		{ };

	bool
	points() const
	{
		return !lines && !symbols;
	};
};

std::istream& operator>>(std::istream& input, J2DCurveInfo& info);
std::ostream& operator<<(std::ostream& output, const J2DCurveInfo& info);

std::istream& operator>>(std::istream& input, J2DSymbolType& type);
std::ostream& operator<<(std::ostream& output, const J2DSymbolType type);

#endif
