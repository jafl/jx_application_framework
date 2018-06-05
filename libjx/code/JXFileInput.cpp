/******************************************************************************
 JXFileInput.cpp

	Input field for entering a path + file.

	BASE CLASS = JXInputField

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include <JXFileInput.h>
#include <JXSelectionManager.h>
#include <JXDNDManager.h>
#include <JXPathInput.h>
#include <JXStringCompletionMenu.h>
#include <JXFontManager.h>
#include <JXColorManager.h>
#include <jXGlobals.h>
#include <jXUtil.h>
#include <JDirInfo.h>
#include <jFileUtil.h>
#include <jDirUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JXFileInput::JXFileInput
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
	itsCompleter(NULL),
	itsCompletionMenu(NULL)
{
	itsAllowInvalidFileFlag = kJFalse;
	itsRequireReadFlag      = kJTrue;
	itsRequireWriteFlag     = kJTrue;
	itsRequireExecFlag      = kJFalse;
	itsExpectURLDropFlag    = kJFalse;
	SetIsRequired();
	GetText()->SetCharacterInWordFunction(IsCharacterInWord);
	GetText()->SetDefaultFont(JFontManager::GetDefaultMonospaceFont());
	ShouldBroadcastCaretLocationChanged(kJTrue);
	SetHint(JGetString("Hint::JXFileInput"));
	ListenTo(this);
	ListenTo(this->GetText());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXFileInput::~JXFileInput()
{
	jdelete itsCompleter;
}

/******************************************************************************
 GetFont (static)

 ******************************************************************************/

JFont
JXFileInput::GetFont()
{
	return JFontManager::GetDefaultMonospaceFont();
}

/******************************************************************************
 HandleUnfocusEvent (virtual protected)

 ******************************************************************************/

void
JXFileInput::HandleUnfocusEvent()
{
	JXInputField::HandleUnfocusEvent();
	GoToEndOfLine();
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
JXFileInput::ApertureResized
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
JXFileInput::BoundsMoved
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
JXFileInput::Receive
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
JXFileInput::SetBasePath
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
 GetFile (virtual)

	Returns kJTrue if the current file name is valid.  In this case, *fullName
	is set to the full path + name, relative to the root directory.

	Use this instead of GetText(), because that may return a relative path.

 ******************************************************************************/

JBoolean
JXFileInput::GetFile
	(
	JString* fullName
	)
	const
{
	const JString& text = GetText().GetText();
	return JI2B(!text.IsEmpty() &&
				(JIsAbsolutePath(text) || HasBasePath()) &&
				JConvertToAbsolutePath(text, &itsBasePath, fullName) &&
				JFileExists(*fullName) &&
				(!itsRequireReadFlag  || JFileReadable(*fullName)) &&
				(!itsRequireWriteFlag || JFileWritable(*fullName)) &&
				(!itsRequireExecFlag  || JFileExecutable(*fullName)));
}

/******************************************************************************
 InputValid (virtual)

 ******************************************************************************/

JBoolean
JXFileInput::InputValid()
{
	if (itsAllowInvalidFileFlag)
		{
		return kJTrue;
		}
	else if (!JXInputField::InputValid())
		{
		return kJFalse;
		}

	const JString& text = GetText()->GetText();
	if (text.IsEmpty())
		{
		return !IsRequired();
		}

	JString fullName;
	const JUtf8Byte* errID = NULL;
	if (JIsRelativePath(text) && !HasBasePath())
		{
		errID = "NoRelPath::JXFileInput";
		RecalcAll();
		}
	else if (!JConvertToAbsolutePath(text, &itsBasePath, &fullName) ||
			 !JFileExists(fullName))
		{
		errID = "DoesNotExist::JXFileInput";
		}
	else if (itsRequireReadFlag && !JFileReadable(fullName))
		{
		errID = "Unreadable::JXFileInput";
		}
	else if (itsRequireWriteFlag && !JFileWritable(fullName))
		{
		errID = "Unwritable::JXFileInput";
		}
	else if (itsRequireExecFlag && !JFileExecutable(fullName))
		{
		errID = "CannotExec::JXFileInput";
		}

	if (JString::IsEmpty(errID))
		{
		return kJTrue;
		}
	else
		{
		(JGetUserNotification())->ReportError(JGetString(errID));
		return kJFalse;
		}
}

/******************************************************************************
 GetTextColor (static)

	Draw the entire text red if the file is invalid.  This is provided
	so tables can draw the text the same way as the input field.

	base can be NULL.  If you use JXFileInput for relative paths, basePath
	should be the path passed to SetBasePath().

 ******************************************************************************/

JColorID
JXFileInput::GetTextColor
	(
	const JString&	fileName,
	const JString&	basePath,
	const JBoolean	requireRead,
	const JBoolean	requireWrite,
	const JBoolean	requireExec
	)
{
	if (fileName.IsEmpty())
		{
		return JColorManager::GetBlackColor();
		}

	JString fullName;
	if ((JIsAbsolutePath(fileName) || !basePath.IsEmpty()) &&
		JConvertToAbsolutePath(fileName, &basePath, &fullName) &&
		(!requireRead  || JFileReadable(fullName)) &&
		(!requireWrite || JFileWritable(fullName)) &&
		(!requireExec  || JFileExecutable(fullName)))
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
JXFileInput::WillAcceptDrop
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

	JString fileName;
	const JSize typeCount = typeList.GetElementCount();
	for (JIndex i=1; i<=typeCount; i++)
		{
		if (typeList.GetElement(i) == urlXAtom &&
			GetDroppedFileName(time, kJFalse, &fileName))
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
JXFileInput::HandleDNDEnter()
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
JXFileInput::HandleDNDHere
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
JXFileInput::HandleDNDLeave()
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
JXFileInput::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	JString fileName;
	if (!itsExpectURLDropFlag)
		{
		JXInputField::HandleDNDDrop(pt, typeList, action, time, source);
		}
	else if (Focus() && GetDroppedFileName(time, kJTrue, &fileName))
		{
		GetText()->SetText(fileName);
		}
}

/******************************************************************************
 GetDroppedFileName (private)

 ******************************************************************************/

JBoolean
JXFileInput::GetDroppedFileName
	(
	const Time		time,
	const JBoolean	reportErrors,
	JString*		fileName
	)
{
	fileName->Clear();

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
			if (fileNameList.GetElementCount() == 1 &&
				(JFileExists(*(fileNameList.GetFirstElement())) ||
				 JDirectoryExists(*(fileNameList.GetFirstElement()))))
				{
				*fileName = JConvertToHomeDirShortcut(*(fileNameList.GetFirstElement()));
				}
			JXReportUnreachableHosts(urlList);
			}

		selMgr->DeleteData(&data, delMethod);
		}

	return !fileName->IsEmpty();
}

/******************************************************************************
 GetTextForChooseFile (virtual)

	Returns a string that can safely be passed to JChooseSaveFile::ChooseFile().

 ******************************************************************************/

JString
JXFileInput::GetTextForChooseFile()
	const
{
	JString text = GetText().GetText();
	if (text.IsEmpty() && HasBasePath())
		{
		text = itsBasePath;
		JAppendDirSeparator(&text);
		}
	if (text.EndsWith(ACE_DIRECTORY_SEPARATOR_STR))
		{
		text.Append("*");
		}
	if (!text.IsEmpty() && JIsRelativePath(text) && HasBasePath())
		{
		text = JCombinePathAndName(itsBasePath, text);
		}
	return text;
}

/******************************************************************************
 ChooseFile

	instr can be NULL, just like in JChooseSaveFile::ChooseFile().

 ******************************************************************************/

JBoolean
JXFileInput::ChooseFile
	(
	const JString& prompt,
	const JString& instr
	)
{
	const JString origName = GetTextForChooseFile();
	JString newName;
	if ((JGetChooseSaveFile())->ChooseFile(prompt, instr, origName, &newName))
		{
		GetText()->SetText(newName);
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 SaveFile

	instr can be NULL, just like in JChooseSaveFile::SaveFile().

 ******************************************************************************/

JBoolean
JXFileInput::SaveFile
	(
	const JString& prompt,
	const JString& instr
	)
{
	const JString origName = GetTextForChooseFile();
	JString newName;
	if ((JGetChooseSaveFile())->SaveFile(prompt, instr, origName, &newName))
		{
		GetText()->SetText(newName);
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
JXFileInput::IsCharacterInWord
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
JXFileInput::HandleKeyPress
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
			itsCompleter->ShowVCSDirs(kJFalse);
			itsCompleter->ShowDirs(kJTrue);
			itsCompleter->ShowFiles(kJTrue);
			itsCompleter->ShouldApplyWildcardFilterToDirs();
			}

		JXPathInput::Complete(this, itsBasePath, itsCompleter, &itsCompletionMenu);
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
JXFileInput::HandleMouseDown
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
 AdjustStylesBeforeBroadcast (virtual protected)

	Draw the invalid portion of the path in red.

 ******************************************************************************/

#ifdef _J_UNIX
static const JUtf8Byte* kThisDirSuffix = "/.";
#elif defined WIN32
static const JUtf8Byte* kThisDirSuffix = "\\.";
#endif

void
JXFileInput::StyledText::AdjustStylesBeforeBroadcast
	(
	const JString&			text,
	JRunArray<JFont>*		styles,
	JStyledText::TextRange*	recalcRange,
	JStyledText::TextRange*	redrawRange,
	const JBoolean			deletion
	)
{
	const JSize totalLength = text.GetCharacterCount();

	JString fullName = text;
	if ((JIsRelativePath(text) && !itsField->HasBasePath()) ||
		!JExpandHomeDirShortcut(text, &fullName))
		{
		fullName.Clear();
		}

	// Last clause because if JConvertToAbsolutePath() succeeds, we don't
	// want to further modify fullName.

	else if (JIsRelativePath(text) &&
			 !JConvertToAbsolutePath(text, &itsField->itsBasePath, &fullName))
		{
		if (itsField->HasBasePath())
			{
			fullName = JCombinePathAndName(itsField->itsBasePath, text);
			}
		else
			{
			fullName.Clear();
			}
		}

	JSize errLength;
	if (fullName.IsEmpty())
		{
		errLength = totalLength;
		}
	else if (!JFileExists(fullName) ||
			 (itsField->itsRequireReadFlag  && !JFileReadable(fullName)) ||
			 (itsField->itsRequireWriteFlag && !JFileWritable(fullName)) ||
			 (itsField->itsRequireExecFlag  && !JFileExecutable(fullName)))
		{
		const JString closestDir = JGetClosestDirectory(fullName, kJFalse);
		if (fullName.BeginsWith(closestDir))
			{
			errLength = fullName.GetCharacterCount() - closestDir.GetCharacterCount();
			}
		else
			{
			errLength = totalLength;
			}
		}
	else
		{
		errLength = 0;
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

	*redrawRange = JStyledText::TextRange(
		JCharacterRange(1, totalLength),
		JUtf8ByteRange(1, text.GetByteCount()));
}
