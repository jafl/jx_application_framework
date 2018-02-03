/******************************************************************************
 SVNInfoLog.h

	Copyright @ 2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_SVNInfoLog
#define _H_SVNInfoLog

#include "SVNTextBase.h"

class JXTextMenu;

class SVNInfoLog : public SVNTextBase
{
public:

	SVNInfoLog(SVNMainDirector* director, JXTextMenu* editMenu,
			   const JCharacter* fullName, const JCharacter* rev,
			   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	virtual ~SVNInfoLog();

	const JString&	GetFullName() const;

	virtual void		UpdateInfoMenu(JXTextMenu* menu);
	virtual void		GetSelectedFiles(JPtrArray<JString>* fullNameList,
										 const JBoolean includeDeleted = kJFalse);
	virtual JBoolean	GetBaseRevision(JString* rev);

protected:

	virtual JError	StartProcess(JProcess** p, int* outFD);

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	const JString	itsFullName;
	const JString	itsRevision;
	JXTextMenu*		itsContextMenu;		// NULL until first used

private:

	void	Execute(const JCharacter* cmd);

	void	CreateContextMenu();
	void	UpdateContextMenu();
	void	HandleContextMenu(const JIndex index);

	// not allowed

	SVNInfoLog(const SVNInfoLog& source);
	const SVNInfoLog& operator=(const SVNInfoLog& source);
};


/******************************************************************************
 GetFullName

 ******************************************************************************/

inline const JString&
SVNInfoLog::GetFullName()
	const
{
	return itsFullName;
}

#endif
