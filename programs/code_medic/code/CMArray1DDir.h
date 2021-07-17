/******************************************************************************
 CMArray1DDir.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_CMArray1DDir
#define _H_CMArray1DDir

#include <JXWindowDirector.h>
#include <JString.h>
#include <JRange.h>

class JTree;
class JTreeNode;
class JXTextButton;
class JXTextMenu;
class CMLink;
class CMVarTreeWidget;
class CMArrayExprInput;
class CMArrayIndexInput;
class CMCommandDirector;

class CMArray1DDir : public JXWindowDirector
{
public:

	CMArray1DDir(CMCommandDirector* supervisor, const JString& expr);
	CMArray1DDir(std::istream& input, const JFileVersion vers,
				 CMCommandDirector* supervisor);

	virtual	~CMArray1DDir();

	virtual void		Activate() override;
	virtual bool	Deactivate() override;
	virtual bool	GetMenuIcon(const JXImage** icon) const override;

	void	StreamOut(std::ostream& output);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	CMLink*				itsLink;
	CMCommandDirector*	itsCommandDir;
	JTree*				itsTree;			// owned by itsWidget
	CMVarTreeWidget*	itsWidget;
	JString				itsExpr;
	JIntRange			itsDisplayRange;
	JIntRange			itsRequestRange;
	JTreeNode*			itsCurrentNode;		// owned by itsTree
	bool			itsWaitingForReloadFlag;

	JXTextMenu*			itsFileMenu;
	JXTextMenu*			itsActionMenu;
	JXTextMenu*			itsHelpMenu;

// begin JXLayout

	CMArrayExprInput*  itsExprInput;
	JXTextButton*      itsStopButton;
	CMArrayIndexInput* itsStartIndex;
	CMArrayIndexInput* itsEndIndex;

// end JXLayout

private:

	void	CMArray1DDirX(CMCommandDirector* supervisor);
	void	BuildWindow();
	void	UpdateWindowTitle();

	void	HandleFileMenu(const JIndex index);

	void	UpdateActionMenu();
	void	HandleActionMenu(const JIndex index);

	void	HandleHelpMenu(const JIndex index);

	void	BeginCreateNodes();
	void	CreateNextNode();
	JString	GetExpression(const JInteger i) const;
	void	CreateNodesFinished();

	// not allowed

	CMArray1DDir(const CMArray1DDir& source);
	const CMArray1DDir& operator=(const CMArray1DDir& source);
};

#endif
