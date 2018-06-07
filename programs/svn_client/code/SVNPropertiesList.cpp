/******************************************************************************
 SVNPropertiesList.cc

	BASE CLASS = SVNListBase

	Copyright @ 2008 by John Lindal.

 ******************************************************************************/

#include "SVNPropertiesList.h"
#include "SVNMainDirector.h"
#include "svnMenus.h"
#include <JXGetStringDialog.h>
#include <JXTextSelection.h>
#include <JXTextMenu.h>
#include <jXGlobals.h>
#include <JTableSelection.h>
#include <JTableSelectionIterator.h>
#include <JSubstitute.h>
#include <JSimpleProcess.h>
#include <jAssert.h>

static const JCharacter* kPropEditCmd   = "svn propedit $prop_name $file_name";
static const JCharacter* kPropRemoveCmd = "svn propdel $prop_name $file_name";

static const JCharacter* kSpecialPropertyList[] =
{
	"svn:ignore",
	"svn:keywords",
	"svn:executable",
	"svn:eol-style",
	"svn:mime-type",
	"svn:externals",
	"svn:needs-lock"
};

const JSize kSpecialPropertyCount = sizeof(kSpecialPropertyList) / sizeof(JCharacter*);

// string ID's

static const JCharacter* kCreatePropertyWindowTitleID = "CreatePropertyWindowTitle::SVNPropertiesList";
static const JCharacter* kCreatePropertyPromptID      = "CreatePropertyPrompt::SVNPropertiesList";

/******************************************************************************
 Constructor

 ******************************************************************************/

SVNPropertiesList::SVNPropertiesList
	(
	SVNMainDirector*	director,
	JXTextMenu*			editMenu,
	const JCharacter*	fullName,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption hSizing,
	const VSizingOption vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	SVNListBase(director, editMenu, GetCommand(fullName), kJFalse, kJFalse, kJFalse, kJFalse,
				scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h),
	itsFullName(fullName),
	itsCreatePropertyDialog(nullptr)
{
	itsRemovePropertyCmdList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert( itsRemovePropertyCmdList != nullptr );

	itsProcessList = jnew JPtrArray<JProcess>(JPtrArrayT::kForgetAll);
	assert( itsProcessList != nullptr );
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SVNPropertiesList::~SVNPropertiesList()
{
	jdelete itsRemovePropertyCmdList;
	jdelete itsProcessList;
}

/******************************************************************************
 GetCommand (static private)

 ******************************************************************************/

JString
SVNPropertiesList::GetCommand
	(
	const JCharacter* fullName
	)
{
	JString cmd = "svn --non-interactive proplist ";
	cmd        += JPrepArgForExec(fullName);
	return cmd;
}

/******************************************************************************
 ShouldDisplayLine (virtual protected)

	Return kJFalse if the line should not be displayed.

 ******************************************************************************/

JBoolean
SVNPropertiesList::ShouldDisplayLine
	(
	JString* line
	)
	const
{
	if (line->BeginsWith(" "))
		{
		line->TrimWhitespace();
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 StyleLine (virtual protected)

 ******************************************************************************/

static const JFontStyle theBoldStyle(kJTrue, kJFalse, 0, kJFalse);

void
SVNPropertiesList::StyleLine
	(
	const JIndex		index,
	const JString&		line,
	const JFontStyle&	errorStyle,
	const JFontStyle&	addStyle,
	const JFontStyle&	removeStyle
	)
{
	for (JSize i=0; i<kSpecialPropertyCount; i++)
		{
		if (line == kSpecialPropertyList[i])
			{
			SetStyle(index, theBoldStyle);
			}
		}
}

/******************************************************************************
 OpenSelectedItems (virtual)

 ******************************************************************************/

void
SVNPropertiesList::OpenSelectedItems()
{
	JTableSelection& s = GetTableSelection();
	JTableSelectionIterator iter(&s);
	JPoint cell;
	JString cmd, prop, file = JPrepArgForExec(itsFullName);
	JSubstitute subst;
	while (iter.Next(&cell))
		{
		const JString* line = (GetStringList()).GetElement(cell.y);

		prop = JPrepArgForExec(*line);

		cmd = kPropEditCmd;
		subst.DefineVariable("prop_name", prop);
		subst.DefineVariable("file_name", file);
		subst.Substitute(&cmd);

		JSimpleProcess::Create(cmd, kJTrue);
		}
}

/******************************************************************************
 CopySelectedItems (virtual protected)

 ******************************************************************************/

void
SVNPropertiesList::CopySelectedItems
	(
	const JBoolean fullPath
	)
{
	JTableSelection& s = GetTableSelection();
	if (!s.HasSelection())
		{
		return;
		}

	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	JTableSelectionIterator iter(&s);
	JPoint cell;
	while (iter.Next(&cell))
		{
		list.Append(*((GetStringList()).GetElement(cell.y)));
		}

	JXTextSelection* data = jnew JXTextSelection(GetDisplay(), list);
	assert( data != nullptr );

	GetSelectionManager()->SetData(kJXClipboardName, data);
}

/******************************************************************************
 GetSelectedFiles (virtual)

 ******************************************************************************/

void
SVNPropertiesList::GetSelectedFiles
	(
	JPtrArray<JString>*	fullNameList,
	const JBoolean		includeDeleted
	)
{
	fullNameList->CleanOut();
}

/******************************************************************************
 ExtractRelativePath (virtual protected)

 ******************************************************************************/

JString
SVNPropertiesList::ExtractRelativePath
	(
	const JString& line
	)
	const
{
	return line;
}

/******************************************************************************
 UpdateActionsMenu (virtual)

 ******************************************************************************/

void
SVNPropertiesList::UpdateActionsMenu
	(
	JXTextMenu* menu
	)
{
	menu->EnableItem(kRefreshCmd);
	menu->EnableItem(kCreatePropertyCmd);

	JTableSelection& s = GetTableSelection();
	if (s.HasSelection())
		{
		menu->EnableItem(kRemoveSelectedPropertiesCmd);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
SVNPropertiesList::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsCreatePropertyDialog && message.Is(JXDialogDirector::kDeactivated))
		{
		const JXDialogDirector::Deactivated* info =
			dynamic_cast<const JXDialogDirector::Deactivated*>(&message);
		assert( info != nullptr );

		if (info->Successful())
			{
			CreateProperty1();
			}

		itsCreatePropertyDialog = nullptr;
		}

	else if (message.Is(JProcess::kFinished))
		{
		const JSize count = itsProcessList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			if (sender == itsProcessList->GetElement(i))
				{
				itsProcessList->RemoveElement(i);
				RemoveNextProperty();
				break;
				}
			}
		}

	else
		{
		SVNListBase::Receive(sender, message);
		}
}

/******************************************************************************
 CreateProperty (virtual)

 ******************************************************************************/

JBoolean
SVNPropertiesList::CreateProperty()
{
	assert( itsCreatePropertyDialog == nullptr );

	itsCreatePropertyDialog =
		jnew JXGetStringDialog(
			GetDirector(), JGetString(kCreatePropertyWindowTitleID),
			JGetString(kCreatePropertyPromptID), "");
	assert( itsCreatePropertyDialog != nullptr );
	ListenTo(itsCreatePropertyDialog);
	itsCreatePropertyDialog->BeginDialog();
	return kJTrue;
}

/******************************************************************************
 CreateProperty1 (private)

 ******************************************************************************/

JBoolean
SVNPropertiesList::CreateProperty1()
{
	assert( itsCreatePropertyDialog != nullptr );

	const JString prop = JPrepArgForExec(itsCreatePropertyDialog->GetString());
	const JString file = JPrepArgForExec(itsFullName);

	JSubstitute subst;
	subst.DefineVariable("prop_name", prop);
	subst.DefineVariable("file_name", file);

	JString cmd = kPropEditCmd;
	subst.Substitute(&cmd);

	JSimpleProcess* p;
	const JError err = JSimpleProcess::Create(&p, cmd, kJTrue);
	if (err.OK())
		{
		itsProcessList->Append(p);
		ListenTo(p);
		return kJTrue;
		}
	else
		{
		err.ReportIfError();
		return kJFalse;
		}
}

/******************************************************************************
 SchedulePropertiesForRemove (virtual)

 ******************************************************************************/

JBoolean
SVNPropertiesList::SchedulePropertiesForRemove()
{
	JTableSelection& s = GetTableSelection();
	JTableSelectionIterator iter(&s);
	JPoint cell;
	JString cmd, prop, file = JPrepArgForExec(itsFullName);
	JSubstitute subst;
	while (iter.Next(&cell))
		{
		const JString* line = (GetStringList()).GetElement(cell.y);

		prop = JPrepArgForExec(*line);

		cmd = kPropRemoveCmd;
		subst.DefineVariable("prop_name", prop);
		subst.DefineVariable("file_name", file);
		subst.Substitute(&cmd);

		itsRemovePropertyCmdList->Append(cmd);
		}

	RemoveNextProperty();
	return kJTrue;
}

/******************************************************************************
 RemoveNextProperty (private)

 ******************************************************************************/

JBoolean
SVNPropertiesList::RemoveNextProperty()
{
	if (!itsRemovePropertyCmdList->IsEmpty())
		{
		const JString cmd = *(itsRemovePropertyCmdList->FirstElement());
		itsRemovePropertyCmdList->DeleteElement(1);

		JSimpleProcess* p;
		const JError err = JSimpleProcess::Create(&p, cmd, kJTrue);
		if (err.OK())
			{
			itsProcessList->Append(p);
			ListenTo(p);
			return kJTrue;
			}
		else
			{
			err.ReportIfError();
			return kJFalse;
			}
		}
	else
		{
		if (GetDirector()->OKToStartActionProcess())
			{
			RefreshContent();
			}

		GetDirector()->ScheduleStatusRefresh();
		return kJTrue;
		}
}
