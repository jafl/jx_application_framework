/******************************************************************************
 CMMemoryDir.h

	Copyright (C) 2011 by John Lindal.

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

	CMMemoryDir(CMCommandDirector* supervisor, const JString& expr);
	CMMemoryDir(std::istream& input, const JFileVersion vers,
				CMCommandDirector* supervisor);

	virtual	~CMMemoryDir();

	virtual void		Activate() override;
	virtual bool	Deactivate() override;
	virtual bool	GetMenuIcon(const JXImage** icon) const override;

	void	StreamOut(std::ostream& output);

	const JString&	GetExpression(DisplayType* type, JSize* count);
	void			SetDisplayType(const DisplayType type);
	void			Update(const JString& data);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	CMLink*				itsLink;
	CMCommandDirector*	itsCommandDir;
	JXStaticText*		itsWidget;
	JString				itsExpr;
	DisplayType			itsDisplayType;
	JSize				itsItemCount;
	CMGetMemory*		itsCmd;			// can be nullptr
	bool			itsShouldUpdateFlag;
	bool			itsNeedsUpdateFlag;
	bool			itsWaitingForReloadFlag;

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
