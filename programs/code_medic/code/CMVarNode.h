/******************************************************************************
 CMVarNode.h

	Copyright © 2001 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_CMVarNode
#define _H_CMVarNode

#include <JNamedTreeNode.h>
#include <JFontStyle.h>

class JColormap;
class CMVarCommand;

class CMVarNode : public JNamedTreeNode
{
	friend class CMInitVarNodeTask;

public:

	CMVarNode(const JBoolean shouldUpdate = kJTrue);

	CMVarNode(JTreeNode* parent, const JCharacter* name,
			  const JCharacter* value);

	virtual	~CMVarNode();

	JBoolean		ValueIsValid() const;
	virtual JString	GetFullName(JBoolean* isPointer = NULL) const = 0;
	const JString&	GetValue() const;
	void			SetValue(const JString& value);
	void			SetValid(const JBoolean valid);
	JFontStyle		GetFontStyle(const JColormap* cmap);

	static JFontStyle	GetFontStyle(const JBoolean isValid, const JBoolean isNew,
									 const JColormap* cmap);

	JSize	GetBase() const;
	void	SetBase(const JSize base);

	JBoolean	IsPointer() const;
	void		MakePointer(const JBoolean pointer, const JBoolean adjustOpenable = kJTrue);

	virtual JBoolean	OKToOpen() const;

	void	ShouldUpdate(const JBoolean update);

	static JBoolean	ShouldUpdate(const Message& message);

	void	GetLastChild(CMVarNode** child);

	CMVarNode*			GetVarParent();
	const CMVarNode*	GetVarParent() const;
	JBoolean			GetVarParent(CMVarNode** parent);
	JBoolean			GetVarParent(const CMVarNode** parent) const;

	CMVarNode*			GetVarChild(const JIndex index);
	const CMVarNode*	GetVarChild(const JIndex index) const;

	static void	TrimExpression(JString* s);

	// called by CMGetLocalVars

	void	UpdateValue();
	void	UpdateValue(CMVarNode* root);
	void	UpdateFailed(const JString& data);

protected:

	virtual void	NameChanged();
	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveGoingAway(JBroadcaster* sender);

	JString	GetFullNameForCFamilyLanguage(JBoolean* isPointer) const;
	JString	GetPathForCFamilyLanguage() const;

private:

	const JBoolean		itsShouldListenToLinkFlag;
	CMVarCommand*		itsValueCommand;
	JString				itsValue;
	JBoolean			itsIsPointerFlag;
	JBoolean			itsValidFlag;
	JBoolean			itsNewValueFlag;
	CMVarCommand*		itsContentCommand;

	// used for base conversion

	JSize		itsBase;		// 0 => default, 1 => ASCII
	JBoolean	itsCanConvertBaseFlag;
	JString*	itsOrigValue;

	// optimized updating

	JBoolean	itsShouldUpdateFlag;
	JBoolean	itsNeedsUpdateFlag;

private:

	void	VarTreeNodeX();

	void	UpdateChildren();
	void	Update(CMVarNode* node);
	void	UpdateContent();
	void	StealChildren(CMVarNode* node);
	void	MergeChildren(CMVarNode* node);

	void	ConvertToBase();

	JBoolean	SameElements(const CMVarNode* node) const;

	// not allowed

	CMVarNode(const CMVarNode& source);
	const CMVarNode& operator=(const CMVarNode& source);
};


/******************************************************************************
 ValueIsValid

 ******************************************************************************/

inline JBoolean
CMVarNode::ValueIsValid()
	const
{
	return itsValidFlag;
}

/******************************************************************************
 GetValue

 ******************************************************************************/

inline const JString&
CMVarNode::GetValue()
	const
{
	return itsValue;
}

/******************************************************************************
 IsPointer

 ******************************************************************************/

inline JBoolean
CMVarNode::IsPointer()
	const
{
	return itsIsPointerFlag;
}

/******************************************************************************
 Base conversion

	Zero means don't perform any conversion.

 ******************************************************************************/

inline JSize
CMVarNode::GetBase()
	const
{
	return itsBase;
}

inline void
CMVarNode::SetBase
	(
	const JSize base
	)
{
	itsBase = base;
	ConvertToBase();
}

#endif
