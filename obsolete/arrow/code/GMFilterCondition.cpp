/******************************************************************************
 GMFilterCondition.cc

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GMFilterCondition.h>

#include <jAssert.h>

const JFileVersion kCurrentSetupVersion	= 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

GMFilterCondition::GMFilterCondition()
	:
	itsType(kFrom),
	itsRelation(kContains)
{
}

GMFilterCondition::GMFilterCondition
	(
	const ConditionType	type,
	const ConditionRelation	relation,
	const JCharacter*		pattern
	)
	:
	itsType(type),
	itsRelation(relation),
	itsPattern(pattern)
{
}

GMFilterCondition::GMFilterCondition
	(
	const GMFilterCondition& source
	)
	:
	itsType(source.itsType),
	itsRelation(source.itsRelation),
	itsPattern(source.itsPattern)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMFilterCondition::~GMFilterCondition()
{
}

/******************************************************************************
 Type (public)

 ******************************************************************************/

GMFilterCondition::ConditionType
GMFilterCondition::GetType()
	const
{
	return itsType;
}

void
GMFilterCondition::SetType
	(
	const ConditionType type
	)
{
	itsType	= type;
}

/******************************************************************************
 Relation (public)

 ******************************************************************************/

GMFilterCondition::ConditionRelation
GMFilterCondition::GetRelation()
	const
{
	return itsRelation;
}

void
GMFilterCondition::SetRelation
	(
	const ConditionRelation relation
	)
{
	itsRelation	= relation;
}

/******************************************************************************
 Pattern (public)

 ******************************************************************************/

const JString&
GMFilterCondition::GetPattern()
	const
{
	return itsPattern;
}

void
GMFilterCondition::SetPattern
	(
	const JCharacter* pattern
	)
{
	itsPattern	= pattern;
}

/******************************************************************************
 ReadSetup (public)

 ******************************************************************************/

void
GMFilterCondition::ReadSetup
	(
	istream& input
	)
{
	JFileVersion version;
	input >> version;
	if (version > kCurrentSetupVersion)
		{
		return;
		}
	int value;
	input >> value;
	itsType	= (ConditionType)value;
	input >> value;
	itsRelation	= (ConditionRelation)value;
	input >> itsPattern;
}

/******************************************************************************
 WriteSetup (private)

 ******************************************************************************/

void
GMFilterCondition::WriteSetup
	(
	ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion << ' ';
	output << (int)itsType << ' ';
	output << (int)itsRelation << ' ';
	output << itsPattern << ' ';
}
