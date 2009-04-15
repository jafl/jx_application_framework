/******************************************************************************
 JCoreLibVersion.h

	Defines current version of JCore library

	Copyright © 1996-2008 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JCoreLibVersion
#define _H_JCoreLibVersion

// These have to be #defined so they can be used for conditional compilation.

#define CURRENT_JCORE_MAJOR_VERSION	2
#define CURRENT_JCORE_MINOR_VERSION	5
#define CURRENT_JCORE_PATCH_VERSION	0

// This is mainly provided so programmers can see the official version number.

static const char* kCurrentJCoreLibVersionStr = "2.5.0";

// version 2.6.0:
//	JOrderedSet:
//		Removed const from GetElement(), GetElementFromEnd(),
//		GetFirstElement(), GetLastElement() return values.
//	JArray:
//		Fixed SearchSorted1() to work with very large indices.
//		Removed const from GetElement() return value.
//	JLinkedList:
//		Removed const from GetElement() return value.
//	JOutPipeStream:
//		Fixed bug so close() closes the file descriptor, when appropriate.
//	JTreeList:
//		Added MakeVisible().
//	JTreeNode:
//		Added GetChildCompareFunction().
//		Added optional argument propagate to SortChildren().
//		Fixed IsRoot() to return kJTrue if not in a JTree and itsParent == NULL.
//	jProcessUtil:
//		Added JSetProcessPriority().
//		Added versions of JExecute() which specify workingDirectory.
//	JProcess:
//		Added SetPriority().
//		Added constructors which specify workingDirectory.
//		*** WaitUntilFinished() now broadcasts and does cleanup.
//			If you were relying on JSimpleProcess to not delete itself in
//			this situation, even with deleteWhenFinished==kJTrue, simply
//			pass deleteWhenFinished=kJFalse to JSimpleProcess::Create().
//	JSimpleProcess:
//		Added constructors which specify workingDirectory.
//		Fixed ReportError() so it flushes the input buffer before
//			displaying the message.  Line breaks are now respected.
//	JThisProcess:
//		Fixed Fork() so it re-initializes JThisProcess in child with correct pid.
//		Fixed Quit/KillAtExit() to remove the process from the Kill/Quit list.
//		Added CheckACEReactor().
//	jStreamUtil:
//		Added JWaitForInput().
//	JMDIServer:
//		Added virtual protected PreprocessArgList().
//	JTEUndoBase:
//		Removed CastTo*() since dynamic_cast<> now does the job.
//	jStreamUtil:
//		Added JEncode/DecodeBase64().
//	JString:
//		Added Encode/DecodeBase64().
//	jVCSUtil:
//		Fixed JUpdateCVSIgnore() to not create file if using SVN or SCCS.
//		Added JGetVCSRepositoryPath().
//		Added kJGitType.
//	JTextEditor:
//		Added boolean deleteToTabStop argument to TEHandleKeyPress().
//		Added CleanAllWhitespace(), CleanSelectedWhitespace(), CleanWhitespace().
//		Optimized string version of SelectionMatches().
//		Fixed bug in regex version of SelectionMatches() so it returns kJTrue
//			if the pattern has look-behind or look-ahead assertions.
//	JTEUndoTyping:
//		Added firstChar,lastChar arguments to HandleDelete() and HandleFwdDelete().
//	Defined JBoolean operator!(JBoolean) so you only need JNegate for the
//		result of a logical expression.  (What took me so long?)
//	JDirInfo:
//		Added version of SetWildcardFilter() that takes JRegex*.
//	JRunArray:
//		Added Insert/Prepend/AppendSlice() as the fastest possible way to
//			copy part of one JRunArray to another JRunArray.
//	jDirUtil:
//		Added JConvertToHomeDirShortcut().
//		Fixed JCleanPath() to leave :// alone, so URL's are not destroyed.
//	JUserNotification:
//		Added Get/SetBreakMessageCROnly() to force line breaks to be respected.
//	jTime:
//		Added JPrintTimeInterval().
//		Added JGetTimezoneOffset() to convert from GMT to local time.
//	JNamedTreeNode:
//		Added FindNamedChild(), GetNamedChild(), GetNamedParent().
//	JFSFileTreeNode:
//		FindChild() moved to base class.

// version 2.5.0:
//	*** All egcs thunks hacks have been removed.
//	JTextEditor:
//		Fixed ReadUNIXManOutput() to handle embedded Unicode characters.
//		Fixed bug in Drag-and-Drop that caused crash in a very rare
//			situation:  drop text ending in a newline at the end of a file
//			that did not end with a newline.
//		Added Will/ShouldShowWhitespace().
//		Added virtual function TEDrawInMargin() to allow derived classes
//			to scribble in the margins, e.g., line numbers or bookmarks.
//		Promoted TEGet/SetLeftMarginWidth() to public.
//		Added optional 2nd argument "force" to TabSelectionLeft() to allow
//			shifting even if some lines do not have enough whitespace.
//		Added kForceShiftSelLeftCmd to CmdIndex.
//		GoToLine() selects the line if text is read-only.
//		Fixed SearchBackward(const JCharacter*,...) so it backs up by
//			the length of the target string if a match is selected.
//	JFontManager:
//		Added pure virtual function GetFontCharSets().
//	JPtrArray:
//		Promoted CleanUp() to public access and renamed to CleanOut().
//			This is safer than DeleteAll().
//	JStringPtrMap:
//		Added CleanOut().  This is safer than DeleteAll().
//	JTable:
//		BeginEditing() and ShiftEditing() now return JBoolean.
//	JIndexRange
//		Removed regmatch_t ctor and operator to decouple from regex engine.
//		Finally added operator+ and operator-.  (What took me so long?)
//	*** Eliminated jStrStreamUtil.h & JStaticBuffer.h because strstream is obsolete.
//	JFileArray:
//		*** Switched interface from strstream to stringstream.
//		*** Switched interface from JStaticBuffer to string.
//	JString:
//		Added functions for std::string.
//	JRegex:
//		Finally fixed bug so ^ will not match at start of range unless
//			range starts at the start of the text or a line.
//		Added support for PCRE engine.  This is controlled by which file
//			libjcore/code/regex.h points to.
//		Added GetSubexpressionIndex().
//		Fixed bug in MatchLastWithin() that caused crash when pattern
//			contained look-behind or look-ahead assertion.
//	JInterpolate:
//		Added support for named subexpressions via ${x}.  This is only
//			available if JRegex uses the PCRE engine.
//	Renamed JUNIXDirInfo & JUNIXDirEntry to JDirInfo & JDirEntry. (Finally!)
//	Renamed jUNIXUtil.h to jSysUtil.h.
//	jFileUtil:
//		Fixed JPrintFileSize() to use powers of 2 instead of powers of 10.
//		*** Renamed JGetTempFileName() to JCreateTempFile().
//			It is now completely safe because it uses mkstemp().
//			Now returns JError instead of JString because file is created for you.
//	jDirUtil:
//		Added JGetPrefsDirectory() because this is different from the
//			user's home directory on Windows.
//		Removed startIndex argument from JGetTempDirectoryName().
//		*** Renamed JGetTempDirectoryName() to JCreateTempDirectory().
//			It is now completely safe because it uses mkstemp().
//			Now returns JError instead of JString because directory is created for you.
//		Fixed JConvertToAbsolutePath() to return the result even if JNameUsed() fails.
//	jMountUtil:
//		Added JFindUserMountPoint() and JGetUserMountPointType().
//		Added isTop and fsType arguments to JIsMounted().
//	*** Changed dynamic_cast() macro to use dynamic_cast<>.
//	Works with Visual C++.
//	JTreeList:
//		Added ShouldOpenSibling/Descendant() to allow control during
//			Shift/Meta-click operations.
//	JUNIXDirInfo:
//		Renamed Has/Set/ClearPermissionsFilter() to Has/Set/ClearDirEntryFilter().
//	Created jVCSUtil for functions related to version control.
//	jStreamUtil:
//		Added versions of all JRead*() and JIgnore*() functions
//			for use with file descriptors.
//		Added JConvertToStream() for use with file descriptors.
//	*** Eliminated JInPipeStream.  Use the new functions in jStreamUtil instead.
//	JMatrix:
//		Added SetRow(), SetCol(), and SetElements().
//	JSimpleProcess:
//		Promoted ReportError() to public so clients who call WaitUntilFinished()
//			can still report results from the process.

// version 2.1.1:
//	JUNIXDirInfo:
//		ResetCSFFilters() no longer clears itsShowHiddenFlag.

// version 2.1.0:
//	Compiles with gcc 3.x.
//	JImage:
//		Now supports truecolor PNG and JPEG.
//	JColormap:
//		Added pure virtual function AllColorsPreallocated().
//	JTextEditor:
//		Fixed code so selection can't change during DND.
//		Renamed CaretPositionChanged message to CaretLocationChanged for
//			consistency with Get/SetCaretLocation() function.
//		Added VIsCharacterInWord() to allow derived classes to use instance
//			data to affect the decision.
//		*** Removed range argument from HandleHTML*().
//		*** Removed err argument from HandleHTMLError().  Errors generated by
//				parser in JTextEditor now bottleneck through HandleHTMLError().
//		*** Removed HandleHTMLComment() because it was useless.
//		*** Added GetHTMLBufferLength().  Use this while parsing HTML instead
//			of GetTextLength() since the latter doesn't work with PasteHTML().
//		Added Get/SetCurrentFont() that takes JFontID.
//		Fixed bug in SetCRMRuleList() so no longer copies ruleList if JTextEditor
//			is supposed to own it.
//		Fixed bug in SetText() to reset insertion font to default font if text
//			is being cleared.
//		Automatically turns on word wrap if gets ridiculously wide.
//		Fixed paste and DND so Mac and DOS newline formats are converted.
//		Added virtual functions NeedsToFilterText() and FilterText() sp
//			derived classes can enforce restrictions.
//		Fixed TabSelectionLeft/Right() so undo works when selection
//			includes partial line.
//	JUNIXDirInfo:
//		Added ShouldApplyWildcardFilterToDirs().
//		Added optional 3rd argument "caseSensitive" to SetWildcardFilter().
//		Added Has/Set/ClearPermissionsFilter().
//	JUNIXDirEntry:
//		Changed interface so all strings are returned as JString&.
//		Fixed GetLinkName() to return JBoolean since it is only used for links.
//	jFileUtil:
//		Fixed JSplitRootAndSuffix() so [0-9]+ is not considered a suffix.
//			Neither is starting or ending with a period.
//		Updated JURLToFileName() to match the latest XDND file dragging spec.
//		Added optional endLineIndex to JExtractFileAndLine().
//	jNew:
//		Fixed "bug" so "if (x) delete y;" works correctly.  But you should always
//			use braces, so you should never have this problem!
//	JFileArray:
//		Promoted FlushChanges() to public.
//	JHTMLScanner:
//		Fixed PHP lexing to correctly parse 'abc' and <<<tag ... tag; strings.
//	JTEStyler:
//		UpdateStyles() now requires JArray<TokenData>*.
//		Changed prototypes of Scan() and SaveTokenStart() and return value of
//			NewTokenStartList().
//		Added virtual protected GetTokenExtraData().
//	JHTMLScanner:
//		Fixed bug so &#x__ is interpreted as a valid character.
//	JOrderedSet:
//		Added IsSorted().
//	Changed JPtrStack/Queue instantiation system.  Check the .tmpls files for details.
//	JPtrStack:
//		Added concept of CleanUpAction and *AsArrays() functions.
//		Added PushCopy().
//	JPtrQueue:
//		Added concept of CleanUpAction and *AsArrays() functions.
//		Added AppendCopy().
//	JTable:
//		Added pure virtual function TableGetApertureWidth().
//		Derived classes must now override protected TableScrollToCellRect()
//			instead of public TableScrollToCell().
//		Derived classes should override new virtual function GetMin1DVisibleWidth().
//	JString:
//		Added ctor for converting unsigned integer to base 10, 2, 8, 16.
//	jStreamUtil:
//		Fixed JReadLine() to handle DOS (\r\n) and Macintosh (\r) formats.
//		Added JIgnoreLine() to handle DOS (\r\n) and Macintosh (\r) formats.
//			This is safer than JIgnoreUntil('\n').
//	jProcessUtil:
//		Fixed JParseArgsForExec() to handle partially quoted strings, e.g.,
//			"scp newplanetsoftware.com:'*.tgz' .", and also to convert unquoted
//			semi-colons to separate arguments.
//	JProcess:
//		Added ShouldDeleteWhenFinished().
//	JBroadcaster:
//		Added Send() and SendWithFeedback() to allow sending messages to an
//			object in a way that is safe but that does not require the C++
//			type safety mechanism of dynamic_cast.  Essentially, this
//			features is equivalent to Objective C object methods.
//		Fixed obscure bug so newly added recipients will never get the current
//			message, only the next one that is sent.
//	Created JTaskIterator.
//	JPartition:
//		Added SetCompartmentSize().
//	JIn/OutPipeStream:
//		Fixed FILE* leak.
//	JMessageProtocol:
//		Disconnect sequence can now be empty.  This turns the feature off.
//		Fixed bug in TranslateAndSend() so it uses the separator passed
//			to the function!
//	JSubstitute:
//		Fixed Substitute() so $$ translates to $ instead of removing the first
//			$ and evaluating the name after the second $.  This is very useful
//			when writing data files for compile_jstrings.
//	Created jMouseUtil.h
//	jMountUtil.h:
//		Added devicePath element to JMountPoint.
//		Fixed bug in JGetUserMountPointList() so it searches for device type
//			name instead of requiring an exact match.

// version 2.0.1:
//	JTextEditor:
//		Fixed bug so undo move drop to end of text redraws correctly.
//		Fixed bug so "replace all in selection" works correctly when source
//			and target do not have the same length.

// version 2.0.0:
//	All StreamOut() functions surround themselves with whitespace so they won't
//		accidentally merge with adjacent data in the stream.
//	Renamed kTrue -> kJTrue and kFalse -> kJFalse.  This completes the clean up
//		of the global namespace.  Use lib/util/rename_symbol to fix your code.
//	Created JStringManager to store all string data, in preparation for i18n.
//	jGlobals:
//		Added arguments to JInitCore() for initializing JStringManager.
//		Added JGetStringManager().
//		Added convenience two functions JGetString() for accessing JStringManager.
//		Added JGetJDataDirectories() and JGetDataDirectories().
//	JError:
//		By default, constructor now retrieves message from JStringManager.
//		Converted ReportError() to ReportIfError().  It now does nothing if OK(),
//			so you no longer need to test for it.
//	JTextEditor:
//		Added search and replace commands to CmdIndex enum.
//		Derived classes must implement TEHasSearchText().
//		Promoted EndsWithNewline() to public.
//		Added option to broadcast new message CaretPositionChanged instead of
//			CaretLineChanged.  Broadcasting both is redundant, so generic classes
//			should accept either message to avoid depending on the setting.
//		Added option to always broadcast TextChanged instead of optimizing.
//		Added GetColumnForChar() and GoToColumn().
//		Added public versions of GetCharLeft/Right().
//		Removed GetDragClip().  Simply call GetSelection().
//		Removed TEClearDragClip().  The OS should buffer the data.
//		Removed WriteDragClipPrivateFormat().
//		Added useInternalClipboard argument to constructor.  This affects how
//			TEClipboardChanged() works.
//		Improved handling of tables in ReadHTML().
//		Added PasteHTML(), PrepareToPasteHTML(), and PasteHTMLFinished().
//		Optimized GetLineTop() and CalcLineIndex().
//		Added SetText() that takes JString&.  This allows using a string that
//			contains NULL.  (Of course, these will be stripped out immediately :)
//		Added third argument acceptBinaryFile to ReadPlainText().
//		Added SetFontBold/Italic/Underline/Strike/Color().  These correspond to
//			the previously existing SetCurrentFont*() functions.
//		Changed SearchForward/Backward() to take match object instead of JFontStyle.
//		Added TabShouldInsertSpaces().
//		TabSelectionLeft() removes spaces as well as tabs.
//		TabSelectionRight() honors the setting of TabShouldInsertSpaces().
//		IsCharacterInWord() is no longer virtual.  Instead, it uses a function pointer.
//			This way, one can change the function without inheriting, so it works
//			for simple objects like input fields.
//		Added Get/SetCharacterInWordFunction().
//		Added Get/SetI18NCharacterInWordFunction() for specifying characters
//			above 0x7F that should be treated as letters.  This is orthogonal to
//			SetCharacterInWordFunction().
//		Added CRLineIndexToVisualLineIndex() and VisualLineIndexToCRLineIndex().
//	JTEStyler:
//		Renamed SetFont() to SetStyle(), since this is more accurate.
//		Added AdjustStyle() to allow modifying styles after scanner has passed by.
//	JTableData:
//		*** Changed inserted and removed messages to plural form.
//	All derived classes of JTableData:
//		Modified interface to take advantage of new JTableData messages.
//		You should search for calls to RemoveRow() and optimize to use RemoveNextRows().
//	JTable:
//		*** Changed row and column manipulation functions to work with multiple
//			rows/columns at a time.  This dramatically reduces the number of
//			messages that need to be broadcast.  The messages are now plural.
//			Removed AllRows/ColsRemoved since they are now redundant.
//		Added version of Print() for EPS.
//		Promoted AdjustToTableData() to protected.
//		Demoted TableSet/AdjustBounds() to private.
//		Optimized GetCellIndex(), GetCellBoundaries(), TableDraw*Borders(),
//			and GetVisibleRange().
//		Added pure virtual functions for updating scrolltabs.
//		Added DrawForPrint() so you can print a table as part of a page layout.
//		Added GetRow/ColHeader().
//	All derived classes of JTable:
//		You should search for calls to RemoveRow() and optimize to use RemoveNextRows().
//	Created JStyleTableData to provide color management for JAuxTableData<JFontStyle>.
//	jDirUtil:
//		JCreateDirectory() now creates intermediate directories if they don't exist.
//			It now returns JNoError() instead of JDirEntryAlreadyExists().
//		Added JRenameDirEntry(), JGetRootDirectory(), JIsRelativePath().
//		Added optional fourth argument to JGetUniqueDirEntryName() allowing you
//			to specify the first index to check.  This is a useful optimization
//			when creating a large number of temporary files because it avoids
//			checking all the existing file names.
//		Added optional second argument startIndex to JGetTempDirectoryName().
//		Added optional third argument basePath to JGetClosestDirectory().
//		Added JGetOwnerID() and JGetOwnerGroup().
//		*** JIsRelativePath() and JIsRootDirectory() require a non-empty string.
//		Added JCreateSymbolicLink().
//		Fixed bug in JGetTrueName() so it doesn't crash if target directory doesn't
//			have execute permission.
//		Fixed bug in JIsRelativePath() so starting with ~ is considered an absolute path.
//	jFileUtil:
//		Added JKillFile().
//		Added optional second argument startIndex to JGetTempFileName().
//		Fixed bug in JUncompressFile() so it will actually work.
//		Added JFileNameToURL() and JURLToFileName().
//		*** JRenameFile() no longer automatically overwrites existing files.
//			You must call JRemoveFile() first.
//	JString:
//		Added RangeValid().
//		Added JMatchCase() and versions of MatchCase() that take destination range.
//		Cleaned up number to string conversion and extended precision to 16 digits.
//		Added sigDigitCount argument to constructor that converts from a number so
//			you can control the number of significant digits independently of the
//			precision.  kStandardExponent switches to scientific notation when the
//			exponent is significantly larger than sigDigitCount.
//		Added ReadDelimited().
//		Added version of Set() that takes JIndexRange.
//		Renamed IsHex() to IsHexValue().
//		Added static versions of Is*() and ConvertTo*() so one no longer needs
//			to construct a JString just to convert it safely into a number.
//			(what took me so long?)
//		Added JSetDiacriticalMap() to process diacritical marks while comparing
//			strings and changing case.
//		*** All calls to strcmp() and strncmp() that are used for sorting should
//			be converted to use JStringCompare() and JCompareMaxN(), respectively.
//		*** All calls to toupper() and tolower() should be converted to use
//			JToUpper() and JToLower(), respectively.
//		*** All calls to isprint(), isalnum(), isalpha(), isupper(), and islower()
//			should be converted to use the corresponding JCore replacement.
//		Fixed bug so EndsWith() now returns kJTrue if strings are equal.
//		Added Get/SetCharacterFromEnd().
//	JPrefsManager:
//		Fixed bug in UpgradeData() so it correctly handles errors from JPrefsFile::Create().
//		GetFullName() returns JError.
//		Constructor requires the full path + name returned by GetFullName().
//		File permissions are set so only user can read it.
//		Added JFileArray::CreateAction argument to Create().
//	JPrefsFile:
//		Constructor takes JCharacter* since derived classes should construct the
//			full name in its Create() function.
//	Merged JUNIXPrefsFile and JUNIXPrefsManager into base classes to avoid system
//		dependent names.  Should not affect code since all classes should derive
//		from JXPrefsManager.
//	JMDIServer:
//		Fixed bug so multiple, simultaneous invocations don't time out and start
//			new processes when a server is already running.  There is no way to
//			avoid multiple processes if no server is running.
//	Merged jNumericConstants.h into jMath.h.
//		Added more digits to kJPi and kJE.
//	Created JColorList.
//	jASCIIConstants.h:
//		Now includes ctype.h for convenience.
//	JCollection:
//		Added GetIndexFromEnd().
//	JOrderedSet:
//		Added Get/SetCompareObject() to allow function to use additional information.
//		*** Switch from GetCompareFunction() to GetCompareObject() where ever
//			possible, because the latter is guaranteed to return something as long
//			as sorting has been set.  The former may return NULL even if a sort
//			has been set.
//		GetCompareFunction() now returns JBoolean.
//		Added RemovePrevElements().
//		Added Get/SetElementFromEnd().
//	JArray:
//		Removed AllElementsRemoved message since it is redundant.
//		Added GetBlockSize().
//		Removed JArrayBlockSize and JArraySlotCount typedefs since both are JSize.
//	JPtrArray:
//		operator= replaced by CopyPointers() and CopyObjects().
//		Constructor, copy constructor, and Copy*() require CleanUpAction.
//			*** If you use a value other than JPtrArrayT::kForgetAll,
//				then be sure to analyze your code for dangerous situations
//				such as copying pointers to another list and setting
//				both lists to JPtrArrayT::kDeleteAll.
//		Overrides SetElement() to require SetElementAction.
//		SetToNull() requires SetElementAction.
//		Renamed kDeleteAllAsObjects to kDeleteAll for consistency with fn names.
//		Renamed kDoNothing to kForgetAll for consistency with SetElementAction.
//		Added versions of InsertAtIndex(), etc. that take "const T&" and make a copy.
//			This is currently only available for JPtrArray<JString> since hardly
//			anything else has a copy constructor.  (What took me so long?)
//	JPtrArrayIterator:
//		Moved class declaration into JPtrArray.h.
//		Added SetPrev() and SetNext() to take second argument SetElementAction.
//		Added DeletePrev/NextAsArray().
//	JPtrArray-JString.h:
//		Removed JCopyStrings() since JPtrArray now has built-in CopyObjects().
//			(Call it with append=kJTrue!)
//		Added stream operators for JStringPtrMap<JString>.
//	JRunArray:
//		Added SumElements() and FindPositiveSum().
//	JHashTable:
//		Added IsEmpty() to parallel JCollection.
//		Renamed DeleteAllAsArray() to DeleteAllAsArrays() for consistency
//			with JPtrArray.
//	JStringPtrMap:
//		Overrides GetElement() with two versions to preserve constness.
//		Added functionality for CleanUpAction.  Constructor requires this value.
//			*** If you use a value other than JPtrArrayT::kForgetAll,
//				then be sure to analyze your code for dangerous situations
//				such as copying pointers to another list and setting
//				both lists to JPtrArrayT::kDeleteAll.
//		Overrides SetElement(), SetOldElement(), and SetContains() to require
//			SetElementAction.
//		Provides versions of SetElement(), etc. that copy the given object.
//	jProcessUtil:
//		Renamed JProgramOnPath() to JProgramAvailable().
//		Fixed JProgramAvailable() and JExecute() to accept ~ notation.
//		Fixed bug in JParseArgsForExec() so it accepts "" as an argument.
//		Added version of JExecute() taking JPtrArray<JString>.  (what took me so long??)
//		Added JParseArgsForExec().
//	JProcess, JSimpleProcess:
//		Added version of Create() taking JPtrArray<JString>.
//		All versions of Create() now return JError.
//	JUNIXDirInfo:
//		Fixed ctor to allow relative paths.
//		Added ResetCSFFilters().
//		Understands ~ notation.
//		Wildcard filter is now case-insensitive.
//		Added copy ctor that accepts new path.
//		Renamed OKToCreateUNIXDirInfo() to OKToCreate().
//		Added SettingsChanged message.
//		*** No longer changes to home directory if current directory becomes
//			invalid.  Instead, DirectoryExists() returns kJFalse.
//		Added ShouldSwitchToValidDirectory().
//		Renamed GetCWD() to GetDirectory() for consistency with DirectoryExists().
//		GoUp() returns JNoError() if already in the root directory.
//		Update() now accepts Boolean argument "force".
//		Added 2nd optional argument "negate" to SetWildcardFilter().
//	JUNIXDirEntry:
//		Update() now accepts Boolean argument "force".
//		Fixed NeedsUpdate() to be const.
//		Added GetAccessTime().
//	JDynamicHistogram:
//		Added CreateSetCount() and CreateIncrementCount().
//	JPTPrinter:
//		Added ShouldPrintReverseOrder().
//	JRTTIBase:
//		Optimized Is() to compare pointers before calling strcmp().
//	Created JExtractHTMLTitle.
//	JThisProcess:
//		Fixed bug in ShouldCatchSignal() so it works when catchIt argument is kJFalse.
//	JHTMLScanner:
//		Fixed handling of escaped characters.  Any number of digits is allowed
//			and a new error kCharacterOutOfRange is used to indicate values that
//			are out of range.  Also updated list of character names to HTML 4.01.
//		*** Because new escaped characters require switching fonts, HandleHTMLTag()
//			can now be passed an empty range.
//		Added new pure virtual function SwitchCharSet() to handle escape sequences
//			that are not covered by iso8859-1.
//		Added new virtual function HandleHTMLScript() for JavaScript, etc.
//		Added GetScriptLanguage().
//	JUserNotification:
//		Added AcceptLicense().
//	JPrefObject:
//		Restructured to allow NULL prefs mgr so library objects can inherit
//			without requiring all applications to implement prefs.
//		*** Zero is no longer a valid ID.  (It never was, but nobody checked.)
//	JFontManager:
//		Added IsExact() so one can determine if approximations were necessary.
//	JSubset:
//		Added SetOriginalSetSize().
//	JStdError.h:
//		Created JNoHomeDirectory.
//		Merged J(File|Directory)DoesNotExist into JDirEntryDoesNotExist
//		Merged J(File|Directory)AlreadyExists into JDirEntryAlreadyExists
//	JPSPrinterBase:
//		Fixed bug so arcs with negative deltaAngle are drawn correctly.
//	jUNIXUtil:
//		Fixed bug in JUNIXSocketExists() for Solaris.
//	JFileArray:
//		Renamed ShouldWriteIndex() to ShouldFlushChanges().
//		Added CreateAction argument to Create().
//	Created JNetworkProtocolBase.
//	JMessageProtocol:
//		Supports asynchronous sending of data.  This is the default.
//	JProgressDisplay:
//		Added version of IncrementProgress() that takes message -and- delta.
//	JPrinter:
//		Added second argument to ctor for PrintSetupFinished message.  This specifies
//			whether or not the data that is saved has changed.  If Changed() returns
//			kJFalse, the document should not be marked as unsaved.
//	JEPSPrinter:
//		Renamed ShouldPrintPreview() to WantsPreview().
//		Renamed PrintPreview() to ShouldPrintPreview() for consistency with rest of JCore.
//	JRegex:
//		Optimized SetPattern() to not compile if the pattern doesn't change.
//	JChooseSaveFile:
//		The version of ChooseFile() that accepts "origName" now allows this parameter
//			to be NULL.
//	jCommandLine:
//		Modified arguments to JCheckForValues() to provide a more useful error message.
//		Added JIsVersionRequest() and JIsHelpRequest().
//	jTime:
//		Added JCheckExpirationDate().
//	jStrStreamUtil.h:
//		Added new_jistrstream macro to allow allocating the object on the heap.
//	JQueue, JStack:
//		Reversed order of template parameters to improve readability.
//	Created jMountUtil.h.
//	Created JAliasArray.

// version 1.5.3:
//	jTypes.h:
//		Added constants defining min and max values for 32 and 64 bit integer types.
//	JFileArray:
//		Changed JFAID_t from unsigned long to JUInt32 so ID's generated on
//			64-bit machines will work on 32-bit machines.
//	jProcessUtil:
//		Fixed bug in JProgramOnPath() so it no longer is fooled by directories
//			that have the same name.  (Directories are executable on UNIX.)
//		Fixed bug in JExecute() so it closes the pipes if Fork() fails.
//	JSimpleProcess:
//		Now redirects stdout to /dev/null.
//		No longer ignores final line of text from stderr if it is not
//			followed by a newline.
//	jFileUtil:
//		Fixed JFileExecutable() so it does not check for readability.
//	jCommandLine:
//		Added JCheckForValues().
//	jUNIXUtil:
//		Added JGetHostName() and JGetUserRealWorldName().
//	JStdError.h:
//		Fixed JProgramNotAvailable so it takes the program name as an argument
//			to the constructor.
//	JTableSelectionIterator:
//		Fixed bug so Prev() now works with 1x1 table.
//	JInPipeStream:
//		Fixed bug in close() so it works even if failbit is set.
//	JOutPipeStream:
//		Fixed bug in close() so it works even if failbit is set.
//	JThisProcess:
//		SIGPIPE is no longer fatal because you have no control over when the
//			other end of the pipe dies.
//	JImage:
//		GIF is now only conditionally supported due to legal issues with Unisys.
//		PNG can now be supported via the latest version of libgd.
//	JMDIServer:
//		Socket name now start with . so it is invisible.  "rm /tmp/*" won't
//			accidentally delete it.

// version 1.5.2:
//	JMinMax.h:
//		Added JLimit() to provide the intersection of JMin() and JMax().
//	JOrderedSet:
//		For convenience, added:
//			RemoveElements(const JOrderedSetT::ElementsRemoved&)
//			MoveElementToIndexWithMsg(const JOrderedSetT::ElementMoved&)
//			SwapElementsWithMsg(const JOrderedSetT::ElementsSwapped&)
//	JFileArray:
//		To avoid global namespace pollution, renamed:
//			kInitialFAVersion -> JFileArray::kInitialVersion
//			kInvalidFAIndex   -> JFAIndex::kInvalidIndex
//			kInvalidFAID      -> JFAID::kInvalidID
//			kMinFAID          -> JFAID::kMinID
//			kMaxFAID          -> JFAID::kMaxID
//	JFileArrayIndex:
//		Renamed FAElementType to JFileArrayIndex::ElementType.
//	Renamed JFAElementID.h to JFAID.h and JFAElementIndex.h to JFAIndex.h.
//	jProcessUtil:
//		Fixed bug in JProgramOnPath() so it no longer checks PATH if the
//			program name contains a slash, not only if it begins with a slash.
//		JExecute() will now return JProgramNotAvailable() before forking instead of
//			printing to stderr after forking.
//		Added JWill/ShouldIncludeCWDOnPath().  If this option is set,
//			JProgramOnPath() and JExecute() will prepend ./ to the program name,
//			if necessary.
//	JStdError:
//		Added JProgramNotAvailable.

// version 1.5.1:
//	JSimpleProcess:
//		Added third argument "detach" to the two simple constructor functions.
//	JVector:
//		Renamed ChangeElement() to IncrementElement() for consistency.
//		Added GetMin/MaxElement().
//	JMatrix:
//		Renamed ChangeElement() to IncrementElement() for consistency.
//	JTextEditor:
//		Added breakCROnly argument to SetAllFontNameAndSize().
//	JUNIXDirEntry:
//		Fixed bug so it works correctly with short files.

// version 1.5.0:
//	JX is now 64-bit clean.
//	jTypes.h:
//		Defined JWord and JUWord.
//		Defined JInt64 and JUInt64 for use on 64-bit systems.
//	JThisProcess:
//		Added Ignore().
//	jRand.h:
//		Added JKHRandInt64() and JRandWord().

// version 1.1.23:
//	Moved JProgramOnPath() from jDirUtil.h to jProcessUtil.h.
//	JFontManager:
//		Added ExtractCharacterSet().
//	JTable:
//		Promoted CellIsSelectable() and GetNextSelectableCell() to public access.
//		Renamed CellIsSelectable() to IsSelectable() to match IsEditable().
//	jFileUtil:
//		Renamed JGetUniqueFileName() to JGetUniqueDirEntryName(), rearranged
//			the arguments, and moved it to jDirUtil.
//	jDirUtil:
//		Removed JOpenTempDirectory() because it was unsafe to use.  You must
//			call JCreateDirectory() yourself and handle any possible errors.
//		Removed JCloseTempDirectory().  Simply call JKillDirectory().
//		Created JGetTempDirectoryName() to parallel JGetTempFileName().
//		Added optional second argument requireWrite to JGetClosestDirectory().
//		Fixed bug in JConvertToRelativePath() that crashed when given
//			directories like {/a/bc, /a/bd}.
//	JPTPrinter:
//		Added option to print line numbers.
//	JChooseSaveFile:
//		Added version of ChooseFile() that allows specifying an initial name.
//	Created JAsynchDataReceiver.
//	JPtrArray-JString.h:
//		Added JCopyStrings().

// version 1.1.22:
//	Renamed all four JIteratorPosition values to start with kJ.
//		multifile replace:  kIteratorStart -> kJIteratorStart
//	JTable:
//		Calls ResetAllButClipping() before TableDrawCell().
//	JTableSelection:
//		Added HasAnchor() and HasBoat().
//	jDirUtil:
//		Added JGetPermissionsString().
//	JUNIXDirEntry:
//		NeedsUpdate() now checks both lstat() and stat() times.
//		Added FollowLink() and GetModeString().
//	JPrefObject:
//		No longer allows NULL prefs mgr.  Fixed GetPrefsManager().
//		Removed HasPrefsManager().

// version 1.1.21:
//	jDirUtil:
//		Added version of JGetHomeDirectory() that accepts user name.
//		Added JConvertToAbsolutePath(), JConvertToRelativePath(),
//			JCleanPath(), JExpandHomeDirShortcut().
//	jFileUtil:
//		Added JCombineRootAndSuffix() and JSplitRootAndSuffix().
//	JTextEditor:
//		Changed prototype of GetInternalClipboard() and GetDragClip() to
//			avoid copying data.
//	JFileArray:
//		Added GetFileName().
//		Create() returns an error if the file is not writable.
//	JTable:
//		Added utility functions to implement default selection behavior.
//	JTableSelection:
//		Added versions of GetBoat/Anchor() that returns kJFalse if they
//			are not set.
//	JPoint:
//		Fixed JPinInRect() to account for the fact that bottomRight of
//			a rectangle is not actually inside the rectangle.
//	JPrefsManager:
//		Broadcasts when data is changed or removed.
//	JPrefObject:
//		Added GetPrefsManager() and GetID().
//		Allows NULL prefs mgr so library objects can inherit without requiring
//			all applications to implement prefs.
//	JPTPrinter:
//		Added pure virtual Print() so one doesn't need to know about derived classes.
//	Created JSimpleProcess as asynchronous version of JRunProgram.
//	To get around the egcs thunks bug, JCollection no longer inherits virtually
//		from JBroadcaster.

// version 1.1.20:
//	Added "&& ! defined JOnlyWantTemplateDefinition" to #if at end of .tmpl
//		files to allow finer control over when templates are instantiated.
//	Renamed original JPrinter to JPagePrinter.
//		OpenDocument() now returns JBoolean.
//	Created new JPrinter to define concepts of Page Setup and Print Setup.
//	Moved enum ImageOrientation from JPSPrinter to JPagePrinter.
//	Created JPTPrinter for printing plain text.
//	JPSPrinterBase:
//		PSOpenDocument() returns kJFalse if it is unable to open the output file.
//	JPSPrinter:
//		OpenDocument() returns kJFalse if it is unable to open the output file.
//		NewPage() returns kJFalse if an error occurs while writing to the output file.
//	JEPSPrinter:
//		OpenDocument() returns kJFalse if it is unable to open the output file.
//	JTextEditor:
//		Moved PrintPlainText() functionality to JPTPrinter.
//		Removed virtual functions:
//			GetPlainTextHeaderLineCount()
//			GetPlainTextFooterLineCount()
//			PrintPlainTextHeader()
//			PrintPlainTextFooter()
//			GetPlainTextTabWidth()
//		Print() is no longer virtual.
//	JTable:
//		Fixed fatal bug in interaction between JTable and JTableSelection when the
//			table doesn't have a JTableData object.
//	JRegex:
//		Fixed fatal bug in Replace().
//		Fixed memory leak in destructor.
//	JFileArray:
//		Fixed fatal bug in ExpandData().

// version 1.1.19:
//	JString:
//		Fixed bug in ConvertTo*() that returned {kJTrue, 0} if the string was empty.
//		Added version of JCompareMaxN() that takes (char*,length).
//		All functions involving locating substrings are now NULL safe.
//	jFileUtil:
//		Added JExtractFileAndLine().
//		JRenameFile():
//			1) strips trailing slashes so rename() doesn't screw up.
//			2) succeeds if the new name is the same as the old name.
//			3) always fails if the new name is an existing directory,
//			   instead of only almost always like rename().
//	jDirUtil:
//		Added JGetClosestDirectory().
//		JRenameDirectory():  same changes as JRenameFile().
//		Added JStripTrailingDirSeparator() and JIsRootDirectory().
//	jFStreamUtil:
//		Removed JOpen/CloseScratchFile() because they cannot be implemented with
//			the latest draft specification of fstream.
//	JUNIXDirInfo:
//		Moved static version of MatchesContentFilter() to JUNIXDirEntry.
//			This function now returns kJFalse if the entry is not a file.
//		Content filter treats file as a single line.
//	JTextEditor:
//		Fixed double-click&drag behavior so characters that are not part of a
//			word are selected one at a time.
//		Created CaretMotion enum and changed TEHandleKeyPress to take this
//			instead of several booleans.
//		Created GetPartialWordStart/End() and implemented caret motion and
//			double click-and-drag behavior based on this.
//		CRM now uses CRMRuleList to detect line prefixes.
//			The default rule is to consider spaces and tabs to be the prefix.
//			You can add extra rules with SetCRMRuleList().
//			Only CRMConvertTab() and CRMGetTabWidth() are now virtual.
//		Renamed CRMGet/SetLineWidth() to Get/SetCRMLineWidth().
//		Renamed CRMGet/SetTabCharCount() to Get/SetCRMTabCharCount().
//			CRM(Get|Set)(LineWidth|TabCharCount)  ->  $1CRM$2
//		Removed CleanAllRightMargins() because CleanRightMargin() now cleans
//			all paragraphs touched by the selection.  If there is no selection,
//			it cleans the paragraph containing the caret, as before.
//		CleanRightMargin() now takes coerce argument.  kJTrue produces the
//			original result; kJFalse results in a more cautious approach.
//		Added kCoerceRightMarginCmd to CmdIndex enum.
//		Promoted GoToBeginningOfLine() to public and added matching GoToEndOfLine().
//		The action controlled by ShouldMoveToFrontOfText() now considers
//			the entire CRM prefix to be non-text, not just the whitespace.
//		Added pure virtual TECaretShouldBlink() which tells derived classes
//			when they should make the caret blink.
//		Demoted TEActivate() and TEDeactivate() to protected since they
//			should only be called by the system dependent derived class.
//		Renamed TEActivate/DeactivateCaret() to TEShow/HideCaret() and
//			demoted it to protected.
//		Decoupled caret and selection and added TEActivate/DeactivateSelection().
//		Fixed bug that caused the object to ignore key presses after one
//			dragged text to another widget so quickly that the source never
//			became the target.
//	JTEStyler:
//		Changed first argument of UpdateStyles() to "const JTextEditor*".
//		Removed assumption that default style is blank.  It now uses the
//			default font style of the given text editor.
//	JTable:
//		Inverted selection rule for arrows when nothing is selected.
//		CreateInputField() now returns JBoolean.  This allows BeginEditing()
//			to fail if the newly created widget cannot get focus.
//		Fixed bug in GetCell() so coordinate of 0 returns kJFalse if the
//			table is empty.
//	JPartition:
//		Read/WriteGeometry() no longer stores elastic index or min sizes
//			since these are always set by the creation code.
//	JLineProtocol:
//		Renamed to JMessageProtocol and generalized to handle arbitrary
//			separator and disconnect sequences.
//		Renamed SendString() to SendData() and changed function names to use Message
//			instead of Line.
//		Added TranslateAndSend().
//		No longer broadcasts empty message if disconnect is received immediately
//			after message separator.
//		close() no longer sends disconnect, for the same reason as in the
//			destructor.
//	JRect:
//		Renamed Offset() to Shift().
//	JLatentPG:
//		Fixed bug in IncrementProgress(char*) so all messages are displayed.
//	JThisProcess:
//		Fork() sets process group id correctly so JProcess signaling functions
//			work correctly on the child process.
//	jUNIXUtil:
//		Added JCreatePipe().
//	JError:
//		Removed default constructor since it isn't really appropriate.
//		Added ReportError() as a shortcut to calling JUserNotification::ReportError().
//			The multifile regex replace pattern that I used was:
//			\(JGetUserNotification\(\)\)->ReportError\(([a-z0-9]+)(\.|->)GetMessage\(\)\)
//			$1$2ReportError()
//	JUnknownError, JUnexpectedError:
//		Constructor takes errno value.  This is included in the message to
//			make it more helpful.
//	jAssert:
//		Created assert_ok() macro for use with JError.  It prints out the error's
//			message if OK() returns kJFalse.  The multifile regex replace pattern
//			that I used was:
//			assert(.+)(\.|->)OK\(\)  ->  assert_ok$1
//	JFontManager:
//		Added UpdateFontID() as convenience function.
//		Added pure virtual GetMonospaceFontNames().
//	instantiate_template.h:
//		Added support for instantiating functions.
//	JInPipeStream, JOutPipeStream:
//		Added close().
//	JKLRand:
//		Fixed UniformULong() so it no longer always returns min.
//	jProcessUtil:
//		Fixed JRunProgram() to return output from stderr in separate argument.
//	jMath:
//		Added JASinh(), JACosh(), JATanh().
//	JComplex:
//		Fixed JPrintComplexNumber() to work correctly with Infinity.
//		Fixed capitalization of JASin(), JACos(), JATan().
//		Added JASinh(), JACosh(), JATanh().
//	jErrno:
//		Added jset_errno().
//	Created JSubstitute.  This replaces and augments JCharEscape.
//		Changed the Add*() functions to Set*() for consistency, and
//			added the corresponding Clear*() functions.
//		Changed IdentityDefault to its negation: IgnoreUnrecognized.
//		Renamed ClearAllOrdinary() to ClearAllEscapes() for consistency.
//	JRegex:
//		All the stupid configuration options are now gone:
//			static defaults and the adjustable replace metacharacter.
//		Cleaned up the constructors and their options.
//		SetReplaceRegex() now accepts any pattern; the return value is
//			merely advisory for those who want to only allow 'clean'
//			patterns.  The syntax is now slightly different due to the use
//			of JInterpolate; in particular '$$' should now be '\$'.
//		Removed the version of Replace() which ignored the internal
//			LiteralReplace flag, for consistency.
//		The search and replace pattern escape engines are both visible so
//			the user can customize their behavior.
//		Deprecated feature: the second form of Replace() (taking only ranges,
//			not a list of subexpression ranges) will eventually go away when
//			the function is converted to take a match object.

// version 1.1.18:
//	JHTMLScanner:
//		HandleHTML*() functions now return JBoolean.  Returning kJFalse
//			cancels the lexing.
//	JTextEditor:
//		When pasting unstyled text over styled text, the result takes the
//			style of the first replaced character instead of the preceding
//			character.
//		Moved dropCopy argument from TEHandleMouseDown() to TEHandleMouseUp().
//		Added dropCopy argument to TEHandleDNDDrop().
//		Added TEGetMinPreferredGUIWidth() so widgets can adjust themselves to
//			avoid wrapping single words across lines.
//	JString:
//		Added JCalcMatchLength().
//		Added default value for caseSensitive to JStringCompare() and JCompareMaxN().
//		GetSubstring(JRange&) accepts empty ranges.
//	JPrefsManager:
//		Added reportError argument to UpgradeData().
//	JUNIXDirInfo:
//		Changed interface to name filter.
//			was:  IsFiltering(), TurnOnCurrentFilter(), TurnOn/OffFilter()
//			now:  Has/Set/ClearWildcardFilter()
//		Added static public utility function BuildRegexFromWildcardFilter().
//		Added Has/Set/ClearContentFilter().
//		Added static public utility function MatchesContentFilter().
//	JTable:
//		Added optional arguments printRowHeader, printColHeader to Print().
//		Added virtual function IsEditable() so derived classes can specify which
//			cells can be edited.
//		Added message PrepareForTableDataChange.  This is broadcast before
//			the table is adjusted to fit a new JTableData object.
//	JRect:
//		Added Expand().
//	Created JPrefObject as a base class for all objects that own an ID in a
//		preferences file.
//	jColor:
//		Created JHSV struct and added cross-constructors with JRGB.

// version 1.1.17:
//	JOrderedSet:
//		Added default value of kJTrue for second argument of InsertSorted().
//	JTable:
//		Broadcasts PrepareForTableDataMessage before updating table.
//	JUNIXDirEntry:
//		Fixed extremely rare, fatal bug that would cause partial-construction.
//		Added NeedsUpdate().
//	JProgressDisplay:
//		Added pure virtual function DisplayBusyCursor().
//	JTextEditor:
//		Added 3rd argument to TESetGUIBounds() to help update scrolltabs.
//		Fixed PrintPlainText() to avoid printing final blank page.
//		Added PrepareToReadHTML() and ReadHTMLFinished().
//		Promoted GetLineTop/Bottom/Height() to public.
//		Promoted CalcCaretLocation(const JPoint&) to protected.
//	jProcessUtil:
//		Fixed bug in JExecute() that prevented EOF from being sent when
//			the write end of the pipe was closed.
//	JIndexRange:
//		Fixed bug that caused JRegex to return incorrect range when
//			a sub-expression matched nothing.
//	JColormap:
//		SetDynamicColor() and SetDynamicColors() are no longer const.
//		Added default implementation of SetDynamicColors().
//		Added PrepareForMassColorAllocation(), MassColorAllocationFinished()
//			to provide hint which allows derived classes to optimize.
//	JImage:
//		ReadGIF() no longer takes second argument allowApproxColors.
//			Color approximation is now controlled by the settings in
//			the colormap object.
//	JLatentPG:
//		Activates immediately if you call IncrementProgress() with a non-NULL message.
//		No longer activates if time limit is exceeded on the last step of a fixed
//			length process.
//	JPtrArray:
//		Added Get/SetCleanUpAction() to specify what to do in the destructor.
//	Created JGaussianElimination(). (in JMatrix.h)

// version 1.1.16:
//	All templates:
//		Fixed instantiation mechanism so multiple versions of a template can
//			be instantiated in the same source file.
//	JPoint,JRect:
//		Added Set().
//	Renamed JRange to JIntRange and drastically changed its interface.
//	Renamed JStringRange to JIndexRange and drastically changed its interface.
//		MatchCase() and Extract() are now members of JString.
//	JString:
//		Added JString(const JCharacter*, const JIndexRange&).
//	JRunArray:
//		Removed JRunIndex since it was identical to JIndex.
//	JTextEditor:
//		No longer inherits from JHTMLScanner.  Uses JTEHTMLScanner instead.
//			You can still override HandleHTML*() in derived classes of
//			JTextEditor, however.
//		Added SearchForward(const JFontStyle&) and SearchBackward(const JFontStyle&).
//	JChooseSaveFile:
//		Removed GetChosen*() and added output argument to Choose*().
//		Reordered arguments to SaveFile() so all functions have the format:
//			prompt, instructions, old value, new value
//		Added pure virtual ChooseMultipleFiles().
//	JTableSelection:
//		Added Get/SetBoat(), Get/SetAnchor(), ExtendSelection().
//		Renamed SelectCellRect() to SelectRect().
//		Renamed ToggleCell() to InvertCell().
//		Added InvertRow(), InvertCol(), and InvertRect().
//	JPackedTableData:
//		Added SetPartialRow(), SetPartialCol(), SetRect(), and ApplyToRect().
//	JTableData:
//		Replaced ElementChanged, RowChanged, ColChanged, AllElementsChanged messages
//			with single, more general RectChanged.
//		Renamed CellIndexValid() to CellValid().
//		Added AdjustCell() to all messages that contain AdjustIndex().
//	JTable:
//		Renamed CellIndexValid() to CellValid().
//		Added AdjustCell() to all messages that contain AdjustIndex().
//		Now broadcasts to JAuxTableData objects -after- updating table.
//	JUNIXDirInfo:
//		Renamed UpdateList() to Update() and ForceUpdateList() to ForceUpdate().
//		Added GoToClosest().
//	JUNIXDirEntry:
//		Replaced EntryNeedsUpdate() with Update().
//		Renamed UpdateEntry() to ForceUpdate().

// version 1.1.15:
//	Created JTEStyler.
//	JTextEditor:
//		Added AdjustStylesBeforeRecalc().
//	JRect, JRange:
//		Renamed JCoverage() to JCovering().
//	JRunArray:
//		Added SetNextElements().
//		Added new versions of InsertElementsAtIndex(), RemoveNextElements(),
//			and FindRun() that require you to specify the run information.
//			They are designed for situations requiring maximal optimization
//			and must be used with extreme care.
//	jStreamUtil:
//		Fixed JReadUntil() and JIgnoreUntil() so they can read NULL's.
//	JString:
//		Fixed operator>> so it can read NULL's.

// version 1.1.14:
//	Created JHTMLScanner.
//	JTextEditor:
//		Inherits from JHTMLScanner and uses this to read HTML.
//	JVector:
//		Fixed var args constructor so it doesn't conflict with fillValue.
//	JPainter:
//		Added RectInside().
//	jFileUtil:
//		Added JFileExecutable(), JPrintFileSize(), JUncompressFile(), JFOpen().
//	jDirUtil:
//		Added JProgramOnPath().
//		Added caseSensitive and newName arguments to JSearchSubdirs().
//	jProcessUtil.h:
//		Added JRunProgram().
//	JImage:
//		Added ReadGIF(), WriteGIF(), GetFileType().
//		Changed prototype for SetImageData() to take unsigned short**.
//		Created AllocateImageData() to allocate data passed to SetImageData().
//		Added GetColormap().
//		Constructor takes 3rd argument JColormap*.
//		Removed JColormap* argument from ReadFromJXPM().
//	JRGB:
//		Added scaling functions and Set().
//	JRegex:
//		Added NeedsBackslashToBeLiteral().
//	JRange:
//		Added Contains(JRange&), Intersection(), Coverage(), operator+=(JRange&).

// version 1.1.13:
//	Check the LICENCE file for information about using ACE and JRegex.
//	Merged jNULL.h into jTypes.h.
//	jASCIIConstants.h:
//		Renamed all constants to start with "kJ" instead of just "k".
//		Added kJNewlineKey.
//	jNumericConstants.h:
//		Renamed all constants to start with "kJ" instead of just "k".
//		Renamed pi to kJPi.
//		Renamed e to kJE.
//	jAssert.h:
//		Remove const_cast and static_cast macros since the real ones appear to be
//			widely supported.  The dynamic_cast macro is still there because
//			-frtti seems to be broken for gcc 2.7.2.
//			To fix your code, use the following regex pair in Code Crusader:
//			"(const|static)_cast\(([^,]+),\s*", "\1_cast<\2>("
//			(Yes, multi-file search is on the priority list :)
//	JAssertBase:
//		Changed values for J_ASSERT_ACTION to avoid calling unsetenv().
//	jStrStreamUtil.h:
//		Removed jistrstream2 macro since its prototype was the same as jistrstream.
//		Added const_cast to strstream version of jistrstream macro for convenience.
//	JTextEditor:
//		Read/WritePlainText() now take a second argument that indicates the
//			file format: UNIX, Macintosh, or DOS.
//		GetClipboard() now returns JBoolean because internal clipboard is not
//			created until it is needed.
//		Debounced drag so Drag-And-Drop starts only after moving mouse by at
//			least 3 pixels.
//		Made all versions of GetSelection() public.
//		Made GetLineLength(), GetLineStart(), and GetLineEnd() public.
//		Inserted 3rd argument to constructor: pasteStyledText.
//		Added WillPasteStyledText() and ShouldPasteStyledText().
//		TEOwnsClipboard() and TEGetExternalClipboard() are now const.
//		Renamed protected function GetClipboard() to GetInternalClipboard().
//		Created new public function GetClipboard().
//		Created message TextSet, which is broadcast from SetText(), ReadPlainText(),
//			ReadHTML(), ReadUNIXManOutput(), and ReadPrivateFormat().
//		RemoveIllegalChars() displays a cancellable progress display if it
//			takes more than a couple of seconds.
//		Added IsReadOnly().
//	JThisProcess:
//		Now uses ACE.
//		Provides WillCatchSignal() and ShouldCatchSignal() to control which
//			signals it actually catches.
//		Created Fork().  This must be called instead of fork() to insure that
//			child processes are handled correctly.
//	JUNIXDirInfo:
//		No longer keeps JUNIXDirEntry for "..".
//		ClosestMatch() returns JBoolean since the list might be empty.
//	JSubset:
//		Generalized JGetRandomSample() to include a range.
//		Added GetRandomSample() and GetRandomDisjointSamples().
//	JFileArray:
//		Now stores full path to file so it is safe to change the working directory.
//	JColormap:
//		Added pure virtual GetSystemColorIndex().
//	JVector:
//		Added more constructors.
//		Added JCrossProduct().  (only works for 3D)
//		Added ChangeElement(), GetElements(), Set().
//	JMatrix:
//		Added more constructors.
//		Added ChangeElement().
//		Changed "Column" to "Col" in function names to be consistent with JTable.
//	JOrderedSet:
//		Second argument of GetInsertionSortIndex() can now be NULL.
//	JArray:
//		Fixed bug in SearchSorted1() that returned "not found" when searching for
//			the single existing element with which == JOrderedSetT::kLastMatch.
//	Created JLatentPG to replace almost all uses of JProgressDisplay.
//	JProgressDisplay:
//		Removed constants kCancellable, kNotCancellable, kCanBackground, and
//			kNoBackground from header file.
//		Changed specification of IncrementProgress(const JSize) so that it
//			accepts the change instead of the total.
//		Made GetCurrentStepCount() public.
//	JPtrArray-JString:
//		istream& operator>>(istream&, JPtrArray<JString>&)
//		ostream& operator<<(ostream&, const JPtrArray<JString>&)

// version 1.1.12:
//	*** The default is now for all child processes inside JProcess objects
//		to be terminated (SIGTERM) when the process terminates.
//	JThisProcess:
//		Added QuitAtExit() and KillAtExit() to clean up child processes.
//			This is mainly useful when SIGINT is received, since destructors
//			should clean up everything otherwise.
//		Added Abort() and Exit() to clean up child processes first.
//		Now catches SIGINT and cleans up child processes before exiting.
//	JProcess:
//		Added AtExit() and KillAtExit().
//	*** JIPC will be gone in the next release.
//		Created JInPipeStream and JOutPipeStream to replace JIPCIOStream when
//			dealing with blocking pipes.
//		Created JLineProtocol to replace non-blocking mode of JIPCLine.
//		Moved jSocketUtil::JUnixSocketExists() to jUNIXUtil::JUNIXSocketExists().
//	JFileArray:
//		Added WillWriteIndex(), ShouldWriteIndex() to control whether or not
//			the file's index is written out after every change.  The default
//			used to be kJTrue, but is now kJFalse.  kJTrue helps protect against
//			data loss if the program crashes.
//	JTextEditor:
//		Added ContainsIllegalChars() and RemoveIllegalChars() for stripping
//			out NULL's, etc before pasting.
//		SetText() and ReadPlainText() strip illegal characters.  They
//			return kJFalse if anything was removed.
//	The jRandom module has been replaced by the JKLRand class.
//	JRegex:
//		Added a constructor which sets the pattern, meta character, and
//			JExtension flag simultaneously.
//		SetReplacePattern now returns a JError.
//		The restriction on the match range indices has been lifted;
//			indices greater than the number of subexpressions are replaced
//			with the empty string "".
//		Added the MatchCase property to determine whether
//			JStringRange::MatchCase is used to coerce a replacement string
//			to the same case pattern as the matched string.
//		Added the 'JCore extensions' to the regex language and the
//			NoJExtended property to control them.
//		Added the NoJExtendedDefault property for convenience.
//		Changed the semantics of the MatchFrom functions so that they take
//			a 1-based index like the rest of JCore rather than a 0-based offset.
//		Minor modifications to the GetPattern interface to support storing
//			the pattern in a JString rather than a char*.
//	JRange:
//		Changed Length() to GetLength() for consistency with JString.
//	JStringRange:
//		Added MatchCase().
//		Added member conversion operators to and from Spencer's
//			regmatch_t type.  (This should have been done a long time ago!)
//	Created JCharEscape to process backslash escapes in strings.
//	JStringMap, etc:
//		Renamed from Mapping to Map, since the extra length didn't convey any
//			extra information.
//	Created JStringPtrMap and JStringPtrMapCursor.

// version 1.1.11:
//	JTextEditor:
//		Changed SetFont*() functions to accept a range instead of a single
//			index, and also added argument to tell whether or not to clear
//			all undo information.
//		Added GetStyles() and SetAllFontNameAndSize().
//		Added TEGetLeftMarginWidth() and TESetLeftMarginWidth().
//	JOrderedSet:
//		Changed messages ElementInserted to ElementsInserted and
//			ElementRemoved to ElementsRemoved.
//		Added RemoveNextElements().
//			Derived classes implement this in an optimal way.
//			(If anybody needs RemovePrevElements(), let me know.)
//	JOrderedSetIterator:
//		Added second constructor to take JOrderedSet<T>*.
//		Changed original constructor to take "const JOrderedSet<T>&"
//			so gcc could decide which to call when.
//		Added non-const version of NewIterator().  Derived classes should
//			override both versions.
//		Added SkipPrev(), SkipNext(), SetPrev(), SetNext(), RemovePrev(), RemoveNext().
//			Derived classes implement these in an optimal way.
//		Modified GetOrderedSet() and added GetConstOrderedSet().
//	JPtrArrayIterator:
//		Added DeletePrev(), DeleteNext().
//	JRunArray:
//		Added GetRunDataRef().
//		Added version of InsertElementsAtIndex() to copy from another JRunArray.
//	JString:
//		Added Set().
//		Renamed IsNumber() to IsFloat() and ConvertToNumber() to ConvertToFloat().
//		Added IsInteger(),ConvertToInteger() and IsUInt(),ConvertToUInt().
//			These accept any base between 2 and 36.
//	jTypes:
//		Added JUInt.
//	JAssertBase:
//		Created Abort() so derived classes can perform clean-up.
//		DefaultAssert() calls Abort() whenever possible.
//	Added JDynamicHistogram.

// version 1.1.10:
//	Merged the following files into jTypes.h:
//		JBinIndex.h, JBoolean.h, JCharacter.h, JCoordinate.h, JFileVersion.h,
//		JFloat.h, JIndex.h, JInteger.h, JSignedIndex.h, JSignedOffset.h,
//		JSize.h, JUnsignedOffset.h
//	Added JI2B() as a shortcut for JConvertToBoolean().
//	Added JF2B() as a shortcut for JFloatToBoolean().
//	JString:
//		Added functions that accept JStringRange.
//	JStack:
//		Added version of Pop() that returns kJFalse if the stack is empty.
//		Added Peek().
//	JQueue:
//		Added versions of GetNext() and PeekNext() that return kJFalse
//			if the queue is empty.
//	JTextEditor:
//		Fixed indenting bugs in HTML parser and added support for <blockquote>.
//		Moved argument "wrapSearch" in SearchForward() and SearchBackward() to
//			next to last position.
//		Added versions of SearchForward() and SearchBackward() that take JRegex.
//		Added SelectionMatches() for use before performing replace operation.
//		Right-arrow with moveEOL no longer crashes if there is no text.
//		PrintPlainText() no longer crashes if there is no text.
//		ReadUNIXManOutput() now shows a progress display if it takes too long.
//			The new second argument lets you decide whether the process should
//			be cancellable.
//	JPrefsManager:
//		Now inherits from JContainer.
//		GetData() returns JBoolean instead of using assert().
//		Added IDValid().
//		UpgradeData() must now operate on the data -after- the file is read.
//			It is no longer passed a JPrefsFile*.
//		Renamed UpdateFile() to SaveToDisk().
//	JPtrArray-JString:
//		Added JSameStrings().
//	JPrinter:
//		Added pure virtual WillPrintBlackWhite().
//	JPSPrinter:
//		WillPrintBlackWhite() returns PSWillPrintBlackWhite().
//		Renamed ReadSetup() to ReadPSSetup() and WriteSetup() to WritePSSetup().
//			These functions are no longer virtual because it is too dangerous.
//	JPSPrinterBase:
//		Renamed WillPrintBlackWhite() to PSWillPrintBlackWhite().
//		Renamed PrintBlackWhite() to PSPrintBlackWhite().
//	JEPSPrinter:
//		Renamed DeletePreviewInfo() to DeletePreviewData().
//		Renamed ReadSetup() to ReadEPSSetup() and WriteSetup() to WriteEPSSetup().
//			These functions are no longer virtual because it is too dangerous.
//	JTable:
//		Changed prototypes of TablePrepareToDrawRow(), TablePrepareToDrawCol()
//			to include the first and last cell to be drawn in each case.

// version 1.1.9:
//	JTable:
//		Added 3rd parameter to TableDraw() to indicate whether or not the
//			far lower right row and column borders should be drawn.  This
//			should usually be done on the screen, but not when printed.
//		Added message TableDataChanged() which is broadcast when SetTableData()
//			is called.
//		AuxTableData objects now update themselves when SetTableData() is called.
//	JValueTableData:
//		Added GetDefaultValue(), SetDefaultValue().
//	JPackedTableData:
//		Added ClearAllElements(), GetDefaultValue(), SetDefaultValue().
//	assert():
//		Changed output format to the standard "file:line" so Code Crusader
//			users can Ctrl-dbl-click on the file name to open it.
//	JTextEditor:
//		Added kShiftSelLeftCmd,kShiftSelRightCmd to CmdIndex.
//		GetCmdStatus() now takes argument isReadOnly.
//		Added more versions of GetDefaultFont().
//		TabSelectionLeft/Right() now accept an argument specifying how many
//			tabs to shift by.

// version 1.1.8:
//	JTextEditor:
//		Added auto-indenting.  Turn it on/off with ShouldAutoIndent().
//		Added CleanRightMargin() to adjust newlines to keep the text within
//			an adjustable number columns (default 75).
//		Added SetType() so you can change the behavior at any time.
//		Added kCleanRightMarginCmd, kToggleReadOnlyCmd to CmdIndex.
//		Added pure virtual function TEDisplayBusyCursor(), which is called
//			before lengthy processes like reading files.
//		You can choose whether moveEOL for the left arrow should move to
//			the beginning of the line or the beginning of the text by
//			calling ShouldMoveToFrontOfText().
//		Text can now be dragged from kSelectableText, but obviously still not to it.
//		Added JColormap* argument to constructor, thereby eliminating
//			need for ColorNameToColorIndex(), RGBToColorIndex(), and
//			ColorIndexToRGB() in derived classes.
//		Optimized Read/WritePrivateFormat().
//		Renamed GetFontManager() to TEGetFontManager().
//		Renamed GetColormap() to TEGetColormap().
//	Merged JColorIndex.h and JDynamicColorInfo.h into jColor.h and created JRGB.
//	Changed contents of struct JDynamicColorInfo.
//	JColormap:
//		Added functions to use JRGB.
//		Added version of SetDynamicColor() that takes JDynamicColorInfo.
//		The "exactMatch" parameter for AllocateStaticColor() can now be NULL.
//	JGetCurrentColormap:
//		GetCurrColormap() now returns "JColormap*".
//	jGlobals:
//		JGetCurrColormap() now returns "JColormap*".
//	Fixed JAssertBase so it prints the assert message even if it
//		doesn't ask "quit or continue?".
//	JOrderedSet:
//		InsertSorted() now returns kJTrue if the element was inserted.
//		It also takes an optional 3rd parameter to return the insertion index.
//	JOrderedSetUtil:
//		Added JCompareIndices(), JCompareSizes().
//	JUNIXDirInfo:
//		Stores JString instead of JString* to avoid allocating too many fragments.
//	JTableSelection:
//		Added GetFirstSelectedCell(), GetLastSelectedCell().

// version 1.1.7:
//	Moved all global variables behind the function interface in jGlobals.h:
//		gAssert, gUserNotification, gChooseSaveFile, gCreatePG,
//		kJDefaultFontName, kJGreekFontName, kJMonospaceFontName
//	Removed font constants from JFontStyle.h:
//		kJTinyFontSize, kJSmallFontSize, kJNormalFontSize,
//		kJMediumFontSize, kJLargeFontSize, kJHugeFontSize
//	JOrderedSet and derived classes:
//		Added SetCompareFunction() and removed compare argument from
//			sorting functions.
//		All derived classes of JOrderedSet must call OrderedSetAssigned()
//			inside operator= -after- copying the data.
//			See notes for JOrderedSet<T>::operator=.
//		Added new message ElementsSwapped.  All derived classes must
//			broadcast this in SwapElements(), instead of two ElementChanged
//			messages.
//		Added new message Sorted.  Derived classes can now implement fast
//			sorting functions and simply broadcast Sorted when done.
//			(e.g. JArray provides QuickSort() which calls qsort() and can
//				  therefore not broadcast ElementMoved.)
//	JArray:
//		Added GetCArray(), AllocateCArray(), QuickSort().
//	JPtrArray:
//		Renamed NullElement() to SetToNull().
//	JRunArray:
//		Now searches and sorts by run.
//	JOrderedSetUtil:
//		Prefixed functions with J.
//		Added JAdjustIndexAfterSwap().
//	JStack,JPtrStack,JQueue,JPtrQueue:
//		These templates are now parametrized on both the storage type
//			and the data type.
//	JStack:
//		The top element is now at the end of the JOrderedSet.
//	JStack, JQueue:
//		Added NewIterator() so one can examine all the elements.
//	JFileArray:
//		Added new message ElementsSwapped.  SwapElements() now broadcasts this
//			instead of two ElementChanged messages.
//		Added versions of SetElement(), InsertElementAtIndex() that take
//			const JCharacter*.
//	JPrefsFile:
//		Merged NewData() into SetData().
//		Added version of SetData() that takes const JCharacter*.
//	Created JColormap.
//	JPainter:
//		Constructor requires JColormap* instead of default color value.
//		Added GetColormap().
//		GetFontManager() is now public.
//		Added functions for drawing dashed lines.
//	JPSPrinterBase:
//		Constructor requires JColormap* instead of color value.
//		Now uses "eofill" instead of "fill" so filling works the same way
//			on the screen an on paper.
//		Added functions for drawing dashed lines.
//	Adjusted constructors for JPrinter, JPSPrinter, JEPSPrinter.
//	JTextEditor:
//		Added multiple undo support.  UseMultipleUndo() turns it on.
//		Added WillBreakCROnly(), SetBreakCROnly().
//		Added ReadUNIXManOutput(), ReadPrivateFormat(), WritePrivateFormat().
//		Added functions to activate and deactivate the caret.
//		TEHandleKeyPress() returns JBoolean.
//	jProcessUtil:
//		JExecute() now sets the process group id of the new process to
//			the new process' id.
//		Added JGetPGID(), JSendSignalToGroup().
//	JProcess:
//		Added GetPGID(), SendSignalToGroup().
//		Quit() and Kill() send their signals to the entire process group
//			so all child processes created by the JExecute()'d process
//			will also disappear.
//		Added IsFinished() and company.
//	JBroadcaster:
//		Added BroadcastWithFeedback().  Derived classes can catch these messages
//			by overriding ReceiveWithFeedback().
//	Created JThisProcess to catch signals and turn them into
//		BroadcastWithFeedback() messages.
//	JImage:
//		Added pure virtual function SetImageData().
//		Added function ReadFromJXPM().  This must be called by a derived class.
//	JString:
//		Added GetCString(), InsertCharacter(), PrependCharacter(), AppendCharacter().
//			(What took me so long???)
//		Proved that JStrings can contain NULL characters.  You just have to
//			remember that they can't be passed to Clib functions like strlen().
//		Renamed JCaseInsensitiveCompare() to JStringCompare().
//		Added functions to take (str,length) and ContainsNULL().
//		Added function JStringEmpty().
//	Created JPtrArray-JString.h to define useful sorting functions for
//		JPtrArray<JString> objects.
//	JTable:
//		Added IsEditing().  Promoted GetEditedCell() to public access.
//	JPackedTableData:
//		Added SetRow() and SetCol() that take a single data element.
//			These set the entire row/col to the same value.
//	JTableSelection:
//		Optimized SelectRow() and SelectCol().
//	JTableSelectionIterator:
//		Fixed bug in Prev(), so now it will actually return the last cell,
//			when appropriate.
//	JStringTable:
//		Added versions of SetString() that take "const JString&".
//	jFileUtil:
//		Added JGetUniqueFileName().
//	jDirUtil:
//		Added JCanEnterDirectory().
//		JSearchSubdirs() now creates a progress display if you don't pass one in.
//	Created JMDIServer.
//		Includes HandleCmdLineOptions() to handle arguments passed to the initial
//			invocation of the program.
//	JRect:
//		Added operator+(JRect,JPoint) and operator-(JRect,JPoint) to complement Offset().
//	assert():
//		The action of the assert() macro is now controlled by an environment
//			variable.  Check the docs for JAssertBase.
//	JUNIXDirEntry:
//		Added types kUnknown and kUnknownLink.
//	JUNIXDirInfo:
//		Renamed ContentsUpdated message to ContentsChanged.
//		PathChanged is now broadcast after ContentsChanged instead of before.
//		If the contents will merely change due to filtering or updating,
//			instead of changing directories, it broadcasts ContentsWillBeUpdated
//			before changing anything.  This gives others a chance to save
//			state that can be restored after ContentsChanged.
//			Note that PathChanged cancels the effect of ContentsWillBeUpdated.
//		Added ShowOthers() to control visibility of JUNIXDirEntry::kUnknown and
//			JUNIXDirEntry::kUnknownLink.
//		Added ChangeProgressDisplay() to allow custom displays.
//		Added ClosestMatch() to support incremental search while typing.
//	JRTTIBase, JBroadcaster:
//		Changed format of type strings to "<class name>::<owner name>" so
//			the string comparisons go faster for different messages broadcast
//			by the same class.
//	Integrated JRegex from Dustin Laurence. (Thanks to Henry Spencer.)
//		One no longer needs libg++ on UNIX.
//		However, the default for JRegex is egrep, while the default for
//			GNU Regex is grep, so you have to either rewrite your regex
//			expressions or tell JRegex to use grep.
//	Integrated JStringMapping and JHashTable from Dustin Laurence.
//	Integrated JMemoryManager from Dustin Laurence.
//	Created JPrefsManager to buffer data in a JPrefsFile and to provide
//		a base class for application-specific preferences management.
//	JRect:
//		Renamed JUnion() to JCoverage().

// version 1.1.6:
//	JBroadcaster:
//		Added ReceiveGoingAway().  Read the warnings in the .doc file!
//	JBroadcastSnooper:
//		Catches ReceiveGoingAway() messages.
//	jSignal.cc:
//		Added JGetSignalName().

// version 1.1.5:
//	JMatrix:
//		operator>> sets ios::failbit instead of using assert() if the data
//			is corrupted.
//	JTextEditor:
//		Added SearchForward() and SearchBackward().
//	JChooseSaveFile:
//		Added second argument to ChooseRPath() and ChooseRWPath().  If not NULL,
//			this specifies the directory to start in.
//	Created JProcess class to encapsulate a child process.
//	jProcessUtil:
//		Added option to errAction: kJAttachToFromFD.
//		Appended "status" argument to both versions JWaitForChild().
//			The value defaults to NULL, so all existing code will compile
//			and work as before.
//	JProcess:
//		Finished() message includes functions to tell why the process finished.
//	JTable:
//		Added second argument "centerInDisplay" to TableScrollToCell().
//			The value defaults to kJFalse, so all existing code will compile
//			and work as before.

// version 1.1.4:
//	jStreamUtil:
//		Added version of JIgnoreUntil() that accepts a string as a delimiter.
//	JString:
//		Changed TrimSpaces() to TrimWhitespace().
//		operator>> sets ios::failbit instead of using assert() if it cannot
//			find the initial double quote.
//		Removed kJStringPrefix and kJStringSuffix because they are too dangerous.
//	JTextEditor:
//		Added ReadHTML().
//		Added new pure virtual function ColorNameToColorIndex().
//		Meta-left-arrow is smart enough to move the caret to the beginning of the
//			*text* on the line.  Pressing it a second time moves the caret to the
//			beginning of the line.
//	JRunArray:
//		Added SetBlockSize().
//	JBoolean:
//		operator>> sets ios::failbit instead of using assert() if it reads
//			an illegal value.

// version 1.1.3:
//	JRTTIBase:
//		Is() now assert()'s that its argument is not NULL.
//	JError:
//		Constructor now takes a third argument "copyMsg".  If this is kJTrue,
//			then the object makes a copy of the message string that is passed in.
//	JComplex:
//		In kDisplayMagAndPhase mode, phase=180 is printed as a negative, real number.
//	JTextEditor:
//		Added PrintPlainText() and associated virtual functions for header, footer,
//			and tabs.
//	JPSPrinterBase:
//		Fonts now use ISOLatin1Encoding instead of StandardEncoding.  This way,
//			the characters on the screen match the characters that are printed.
//		Sped up printing by removing all calls to endl.

// version 1.1.2:
//	Renamed all utility modules to standardize capitalization:
//		jFileUtil, jDirUtil, jProcessUtil, jUNIXUtil, jMath, jRandom,
//		jStreamUtil, jFStreamUtil, jStrStreamUtil, jTime, jAssert,
//		jCommandLine, jMemory, jNumericConstants, jErrno, jSignal,
//		jFStreamOpenModes
//	Added k<type>Min, k<type>Max to all typedefs. (e.g. JIndex and JFloat)
//	jProcessUtil:
//		Completely reorganized the module.  Check the .doc file.
//	Renamed the error JFileExists to JFileAlreadyExists.
//	jFileUtil:
//		Added JGetFileLength().
//	JTextEditor:
//		Double clicking beyond the end of a line that ends with a word
//			now selects the word instead of the newline character.
//		Double clicking beyond the end of the text when the text does
//			not end with a newline now selects the entire last word.
//		Shift-arrow now selects text.
//		Styled text format now specifies text by length instead of by
//			delimiters.
//		TextChanged message now guarantees that "text has already changed".
//	jAssert:
//		Created global object "JAssertBase* gAssert".  If this is not NULL,
//			it is used to process failed assert()'s.  It it is NULL, the
//			usual text message is displayed.
//		Check JAssertBase.doc for information on creating derived classes.
//	JError:
//		Added default constructor.
//	JString:
//		Added Read() and JCopyMaxN().
//		Extended JString(JFloat,...) constructor to allow specification
//			of desired exponent.
//	JFontManager:
//		Added more versions of GetStringWidth().
//		The single virtual version of GetStringWidth() has a new prototype.
//	JTableData:
//		Added message AllElementsChanged().
//	Created JPackedTableData to store table data in a single JRunArray.
//	JAuxTableData now derives from JPackedTableData.
//	Created JTableSelection to keep track of selected cells in a JTable.
//	JTable:
//		Added GetTableSelection() and HilightIfSelected().
//		Print() asserts that nothing is being edited.  It is the client's
//			responsibility to call EndEditing() before calling Print().
//	Created JTableSelectionIterator to provide a simple way to get the
//		coordinates of the currently selected cells.
//	JBroadcaster:
//		Made HasSenders(), GetSenderCount(), HasRecipients(),
//			GetRecipientCount() public.
//	JArray:
//		Added conversion operator to return "const T*".
//	Split JPSPrinter into JPSPrinterBase and JPSPrinter.
//	Created JEPSPrinter.
//	jStreamUtil:
//		JReadUntil() returns kJFalse if it hits an end-of-file.  Previously,
//			it only returned kJFalse if it got an error while reading.
//			If kJFalse is returned, it does not change the value of *delimiter.
//		Added "found" return variable to all JReadUntil*() functions.
//		Improved JIgnoreUntil() functions.
//	Removed JInputPipe and JOutputPipe.  The correct way is now to first call
//		JExecute() and then construct JIPCIOStream's with the fd's.
//	Defined JFontID to be an index into JFontManager's list of allocated
//		fonts.  This way, we don't care how the system wants to reference a
//		font, and 0 is guaranteed never to be used.
//	JPtrArray:
//		Added NullElement() as convenience to avoid
//			SetElement(i, static_cast(T*, NULL)).
//	JComplex:
//		Added second version of JPrintComplexNumber().
//	Integrated JIPC from Dustin Laurence.  Check the test suite for the right
//		ways to use these classes.
//	JPartition:
//		Added ReadGeometry() and WriteGeometry().
//	jMath:
//		JSign() returns an int because this will automatically upgrade to
//			double, whereas, if it returned a double, it wouldn't automatically
//			convert to int.
//	Created JIPC suite for socket communication.
//	Eliminated JUNIXDirInfo::DirectoryUnreadable in favor of JAccessDenied.

// version 1.1.1:
//	Renamed all global functions to start with J to avoid naming conflicts
//		under Visual C++.  All the functions in the following files were renamed:
//		jmath.h, jrandom.h, jtime.h, jdirUtil.h, jfileUtil.h, jstreamUtil.h,
//		jfstreamUtil.h, jstrstreamUtil.h
//	jtrunc() renamed to JTruncate().
//	Renamed the following functions to start with J:
//		JBoolean.h: ConvertToBoolean(), Negate()
//		JPoint.h:   PinInRect()
//		JRect.h:    Intersection(), Union()
//		JString.h:  CaseInsensitiveCompare(), CompareMaxN()
//		JSubset.h:  Intersection(), Union(), GetRandomSample(),
//					AddToSubsetInStream(), RemoveFromSubsetInStream()
//		JVector.h:  OuterProduct(), DimensionsEqual(),
//					Dot() -> JDotProduct()
//		JMatrix.h:  IdentityMatrix(), DimensionsEqual()
//	jfstreamOpenModes.h:
//		Changed JFstreamOpenMode to JFStreamOpenMode.
//		Renamed kTextFile to kJTextFile.
//		Renamed kBinaryFile to kJBinaryFile.
//	jisascii() renamed to JIsASCII() for consistency.
//	jCommandLine.h renamed to jcommandLine.h for consistency.
//	jNumericConstants.h renamed to jnumericConstants.h for consistency.
//	JDirError changed to JError.  JError will now evolve to completely replace errno.
//		Moved enum declaration to JError.h.
//	JGetFileModificationTime() generalized to JGetModificationTime() and
//		moved to jdirUtil.h.
//	jdirUtil.h:
//		Added JGetPermissions() and JSetPermissions().
//		Added JRenameDirectory().
//	jfileUtil.h:
//		Added JRenameFile() and JRemoveFile().
//	jstreamUtil:
//		Changed prototype for JReadAll() to make function safer for large files.
//	jfstreamUtil:
//		Changed prototypes for JReadFile() to make functions safer for large files.
//	jrandom.h:
//		Added JRndUInt().
//	Moved JGetRandomSample() to JSubset.h and eliminated jGetRandomSample.h.
//		Rewrote it to run in O(N) time.
//	JFileArray:
//		Files now contain a "locked" flag so they cannot be opened by more than
//			one program at a time.
//		Create() functions now return error codes instead of JBoolean.
//	JUNIXPrefsFile:
//		Constructor is now protected.  Use the Create() function instead.
//	JTextEditor:
//		Implemented reading and writing styled text in a private format.
//		Two new pure virtual functions are required in order to read and write
//			styled text:  RGBToColorIndex(), ColorIndexToRGB()
//	JBoolean.h:
//		Added JFloatToBoolean() to convert floating-point numbers to JBoolean.
//	Created JComplex.h to provide JComplex type, some math functions that ANSI
//		didn't bother to implement, and JPrintComplexNumber().
//	JOrderedSet:
//		Cleaned up sorting and searching functions.
//		BinarySearch() renamed to SearchSorted().  This now has a new 3rd
//			argument "which" that specifies what to return if more than one
//			element matches the target.
//		InsertSorted() now takes 3rd argument "insertIfDuplicate".
//			Because of this, it no longer an index.
//		Created SearchSorted1().  This is what all the other functions call.
//			It is virtual so derived classes can optimize it.
//	Created JRTTIBase to be base class for all objects that need RTTI.
//	JBroadcaster::Message:
//		Now derived from JRTTIBase.
//		operator== and operator!= now available via JRTTIBase.
//	Created JError class to be the base class of all errors returned by JCore
//		functions.  JError.h defines JError, JNoError, JUnknownError,
//		JUnexpectedError.  JStdError defines classes to replace common errno
//		values.
//	JUNIXDirEntry:
//		Renamed GetPathAndName() to GetFullName().
//		Implemented SetMode() functions.
//	Created junixUtil to contain JGetUserName() and JGetGroupName().
//	Created jprocessUtil to contain JExecute().  This replaces all calls
//		to exec*(), system(), popen(), pipe(), etc.
//	JKillDirectory() now returns JBoolean.

// version 1.1.0:
//	jdirUtil:
//		Appended "JProgressDisplay* pg = NULL" to argument list of SearchSubdirs().
//		By having the pg passed in, jdirUtil no longer requires gCreatePG.
//	JTextEditor:
//		Added functionality to Drag-And-Drop between objects.
//		Added new pure virtual function TEBeginDND().

// version 1.0.10:
//	Changes made to JX.

// version 1.0.9:
//	Removed const from return type of JMin() and JMax().

// version 1.0.8:
//	Changes made to JX.

// version 1.0.7:
//	Fixed template instantiation mechanism to allow multiple instantiations
//		per user source file.

// version 1.0.6:
//	Changes made to JX.

// version 1.0.5:
//	JRunArray:
//		SetRunData() is now public.
//	JRunArrayIterator:
//		Fixed bug so it works when you call SetElement(), MoveElementToIndex(),
//			SwapElements(), and Sort().
//	JFileArray:
//		Internal changes to make it work on 64-bit machines.

// version 1.0.4:
//	Various minor changes to make the library compile on 64-bit machines.
//	jTime.h:
//		Defined J_TIME_T_MAX.
//	JTable:
//		Fixed fatal bug in MoveRow() and MoveCol().
//		Aux data objects are now kept in sync even if table has no
//			main data object.
//		Fixed bug in drawing cell borders.
//	JBroadcaster:
//		Fixed StopListening() so it won't fail even if the specified object
//			isn't actually being listened to.
//	JPSPrinter:
//		Image() doesn't output anything if the current page is not supposed
//			to be printed.
//	JTextEditor:
//		Fixed obscure, benign bug in recalculation code.
//	JUNIXDirInfo:
//		If the current directory loses read permissions, it automatically
//			switches to either the users's home directory or /.

// version 1.0.3:
//	jstrstreamUtil.h:
//		Created macros jistrstream() and jistrstream2() to provide way to
//			switch between using a broken istrstream class and a broken
//			strstream class.  Updated testJFileArray and testJPrefsFile.
//	JTextEditor:
//		Double clicking in an empty input field no longer crashes.
//		TabSelectionLeft() and TabSelectionRight() no longer crash when
//			the text buffer is empty.

// version 1.0.2:
//	JBroadcastSnooper:
//		Fixed prototype of Receive() function so it overrides JBroadcaster::Receive().
//		(Don't you wish C++ would provide a way to catch that error?)

// version 1.0.1:
//	JTextEditor:
//		The selection now extends immediately when the right mouse button is
//		clicked instead of waiting until the mouse moves.
//	JProgressDisplay:
//		ProcessType and GetCurrentProcessType() are now public.
//		Also added ProcessRunning().

#endif
