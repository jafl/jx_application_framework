/******************************************************************************
 SVNTabBase.h

	Copyright @ 2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_SVNTabBase
#define _H_SVNTabBase

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
									 const bool includeDeleted = false) = 0;
	virtual void	GetSelectedFilesForDiff(JPtrArray<JString>* fullNameList,
											JArray<JIndex>* revList);
	virtual void	OpenFiles();
	virtual void	ShowFiles();

	virtual bool	GetBaseRevision(JString* rev);
	void				CompareEdited(const JString& rev);
	void				CompareCurrent(const JString& rev = JString::empty);
	void				ComparePrev(const JString& rev = JString::empty);

	virtual bool	ScheduleForAdd();
	virtual bool	ScheduleForRemove();
	virtual bool	ForceScheduleForRemove();
	virtual bool	Resolved();
	virtual void		Commit();
	virtual bool	Revert();
	virtual bool	CreateDirectory();
	virtual bool	DuplicateItem();
	virtual bool	CreateProperty();
	virtual bool	SchedulePropertiesForRemove();
	virtual bool	Ignore();

	virtual bool	CanCheckOutSelection() const;
	virtual void		CheckOutSelection();

private:

	SVNMainDirector*	itsDirector;

private:

	bool	Prepare(JString* cmd, const JUtf8Byte* warnMsgID = nullptr,
						const bool includeDeleted = false);
	bool	Execute(const JString& cmd, const JUtf8Byte* warnMsgID = nullptr,
						const bool includeDeleted = false,
						const bool blocking = true);
	void		Compare(const JString& rev, const bool isPrev);
	bool	ExecuteDiff(const JString& cmd, const JString& rev, const bool isPrev);
	bool	ExecuteJCCDiff(const JString& rev, const bool isPrev);

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
