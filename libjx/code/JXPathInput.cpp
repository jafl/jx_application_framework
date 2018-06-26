/******************************************************************************
 JXPathInput.cpp

	Input field for entering a file path.

	BASE CLASS = JXInputField

	Copyright (C) 1996 by Glenn W. Bach.
	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#include "JXPathInput.h"
#include "JXSelectionManager.h"
#include "JXDNDManager.h"
#include "JXStringCompletionMenu.h"
#include "JXFontManager.h"
#include "JXColorManager.h"
#include "jXGlobals.h"
#include "jXUtil.h"
#include <JStringIterator.h>
#include <JDirInfo.h>
#include <jDirUtil.h>
#include <jFileUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXPathInput::JXPathInput
	(
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXInputField(jnew StyledText(this, enclosure->GetFontManager()),
				 enclosure, hSizing, vSizing, x,y, w,h),
	itsCompleter(nullptr),
	itsCompletionMenu(nullptr)
{
	itsAllowInvalidPathFlag = kJFalse;
	itsRequireWriteFlag     = kJFalse;
	itsExpectURLDropFlag    = kJFalse;
	SetIsRequired();
	GetText()->SetCharacterInWordFunction(IsCharacterInWord);
	GetText()->SetDefaultFont(JFontManager::GetDefaultMonospaceFont());
	ShouldBroadcastCaretLocationChanged(kJTrue);
	SetHint(JGetString("Hint::JXPathInput"));
	ListenTo(this);
	ListenTo(this->GetText());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXPathInput::~JXPathInput()
{
	jdelete itsCompleter;
}

/******************************************************************************
 GetFont (static)

 ******************************************************************************/

JFont
JXPathInput::GetFont()
{
	return JFontManager::GetDefaultMonospaceFont();
}

/******************************************************************************
 HandleUnfocusEvent (virtual protected)

 ******************************************************************************/

void
JXPathInput::HandleUnfocusEvent()
{
	JXInputField::HandleUnfocusEvent();
	GoToEndOfLine();
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
JXPathInput::ApertureResized
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
JXPathInput::BoundsMoved
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
		ClearHint();
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXPathInput::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	JXInputField::Receive(sender, message);		// update JStyledText first

	if (sender == this->GetText() && message.Is(JStyledText::kTextSet))
		{
		GoToEndOfLine();
		}
	else if (sender == this && message.Is(JTextEditor::kCaretLocationChanged))
		{
		JIndex i;
		WantInput(kJTrue,
				  JI2B(GetCaretLocation(&i) && i == GetText()->GetText().GetCharacterCount()+1),
				  WantsModifiedTab());
		}
}

/******************************************************************************
 SetBasePath

	This is used if a relative path is entered.  It must be an absolute path.

 ******************************************************************************/

void
JXPathInput::SetBasePath
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
		RecalcAll();
		}
}

/******************************************************************************
 GetPath (virtual)

	Returns kJTrue if the current path is valid.  In this case, *path is
	set to the full path, relative to the root directory.

	Use this instead of GetText(), because that may return a relative path.

 ******************************************************************************/

JBoolean
JXPathInput::GetPath
	(
	JString* path
	)
	const
{
	const JString& text = GetText().GetText();
	return JI2B(!text.IsEmpty() &&
				(JIsAbsolutePath(text) || HasBasePath()) &&
				JConvertToAbsolutePath(text, &itsBasePath, path) &&
				JDirectoryExists(*path) &&
				JDirectoryReadable(*path) &&
				JCanEnterDirectory(*path) &&
				(!itsRequireWriteFlag || JDirectoryWritable(*path)));
}

/******************************************************************************
 InputValid (virtual)

 ******************************************************************************/

JBoolean
JXPathInput::InputValid()
{
	if (itsAllowInvalidPathFlag)
		{
		return kJTrue;
		}
	else if (!JXInputField::InputValid())
		{
		return kJFalse;
		}

	const JString& text = GetText()->GetText();
	if (text.IsEmpty())		// paranoia -- JXInputField should have reported
		{
		return !IsRequired();
		}

	JString path;
	if (JIsRelativePath(text) && !HasBasePath())
		{
		(JGetUserNotification())->ReportError(JGetString("NoRelPath::JXPathInput"));
		RecalcAll();
		return kJFalse;
		}
	if (!JConvertToAbsolutePath(text, &itsBasePath, &path))
		{
		(JGetUserNotification())->ReportError(JGetString("InvalidDir::JXPathInput"));
		RecalcAll();
		return kJFalse;
		}

	const JString currDir = JGetCurrentDirectory();
	const JError err      = JChangeDirectory(path);
	JChangeDirectory(currDir);

	if (err.OK())
		{
		if (!JDirectoryReadable(path))
			{
			(JGetUserNotification())->ReportError(JGetString("Unreadable::JXPathInput"));
			RecalcAll();
			return kJFalse;
			}
		else if (itsRequireWriteFlag && !JDirectoryWritable(path))
			{
			(JGetUserNotification())->ReportError(JGetString("DirNotWritable::JXGlobal"));
			RecalcAll();
			return kJFalse;
			}
		else
			{
			return kJTrue;
			}
		}

	const JUtf8Byte* errID;
	if (err == kJAccessDenied)
		{
		errID = "AccessDenied::JXPathInput";
		}
	else if (err == kJBadPath)
		{
		errID = "DirectoryDoesNotExist::JXGlobal";
		}
	else if (err == kJComponentNotDirectory)
		{
		errID = "CompNotDir::JXPathInput";
		}
	else
		{
		errID = "InvalidDir::JXPathInput";
		}

	(JGetUserNotification())->ReportError(JGetString(errID));
	RecalcAll();
	return kJFalse;
}

/******************************************************************************
 GetTextColor (static)

	Draw the entire text red if the path is invalid.  This is provided
	so tables can draw the text the same way as the input field.

	base can be nullptr.  If you use JXPathInput for relative paths, base
	should be the path passed to SetBasePath().

 ******************************************************************************/

JColorID
JXPathInput::GetTextColor
	(
	const JString&	path,
	const JString&	base,
	const JBoolean	requireWrite
	)
{
	if (path.IsEmpty())
		{
		return JColorManager::GetBlackColor();
		}

	JString fullPath;
	if ((JIsAbsolutePath(path) || !base.IsEmpty()) &&
		JConvertToAbsolutePath(path, &base, &fullPath) &&
		JDirectoryReadable(fullPath) &&
		JCanEnterDirectory(fullPath) &&
		(!requireWrite || JDirectoryWritable(fullPath)))
		{
		return JColorManager::GetBlackColor();
		}
	else
		{
		return JColorManager::GetRedColor();
		}
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

	Accept text/uri-list.

 ******************************************************************************/

JBoolean
JXPathInput::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	itsExpectURLDropFlag = kJFalse;

	const Atom urlXAtom = GetSelectionManager()->GetURLXAtom();

	JString dirName;
	for (const Atom type : typeList)
		{
		if (type == urlXAtom && GetDroppedDirectory(time, kJFalse, &dirName))
			{
			*action = GetDNDManager()->GetDNDActionPrivateXAtom();
			itsExpectURLDropFlag = kJTrue;
			return kJTrue;
			}
		}

	return JXInputField::WillAcceptDrop(typeList, action, pt, time, source);
}

/******************************************************************************
 HandleDNDEnter (virtual protected)

	This is called when the mouse enters the widget.

 ******************************************************************************/

void
JXPathInput::HandleDNDEnter()
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
JXPathInput::HandleDNDHere
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
JXPathInput::HandleDNDLeave()
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
JXPathInput::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	JString dirName;
	if (!itsExpectURLDropFlag)
		{
		JXInputField::HandleDNDDrop(pt, typeList, action, time, source);
		}
	else if (Focus() && GetDroppedDirectory(time, kJTrue, &dirName))
		{
		GetText()->SetText(dirName);
		}
}

/******************************************************************************
 GetDroppedDirectory (private)

 ******************************************************************************/

JBoolean
JXPathInput::GetDroppedDirectory
	(
	const Time		time,
	const JBoolean	reportErrors,
	JString*		dirName
	)
{
	dirName->Clear();

	JXSelectionManager* selMgr = GetSelectionManager();

	Atom returnType;
	unsigned char* data;
	JSize dataLength;
	JXSelectionManager::DeleteMethod delMethod;
	if (selMgr->GetData(GetDNDManager()->GetDNDSelectionName(),
						time, GetSelectionManager()->GetURLXAtom(),
						&returnType, &data, &dataLength, &delMethod))
		{
		if (returnType == GetSelectionManager()->GetURLXAtom())
			{
			JPtrArray<JString> fileNameList(JPtrArrayT::kDeleteAll),
							   urlList(JPtrArrayT::kDeleteAll);
			JXUnpackFileNames((char*) data, dataLength, &fileNameList, &urlList);
			if (fileNameList.GetElementCount() == 1 &&
				(JDirectoryExists(*(fileNameList.GetFirstElement())) ||
				 JFileExists(*(fileNameList.GetFirstElement()))))
				{
				*dirName = JConvertToHomeDirShortcut(*(fileNameList.GetFirstElement()));
				}
			JXReportUnreachableHosts(urlList);
			}

		selMgr->DeleteData(&data, delMethod);
		}

	return !dirName->IsEmpty();
}

/******************************************************************************
 GetTextForChoosePath (virtual)

	Returns a string that can safely be passed to JChooseSaveFile::ChooseFile().

 ******************************************************************************/

JString
JXPathInput::GetTextForChoosePath()
	const
{
	JString text = GetText().GetText();
	if (text.IsEmpty() && HasBasePath())
		{
		text = itsBasePath;
		}
	else if (!text.IsEmpty() && JIsRelativePath(text) && HasBasePath())
		{
		text = JCombinePathAndName(itsBasePath, text);
		}
	return text;
}

/******************************************************************************
 ChoosePath

	Calls either ChooseRPath() or ChooseRWPath(), as appropriate based
	on itsRequireWriteFlag.

	instr can be nullptr, just like in JChooseSaveFile::Choose*Path().

 ******************************************************************************/

JBoolean
JXPathInput::ChoosePath
	(
	const JString& prompt,
	const JString& instr
	)
{
	JString origPath = GetTextForChoosePath();
	JString newPath;
	if ((itsRequireWriteFlag &&
		 (JGetChooseSaveFile())->ChooseRWPath(prompt, instr, origPath, &newPath)) ||
		(!itsRequireWriteFlag &&
		 (JGetChooseSaveFile())->ChooseRPath(prompt, instr, origPath, &newPath)))
		{
		GetText()->SetText(newPath);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 IsCharacterInWord (static)

 ******************************************************************************/

JBoolean
JXPathInput::IsCharacterInWord
	(
	const JUtf8Character& c
	)
{
	return JI2B( c != ACE_DIRECTORY_SEPARATOR_CHAR );
}

/******************************************************************************
 HandleKeyPress (virtual)

 ******************************************************************************/

void
JXPathInput::HandleKeyPress
	(
	const int				key,
	const JXKeyModifiers&	modifiers
	)
{
	if (key == '\t')
		{
		if (itsCompleter == nullptr)
			{
			if (!JDirInfo::Create(JGetRootDirectory(), &itsCompleter))
				{
				return;
				}
			itsCompleter->ShowHidden(kJTrue);
			itsCompleter->ShowVCSDirs(kJFalse);
			itsCompleter->ShowDirs(kJTrue);
			itsCompleter->ShowFiles(kJFalse);
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
		JXInputField::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXPathInput::HandleMouseDown
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

	Returns kJTrue if the path or file name was completed.

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

JBoolean
JXPathInput::Complete
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
		return kJFalse;
		}

	// catch empty path

	if (te->GetText()->IsEmpty())
		{
		const JString path = JGetRootDirectory();
		te->Paste(path);
		return kJTrue;
		}

	// convert to absolute path

	JString fullName;
	if (!JExpandHomeDirShortcut(te->GetText()->GetText(), &fullName))
		{
		return kJFalse;
		}
	if (JIsRelativePath(fullName))
		{
		if (basePath.IsEmpty())
			{
			return kJFalse;
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
		return kJFalse;
		}

	completer->SetWildcardFilter(name, kJFalse, kJTrue);
	if (completer->IsEmpty())
		{
		return kJFalse;
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
			iter.MoveTo(kJIteratorStartAfter, matchLength);
			iter.RemoveAllNext();
			}
		}

	// use the completion list

	if (matchCount > 0 &&
		maxPrefix.GetCharacterCount() > fullName.GetCharacterCount())
		{
		iter.MoveTo(kJIteratorStartAfter, fullName.GetCharacterCount());
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

		return kJTrue;
		}
	else if (matchCount > 1)
		{
		if (*menu == nullptr)
			{
			*menu = jnew JXStringCompletionMenu(te, kJFalse);
			assert( *menu != nullptr );
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
		return kJTrue;
		}
	else
		{
		return kJFalse;
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
JXPathInput::StyledText::AdjustStylesBeforeBroadcast
	(
	const JString&			text,
	JRunArray<JFont>*		styles,
	JStyledText::TextRange*	recalcRange,
	JStyledText::TextRange*	redrawRange,
	const JBoolean			deletion
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
			 !JConvertToAbsolutePath(text, &itsField->itsBasePath, &fullPath))
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

	JSize errLength;
	if (fullPath.IsEmpty())
		{
		errLength = totalLength;
		}
	else
		{
		const JString closestDir = JGetClosestDirectory(fullPath, itsField->itsRequireWriteFlag);
		if (fullPath.BeginsWith(closestDir))
			{
			errLength = fullPath.GetCharacterCount() - closestDir.GetCharacterCount();
			}
		else
			{
			errLength = totalLength;
			}
		}

	if (errLength > 0 && text.EndsWith(kThisDirSuffix))
		{
		errLength++;	// trailing . is trimmed
		}

	JFont f = styles->GetFirstElement();

	styles->RemoveAll();
	if (errLength >= totalLength)
		{
		f.SetColor(JColorManager::GetRedColor());
		styles->AppendElements(f, totalLength);
		}
	else
		{
		f.SetColor(JColorManager::GetBlackColor());
		styles->AppendElements(f, totalLength - errLength);

		if (errLength > 0)
			{
			f.SetColor(JColorManager::GetRedColor());
			styles->AppendElements(f, errLength);
			}
		}

	*recalcRange = *redrawRange = JStyledText::TextRange(
		JCharacterRange(1, totalLength),
		JUtf8ByteRange(1, text.GetByteCount()));
}
