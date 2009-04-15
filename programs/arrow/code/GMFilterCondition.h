/******************************************************************************
 GMFilterCondition.h

	Interface for the GMFilterCondition class

	Copyright © 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GMFilterCondition
#define _H_GMFilterCondition

#include <JString.h>

class GMFilterCondition
{
public:

	enum ConditionType
	{
		kFrom = 1,
		kTo,
		kAnyTo,
		kSubject,
		kCC,
		kReplyTo,
		kAnyHeader,
		kBody
	};

	enum ConditionRelation
	{
		kContains = 1,
		kIs,
		kBeginsWith,
		kEndsWith
	};

public:

	GMFilterCondition();
	GMFilterCondition(const ConditionType type,
					  const ConditionRelation relation,
					  const JCharacter* pattern);
	GMFilterCondition(const GMFilterCondition& source);
	virtual ~GMFilterCondition();

	ConditionType		GetType() const;
	void				SetType(const ConditionType type);

	ConditionRelation	GetRelation() const;
	void				SetRelation(const ConditionRelation relation);

	const JString&		GetPattern() const;
	void				SetPattern(const JCharacter* pattern);

	void				ReadSetup(istream& input);
	void				WriteSetup(ostream& output) const;

private:

	ConditionType		itsType;
	ConditionRelation	itsRelation;
	JString				itsPattern;

private:

	// not allowed

	const GMFilterCondition& operator=(const GMFilterCondition& source);
};

#endif
