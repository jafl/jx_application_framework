/******************************************************************************
 CMVarNode.h

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_CMVarNode
#define _H_CMVarNode

#include <JNamedTreeNode.h>
#include <JFontStyle.h>

class CMVarCommand;

class CMVarNode : public JNamedTreeNode
{
	friend class CMInitVarNodeTask;

public:

	CMVarNode(const bool shouldUpdate = true);
	CMVarNode(JTreeNode* parent, const JString& name, const JString& value);

	virtual	~CMVarNode();

	bool		ValueIsValid() const;
	virtual JString	GetFullName(bool* isPointer = nullptr) const = 0;
	const JString&	GetValue() const;
	void			SetValue(const JString& value);
	void			SetValid(const bool valid);
	JFontStyle		GetFontStyle();

	static JFontStyle	GetFontStyle(const bool isValid, const bool isNew);

	JSize	GetBase() const;
	void	SetBase(const JSize base);

	bool	IsPointer() const;
	void		MakePointer(const bool pointer, const bool adjustOpenable = true);

	virtual bool	OKToOpen() const override;

	void	ShouldUpdate(const bool update);

	static bool	ShouldUpdate(const Message& message);

	void	GetLastChild(CMVarNode** child);

	CMVarNode*			GetVarParent();
	const CMVarNode*	GetVarParent() const;
	bool			GetVarParent(CMVarNode** parent);
	bool			GetVarParent(const CMVarNode** parent) const;

	CMVarNode*			GetVarChild(const JIndex index);
	const CMVarNode*	GetVarChild(const JIndex index) const;

	static void	TrimExpression(JString* s);

	// called by CMGetLocalVars

	void	UpdateValue();
	void	UpdateValue(CMVarNode* root);
	void	UpdateFailed(const JString& data);

protected:

	virtual void	NameChanged() override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

	JString	GetFullNameForCFamilyLanguage(bool* isPointer) const;
	JString	GetPathForCFamilyLanguage() const;

private:

	const bool		itsShouldListenToLinkFlag;
	CMVarCommand*		itsValueCommand;
	JString				itsValue;
	bool			itsIsPointerFlag;
	bool			itsValidFlag;
	bool			itsNewValueFlag;
	CMVarCommand*		itsContentCommand;

	// used for base conversion

	JSize		itsBase;		// 0 => default, 1 => ASCII
	bool	itsCanConvertBaseFlag;
	JString*	itsOrigValue;

	// optimized updating

	bool	itsShouldUpdateFlag;
	bool	itsNeedsUpdateFlag;

private:

	void	VarTreeNodeX();

	void	UpdateChildren();
	void	Update(CMVarNode* node);
	void	UpdateContent();
	void	StealChildren(CMVarNode* node);
	void	MergeChildren(CMVarNode* node);

	void	ConvertToBase();

	bool	SameElements(const CMVarNode* node) const;

	// not allowed

	CMVarNode(const CMVarNode& source);
	const CMVarNode& operator=(const CMVarNode& source);
};


/******************************************************************************
 ValueIsValid

 ******************************************************************************/

inline bool
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

inline bool
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
