/******************************************************************************
 CMVarNode.cpp

	BASE CLASS = public JNamedTreeNode

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "CMVarNode.h"
#include "CMVarCommand.h"
#include "CMInitVarNodeTask.h"
#include "cmGlobals.h"
#include "cbmUtil.h"
#include <JTree.h>
#include <JStringIterator.h>
#include <JRegex.h>
#include <JColorManager.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CMVarNode::CMVarNode				// root node
	(
	const bool shouldUpdate		// false for Local Variables
	)
	:
	JNamedTreeNode(nullptr, JString::empty),
	itsShouldListenToLinkFlag(shouldUpdate)
{
	VarTreeNodeX();
	itsValidFlag = true;

	if (itsShouldListenToLinkFlag)
		{
		ListenTo(CMGetLink());
		}
}

CMVarNode::CMVarNode
	(
	JTreeNode*		parent,
	const JString&	name,
	const JString&	value
	)
	:
	JNamedTreeNode(nullptr, name.IsEmpty() ? JString(" ", JString::kNoCopy) : name, false),
	itsShouldListenToLinkFlag(false),
	itsValue(value)
{
	VarTreeNodeX();

	if (parent != nullptr)
		{
		parent->Append(this);
		if (parent->IsRoot() && !name.IsEmpty())
			{
			auto* task = jnew CMInitVarNodeTask(this);
			assert( task != nullptr );
			task->Go();
			}
		}
}

// private

void
CMVarNode::VarTreeNodeX()
{
	itsValueCommand		= nullptr;
	itsIsPointerFlag	= false;
	itsValidFlag		= !itsValue.IsEmpty();
	itsNewValueFlag		= false;
	itsContentCommand	= nullptr;

	itsShouldUpdateFlag = false;	// window is always initially hidden
	itsNeedsUpdateFlag  = false;

	itsBase               = 0;
	itsCanConvertBaseFlag = false;
	itsOrigValue          = nullptr;
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
	itsNewValueFlag = !itsValue.IsEmpty() && value != itsValue;
	itsValue        = value;	// set *after* checking value

	itsValue.TrimWhitespace();
	if (itsOrigValue != nullptr)
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
	JUtf8Byte			c;
	const JUtf8Byte*	s;
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
	itsCanConvertBaseFlag = false;
	if (itsOrigValue != nullptr && !itsOrigValue->IsEmpty())
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
		return;		// avoid JRegex match
		}

	const JStringMatch m = valuePattern.Match(itsValue, JRegex::kIncludeSubmatches);
	if (!m.IsEmpty())
		{
		JString vStr = m.GetSubstring(1);

		JUInt v;
		if (vStr.GetFirstCharacter() == '-')
			{
			JInteger v1;
			itsCanConvertBaseFlag = itsBase != 1 &&
				vStr.ConvertToInteger(&v1, 10);
			if (itsCanConvertBaseFlag)
				{
				v = (JUInt) v1;
				}
			}
		else
			{
			itsCanConvertBaseFlag = vStr.ConvertToUInt(&v, vStr.GetFirstCharacter() == '0' ? 8 : 10) &&
				(itsBase != 1 || v <= 255);
			}

		if (itsCanConvertBaseFlag)
			{
			// save value for when base reset to "default"

			itsOrigValue = jnew JString(itsValue);
			assert( itsOrigValue != nullptr );

			// replace only the value, preserving whatever else is there

			if (itsBase == 1)
				{
				assert( 0 <= v && v <= 255 );

				vStr  = JString(v, JString::kBase16, true);
				vStr += " '";

				bool found = false;
				for (JUnsignedOffset i=0; i<kSpecialCharCount; i++)
					{
					if (JUtf8Character(v) == kSpecialCharInfo[i].c)
						{
						vStr += kSpecialCharInfo[i].s;
						found = true;
						}
					}
				if (!found)
					{
					vStr.Append(JUtf8Character(v));
					}

				vStr.Append("'");
				}
			else
				{
				vStr = JString(v, (JString::Base) itsBase, true);
				if (itsBase == 8)
					{
					vStr.Prepend("0");
					}
				}

			const JCharacterRange r = m.GetCharacterRange(1);
			JStringIterator iter(&itsValue, kJIteratorStartBefore, r.first);
			iter.BeginMatch();
			iter.SkipNext(r.GetCount());
			iter.FinishMatch();
			iter.ReplaceLastMatch(vStr);
			iter.Invalidate();

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
	const bool valid
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
CMVarNode::GetFontStyle()
{
	if (!itsValidFlag)
		{
		return JFontStyle(JColorManager::GetGrayColor(50));
		}
	else if (itsNewValueFlag)
		{
		return JFontStyle(JColorManager::GetBlueColor());
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
	const bool isValid,
	const bool isNew
	)
{
	if (!isValid)
		{
		return JFontStyle(JColorManager::GetGrayColor(50));
		}
	else if (isNew)
		{
		return JFontStyle(JColorManager::GetBlueColor());
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

bool
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
			itsNeedsUpdateFlag = true;
			}
		}

	else if (sender == itsValueCommand &&
			 message.Is(CMVarCommand::kValueUpdated))
		{
		const auto& info =
			dynamic_cast<const CMVarCommand::ValueMessage&>(message);

		SetValid(true);
		Update(info.GetRootNode());
		}
	else if (sender == itsValueCommand &&
			 message.Is(CMVarCommand::kValueFailed))
		{
		SetValue(itsValueCommand->GetData());
		MakePointer(false);
		}

	else if (sender == itsContentCommand &&
			 message.Is(CMVarCommand::kValueUpdated))
		{
		const auto& info =
			dynamic_cast<const CMVarCommand::ValueMessage&>(message);

		CMVarNode* root = info.GetRootNode();

		// value or pointer

		if (!root->HasChildren())
			{
			DeleteAllChildren();
			CMVarNode* child = CMGetLink()->CreateVarNode(this, root->GetName(),
															root->GetFullName(),
															root->GetValue());
			assert( child != nullptr );
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
		CMVarNode* child = CMGetLink()->CreateVarNode(this, JString::empty, JString::empty, itsContentCommand->GetData());
		assert( child != nullptr );
		child->SetValid(false);
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

bool
CMVarNode::ShouldUpdate
	(
	const Message& message
	)
{
	if (message.Is(CMLink::kProgramStopped))
		{
		const auto& info =
			dynamic_cast<const CMLink::ProgramStopped&>(message);

		const CMLocation* loc;
		return info.GetLocation(&loc) && !(loc->GetFileName()).IsEmpty();
		}
	else
		{
		return message.Is(CMLink::kValueChanged)      ||
					message.Is(CMLink::kThreadChanged)     ||
					message.Is(CMLink::kFrameChanged)      ||
					message.Is(CMLink::kCoreLoaded)        ||
					message.Is(CMLink::kCoreCleared)       ||
					message.Is(CMLink::kAttachedToProcess) ||
					message.Is(CMLink::kDetachedFromProcess);
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
		JStringIterator iter(s, kJIteratorStartAfter, 1);
		JUtf8Character c;
		if (!CBMBalanceForward(kCBCLang, &iter, &c) || !iter.AtEnd())
			{
			break;
			}

		iter.MoveTo(kJIteratorStartAtBeginning, 0);
		iter.RemoveNext();
		iter.MoveTo(kJIteratorStartAtEnd, 0);
		iter.RemovePrev();

		s->TrimWhitespace();
		}
}

/******************************************************************************
 ShouldUpdate

 ******************************************************************************/

void
CMVarNode::ShouldUpdate
	(
	const bool update
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

	itsNeedsUpdateFlag = false;
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
		itsValueCommand = CMGetLink()->CreateVarValueCommand(expr);
		ListenTo(itsValueCommand);

		SetValid(false);
		itsValueCommand->CMCommand::Send();
		}
	else
		{
		SetValue(JString::empty);
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
	SetValid(true);
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
	MakePointer(false);
	SetValid(false);		// faster after deleting children
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
		MakePointer(node->itsIsPointerFlag, false);
		MergeChildren(node);
		}
	else
		{
		MakePointer(node->itsIsPointerFlag, false);
		StealChildren(node);
		}
}

/******************************************************************************
 SameElements (private)

	Returns true if the given node has the same number of children with
	the same names as we do.

 ******************************************************************************/

bool
CMVarNode::SameElements
	(
	const CMVarNode* node
	)
	const
{
	bool sameElements = false;

	if (node->GetChildCount() == GetChildCount())
		{
		sameElements = true;

		const JSize count = GetChildCount();
		for (JIndex i=1; i<=count; i++)
			{
			if ((GetVarChild(i))->GetName() != (node->GetVarChild(i))->GetName())
				{
				sameElements = false;
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
	const bool pointer,
	const bool adjustOpenable
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
	itsContentCommand = CMGetLink()->CreateVarContentCommand(expr);
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
	auto* n = dynamic_cast<CMVarNode*>(p);
	assert( n != nullptr );
	return n;
}

const CMVarNode*
CMVarNode::GetVarParent()
	const
{
	const JTreeNode* p = GetParent();
	const auto* n = dynamic_cast<const CMVarNode*>(p);
	assert( n != nullptr );
	return n;
}

bool
CMVarNode::GetVarParent
	(
	CMVarNode** parent
	)
{
	JTreeNode* p;
	if (GetParent(&p))
		{
		*parent = dynamic_cast<CMVarNode*>(p);
		assert( *parent != nullptr );
		return true;
		}
	else
		{
		*parent = nullptr;
		return false;
		}
}

bool
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
		assert( *parent != nullptr );
		return true;
		}
	else
		{
		*parent = nullptr;
		return false;
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
	auto* node = dynamic_cast<CMVarNode*>(GetChild(index));
	assert (node != nullptr);
	return node;
}

const CMVarNode*
CMVarNode::GetVarChild
	(
	const JIndex index
	)
	const
{
	const auto* node = dynamic_cast<const CMVarNode*>(GetChild(index));
	assert (node != nullptr);
	return node;
}

/******************************************************************************
 GetFullNameForCFamilyLanguage (protected)

 ******************************************************************************/

JString
CMVarNode::GetFullNameForCFamilyLanguage
	(
	bool* isPointer
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
		const bool found = parent->FindChild(this, &i);
		assert( found );
		str = parent->GetFullName(isPointer);
		if (!str.BeginsWith("(") || !str.EndsWith(")"))
			{
			str.Prepend("(");
			str.Append(")");
			}
		str += "[" + JString((JUInt64) i-1) + "]";
		}
	else if (name.BeginsWith("<"))
		{
		if (isPointer != nullptr)
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
			JStringIterator iter(&str);
			iter.RemoveNext(7);
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

	bool isPointer = IsPointer();
	str  = GetFullName(&isPointer);
	str += isPointer ? "->" : ".";
	return str;
}
