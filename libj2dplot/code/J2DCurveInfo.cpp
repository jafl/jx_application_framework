/******************************************************************************
 J2DCurveInfo.cpp

	Copyright @ 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <J2DCurveInfo.h>
#include <JString.h>
#include <jAssert.h>

// setup version

const JFileVersion kCurrentSetupVersion = 1;

	// version 1:  removed points

/******************************************************************************
 Stream operators for J2DCurveInfo

 ******************************************************************************/

istream&
operator>>
	(
	istream&		input,
	J2DCurveInfo&	info
	)
{
	JFileVersion vers;
	input >> vers;
	assert( vers <= kCurrentSetupVersion );
	input >> info.show >> info.lines >> info.symbols;

	if (vers == 0)
		{
		JBoolean points;
		input >> points;
		}

	input >> info.own >> info.xerrors >> info.yerrors >> info.protect;
	input >> info.color >> info.symbol >> *(info.name);
	return input;
}

ostream&
operator<<
	(
	ostream&			output,
	const J2DCurveInfo&	info
	)
{
	output << kCurrentSetupVersion;
	output << ' ' << info.show;
	output << ' ' << info.lines;
	output << ' ' << info.symbols;
	output << ' ' << info.own;
	output << ' ' << info.xerrors;
	output << ' ' << info.yerrors;
	output << ' ' << info.protect;
	output << ' ' << info.color;
	output << ' ' << info.symbol;
	output << ' ' << *(info.name);
	return output;
}

/******************************************************************************
 Stream functions for J2DSymbolType

 ******************************************************************************/

istream&
operator>>
	(
	istream&		input,
	J2DSymbolType&	type
	)
{
	long temp;
	input >> temp;
	type = (J2DSymbolType) temp;
	assert( 0 <= type && type < kSymbolCount );
	return input;
}

ostream&
operator<<
	(
	ostream&			output,
	const J2DSymbolType	type
	)
{
	output << (long) type;
	return output;
}
