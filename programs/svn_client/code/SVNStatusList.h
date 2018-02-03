/******************************************************************************
 SVNStatusList.h

	Copyright @ 2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_SVNStatusList
#define _H_SVNStatusList

#include "SVNListBase.h"

class SVNStatusList : public SVNListBase
{

public:

	SVNStatusList(SVNMainDirector* director, JXTextMenu* editMenu,
				  JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				  const HSizingOption hSizing, const VSizingOption vSizing,
				  const JCoordinate x, const JCoordinate y,
				  const JCoordinate w, const JCoordinate h);

	virtual ~SVNStatusList();

	virtual void		UpdateActionsMenu(JXTextMenu* menu);
	virtual JBoolean	Ignore();

protected:

	virtual void	StyleLine(const JIndex index, const JString& line,
							  const JFontStyle& errorStyle,
							  const JFontStyle& addStyle,
							  const JFontStyle& removeStyle);
	virtual JString	ExtractRelativePath(const JString& line) const;

	virtual void	UpdateContextMenu(JXTextMenu* menu);

private:

	// not allowed

	SVNStatusList(const SVNStatusList& source);
	const SVNStatusList& operator=(const SVNStatusList& source);
};

#endif
