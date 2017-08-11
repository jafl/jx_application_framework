/******************************************************************************
 CMVarNode.cpp

	BASE CLASS = public JNamedTreeNode

	Copyright (C) 2001 by John Lindal.  All rights reserved.

 *****************************************************************************/

#include "CMVarNode.h"
#include "CMVarCommand.h"
#include "CMInitVarNodeTask.h"
#include "cmGlobals.h"
#include "cbmUtil.h"
#include <JTree.h>
#include <JRegex.h>
#include <JColormap.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CMVarNode::CMVarNode				// root node
	(
	const JBoolean shouldUpdate		// kJFalse for Local Variables
	)
	:
	JNamedTreeNode(NULL, ""),
	itsShouldListenToLinkFlag(shouldUpdate)
{
	VarTreeNodeX();
	itsValidFlag = kJTrue;

	if (itsShouldListenToLinkFlag)
		{
		ListenTo(CMGetLink());
		}
}

CMVarNode::CMVarNode
	(
	JTreeNode*			parent,
	const JCharacter*	name,
	const JCharacter*	value
	)
	:
	JNamedTreeNode(NULL, JString::IsEmpty(name) ? " " : name, kJFalse),
	itsShouldListenToLinkFlag(kJFalse),
	itsValue(value)
{
	VarTreeNodeX();

	if (parent != NULL)
		{
		parent->Append(this);
		if (parent->IsRoot() && !JString::IsEmpty(name))
			{
			CMInitVarNodeTask* task = jnew CMInitVarNodeTask(this);
			assert( task != NULL );
			task->Go();
			}
		}
}

// private

void
CMVarNode::VarTreeNodeX()
{
	itsValueCommand		= NULL;
	itsIsPointerFlag	= kJFalse;
	itsValidFlag		= !itsValue.IsEmpty();
	itsNewValueFlag		= kJFalse;
	itsContentCommand	= NULL;

	itsShouldUpdateFlag = kJFalse;	// window is always initially hidden
	itsNeedsUpdateFlag  = kJFalse;

	itsBase               = 0;
	itsCanConvertBaseFlag = kJFalse;
	itsOrigValue          = NULL;
	ConvertToBase();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CMVarNode::~CMVarNode()
{
	jdelete itsValueCommand;
	jdelete itsContentCommand;
	jdelete itsOrigValue;
}

/******************************************************************************
 SetValue

 ******************************************************************************/

void
CMVarNode::SetValue
	(
	const JString& value
	)
{
	itsNewValueFlag = JI2B(!itsValue.IsEmpty() && value != itsValue);
	itsValue        = value;	// set *after* checking value

	itsValue.TrimWhitespace();
	if (itsOrigValue != NULL)
		{
		itsOrigValue->Clear();
		}
	ConvertToBase();

	JTree* tree;
	if (!itsCanConvertBaseFlag && GetTree(&tree))
		{
		tree->BroadcastChange(this);
		}
}

/******************************************************************************
 ConvertToBase (private)

 ******************************************************************************/

static const JRegex valuePattern = "^(-?[[:digit:]]+)([[:space:]]+'.*)?$";

struct CMSpecialCharInfo
{
	JCharacter			c;
	const JCharacter*	s;
};

static const CMSpecialCharInfo kSpecialCharInfo[] =
{
	{ '\a', "\\a"  },
	{ '\b', "\\b"  },
	{ '\f', "\\f"  },
	{ '\n', "\\n"  },
	{ '\r', "\\r"  },
	{ '\t', "\\t"  },
	{ '\v', "\\v"  },
	{ '\\', "\\\\" },
	{ '\'', "\\'"  },
	{ '"',  "\\\"" },
	{ '\0', "\\0"  }
};

const JSize kSpecialCharCount = sizeof(kSpecialCharInfo) / sizeof(CMSpecialCharInfo);

void
CMVarNode::ConvertToBase()
{
	itsCanConvertBaseFlag = kJFalse;
	if (itsOrigValue != NULL && !itsOrigValue->IsEmpty())
		{
		JTree* tree;
		if (itsValue != *itsOrigValue && GetTree(&tree))
			{
			tree->BroadcastChange(this);
			}
		itsValue = *itsOrigValue;
		}

	if (itsBase == 0 || itsIsPointerFlag)
		{
		return;		// avoid constructing matchList
		}

	JArray<JIndexRange> matchList;
	if (valuePattern.Match(itsValue, &matchList))
		{
		JString vStr = itsValue.GetSubstring(matchList.GetElement(2));

		JUInt v;
		if (vStr.GetFirstCharacter() == '-')
			{
			JInteger v1;
			itsCanConvertBaseFlag = JI2B(
				itsBase != 1 &&
				vStr.ConvertToInteger(&v1, 10));
			if (itsCanConvertBaseFlag)
				{
				v = (JUInt) v1;
				}
			}
		else
			{
			itsCanConvertBaseFlag = JI2B(
				vStr.ConvertToUInt(&v, vStr.GetFirstCharacter() == '0' ? 8 : 10) &&
				(itsBase != 1 || v <= 255));
			}

		if (itsCanConvertBaseFlag)
			{
			// save value for when base reset to "default"

			itsOrigValue = jnew JString(itsValue);
			assert( itsOrigValue != NULL );

			// replace only the value, preserving whatever else is there

			if (itsBase == 1)
				{
				assert( 0 <= v && v <= 255 );

				vStr  = JString(v, JString::kBase16, kJTrue);
				vStr += " '";

				JBoolean found = kJFalse;
				for (JIndex i=0; i<kSpecialCharCount; i++)
					{
					if (JCharacter(v) == kSpecialCharInfo[i].c)
						{
						vStr += kSpecialCharInfo[i].s;
						found = kJTrue;
						}
					}
				if (!found)
					{
					vStr.AppendCharacter(v);
					}

				vStr.AppendCharacter('\'');
				}
			else
				{
				vStr = JString(v, (JString::Base) itsBase, kJTrue);
				if (itsBase == 8)
					{
					vStr.PrependCharacter('0');
					}
				}

			JIndexRange r;
			itsValue.ReplaceSubstring(matchList.GetElement(2), vStr, &r);

			JTree* tree;
			if (GetTree(&tree))
				{
				tree->BroadcastChange(this);
				}
			}
		}
}

/******************************************************************************
 SetValid

 ******************************************************************************/

void
CMVarNode::SetValid
	(
	const JBoolean valid
	)
{
	if (valid != itsValidFlag)
		{
		itsValidFlag = valid;

		JTree* tree;
		if (GetTree(&tree))
			{
			tree->BroadcastChange(this);
			}
		}

	// pointer values update their own contents

	if (!valid || !itsIsPointerFlag)
		{
		const JSize count = GetChildCount();
		for (JIndex i=1; i<=count; i++)
			{
			(GetVarChild(i))->SetValid(valid);
			}
		}
}

/******************************************************************************
 GetFontStyle

 ******************************************************************************/

JFontStyle
CMVarNode::GetFontStyle
	(
	const JColormap* cmap
	)
{
	if (!itsValidFlag)
		{
		return JFontStyle(cmap->GetGrayColor(50));
		}
	else if (itsNewValueFlag)
		{
		return JFontStyle(cmap->GetBlueColor());
		}
	else
		{
		return JFontStyle();
		}
}

// static

JFontStyle
CMVarNode::GetFontStyle
	(
	const JBoolean		isValid,
	const JBoolean		isNew,
	const JColormap*	cmap
	)
{
	if (!isValid)
		{
		return JFontStyle(cmap->GetGrayColor(50));
		}
	else if (isNew)
		{
		return JFontStyle(cmap->GetBlueColor());
		}
	else
		{
		return JFontStyle();
		}
}

/******************************************************************************
 OKToOpen (virtual)

	Update ourselves if we are being opened.

 ******************************************************************************/

JBoolean
CMVarNode::OKToOpen()
	const
{
	if (itsIsPointerFlag && !HasChildren())
		{
		const_cast<CMVarNode*>(this)->UpdateContent();
		}

	return JNamedTreeNode::OKToOpen();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CMVarNode::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == CMGetLink() && ShouldUpdate(message))
		{
		// root node only

		if (itsShouldUpdateFlag)
			{
			UpdateChildren();
			}
		else
			{
			itsNeedsUpdateFlag = kJTrue;
			}
		}

	else if (sender == itsValueCommand &&
			 message.Is(CMVarCommand::kValueUpdated))
		{
		const CMVarCommand::ValueMessage& info =
			dynamic_cast<const CMVarCommand::ValueMessage&>(message);

		SetValid(kJTrue);
		Update(info.GetRootNode());
		}
	else if (sender == itsValueCommand &&
			 message.Is(CMVarCommand::kValueFailed))
		{
		SetValue(itsValueCommand->GetData());
		MakePointer(kJFalse);
		}

	else if (sender == itsContentCommand &&
			 message.Is(CMVarCommand::kValueUpdated))
		{
		const CMVarCommand::ValueMessage& info =
			dynamic_cast<const CMVarCommand::ValueMessage&>(message);

		CMVarNode* root = info.GetRootNode();

		// value or pointer

		if (!root->HasChildren())
			{
			DeleteAllChildren();
			CMVarNode* child = (CMGetLink())->CreateVarNode(this, root->GetName(),
															root->GetFullName(),
															root->GetValue());
			assert( child != NULL );
			child->MakePointer(root->itsIsPointerFlag);
			}

		// struct or static array

		else if (SameElements(root))
			{
			MergeChildren(root);
			}
		else
			{
			StealChildren(root);
			}
		}
	else if (sender == itsContentCommand &&
			 message.Is(CMVarCommand::kValueFailed))
		{
		DeleteAllChildren();
		CMVarNode* child = (CMGetLink())->CreateVarNode(this, "", "", itsContentCommand->GetData());
		assert( child != NULL );
		child->SetValid(kJFalse);
		}

	else
		{
		JNamedTreeNode::Receive(sender, message);
		}
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

 ******************************************************************************/

void
CMVarNode::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (itsShouldListenToLinkFlag && !CMIsShuttingDown())
		{
		ListenTo(CMGetLink());
		}

	JNamedTreeNode::ReceiveGoingAway(sender);
}

/******************************************************************************
 ShouldUpdate (static)

 ******************************************************************************/

JBoolean
CMVarNode::ShouldUpdate
	(
	const Message& message
	)
{
	if (message.Is(CMLink::kProgramStopped))
		{
		const CMLink::ProgramStopped& info =
			dynamic_cast<const CMLink::ProgramStopped&>(message);

		const CMLocation* loc;
		return JI2B(info.GetLocation(&loc) && !(loc->GetFileName()).IsEmpty());
		}
	else
		{
		return JI2B(message.Is(CMLink::kValueChanged)      ||
					message.Is(CMLink::kThreadChanged)     ||
					message.Is(CMLink::kFrameChanged)      ||
					message.Is(CMLink::kCoreLoaded)        ||
					message.Is(CMLink::kCoreCleared)       ||
					message.Is(CMLink::kAttachedToProcess) ||
					message.Is(CMLink::kDetachedFromProcess));
		}
}

/******************************************************************************
 NameChanged (virtual protected)

 ******************************************************************************/

void
CMVarNode::NameChanged()
{
	if (!HasTree() || GetDepth() != 1)
		{
		// parser may be busy; don't interfere
		return;
		}

	const JString& name = GetName();

	JString n = name;
	TrimExpression(&n);
	if (n != name)
		{
		SetName(n);
		return;		// calls us again
		}

	UpdateValue();
}

/******************************************************************************
 TrimExpression (static)

 ******************************************************************************/

void
CMVarNode::TrimExpression
	(
	JString* s
	)
{
	s->TrimWhitespace();
	while (!s->IsEmpty() &&
		   s->GetFirstCharacter() == '(' &&
		   s->GetLastCharacter()  == ')')
		{
		JIndex i = 2;
		if (!CBMBalanceForward(kCBCLang, *s, &i) || i < s->GetLength())
			{
			break;
			}

		s->RemoveSubstring(1, 1);
		s->RemoveSubstring(s->GetLength(), s->GetLength());
		s->TrimWhitespace();
		}
}

/******************************************************************************
 ShouldUpdate

 ******************************************************************************/

void
CMVarNode::ShouldUpdate
	(
	const JBoolean update
	)
{
	itsShouldUpdateFlag = update;
	if (itsShouldUpdateFlag && itsNeedsUpdateFlag)
		{
		UpdateChildren();
		}
}

/******************************************************************************
 UpdateChildren (private)

 ******************************************************************************/

void
CMVarNode::UpdateChildren()
{
	const JSize count = GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		(GetVarChild(i))->UpdateValue();
		}

	itsNeedsUpdateFlag = kJFalse;
}

/******************************************************************************
 UpdateValue

	Called by self and *GetLocalVars.

 ******************************************************************************/

void
CMVarNode::UpdateValue()
{
	if (HasTree() && GetDepth() == 1 && !(GetName()).IsEmpty())
		{
		jdelete itsValueCommand;

		const JString expr = GetFullName();
		itsValueCommand = (CMGetLink())->CreateVarValueCommand(expr);
		ListenTo(itsValueCommand);

		SetValid(kJFalse);
		itsValueCommand->CMCommand::Send();
		}
	else
		{
		SetValue("");
		}
}

/******************************************************************************
 UpdateValue

	Called by *GetLocalVars.

 ******************************************************************************/

void
CMVarNode::UpdateValue
	(
	CMVarNode* root
	)
{
	SetValid(kJTrue);
	Update(root);
}

/******************************************************************************
 UpdateFailed

	Called by *GetLocalVars.

 ******************************************************************************/

void
CMVarNode::UpdateFailed
	(
	const JString& data
	)
{
	SetValue(data);
	MakePointer(kJFalse);
	SetValid(kJFalse);		// faster after deleting children
}

/******************************************************************************
 Update (private)

 ******************************************************************************/

void
CMVarNode::Update
	(
	CMVarNode* node
	)
{
	SetValue(node->GetValue());
	SetValid(node->ValueIsValid());

	// value or pointer

	if (!node->HasChildren())
		{
		MakePointer(node->itsIsPointerFlag);
		if (itsIsPointerFlag && HasChildren())
			{
			UpdateContent();
			}
		}

	// struct or static array

	else if (SameElements(node))
		{
		MakePointer(node->itsIsPointerFlag, kJFalse);
		MergeChildren(node);
		}
	else
		{
		MakePointer(node->itsIsPointerFlag, kJFalse);
		StealChildren(node);
		}
}

/******************************************************************************
 SameElements (private)

	Returns kJTrue if the given node has the same number of children with
	the same names as we do.

 ******************************************************************************/

JBoolean
CMVarNode::SameElements
	(
	const CMVarNode* node
	)
	const
{
	JBoolean sameElements = kJFalse;

	if (node->GetChildCount() == GetChildCount())
		{
		sameElements = kJTrue;

		const JSize count = GetChildCount();
		for (JIndex i=1; i<=count; i++)
			{
			if ((GetVarChild(i))->GetName() != (node->GetVarChild(i))->GetName())
				{
				sameElements = kJFalse;
				break;
				}
			}
		}

	return sameElements;
}

/******************************************************************************
 MakePointer

 ******************************************************************************/

void
CMVarNode::MakePointer
	(
	const JBoolean pointer,
	const JBoolean adjustOpenable
	)
{
	itsIsPointerFlag = pointer;

	if (adjustOpenable)
		{
		ShouldBeOpenable(itsIsPointerFlag);
		}
}

/******************************************************************************
 UpdateContent (private)

 ******************************************************************************/

void
CMVarNode::UpdateContent()
{
	assert( itsIsPointerFlag );

	jdelete itsContentCommand;

	const JString expr = GetFullName();
	itsContentCommand = (CMGetLink())->CreateVarContentCommand(expr);
	ListenTo(itsContentCommand);

	itsContentCommand->CMCommand::Send();
}

/******************************************************************************
 StealChildren (private)

 ******************************************************************************/

void
CMVarNode::StealChildren
	(
	CMVarNode* node
	)
{
	DeleteAllChildren();

	const JSize count = node->GetChildCount();
	for (JIndex i=1; i<=count; i++)
		{
		Append(node->GetChild(1));
		}
}

/******************************************************************************
 MergeChildren (private)

 ******************************************************************************/

void
CMVarNode::MergeChildren
	(
	CMVarNode* node
	)
{
	const JSize count = GetChildCount();
	assert( count == node->GetChildCount() );

	for (JIndex i=1; i<=count; i++)
		{
		(GetVarChild(i))->Update(node->GetVarChild(i));
		}
}

/******************************************************************************
 GetLastChild

 ******************************************************************************/

void
CMVarNode::GetLastChild
	(
	CMVarNode** child
	)
{
	*child = this;

	const JSize count = GetChildCount();
	if (count > 0)
		{
		GetVarChild(count)->GetLastChild(child);
		}
}

/******************************************************************************
 GetVarParent

 ******************************************************************************/

CMVarNode*
CMVarNode::GetVarParent()
{
	JTreeNode* p = GetParent();
	CMVarNode* n = dynamic_cast<CMVarNode*>(p);
	assert( n != NULL );
	return n;
}

const CMVarNode*
CMVarNode::GetVarParent()
	const
{
	const JTreeNode* p = GetParent();
	const CMVarNode* n = dynamic_cast<const CMVarNode*>(p);
	assert( n != NULL );
	return n;
}

JBoolean
CMVarNode::GetVarParent
	(
	CMVarNode** parent
	)
{
	JTreeNode* p;
	if (GetParent(&p))
		{
		*parent = dynamic_cast<CMVarNode*>(p);
		assert( *parent != NULL );
		return kJTrue;
		}
	else
		{
		*parent = NULL;
		return kJFalse;
		}
}

JBoolean
CMVarNode::GetVarParent
	(
	const CMVarNode** parent
	)
	const
{
	const JTreeNode* p;
	if (GetParent(&p))
		{
		*parent = dynamic_cast<const CMVarNode*>(p);
		assert( *parent != NULL );
		return kJTrue;
		}
	else
		{
		*parent = NULL;
		return kJFalse;
		}
}

/******************************************************************************
 GetVarChild

 ******************************************************************************/

CMVarNode*
CMVarNode::GetVarChild
	(
	const JIndex index
	)
{
	CMVarNode* node = dynamic_cast<CMVarNode*>(GetChild(index));
	assert (node != NULL);
	return node;
}

const CMVarNode*
CMVarNode::GetVarChild
	(
	const JIndex index
	)
	const
{
	const CMVarNode* node = dynamic_cast<const CMVarNode*>(GetChild(index));
	assert (node != NULL);
	return node;
}

/******************************************************************************
 GetFullNameForCFamilyLanguage (protected)

 ******************************************************************************/

JString
CMVarNode::GetFullNameForCFamilyLanguage
	(
	JBoolean* isPointer
	)
	const
{
	JString str;
	if (IsRoot())
		{
		return str;
		}

	const CMVarNode* parent = GetVarParent();
	const JString& name     = GetName();
	if (parent->IsRoot())
		{
		str = "(" + name + ")";
		}
	else if (name.IsEmpty())
		{
		JIndex i;
		const JBoolean found = parent->FindChild(this, &i);
		assert( found );
		str = parent->GetFullName(isPointer);
		if (!str.BeginsWith("(") || !str.EndsWith(")"))
			{
			str.PrependCharacter('(');
			str.AppendCharacter(')');
			}
		str += "[" + JString(i-1, JString::kBase10) + "]";
		}
	else if (name.BeginsWith("<"))
		{
		if (isPointer != NULL)
			{
			*isPointer = parent->IsPointer();
			}
		str = parent->GetFullName(isPointer);
		}
	else if (name.BeginsWith("["))
		{
		str = parent->GetFullName(isPointer) + name;
		}
	else if (name.BeginsWith("*"))
		{
		str = parent->GetPathForCFamilyLanguage() + "(" + name + ")";
		}
	else
		{
		str = name;
		if (str.BeginsWith("static "))
			{
			str.RemoveSubstring(1,7);
			}
		str.Prepend(parent->GetPathForCFamilyLanguage());
		}

	return str;
}

/******************************************************************************
 GetPathForCFamilyLanguage (protected)

 ******************************************************************************/

JString
CMVarNode::GetPathForCFamilyLanguage()
	const
{
	JString str;
	if (IsRoot())
		{
		return str;
		}

	JBoolean isPointer = IsPointer();
	str  = GetFullName(&isPointer);
	str += isPointer ? "->" : ".";
	return str;
}
