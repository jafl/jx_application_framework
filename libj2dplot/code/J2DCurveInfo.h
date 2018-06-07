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
	JBoolean		show;
	JBoolean		lines;
	JBoolean		symbols;
	JBoolean		own;
	JBoolean		xerrors;
	JBoolean		yerrors;
	JBoolean		protect;
	JIndex			color;
	J2DSymbolType	symbol;
	JString*		name;

	J2DCurveInfo(const JBoolean s,
				const JBoolean l,
				const JBoolean sym,
				const JBoolean o,
				const JBoolean x,
				const JBoolean y,
				const JBoolean prot,
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
		show(kJTrue), lines(kJTrue), symbols(kJFalse),
		own(kJFalse), xerrors(kJTrue), yerrors(kJTrue),
		protect(kJFalse), color(1),
		symbol(kCircle), name(nullptr)
		{ };

	JBoolean
	points() const
	{
		return JNegate( lines || symbols );
	};
};

std::istream& operator>>(std::istream& input, J2DCurveInfo& info);
std::ostream& operator<<(std::ostream& output, const J2DCurveInfo& info);

std::istream& operator>>(std::istream& input, J2DSymbolType& type);
std::ostream& operator<<(std::ostream& output, const J2DSymbolType type);

#endif
