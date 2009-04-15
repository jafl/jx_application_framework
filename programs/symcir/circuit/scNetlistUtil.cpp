/******************************************************************************
 scNetlistUtil.cpp

	These routines help when parsing Spice netlist files.

	Copyright © 1995 John Lindal. All rights reserved.

 ******************************************************************************/

#include <scStdInc.h>
#include "scNetlistUtil.h"
#include <string.h>

/******************************************************************************
 GetSpiceMultiplier

 ******************************************************************************/

JFloat
GetSpiceMultiplier
	(
	const JCharacter* type
	)
{
	JFloat mult = 1.0;
	if (strcmp(type,"F")==0)
		{
		mult = 1e-15;
		}
	else if (strcmp(type,"P")==0)
		{
		mult = 1e-12;
		}
	else if (strcmp(type,"N")==0)
		{
		mult = 1e-9;
		}
	else if (strcmp(type,"U")==0)
		{
		mult = 1e-6;
		}
	else if (strcmp(type,"M")==0)
		{
		mult = 1e-3;
		}
	else if (strcmp(type,"K")==0)
		{
		mult = 1e+3;
		}
	else if (strcmp(type,"MEG")==0)
		{
		mult = 1e+6;
		}
	else if (strcmp(type,"G")==0)
		{
		mult = 1e+9;
		}
	else if (strcmp(type,"T")==0)
		{
		mult = 1e+12;
		}
	return mult;
}
