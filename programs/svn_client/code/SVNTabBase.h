/******************************************************************************
 SVNTabBase.h

	Copyright @ 2008 by John Lindal.  All rights reserved.

 ******************************************************************************/

#ifndef _H_SVNTabBase
#define _H_SVNTabBase

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPtrArray-JString.h>

class JXTextMenu;
class SVNMainDirector;

class SVNTabBase
{

public:

	SVNTabBase(SVNMainDirector* director);

	virtual ~SVNTabBase();

	SVNMainDirector*	GetDirector();
	const JString&		GetPath() const;

	virtual void	UpdateActionsMenu(JXTextMenu* menu) = 0;
	virtual void	UpdateInfoMenu(JXTextMenu* menu) = 0;
	virtual void	RefreshContent() = 0;
	virtual void	GetSelectedFiles(JPtrArray<JString>* fullNameList,
									 const JBoolean includeDeleted = kJFalse) = 0;
	virtual void	GetSelectedFilesForDiff(JPtrArray<JString>* fullNameList,
											JArray<JIndex>* revList);
	virtual void	OpenFiles();
	virtual void	ShowFiles();

	virtual JBoolean	GetBaseRevision(JString* rev);
	void				CompareEdited(const JCharacter* rev);
	void				CompareCurrent(const JCharacter* rev = NULL);
	void				ComparePrev(const JCharacter* rev = NULL);

	virtual JBoolean	ScheduleForAdd();
	virtual JBoolean	ScheduleForRemove();
	virtual JBoolean	ForceScheduleForRemove();
	virtual JBoolean	Resolved();
	virtual void		Commit();
	virtual JBoolean	Revert();
	virtual JBoolean	CreateDirectory();
	virtual JBoolean	DuplicateItem();
	virtual JBoolean	CreateProperty();
	virtual JBoolean	SchedulePropertiesForRemove();
	virtual JBoolean	Ignore();

	virtual JBoolean	CanCheckOutSelection() const;
	virtual void		CheckOutSelection();

private:

	SVNMainDirector*	itsDirector;

private:

	JBoolean	Prepare(JString* cmd, const JCharacter* warnMsgID = NULL,
						const JBoolean includeDeleted = kJFalse);
	JBoolean	Execute(const JCharacter* cmd, const JCharacter* warnMsgID = NULL,
						const JBoolean includeDeleted = kJFalse,
						const JBoolean blocking = kJTrue);
	void		Compare(const JString& rev, const JBoolean isPrev);
	JBoolean	ExecuteDiff(const JCharacter* cmd, const JCharacter* rev, const JBoolean isPrev);
	JBoolean	ExecuteJCCDiff(const JString& rev, const JBoolean isPrev);

	// not allowed

	SVNTabBase(const SVNTabBase& source);
	const SVNTabBase& operator=(const SVNTabBase& source);
};


/******************************************************************************
 GetDirector

 ******************************************************************************/

inline SVNMainDirector*
SVNTabBase::GetDirector()
{
	return itsDirector;
}

#endif
