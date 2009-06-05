/******************************************************************************
 JXPathInput.cpp

	Input field for entering a file path.

	BASE CLASS = JXInputField

	Copyright © 1996 by Glenn W. Bach.
	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXPathInput.h>
#include <JXSelectionManager.h>
#include <JXDNDManager.h>
#include <JXStringCompletionMenu.h>
#include <JXColormap.h>
#include <jXGlobals.h>
#include <jXUtil.h>
#include <JDirInfo.h>
#include <jDirUtil.h>
#include <jFileUtil.h>
#include <jAssert.h>

static const JCharacter* kFontName = "6x13";
const JSize kFontSize              = 12;

// string ID's

static const JCharacter* kHintID         = "Hint::JXPathInput";
static const JCharacter* kNoRelPathID    = "NoRelPath::JXPathInput";
static const JCharacter* kInvalidPathID  = "InvalidPath::JXPathInput";
static const JCharacter* kUnreadableID   = "Unreadable::JXPathInput";
static const JCharacter* kUnwritableID   = "Unwritable::JXPathInput";
static const JCharacter* kAccessDeniedID = "AccessDenied::JXPathInput";
static const JCharacter* kBadPathID      = "BadPath::JXPathInput";
static const JCharacter* kCompNotDirID   = "CompNotDir::JXPathInput";
static const JCharacter* kInvalidDirID   = "InvalidDir::JXPathInput";

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
	JXInputField(enclosure, hSizing, vSizing, x,y, w,h),
	itsCompleter(NULL),
	itsCompletionMenu(NULL)
{
	itsAllowInvalidPathFlag = kJFalse;
	itsRequireWriteFlag     = kJFalse;
	itsExpectURLDropFlag    = kJFalse;
	SetIsRequired();
	SetCharacterInWordFunction(IsCharacterInWord);
	SetDefaultFontName(kFontName);
	ShouldBroadcastCaretLocationChanged(kJTrue);
	SetHint(JGetString(kHintID));
	ListenTo(this);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXPathInput::~JXPathInput()
{
	delete itsCompleter;
}

/******************************************************************************
 GetFont (static)

 ******************************************************************************/

const JCharacter*
JXPathInput::GetFont
	(
	JSize* size
	)
{
	*size = kFontSize;
	return kFontName;
}

/******************************************************************************
 HandleUnfocusEvent (virtual protected)

 ******************************************************************************/

void
JXPathInput::HandleUnfocusEvent()
{
	JXInputField::HandleUnfocusEvent();
	SetCaretLocation(GetTextLength() + 1);
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
		SetCaretLocation(GetTextLength() + 1);
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
		SetHint(GetText());
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
	if (sender == this && message.Is(JTextEditor::kTextSet))
		{
		SetCaretLocation(GetTextLength() + 1);
		}
	else if (sender == this && message.Is(JTextEditor::kCaretLocationChanged))
		{
		JIndex i;
		WantInput(kJTrue,
				  JI2B(GetCaretLocation(&i) && i == GetTextLength()+1),
				  WantsModifiedTab());
		}

	JXInputField::Receive(sender, message);
}

/******************************************************************************
 SetBasePath

	This is used if a relative path is entered.  It must be an absolute path.

 ******************************************************************************/

void
JXPathInput::SetBasePath
	(
	const JCharacter* path
	)
{
	if (JStringEmpty(path))
		{
		ClearBasePath();
		}
	else
		{
		assert( !JIsRelativePath(path) );
		itsBasePath = path;
		RecalcAll(kJTrue);
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
	const JString& text = GetText();
	return JI2B(!text.IsEmpty() &&
				(!JIsRelativePath(text) || HasBasePath()) &&
				JConvertToAbsolutePath(text, itsBasePath, path) &&
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

	const JString& text = GetText();
	if (text.IsEmpty())		// paranoia -- JXInputField should have reported
		{
		return !IsRequired();
		}

	JString path;
	if (JIsRelativePath(text) && !HasBasePath())
		{
		(JGetUserNotification())->ReportError(JGetString(kNoRelPathID));
		RecalcAll(kJTrue);
		return kJFalse;
		}
	if (!JConvertToAbsolutePath(text, itsBasePath, &path))
		{
		(JGetUserNotification())->ReportError(JGetString(kInvalidPathID));
		RecalcAll(kJTrue);
		return kJFalse;
		}

	const JString currDir = JGetCurrentDirectory();
	const JError err      = JChangeDirectory(path);
	JChangeDirectory(currDir);

	if (err.OK())
		{
		if (!JDirectoryReadable(path))
			{
			(JGetUserNotification())->ReportError(JGetString(kUnreadableID));
			RecalcAll(kJTrue);
			return kJFalse;
			}
		else if (itsRequireWriteFlag && !JDirectoryWritable(path))
			{
			(JGetUserNotification())->ReportError(JGetString(kUnwritableID));
			RecalcAll(kJTrue);
			return kJFalse;
			}
		else
			{
			return kJTrue;
			}
		}

	const JCharacter* errID;
	if (err == kJAccessDenied)
		{
		errID = kAccessDeniedID;
		}
	else if (err == kJBadPath)
		{
		errID = kBadPathID;
		}
	else if (err == kJComponentNotDirectory)
		{
		errID = kCompNotDirID;
		}
	else
		{
		errID = kInvalidDirID;
		}

	(JGetUserNotification())->ReportError(JGetString(errID));
	RecalcAll(kJTrue);
	return kJFalse;
}

/******************************************************************************
 AdjustStylesBeforeRecalc (virtual protected)

	Draw the invalid portion of the path in red.

 ******************************************************************************/

#ifdef _J_UNIX
static const JCharacter* kThisDirSuffix = "/.";
#elif defined WIN32
static const JCharacter* kThisDirSuffix = "\\.";
#endif

void
JXPathInput::AdjustStylesBeforeRecalc
	(
	const JString&		buffer,
	JRunArray<Font>*	styles,
	JIndexRange*		recalcRange,
	JIndexRange*		redrawRange,
	const JBoolean		deletion
	)
{
	const JColormap* colormap = GetColormap();
	const JSize totalLength   = buffer.GetLength();

	JString fullPath = buffer;
	if ((JIsRelativePath(buffer) && !HasBasePath()) ||
		!JExpandHomeDirShortcut(buffer, &fullPath))
		{
		fullPath.Clear();
		}

	// Last clause because if JConvertToAbsolutePath() succeeds, we don't
	// want to further modify fullName.

	else if (JIsRelativePath(buffer) &&
			 !JConvertToAbsolutePath(buffer, itsBasePath, &fullPath))
		{
		if (HasBasePath())
			{
			fullPath = JCombinePathAndName(itsBasePath, buffer);
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
		const JString closestDir = JGetClosestDirectory(fullPath, itsRequireWriteFlag);
		if (fullPath.BeginsWith(closestDir))
			{
			errLength = fullPath.GetLength() - closestDir.GetLength();
			}
		else
			{
			errLength = totalLength;
			}
		}

	if (errLength > 0 && buffer.EndsWith(kThisDirSuffix))
		{
		errLength++;	// trailing . is trimmed
		}

	Font f = styles->GetFirstElement();

	styles->RemoveAll();
	if (errLength >= totalLength)
		{
		f.style.color = colormap->GetRedColor();
		styles->AppendElements(f, totalLength);
		}
	else
		{
		f.style.color = colormap->GetBlackColor();
		styles->AppendElements(f, totalLength - errLength);

		if (errLength > 0)
			{
			f.style.color = colormap->GetRedColor();
			styles->AppendElements(f, errLength);
			}
		}

	*redrawRange += JIndexRange(1, totalLength);
}

/******************************************************************************
 GetTextColor (static)

	Draw the entire text red if the path is invalid.  This is provided
	so tables can draw the text the same way as the input field.

	base can be NULL.  If you use JXPathInput for relative paths, base
	should be the path passed to SetBasePath().

 ******************************************************************************/

JColorIndex
JXPathInput::GetTextColor
	(
	const JCharacter*	path,
	const JCharacter*	base,
	const JBoolean		requireWrite,
	const JColormap*	colormap
	)
{
	if (JStringEmpty(path))
		{
		return colormap->GetBlackColor();
		}

	JString fullPath;
	if ((!JIsRelativePath(path) || !JStringEmpty(base)) &&
		JConvertToAbsolutePath(path, base, &fullPath) &&
		JDirectoryReadable(fullPath) &&
		JCanEnterDirectory(fullPath) &&
		(!requireWrite || JDirectoryWritable(fullPath)))
		{
		return colormap->GetBlackColor();
		}
	else
		{
		return colormap->GetRedColor();
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

	const Atom urlXAtom = (GetSelectionManager())->GetURLXAtom();

	JString dirName;
	const JSize typeCount = typeList.GetElementCount();
	for (JIndex i=1; i<=typeCount; i++)
		{
		if (typeList.GetElement(i) == urlXAtom &&
			GetDroppedDirectory(time, kJFalse, &dirName))
			{
			*action = (GetDNDManager())->GetDNDActionPrivateXAtom();
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
		SetText(dirName);
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
	if (selMgr->GetData((GetDNDManager())->GetDNDSelectionName(),
						time, selMgr->GetURLXAtom(),
						&returnType, &data, &dataLength, &delMethod))
		{
		if (returnType == selMgr->GetURLXAtom())
			{
			JPtrArray<JString> fileNameList(JPtrArrayT::kDeleteAll),
							   urlList(JPtrArrayT::kDeleteAll);
			JXUnpackFileNames((char*) data, dataLength, &fileNameList, &urlList);
			if (fileNameList.GetElementCount() == 1 &&
				(JDirectoryExists(*(fileNameList.FirstElement())) ||
				 JFileExists(*(fileNameList.FirstElement()))))
				{
				*dirName = *(fileNameList.FirstElement());
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
	JString text = GetText();
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

	instr can be NULL, just like in JChooseSaveFile::Choose*Path().

 ******************************************************************************/

JBoolean
JXPathInput::ChoosePath
	(
	const JCharacter* prompt,
	const JCharacter* instr
	)
{
	JString origPath = GetTextForChoosePath();
	JString newPath;
	if ((itsRequireWriteFlag &&
		 (JGetChooseSaveFile())->ChooseRWPath(prompt, instr, origPath, &newPath)) ||
		(!itsRequireWriteFlag &&
		 (JGetChooseSaveFile())->ChooseRPath(prompt, instr, origPath, &newPath)))
		{
		SetText(newPath);
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
	const JString&	text,
	const JIndex	charIndex
	)
{
	return JI2B( text.GetCharacter(charIndex) != ACE_DIRECTORY_SEPARATOR_CHAR );
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
		if (itsCompleter == NULL)
			{
			if (!JDirInfo::Create(JGetRootDirectory(), &itsCompleter))
				{
				return;
				}
			itsCompleter->ShowHidden(kJTrue);
			itsCompleter->ShowDirs(kJTrue);
			itsCompleter->ShowFiles(kJFalse);
			itsCompleter->ShouldApplyWildcardFilterToDirs();
			}

		Complete(this, itsBasePath, itsCompleter, &itsCompletionMenu);
		}
	else
		{
		if (itsCompletionMenu != NULL)
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
	if (itsCompletionMenu != NULL)
		{
		itsCompletionMenu->ClearRequestCount();
		}
	JXInputField::HandleMouseDown(pt, button, clickCount, buttonStates, modifiers);
}

/******************************************************************************
 Complete (static)

	Returns kJTrue if the path or file name was completed.

	basePath can be NULL.  *menu will be constructed if it is NULL.

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
	const JCharacter*			basePath,	// can be NULL
	JDirInfo*					completer,
	JXStringCompletionMenu**	menu		// constructed if NULL
	)
{
	// only complete if caret is at end of text

	JIndex caretIndex;
	if (!te->GetCaretLocation(&caretIndex) ||
		caretIndex != te->GetTextLength()+1)
		{
		return kJFalse;
		}

	// catch empty path

	if (te->IsEmpty())
		{
		const JString path = JGetRootDirectory();
		te->Paste(path);
		return kJTrue;
		}

	// convert to absolute path

	JString fullName;
	if (!JExpandHomeDirShortcut(te->GetText(), &fullName))
		{
		return kJFalse;
		}
	if (JIsRelativePath(fullName))
		{
		if (JStringEmpty(basePath))
			{
			return kJFalse;
			}
		fullName = JCombinePathAndName(basePath, fullName);
		}

	// if completing ~ rather than ~/

	if (fullName.EndsWith(ACE_DIRECTORY_SEPARATOR_STR) &&
		!(te->GetText()).EndsWith(ACE_DIRECTORY_SEPARATOR_STR))
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
		name.AppendCharacter('*');
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

	const JSize matchCount = completer->GetEntryCount();
	JString entryName;
	for (JIndex i=2; i<=matchCount; i++)
		{
		entryName                = jGetFullName(completer, i);
		const JSize matchLength  = JCalcMatchLength(maxPrefix, entryName);
		const JSize prefixLength = maxPrefix.GetLength();
		if (matchLength < prefixLength)
			{
			maxPrefix.RemoveSubstring(matchLength+1, prefixLength);
			}
		}

	// use the completion list

	if (matchCount > 0 &&
		maxPrefix.GetLength() > fullName.GetLength())
		{
		maxPrefix.RemoveSubstring(1, fullName.GetLength());
		if (matchCount == 1 && (completer->GetEntry(1)).IsDirectory())
			{
			JAppendDirSeparator(&maxPrefix);
			}
		te->Paste(maxPrefix);		// so Undo removes completion

		if (*menu != NULL)
			{
			(**menu).ClearRequestCount();
			}

		return kJTrue;
		}
	else if (matchCount > 1)
		{
		if (*menu == NULL)
			{
			*menu = new JXStringCompletionMenu(te, kJFalse);
			assert( *menu != NULL );
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

		(**menu).CompletionRequested(name.GetLength()-1);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}
