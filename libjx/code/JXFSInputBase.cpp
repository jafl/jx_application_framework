/******************************************************************************
 JXFSInputBase.cpp

	Base class for input field that accepts a file system object (file or
	directory).

	BASE CLASS = JXInputField

	Copyright (C) 2018 by John Lindal.

 ******************************************************************************/

#include "JXFSInputBase.h"
#include "JXStringCompletionMenu.h"
#include "JXUrgentFunctionTask.h"
#include "jXUtil.h"
#include <jx-af/jcore/JFontManager.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

JXFSInputBase::JXFSInputBase
	(
	StyledText*			text,
	const bool			showFilesForCompletion,
	const JUtf8Byte*	defaultHintID,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXInputField(text, enclosure, hSizing, vSizing, x,y, w,h),
	itsExpectURLDropFlag(false),
	itsCompleter(nullptr),
	itsCompletionMenu(nullptr),
	itsShowFilesForCompletionFlag(showFilesForCompletion),
	itsDefaultHintID(defaultHintID)
{
	SetIsRequired();
	GetText()->SetCharacterInWordFunction(IsCharacterInWord);
	GetText()->SetDefaultFont(JFontManager::GetDefaultMonospaceFont());
	SetHint(JGetString(itsDefaultHintID));
	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFSInputBase::~JXFSInputBase()
{
	jdelete itsCompleter;
}

/******************************************************************************
 GetFont (static)

 ******************************************************************************/

JFont
JXFSInputBase::GetFont()
{
	return JFontManager::GetDefaultMonospaceFont();
}

/******************************************************************************
 HandleUnfocusEvent (virtual protected)

 ******************************************************************************/

void
JXFSInputBase::HandleUnfocusEvent()
{
	JXInputField::HandleUnfocusEvent();
	GoToEndOfLine();
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
JXFSInputBase::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXInputField::ApertureResized(dw,dh);
	if (!HasFocus())
	{
		GoToEndOfLine();
	}
}

/******************************************************************************
 BoundsMoved (virtual protected)

	Show the full text in the hint if part of it is not visible in the widget.

 ******************************************************************************/

void
JXFSInputBase::BoundsMoved
	(
	const JCoordinate dx,
	const JCoordinate dy
	)
{
	JXInputField::BoundsMoved(dx, dy);

	const JRect ap = GetAperture();
	if (ap.left != 0)
	{
		SetHint(GetText()->GetText());
	}
	else
	{
		SetHint(JGetString(itsDefaultHintID));
	}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXFSInputBase::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXInputField::Receive(sender, message);		// update JStyledText first

	if (sender == this && message.Is(JStyledText::kTextSet))
	{
		GoToEndOfLine();
	}
	else if (sender == this && message.Is(JTextEditor::kCaretLocationChanged))
	{
		auto* task = jnew JXUrgentFunctionTask(this, [this]()
		{
			JIndex i;
			WantInput(true,
					  GetCaretLocation(&i) && i == GetText()->GetText().GetCharacterCount()+1,
					  WantsModifiedTab());
		},
		"JXFSInputBase::WantInput");
		task->Go();
	}
}

/******************************************************************************
 SetBasePath

	This is used if a relative path is entered.  It must be an absolute path.

 ******************************************************************************/

void
JXFSInputBase::SetBasePath
	(
	const JString& path
	)
{
	if (path.IsEmpty())
	{
		ClearBasePath();
	}
	else
	{
		assert( JIsAbsolutePath(path) );
		itsBasePath = path;
		GetText()->RestyleAll();
	}
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

	Accept text/uri-list.

 ******************************************************************************/

bool
JXFSInputBase::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	itsExpectURLDropFlag = false;

	const Atom urlXAtom = GetSelectionManager()->GetURLXAtom();

	JString name;
	for (const auto type : typeList)
	{
		if (type == urlXAtom && GetDroppedEntry(time, false, &name))
		{
			*action = GetDNDManager()->GetDNDActionPrivateXAtom();
			itsExpectURLDropFlag = true;
			return true;
		}
	}

	return JXInputField::WillAcceptDrop(typeList, action, pt, time, source);
}

/******************************************************************************
 HandleDNDEnter (virtual protected)

	This is called when the mouse enters the widget.

 ******************************************************************************/

void
JXFSInputBase::HandleDNDEnter()
{
	if (!itsExpectURLDropFlag)
	{
		JXInputField::HandleDNDEnter();
	}
}

/******************************************************************************
 HandleDNDHere (virtual protected)

	This is called while the mouse is inside the widget.

 ******************************************************************************/

void
JXFSInputBase::HandleDNDHere
	(
	const JPoint&	pt,
	const JXWidget*	source
	)
{
	if (!itsExpectURLDropFlag)
	{
		JXInputField::HandleDNDHere(pt, source);
	}
}

/******************************************************************************
 HandleDNDLeave (virtual protected)

	This is called when the mouse leaves the widget without dropping data.

 ******************************************************************************/

void
JXFSInputBase::HandleDNDLeave()
{
	if (!itsExpectURLDropFlag)
	{
		JXInputField::HandleDNDLeave();
	}
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

	This is called when the data is dropped.  The data is accessed via
	the selection manager, just like Paste.

	Since we only accept text/uri-list, we don't bother to check typeList.

 ******************************************************************************/

void
JXFSInputBase::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	JString name;
	if (!itsExpectURLDropFlag)
	{
		JXInputField::HandleDNDDrop(pt, typeList, action, time, source);
	}
	else if (Focus() && GetDroppedEntry(time, true, &name))
	{
		GetText()->SetText(name);
	}
}

/******************************************************************************
 GetDroppedEntry (protected)

 ******************************************************************************/

bool
JXFSInputBase::GetDroppedEntry
	(
	const Time		time,
	const bool	reportErrors,
	JString*		name
	)
{
	name->Clear();

	JXSelectionManager* selMgr = GetSelectionManager();

	Atom returnType;
	unsigned char* data;
	JSize dataLength;
	JXSelectionManager::DeleteMethod delMethod;
	if (selMgr->GetData(GetDNDManager()->GetDNDSelectionName(),
						time, selMgr->GetURLXAtom(),
						&returnType, &data, &dataLength, &delMethod))
	{
		if (returnType == selMgr->GetURLXAtom())
		{
			JPtrArray<JString> fileNameList(JPtrArrayT::kDeleteAll),
							   urlList(JPtrArrayT::kDeleteAll);
			JXUnpackFileNames((char*) data, dataLength, &fileNameList, &urlList);
			if (fileNameList.GetItemCount() == 1 &&
				(JDirectoryExists(*(fileNameList.GetFirstItem())) ||
				 JFileExists(*(fileNameList.GetFirstItem()))))
			{
				*name = JConvertToHomeDirShortcut(*(fileNameList.GetFirstItem()));
			}
			JXReportUnreachableHosts(urlList);
		}

		selMgr->DeleteData(&data, delMethod);
	}

	return !name->IsEmpty();
}

/******************************************************************************
 IsCharacterInWord (static)

 ******************************************************************************/

bool
JXFSInputBase::IsCharacterInWord
	(
	const JUtf8Character& c
	)
{
	return c != ACE_DIRECTORY_SEPARATOR_CHAR;
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
JXFSInputBase::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	if (c == '\t')
	{
		if (itsCompleter == nullptr)
		{
			if (!JDirInfo::Create(JGetRootDirectory(), &itsCompleter))
			{
				return;
			}
			itsCompleter->ShowHidden(true);
			itsCompleter->ShowVCSDirs(false);
			itsCompleter->ShowDirs(true);
			itsCompleter->ShowFiles(itsShowFilesForCompletionFlag);
			itsCompleter->ShouldApplyWildcardFilterToDirs();
		}

		Complete(this, itsBasePath, itsCompleter, &itsCompletionMenu);
	}
	else
	{
		if (itsCompletionMenu != nullptr)
		{
			itsCompletionMenu->ClearRequestCount();
		}
		JXInputField::HandleKeyPress(c, keySym, modifiers);
	}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXFSInputBase::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsCompletionMenu != nullptr)
	{
		itsCompletionMenu->ClearRequestCount();
	}
	JXInputField::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);
}

/******************************************************************************
 Complete (static)

	Returns true if the path or file name was completed.

	basePath can be nullptr.  *menu will be constructed if it is nullptr.

 ******************************************************************************/

inline JString
jGetFullName
	(
	const JDirInfo*	completer,
	const JIndex	index
	)
{
	const JDirEntry& e = completer->GetEntry(index);
	JString fullName   = e.GetFullName();
	if (e.IsDirectory())
	{
		JAppendDirSeparator(&fullName);
	}
	return fullName;
}

inline JString
jGetName
	(
	const JDirInfo*	completer,
	const JIndex	index
	)
{
	const JDirEntry& e = completer->GetEntry(index);
	JString name       = e.GetName();
	if (e.IsDirectory())
	{
		JAppendDirSeparator(&name);
	}
	return name;
}

bool
JXFSInputBase::Complete
	(
	JXInputField*				te,
	const JString&				basePath,	// can be empty
	JDirInfo*					completer,
	JXStringCompletionMenu**	menu		// constructed if nullptr
	)
{
	// only complete if caret is at end of text

	JIndex caretIndex;
	if (!te->GetCaretLocation(&caretIndex) ||
		caretIndex != te->GetText()->GetText().GetCharacterCount()+1)
	{
		return false;
	}

	// catch empty path

	if (te->GetText()->IsEmpty())
	{
		const JString path = JGetRootDirectory();
		te->Paste(path);
		return true;
	}

	// convert to absolute path

	JString fullName;
	if (!JExpandHomeDirShortcut(te->GetText()->GetText(), &fullName))
	{
		return false;
	}
	if (JIsRelativePath(fullName))
	{
		if (basePath.IsEmpty())
		{
			return false;
		}
		fullName = JCombinePathAndName(basePath, fullName);
	}

	// if completing ~ rather than ~/

	if (fullName.EndsWith(ACE_DIRECTORY_SEPARATOR_STR) &&
		!te->GetText()->GetText().EndsWith(ACE_DIRECTORY_SEPARATOR_STR))
	{
		JStripTrailingDirSeparator(&fullName);
	}

	// get path and wildcard filter

	JString path, name;
	if (fullName.EndsWith(ACE_DIRECTORY_SEPARATOR_STR))
	{
		path = fullName;
		name = "*";
	}
	else
	{
		JSplitPathAndName(fullName, &path, &name);
		name.Append("*");
	}

	// build completion list

	if (!(completer->GoTo(path)).OK())
	{
		return false;
	}

	completer->SetWildcardFilter(name, false, true);
	if (completer->IsEmpty())
	{
		return false;
	}

	// check for characters common to all matches

	JString maxPrefix = jGetFullName(completer, 1);
	JStringIterator iter(&maxPrefix);

	const JSize matchCount = completer->GetEntryCount();
	JString entryName;
	for (JIndex i=2; i<=matchCount; i++)
	{
		entryName               = jGetFullName(completer, i);
		const JSize matchLength = JString::CalcCharacterMatchLength(maxPrefix, entryName);
		if (matchLength < maxPrefix.GetCharacterCount())
		{
			iter.MoveTo(JStringIterator::kStartAfterChar, matchLength);
			iter.RemoveAllNext();
		}
	}

	// use the completion list

	if (matchCount > 0 &&
		maxPrefix.GetCharacterCount() > fullName.GetCharacterCount())
	{
		iter.MoveTo(JStringIterator::kStartAfterChar, fullName.GetCharacterCount());
		iter.RemoveAllPrev();
		if (matchCount == 1 && (completer->GetEntry(1)).IsDirectory())
		{
			JAppendDirSeparator(&maxPrefix);	// invalidates iter
		}
		iter.Invalidate();			// avoid double iterator
		te->Paste(maxPrefix);		// so Undo removes only completion

		if (*menu != nullptr)
		{
			(**menu).ClearRequestCount();
		}

		return true;
	}
	else if (matchCount > 1)
	{
		iter.Invalidate();

		if (*menu == nullptr)
		{
			*menu = jnew JXStringCompletionMenu(te, false);
		}
		else
		{
			(**menu).RemoveAllItems();
		}

		for (JIndex i=1; i<=matchCount; i++)
		{
			entryName = jGetName(completer, i);
			(**menu).AddString(entryName);
		}

		(**menu).CompletionRequested(name.GetCharacterCount()-1);
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 AdjustStylesBeforeBroadcast (virtual protected)

	Draw the invalid portion of the path in red.

 ******************************************************************************/

#ifdef _J_UNIX
static const JUtf8Byte* kThisDirSuffix = "/.";
#elif defined WIN32
static const JUtf8Byte* kThisDirSuffix = "\\.";
#endif

void
JXFSInputBase::StyledText::AdjustStylesBeforeBroadcast
	(
	const JString&			text,
	JRunArray<JFont>*		styles,
	JStyledText::TextRange*	recalcRange,
	JStyledText::TextRange*	redrawRange,
	const bool			deletion
	)
{
	const JSize totalLength = text.GetCharacterCount();

	JString fullPath = text;
	if ((JIsRelativePath(text) && !itsField->HasBasePath()) ||
		!JExpandHomeDirShortcut(text, &fullPath))
	{
		fullPath.Clear();
	}

	// Last clause because if JConvertToAbsolutePath() succeeds, we don't
	// want to further modify fullName.

	else if (JIsRelativePath(text) &&
			 !JConvertToAbsolutePath(text, itsField->itsBasePath, &fullPath))
	{
		if (itsField->HasBasePath())
		{
			fullPath = JCombinePathAndName(itsField->itsBasePath, text);
		}
		else
		{
			fullPath.Clear();
		}
	}

	JSize errLength = ComputeErrorLength(itsField, totalLength, fullPath);

	if (errLength > 0 && text.EndsWith(kThisDirSuffix))
	{
		errLength++;	// trailing . is trimmed
	}

	JFont f = styles->GetFirstItem();

	styles->RemoveAll();
	if (errLength >= totalLength)
	{
		f.SetColor(JColorManager::GetRedColor());
		styles->AppendItems(f, totalLength);
	}
	else
	{
		f.SetColor(JColorManager::GetBlackColor());
		styles->AppendItems(f, totalLength - errLength);

		if (errLength > 0)
		{
			f.SetColor(JColorManager::GetRedColor());
			styles->AppendItems(f, errLength);
		}
	}

	*recalcRange = *redrawRange = JStyledText::TextRange(
		JCharacterRange(1, totalLength),
		JUtf8ByteRange(1, text.GetByteCount()));
}
