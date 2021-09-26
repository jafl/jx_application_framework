/******************************************************************************
 J2DCurveInfo.cpp

	Copyright @ 1998 by Glenn W. Bach.

 ******************************************************************************/

#include "jx-af/j2dplot/J2DCurveInfo.h"
#include <jx-af/jcore/JString.h>
#include <jx-af/jcore/jAssert.h>

// setup version

const JFileVersion kCurrentSetupVersion = 1;

	// version 1:  removed points

/******************************************************************************
 Stream operators for J2DCurveInfo

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&		input,
	J2DCurveInfo&	info
	)
{
	JFileVersion vers;
	input >> vers;
	assert( vers <= kCurrentSetupVersion );
	input >> JBoolFromString(info.show)
		  >> JBoolFromString(info.lines)
		  >> JBoolFromString(info.symbols);

	if (vers == 0)
	{
		bool points;
		input >> JBoolFromString(points);
	}

	input >> JBoolFromString(info.own)
		  >> JBoolFromString(info.xerrors)
		  >> JBoolFromString(info.yerrors)
		  >> JBoolFromString(info.protect);
	input >> info.color >> info.symbol >> *(info.name);
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&			output,
	const J2DCurveInfo&	info
	)
{
	output << kCurrentSetupVersion;
	output << ' ' << JBoolToString(info.show)
				  << JBoolToString(info.lines)
				  << JBoolToString(info.symbols)
				  << JBoolToString(info.own)
				  << JBoolToString(info.xerrors)
				  << JBoolToString(info.yerrors)
				  << JBoolToString(info.protect);
	output << ' ' << info.color;
	output << ' ' << info.symbol;
	output << ' ' << *(info.name);
	return output;
}

/******************************************************************************
 Stream functions for J2DSymbolType

 ******************************************************************************/

std::istream&
operator>>
	(
	std::istream&		input,
	J2DSymbolType&	type
	)
{
	long temp;
	input >> temp;
	type = (J2DSymbolType) temp;
	assert( 0 <= type && type < kSymbolCount );
	return input;
}

std::ostream&
operator<<
	(
	std::ostream&			output,
	const J2DSymbolType	type
	)
{
	output << (long) type;
	return output;
}
