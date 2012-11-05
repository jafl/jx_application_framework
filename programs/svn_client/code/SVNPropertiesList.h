/******************************************************************************
 SVNPropertiesList.h

	Copyright @ 2008 by John Lindal.  All rights reserved.

 ******************************************************************************/

#ifndef _H_SVNPropertiesList
#define _H_SVNPropertiesList

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include "SVNListBase.h"

class JXGetStringDialog;

class SVNPropertiesList : public SVNListBase
{

public:

	SVNPropertiesList(SVNMainDirector* director, JXTextMenu* editMenu,
					  const JCharacter* fullName,
					  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~SVNPropertiesList();

	virtual void		UpdateActionsMenu(JXTextMenu* menu);
	virtual void		GetSelectedFiles(JPtrArray<JString>* fullNameList,
										 const JBoolean includeDeleted = kJFalse);
	virtual void		OpenSelectedItems();
	virtual JBoolean	CreateProperty();
	virtual JBoolean	SchedulePropertiesForRemove();

protected:

	virtual JBoolean	ShouldDisplayLine(JString* line) const;
	virtual void		StyleLine(const JIndex index, const JString& line,
								  const JFontStyle& errorStyle,
								  const JFontStyle& addStyle,
								  const JFontStyle& removeStyle);
	virtual JString		ExtractRelativePath(const JString& line) const;
	virtual void		CopySelectedItems(const JBoolean fullPath);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	const JString			itsFullName;
	JXGetStringDialog*		itsCreatePropertyDialog;
	JPtrArray<JString>*		itsRemovePropertyCmdList;
	JPtrArray<JProcess>*	itsProcessList;

private:

	static JString	GetCommand(const JCharacter* fullName);

	JBoolean	CreateProperty1();
	JBoolean	RemoveNextProperty();

	// not allowed

	SVNPropertiesList(const SVNPropertiesList& source);
	const SVNPropertiesList& operator=(const SVNPropertiesList& source);
};

#endif
