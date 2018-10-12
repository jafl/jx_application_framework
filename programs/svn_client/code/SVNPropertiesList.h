/******************************************************************************
 SVNPropertiesList.h

	Copyright @ 2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_SVNPropertiesList
#define _H_SVNPropertiesList

#include "SVNListBase.h"

class JXGetStringDialog;

class SVNPropertiesList : public SVNListBase
{

public:

	SVNPropertiesList(SVNMainDirector* director, JXTextMenu* editMenu,
					  const JString& fullName,
					  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~SVNPropertiesList();

	virtual void		UpdateActionsMenu(JXTextMenu* menu) override;
	virtual void		GetSelectedFiles(JPtrArray<JString>* fullNameList,
										 const JBoolean includeDeleted = kJFalse) override;
	virtual void		OpenSelectedItems() override;
	virtual JBoolean	CreateProperty() override;
	virtual JBoolean	SchedulePropertiesForRemove() override;

protected:

	virtual JBoolean	ShouldDisplayLine(JString* line) const override;
	virtual void		StyleLine(const JIndex index, const JString& line,
								  const JFontStyle& errorStyle,
								  const JFontStyle& addStyle,
								  const JFontStyle& removeStyle) override;
	virtual JString		ExtractRelativePath(const JString& line) const override;
	virtual void		CopySelectedItems(const JBoolean fullPath) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	const JString			itsFullName;
	JXGetStringDialog*		itsCreatePropertyDialog;
	JPtrArray<JString>*		itsRemovePropertyCmdList;
	JPtrArray<JProcess>*	itsProcessList;

private:

	static JString	GetCommand(const JString& fullName);

	JBoolean	CreateProperty1();
	JBoolean	RemoveNextProperty();

	// not allowed

	SVNPropertiesList(const SVNPropertiesList& source);
	const SVNPropertiesList& operator=(const SVNPropertiesList& source);
};

#endif
