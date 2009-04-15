/******************************************************************************
 GMFilter.cc

	BASE CLASS = <NONE>

	Copyright © 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GMFilter.h>
#include <GMFilterCondition.h>
#include <GMFilterAction.h>

#include <jAssert.h>

const JFileVersion kCurrentSetupVersion	= 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

GMFilter::GMFilter()
{
	GMFilterX();
}

GMFilter::GMFilter
	(
	const GMFilter& source
	)
{
	GMFilterX();
	itsNickname = source.itsNickname;

	const JPtrArray<GMFilterCondition>& conditions = source.GetConditions();
	const JSize ccount = conditions.GetElementCount();
	for (JIndex i = 1; i <= ccount; i++)
		{
		GMFilterCondition* condition = new GMFilterCondition(*(conditions.NthElement(i)));
		assert(condition != NULL);
		itsConditions->Append(condition);
		}

	const JPtrArray<GMFilterAction>& actions = source.GetActions();
	const JSize acount = actions.GetElementCount();
	for (JIndex i = 1; i <= acount; i++)
		{
		GMFilterAction* action = new GMFilterAction(*(actions.NthElement(i)));
		assert(action != NULL);
		itsActions->Append(action);
		}
}

void
GMFilter::GMFilterX()
{
	itsConditions	= new JPtrArray<GMFilterCondition>(JPtrArrayT::kForgetAll);
	assert(itsConditions != NULL);

	itsActions	= new JPtrArray<GMFilterAction>(JPtrArrayT::kForgetAll);
	assert(itsActions != NULL);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMFilter::~GMFilter()
{
	itsConditions->DeleteAll();
	delete itsConditions;

	itsActions->DeleteAll();
	delete itsActions;
}

/******************************************************************************
 Nickname

 ******************************************************************************/

const JString&
GMFilter::GetNickname()
	const
{
	return itsNickname;
}

void
GMFilter::SetNickname
	(
	const JCharacter* nickname
	)
{
	itsNickname	= nickname;
}

/******************************************************************************
 Conditions

 ******************************************************************************/

JPtrArray<GMFilterCondition>*
GMFilter::GetConditions()
{
	return itsConditions;
}

void
GMFilter::SetConditions
	(
	const JPtrArray<GMFilterCondition>& conditions
	)
{
	itsConditions->DeleteAll();
	const JSize count	= conditions.GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		const GMFilterCondition* oldcond	= conditions.NthElement(i);
		GMFilterCondition* newcond			= new GMFilterCondition(*oldcond);
		assert(newcond != NULL);
		itsConditions->Append(newcond);
		}
}

/******************************************************************************
 Conditions

 ******************************************************************************/

JPtrArray<GMFilterAction>*
GMFilter::GetActions()
{
	return itsActions;
}

void
GMFilter::SetActions
	(
	const JPtrArray<GMFilterAction>& actions
	)
{
	itsActions->DeleteAll();
	const JSize count	= actions.GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		const GMFilterAction* oldcond	= actions.NthElement(i);
		GMFilterAction* newcond			= new GMFilterAction(*oldcond);
		assert(newcond != NULL);
		itsActions->Append(newcond);
		}
}

/******************************************************************************
 ReadSetup (public)

 ******************************************************************************/

void
GMFilter::ReadSetup
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

	input >> itsNickname;
	JSize count;
	input >> count;
	for (JIndex i = 1; i <= count; i++)
		{
		GMFilterCondition* condition	= new GMFilterCondition();
		assert(condition != NULL);
		condition->ReadSetup(input);
		itsConditions->Append(condition);
		}
	input >> count;
	for (JIndex i = 1; i <= count; i++)
		{
		GMFilterAction* action	= new GMFilterAction();
		assert(action != NULL);
		action->ReadSetup(input);
		itsActions->Append(action);
		}
}

/******************************************************************************
 WriteSetup (private)

 ******************************************************************************/

void
GMFilter::WriteSetup
	(
	ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion << ' ';
	output << itsNickname << ' ';
	const JSize ccount	= itsConditions->GetElementCount();
	output << ccount << ' ';
	for (JIndex i = 1; i <= ccount; i++)
		{
		GMFilterCondition* condition	= itsConditions->NthElement(i);
		condition->WriteSetup(output);
		}
	const JSize acount	= itsActions->GetElementCount();
	output << acount << ' ';
	for (JIndex i = 1; i <= acount; i++)
		{
		GMFilterAction* action	= itsActions->NthElement(i);
		action->WriteSetup(output);
		}
}
