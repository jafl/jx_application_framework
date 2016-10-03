/******************************************************************************
 CMMemoryDir.h

	Copyright (C) 2011 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_CMMemoryDir
#define _H_CMMemoryDir

#include <JXWindowDirector.h>
#include <JString.h>

class JXTextButton;
class JXTextMenu;
class JXStaticText;
class CMLink;
class CMGetMemory;
class CMArrayExprInput;
class CMArrayIndexInput;
class CMCommandDirector;

class CMMemoryDir : public JXWindowDirector
{
public:

	// remember to sync itsDisplayTypeMenu and CMGetMemory::kCommandName

	enum DisplayType
	{
		kHexByte = 1,
		kHexShort,
		kHexWord,
		kHexLong,
		kChar,
		kAsm
	};

public:

	CMMemoryDir(CMCommandDirector* supervisor, const JCharacter* expr);
	CMMemoryDir(istream& input, const JFileVersion vers,
				CMCommandDirector* supervisor);

	virtual	~CMMemoryDir();

	virtual void		Activate();
	virtual JBoolean	Deactivate();
	virtual JBoolean	GetMenuIcon(const JXImage** icon) const;

	void	StreamOut(ostream& output);

	const JString&	GetExpression(DisplayType* type, JSize* count);
	void			SetDisplayType(const DisplayType type);
	void			Update(const JCharacter* data);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveGoingAway(JBroadcaster* sender);

private:

	CMLink*				itsLink;
	CMCommandDirector*	itsCommandDir;
	JXStaticText*		itsWidget;
	JString				itsExpr;
	DisplayType			itsDisplayType;
	JSize				itsItemCount;
	CMGetMemory*		itsCmd;			// can be NULL
	JBoolean			itsShouldUpdateFlag;
	JBoolean			itsNeedsUpdateFlag;
	JBoolean			itsWaitingForReloadFlag;

	JXTextMenu*			itsFileMenu;
	JXTextMenu*			itsActionMenu;
	JXTextMenu*			itsHelpMenu;

// begin JXLayout

	CMArrayExprInput*  itsExprInput;
	JXTextMenu*        itsDisplayTypeMenu;
	CMArrayIndexInput* itsItemCountInput;

// end JXLayout

private:

	void	CMMemoryDirX(CMCommandDirector* supervisor);
	void	BuildWindow();
	void	UpdateWindowTitle();
	void	UpdateDisplayTypeMenu();
	void	Update();

	void	HandleFileMenu(const JIndex index);

	void	UpdateActionMenu();
	void	HandleActionMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);

	// not allowed

	CMMemoryDir(const CMMemoryDir& source);
	const CMMemoryDir& operator=(const CMMemoryDir& source);
};


/******************************************************************************
 GetExpression

 *****************************************************************************/

inline const JString&
CMMemoryDir::GetExpression
	(
	DisplayType*	type,
	JSize*			count
	)
{
	*type  = itsDisplayType;
	*count = itsItemCount;
	return itsExpr;
}

#endif
