/******************************************************************************
 JXLibVersion.h

	Defines current version of JX library

	Copyright (C) 1996-2011 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXLibVersion
#define _H_JXLibVersion

// This has to be a #define so it can be used for conditional compilation.

#define CURRENT_JX_MAJOR_VERSION	4
#define CURRENT_JX_MINOR_VERSION	0
#define CURRENT_JX_PATCH_VERSION	0

// This is mainly provided so programmers can see the official version number.

static const char* kCurrentJXLibVersionStr = "4.0.0";

// version 4.0.0:
//	Upgraded to new api's using JFont.
//	*** Upgraded jxlayout to work with latest version of fdesign
//		Bug in fdesign makes it impossible to set FL_NO_BOX, so you have
//			to edit the file manually after saving it.
//	Fixed JXSelectionManager to send required (broken) data for XA_ATOM (XXXATOM).
//		*** Replaced GetTextXAtom() with GetUtf8StringXAtom().
//		*** Removed GetCompoundTextXAtom().
//	Added HistoryDirection to JXHistoryMenuBase to support menus that are
//		anchored near the bottom of the screen.
//	JXContainer:
//		Added ExpandToFitContent() to adjust layouts to fit translated/resized text.
//			This is automagically called by JXWindow after it has been constructed.
//	Implemented ToString() for various widgets.
//	JXStaticText:
//		Added option to horizontally center labels.
//	JXTextMenu:
//		Menu font is no longer independently adjustable via ~/.jx/menu_font.
//	JXApplication:
//		--debug-ftc-horiz		debugging info for horizontal FTC
//		--debug-ftc-vert		debugging info for vertical FTC
//		--debug-ftc-noop		extra debugging for noop cycles in FTC
//		--debug-ftc-overlap		extra debugging for FTC overlap calculations
//		--pseudotranslate		fake translation to test layout
//	JXToolBar:
//		*** Removed minWidth & minHeight ctor arguments, because FTC can
//			change the values.
//	JXMenuBar:
//		Fixed bug in FindMenu() so it searches overflow.  (What took me so long?)
//	*** Replaced in-app help with web-based help.
//	JXHelpManager:
//	*** Removed JXHelpDirector, etc.
//		Added methods ShowChangeLog() and ShowCredits().
//	*** App must define HELP_URL and CHANGE_LOG_URL.
//	All strings have been extracted to jx_strings.
//	JXTipOfTheDayDialog
//		*** Switched from HTML to limited markdown.  Changed tip separator to "====="

// version 3.1.0:
//	JXGetStringDialog:
//		Promoted GetInputField() to public.
//	JXTreeListWidget:
//		Fixed crash when item being toggled by drag disappears before mouse up.
//	JXPrefsManager:
//		Added default implementation of CleanUpBeforeSuddenDeath()
//		Add virtual SaveAllBeforeDestruct() which must be called by the
//			leaf class destructor.
//		Periodically saves all data to disk to protected against crashes.

// version 3.0.0:
//	*** Run new jxlayout on all .fd files.
//	Created jx_memory_debugger.
//	Switched all uses of "6x13" to JGetMonospaceFontName().
//	JXFontManager:
//		Added support for anti-aliased fonts.
//		Removed JXColorManager* arg from ctor.
//	JXGC:
//		Added support for anti-aliased fonts.
//		Removed JXColorManager* arg from ctor.
//	JXWindowPainter:
//		Rotated strings are anti-aliased.
//		Fixed bugs in placement of rotated text.
//		Promoted GetDrawable() to public.
//	JXPSPrintSetupDialog:
//		Fixed bug so Print button is enabled when dialog is opened.
//		Fixed crash when print to printer and file name input is empty.
//	JXColorManager:
//		Private colormaps are obsolete.
//		Renamed AllocateStaticNamedColor() to GetColor(),
//		Renamed AllocateStaticColor() to GetColor().
//			Now returns JColorID.  Removed exactMatch argument
//		Replaced GetGray*Color() with GetGrayColor(percentage).
//		Removed public version of Create(), GetMaxColorCount(),
//			CanAllocateDynamicColors(), AllocateDynamicColor(),
//			SetDynamicColor(), SetDynamicColors(), UsingColor(),
//			DeallocateColor(), PrepareForMassColorAllocation(),
//			MassColorAllocationFinished(), Will/ShouldApproximateColors(),
//			Will/ShouldPreemptivelyApproximateColors(),
//			Get/SetColorDistanceFunction(), Get/SetColorBoxHalfWidth(),
//			Get/SetPreemptiveColorBoxHalfWidth(), XPMColorDistance(),
//			AllColorsPreallocated(), CalcPreallocatedXPixel(), GetColorIndex(),
//			GetXPixel(), GetSystemColorIndex().
//			NewColormap message, JXColorDistanceFn
//	JXWindow:
//		Removed ownsColormap and colormap arguments from ctor.
//		Removed --desktop-h & --desktop-v options.  They are not useful.
//		Replaced ~/.jx/wm_offset & ~/.jx/wm_desktop_style with
//			AnalyzeWindowManager(), since you may run programs from a single
//			computer on multiple X servers, each with a different window manager
//			with different quirks.
//		Fixed Show() and Raise() so they don't destroy the undocked window
//			position while the window is docked.
//		Optimized GetRootChild() to cache the result.  (Reparenting clears it.)
//	JXDisplay:
//		Removed AllowPrivateColormap() and ForcePrivateColormap().
//		When created, analyzes window manager behavior to detect quirks for
//			which JXWindow must compensate.  This information is cached on the
//			root window to avoid wasting time at the next invocation.
//	JXImage:
//		Removed allowApproxColors argument from all CreateFrom*().
//		Removed JXColorManager* arg from all ctors and CreateFrom*().
//		Added optional initState arg to basic ctor, to allow creating XImage.
//	JXImageMask:
//		Removed JXColorManager* arg from all ctors and CreateFrom*().
//	JXImageSelection:
//		Removed allowApproxColors argument from GetImage().
//	JXStyleMenu:
//		Removed first ctor arg allowChooseCustomColors.
//		Removed HandleCustomColor().  HandleMenuItem() is called for all items.
//	Created JXColorWheel.
//	JXChooseColorDialog:
//		Added color wheel.
//	jXGlobals:
//		Removed JXGet*FontName().
//	JXXFontMenu:
//		Renamed FontNeedsUpdate to NameNeedsUpdate, FontChanged to NameChanged.
//	JXTextSelection:
//		Removed JXColorManager* arg from ctor.
//	JXPSPrinter:
//		Removed JXColorManager* arg from ctor.
//	JXEPSPrinter:
//		Removed JXColorManager* arg from ctor.
//	JXWidget:
//		Added Will/ShouldAllowUnboundedScrolling().
//	JXStaticText:
//		Added SetToLabel().  jxlayout uses this to make labels in dialogs look nicer.
//	Removed kJXDefaultFontSize.  Use kJDefaultFontSize or kJDefaultMonoFontSize.
//	JXButton:
//		Automatically expands to make space for border when ^M shortcut is added.
//	JXGetStringDialog:
//		Added optional arg "password" to make the dialog usable for passwords.
//	JXTabGroup:
//		Tabs now scroll when you use the horizontal scroll buttons (6,7)
//			in addition to the vertical scroll buttons (4,5).
//	JXPasswordInput:
//		Renders dots instead of hashes, and displays symbol when Caps Lock is on.
//	JXWindowDirector:
//		Demoted SetWindow() to private.  JXWindow now calls it automatically.
//	JXMenu:
//		Added ItemType enum.
//		Added IsFromShortcut() to NeedsUpdate message.
//	JXMenuData:
//		Collapsed isCheckbox and isRadio args for Insert/Prepend/AppendItem()
//			into single type.
//		Replaced IsCheckbox() with GetType().
//	JXTextMenu:
//		Collapsed isCheckbox and isRadio args for Insert/Prepend/AppendItem()
//			into single type.
//	JXImageMenu:
//		Collapsed isCheckbox and isRadio args for Insert/Prepend/AppendItem()
//			into single type.
//	JXToolBar:
//		Fixed bug so all buttons associated with deactivated menus are disabled.
//	JXTEBase:
//		Added menu items "Clean all std::ws & alignment" and
//			"Clean std::ws & align in selection"
//	JXColHeaderWidget:
//		Added GetDragType() for use by derived classes overriding HandleMouseDown().
//	JXRowHeaderWidget:
//		Added GetDragType() for use by derived classes overriding HandleMouseDown().
//	JXIdleTask:
//		Added Start(), Stop().
//	JXUrgentTask:
//		Added Go().
//	JXScrollableWidget:
//		Sets back color = focus color if it has scrollbars.
//		Fixed crash in ScrollForWheel() when side-scrolling and hScrollbar is nullptr.
//	JXInputField:
//		Added context menu for basic commands.
//	JXFileDocument:
//		Preserves file ownership.  (What took me so long?)
//	JXSearchTextDialog:
//		Added SetRegexSearch(), SetRegexReplace().
//	Created JXVIKeyHandler.
//	JXFileInput, JXPathInput:
//		Tab completion ignores VCS directories (CVS, .svn, .git, etc.)

// version 2.6.0:
//	JXWebBrowser:
//		Fixed ShowFileLocation() to accept $p for the file path.
//		Updated default commands to leverage "open" on OS X.
//		Added ShowFileLocations() to optimize showing multiple files.
//	JXFSBindingManager:
//		Updated default commands to leverage "open" on OS X.
//	JXTabGroup:
//		Added protected FindTab().
//		Added option to make tabs closable.  Override OKToDeleteTab() if
//			you want to close the tab yourself.
//		Added pre/post margin for text of each tab.
//		Added virtual DrawTab() to allow drawing stuff in the
//			pre/post margin of each tab.
//		Fixed two versions of ShowTab() to use identical logic.
//		Closing current tab re-selects previously selected tab, not adjacent tab.
//		When enclosure is expanded or tab is removed, scrolls tabs
//			to make more of them visible, when possible.
//		Fixed bugs so minimum compartment sizes are set correctly.
//	Created JXDockTabGroup to provide augmented context menu and close
//		button in each tab.
//	JXImageButton:
//		Added FitToImage().
//	JXSelectionData:
//		Fixed bug so it works on 64-bit computers.
//	JXWindow:
//		Place() resizes window to fit the screen, if the window is too large.
//		Optimized handling of expose events.
//		Ctrl-Page-Up/Down cycles through docked windows in a pane.
//		Global JX preference to have focus follow the mouse between docked windows.
//		Updated placement code to handle latest curveball from OS X.
//		Supports _NET_WM_PING and _NET_WM_WINDOW_TYPE protocols.
//		Added skipDocking argument to ReadGeometry().
//	JXProgressDisplay:
//		Updated ProcessBeginning() so it hides the unused widgets and
//			initializes the visible ones.  (What took me so long?)
//		Added optional label argument to SetItems().
//	JXApplication:
//		Fixed Suspend() to close all open menus.
//		Added --xdebug flag which turns on synchronous processing of X errors.
//			This allows you to get a stack trace right where the problem occured.
//		Added --desktop-h and --desktop-v for specifying how much to chop off from
//			the root window size.  Mostly useful for OS X.
//	JXMDIServer:
//		Implemented PreprocessArgList() to remove JXApplication options which only
//			make sense for an initial invocation.
//		Added support for --quit argument, which invokes JXApplication::Quit().
//	JXDocument:
//		Removed CastToJXFileDocument() since dynamic_cast<> now does the job.
//	JXMenu:
//		Removed CastTo*() since dynamic_cast<> now does the job.
//		Optimized PrepareToOpenMenu() to only call parent if not already open.
//		Optimized PrepareToOpenMenu() to short-circuit if updating for JXToolbar
//			and none of the items have an ID.
//	Added awareness of Xinerama.  Dialogs are now centered on the "active" screen.
//	Created JXSharedPrefObject to allow JXSharedPrefsManager to be more general.
//		Now, JXSharedPrefsManager only needs reserved ID's.  It does not need
//		to know details about every object for which it stores data.
//	JXSharedPrefsManager:
//		Fixed bug so signalling now works again and changes in one application
//			propagate to all other running applications.
//	JXHelpManager:
//		Fixed bug so it updated prefs when print setup changes.
//	Merged JXSpellChecker from arrow.
//	JXTEBase:
//		*** Added show/allowCheckSpelling arguments to AppendEditMenu() and
//			allowCheckSpelling argument to ShareEditMenu().
//		*** Added show/allowCleanWhitespace arguments to AppendEditMenu() and
//			allowCleanWhitespace argument to ShareEditMenu().
//		Added version of Append/ShareEditMenu() with no JBoolean arguments.
//			This covers the most common case where only
//			Undo/Cut/Copy/Paste/Clear/SelectAll is needed.
//		Implements "hungry delete" if tab inserts spaces.  The shift key
//			toggles the behavior, regardless of the mode.
//		Added Save/RestoreDisplayState().
//	Created JXImageCache to store mapping between JXPM and JXImage*.
//	JXDisplay:
//		Added GetImageCache().
//		Added RegisterXAtoms().  Application startup over a network is now
//			faster because individual RegisterXAtom() calls have been combined.
//	JXTextMenu:
//		Uses JXImageCache to minimize creation of JXImages.
//	JXContainer:
//		Fixed subtle bug in Show()/Hide() and Activate()/Deactivate() which
//			caused crashes if Show()/Activate() was invoked by derived class
//			Hide()/Deactivate().
//		Added Suspend()/Resume().  This is subtly different from Activate()/
//			Deactivate():  It doesn't unfocus the widget, but it still looks
//			inactive.  IsActive() returns kJFalse when suspended.
//		*** Added pt argument to WillAcceptDrop(), to allow drop targets to
//			sub-divide their contents.
//	JXUNDialogBase:
//		Updated to respect JUserNotification::BreakMessageCROnly.
//	JXWDManager:
//		Added GetPermanentDirectors() and GetDirectors().
//	Added jx_busy_[1-8] XPM images for busy indicator.
//	Created JXBusyIconTask.
//	JXCardFile:
//		Added GetCardIndex().
//		Re-broadcasts JOrdredSet messages to allow external data to be kept in sync.
//	JXDockWidget:
//		Shows hint if window is too small to fit when trying to drop it.
//		When window title changes, rearranges tabs to keep them alphabetized.
//	JXDNDManager:
//		Fixed bug so DND between docked windows in same application works.
//		Updated to latest XDND spec.
//	JXScrollableWidget:
//		Added Save/RestoreDisplayState().
//		*** Remove all calls to ScrollForDrag() from implementations of HandleDNDHere().
//			Scrolling during DND is now done automatically.
//		ScrollForWheel() supports horizontal scrolling via mouse buttons 6 & 7.
//	Created JXGetStringDialog.
//	Renamed JXNewDisplayDialog to JXOpenDisplayDialog.
//	JXCSFDialogBase:
//		*** Added desktopButton argument to SetObjects().
//	JXMenuBar:
//		Added FindMenu().
//	JXKeyModifiers:
//		Added operator== and operator!=
//	JXButtonStates:
//		Added operator== and operator!=
//		Added kJXButton6 & kJXButton7.
//	JXFileListTable:
//		Modified AddFile() to always insert the file, even if it does not
//			exist.  This aligns better with reality, since existence at the
//			time of insertion is no guarantee of existence later on.  This
//			also simplifies the usage, because fullNameIndex will always be
//			valid.
//	JXGoToLineDialog:
//		Added option to enter physical line number.
//	jXUtil:
//		Fixed JXUnpackStrings to ignore trailing separator.
//	JXSelectionManager:
//		Work-around for broken XDND file implementations that do not include
//			the host name in the URL's.
//	JXPSPrintSetupDialog:
//		Fixed dialog to accept file name typed into file input field.
//		Fixed file input field to accept ~.
//	JXEPSPrintSetupDialog:
//		Fixed dialog to accept file name typed into file input field.
//		Fixed file input field to accept ~.
//	Collapsed JXSelectTabTask into JXDockWindowTask.
//	JXDockManager:
//		Added SetIcon() to allow setting an icon for all docks.

// version 2.5.0:
//	*** All egcs thunks hacks have been removed.
//	JXWindow:
//		Fixed MapNotify processing to listen for changes in WM_STATE
//			instead of requiring WM_STATE to be set before MapNotify.
//		Fixed shortcut mechanism to use raw keysym instead of translated key.
//			This allows JX to tell the difference between Ctrl-left-arrow and Ctrl-].
//		Fixed bug in shortcut mechanism so Ctrl shortcuts for widgets now work.
//		SetMouseContainer() now calls itsDisplay->SetMouseContainer() in order
//			to compensate for missing EnterNotify messages -- mainly after a
//			pop-up menu is closed.
//		Fixed bug so attempting to set focus to a disabled widget will not
//			change the current focus.
//	JXMenu:
//		Top-level menus set focus back to their window when closed.
//			This is a work-around for the Gnome 2.0 desktop.
//		Fixed bug so window opens either above or below the widget, but will
//			never cover the widget.  (Finally!)
//		Parent menu is now also updated when shortcut is invoked.  Previously,
//			only the child menu was updated, but this was not consistent with
//			what happens when the mouse is used.
//	JXMenuBar:
//		If menus do not fit on the menubar, they are placed in an overflow menu.
//	JXTabGroup:
//		Added right-click menu to allow user to place tabs.
//		Added Read/WriteSetup() to save user's tab placement.
//		Added scroll arrows when tabs cannot all be displayed.
//		Fixed ShowTab() to scroll down if tab is not visible.
//		When the mouse moves over a tab during a DND operation, the tab is selected.
//	JXDockWidget:
//		Now embedded inside JXTabGroup to allow easy switching between windows.
//		Added version of Dock() that takes JXWindowDirector.
//	JXHoriz/VertDockPartition:
//		*** GetCompartment() no longer returns JXDockWidgets!
//		Saves elastic compartment indicies.
//		Fixed bug so both min width and min height are computed correctly.
//	JXScrollbar:
//		Added "Scroll here", "Scroll to top", "Scroll to bottom" to right-click menu.
//	JXDirTable:
//		Raises the window after a file drop.
//	Created JXFontCharSetMenu.
//	JXChooseMonoFont:
//		Added character set menu.
//	JXPathHistoryMenu:
//		Now lists mount points (e.g., /mnt/floppy) at top of menu.
//	JXWebBrowser:
//		Fixed ShowFileLocation() to work with file names that contain spaces.
//			(Removed call to JPrepArgForExec().)
//	JXImageWidget:
//		Added SetXPM().
//	JXScrollbarSet:
//		Destructor now sets member pointers to nullptr to avoid giving out
//			deleted pointers in Get*Scrollbar().
//	Created JXTipOfTheDay.
//	JXFileInput:
//		Now accepts drop of directory, since user might want to type file name.
//		GetFile() and GetTextForChooseFile() are now virtual.
//	JXPathInput:
//		Now accepts drop of file, since user might want to delete file name.
//		GetPath() is now virtual.
//		Added GetTextForChoosePath().
//	Renamed JXSelectionManager::Data to JXSelectionData to make Visual C++ happy.
//	JXFileDocument:
//		*** Changed prototype for HandleFileModifiedByOthers().
//		Added optional parameters modTimeChanged and permsChanged to
//			FileModifiedByOthers().
//	JXEditTable:
//		*** Replaced DeleteXInputField() with PrepareDeleteXInputField()
//			because it is safer to delete the input field after the event
//			processing finishes.
//	JXDeleteObjectTask:
//		Fixed bug to avoid double-deletion in case somebody else deletes the
//			object first.
//	JXDNDManager:
//		Fixed BeginDND() to return kJFalse if already dragging, rather than
//			asserting.  This avoids crashes if the user clicks another of the
//			mouse buttons while dragging.
//	JXStandAlonePG:
//		Progress value is displayed in window title, so it is still visible
//			when window is minimized.
//	JXCheckboxGroup:
//		Added Insert().  (What took me so long???)
//	Created JXAtMostOneCBGroup.

// version 2.1.1:
//	JXWindow:
//		Fixed Meta-Shift- shortcuts to work when Caps Lock is down.
//	JXChooseSaveFile:
//		Fixed JUNIXDirInfo::ResetCSFFilters() to not clear itsShowHiddenFlag.

// version 2.1.0:
//	Compiles with gcc 3.x.
//	JXTEBase:
//		Fixed bug so DND bombsight is removed when "ask" drop is cancelled.
//		Fixed bug so shortcuts for margin adjustment and read only menu items
//			can be changed via the string database.
//		Converted FilterSelectionData() to override of JTextEditor::FilterText().
//			This removes the need for separate code in HandleKeyPress().
//		Search & Replace menus now allow "Find selection" even if nothing is
//			selected.  It acts as if the user had double clicked on the word
//			containing the caret.
//	JXMenu:
//		Renamed Get/SetStyle() to Get/SetDefaultStyle() and added new functions
//			Get/SetDisplayStyle() to control the style that is actually displayed
//			when the user runs the program.
//		Pop-up menus stay open if user clicks and then releases the mouse.
//	Created JXMacWinPrefsDialog to allow easy access to JXMenu::SetDisplayStyle().
//	JXMenuTable:
//		Changed prototype of CellToItemIndex() to include cursor location.
//	JXTextMenu:
//		Menu font is adjustable via ~/.jx/menu_font.  First line is font name.
//			Second line is font size.
//		Increased height of separator region to improve visual grouping of items
//			and to provide buffer zone to make it harder to select wrong item.
//	Created JXDirectSaveAction to allow anybody to implement XDS.
//	JXDSSSelection:
//		Contructor now requires JXWindow* and JXDirectSaveAction*.
//	JXDirectSaveSource:
//		Added static public function Init() to help others implement XDS.
//	JXPathInput:
//		Finally provides tab completion.
//	JXFileInput:
//		Finally provides tab completion.
//		No longer draws file name red when using ~ notation.
//	Created JXStringCompletionMenu.
//	JXWebBrowser:
//		Renamed ShowFile() to ShowFileContent().
//		Added ShowFileLocation() to open a file manager window.
//	JXDocumentMenu:
//		Files are now alphabetized to make them easier to find.  (what took me so long?)
//	Created JXWDManager suite.  This provides JXWDMenu just like JXDocumentManager
//		provides JXDocumentMenu, except that JXWindowDirectors must register
//		themselves explicitly.  JXDocuments register themselves automatically.
//	JXDisplay:
//		Added GetWDManager().
//		Added CloseAllOtherWindows().
//		If Alt modifier is mapped, but Meta is not, Meta is automatically mapped to Alt.
//			(mainly to get Meta to work on Mandrake Linux)
//	JXDocument:
//		*** Changed prototype of virtual function GetName() and moved to
//			JXWindowDirector.
//		*** Changed prototype of virtual function GetMenuIcon().
//		Added pure virtual functions OKToRevert() and CanRevert().
//		Added RevertToSaved().
//	JXFileDocument:
//		Added optional argument fullName to SaveInNewFile().
//		Added RevertIfChangedByOthers().
//	JXApplication:
//		Fixed bug in urgent task processing so new task can safely be created
//			while others are being performed.
//		Fixed secondary event loop to automatically close all menus when it is
//			first invoked.  This prevents deadlock because the menu grabs the
//			cursor, but only the blocking window processes events.
//	JXWindow:
//		Added hack in HandleButtonPress() to detect case when window manager
//			doesn't set state to notify us that the window has been deiconified.
//		Renamed SetFocusWhenShow() to ShouldFocusWhenShow() for consistency.
//		Added GetMin/MaxSize().
//		Added Dock() and Undock().  JXWindows can be docked even in windows
//			belonging to other programs.
//	jXGlobals:
//		Added JXGetProgramDataDirectories().
//	JXGetNewDirDialog:
//		Appended optional argument modal to ctor to make it usable by others.
//	JXScrollableWidget:
//		Added version of ScrollForWheel() that doesn't require a JXScrollbarSet.
//	JXDeleteObjectTask:
//		Fixed bug so object is deleted if task is deleted before Perform() is called.
//	JXProgressTask:
//		Added ctor to allow providing a progress display.
//	JXSaveFileDialog:
//		Fixed bug in JXDirTable so up/down arrows select last/first directory
//			even if last/first item is a file.
//	JXFileListTable:
//		Added ClearIncrementalSearchBuffer().
//	JXStringList:
//		Added incremental searching.
//	JXRowHeaderWidget:
//		Added functions to change the row titles.
//		Fixed bug so NeedsToBeWidened message contains a more reasonable value.
//	jXPainterUtil:
//		Added betweenWidth to JXDrawEngravedFrame() and JXDrawEmbossedFrame().
//	JXEmbossedRect, JXEngravedRect:
//		Added functionality to support new jXPainterUtil functionality.
//		Fixed bug so width border width is now updated correctly when setting widths.
//	JXContainer:
//		Fixed bug so sloppy DrawBorder() function won't interfere with aperture.
//	Created JXSplashDialog for displaying a splash screen during program init.
//	Created JXDockManager suite to allow any JXWindow to be docked via JXDocktab.
//		JXMenuBar automatically creates a JXDocktab if JXDockManager exists.
//	JXHoriz/VertPartition:
//		Added second ctor that does not create any containers.  This allows
//			a derived class to call CreateInitialCompartments(), thereby using
//			the derived class' implementation of CreateCompartment().
//	JXDNDManager:
//		BeginDND() can now be called with TargetFinder object if one needs to
//			use a different algorithm for target acquisition.
//			(use with extreme caution :)
//	JXWidget:
//		Fixed bug so everything redraws correctly when a JXWidget is created
//			after the window becomes visible.
//	JXWindowIcon:
//		No longer requests focus in HandleMouseEnter() because fvwm2 does not
//			show focus correctly, so it is too dangerous, especially in System G.
//	Created JXTabGroup.  This builds on JXCardFile to provide a set of tabs which
//		the user can click on to display different panels in a single window.
//	JXFontNameMenu:
//		SetFontName() returns JBoolean since not all available fonts are listed.
//	JXAtLeastOneCBGroup:
//		Fixed bug for case:  enabled / enabled / disabled.
//	JXFontSizeMenu:
//		All menu items now have ID's, so they can be in the toolbar.
//	JXStyleMenu:
//		All menu items now have ID's, so they can be in the toolbar.
//	Eliminated jXActions.h because it was useless.
//	jXConstants.h:
//		Removed kJXDebounceWidth.  Use new function JMouseMoved() instead.
//	JXFontManager:
//		Added profontwindows to the list of known monospace fonts.

// version 2.0.0:
//	*** Added support for all European languages.  JXHelpManager now has a
//		special section called "JXComposeHelp" that lists all the special
//		key sequences that are defined to generate special characters.
//		These sequences are stored in a global JXComposeRuleList object,
//		which is accessible via jXGlobals.h.
//	*** Clipboard has been completely restructured.  Data is stored in
//		derived class of JXSelectionManager::Data instead of inside widget.
//		This guarantees a persistent clipboard, removes restriction that only
//		widgets can put data on the clipboard, and frees all widgets
//		from buffering the data and performing data conversions.  DND
//		also uses this new system.
//	JXSelectionManager:
//		Renamed:
//			GetSelectionData()           -> GetData()
//			DeleteSelectionData()        -> DeleteData()
//			SendDeleteSelectionRequest() -> SendDeleteRequest()
//			ClearSelectionOwner()        -> ClearData()
//		Removed window argument from GetAvailableTypes(), GetData(), SendDeleteRequest().
//		Removed GetSelectionOwner().
//		Added a dirty hack to allow pasting from rxvt and other brain-dead,
//			non-protocol-conformant programs.
//		Fixed bug in handling TARGETS so pasting from Gnome, acroread, etc. works.
//	JXWidget:
//		Removed ConvertSelection(), OwnsSelection(), BecomeSelectionOwner(),
//			RelinquishSelectionOwnership(), LostSelectionOwnership(),
//			Add/RemoveSelectionTarget(), GetSelectionTargets(),
//			GetSelectionTimestamp().
//		Replaced BecomeSelectionOwner() with JXSelectionManager::SetData().
//		DND requires overriding new virtual GetSelectionData().
//		Added AdjustBounds().  (what took me so long?)
//		Fixed GetDragPainter() to return JBoolean.
//		Added DNDCompletelyFinished() message.  Read the warnings!
//	Created JXTextSelection, JXImageSelection, and JXFileSelection.
//	Removed JXTextClipboard since it is now unnecessary.
//	JXTEBase:
//		Added functions for creating and managing Search or Search & Replace menu,
//			and for searching and replacing using the global JXSearchTextDialog.
//		Removed menuStyle argument from Append/ShareEditMenu() since it is now a
//			global value stored by JXMenu.
//		Renamed kCtrlAltPWMod to kCtrlMetaPWMod.
//		Added stream in/out operators for PartialWordModifier.
//		Added CaretShouldFollowScroll().
//	Created JXSearchTextDialog.  This is global and extensible.
//		It also supports the new XSearch protocol for sharing search settings
//		between programs.  (http://www.newplanetsoftware.com/xsearch/)
//	Created JXSharedPrefsManager to share JXHelpManager and other settings
//		between all programs that use JX.
//	JXTable:
//		Added ContinueSelectionDrag() to perform the standard actions above
//			and beyond what JTable::ContinueSelectionDrag() is capable of doing.
//		Updates scrolltabs to track changes in rows and columns.
//	JXStyleTable:
//		Moved all work to JStyleTableData so classes derived from JXTable
//			can also use the functionality.
//		Removed virtual functions FontChanged() and CellStyleChanged() because
//			these are now messages from JStyleTableData.
//		Added SetAllCellStyles().
//	JXStringList:
//		GetFont() now returns font name as const JString&.
//		Now uses JStyleTableData to manage color allocation.
//	JXFileListSet:
//		Promoted GetScrollbarSet() and SetTable() to public.
//		Removed menuStyle argument from constructor since it is now a
//			global value stored by JXMenu.
//	JXFileListTable:
//		Now uses a single column to simplify the code.
//		GetFileName() returns JString instead of const JString&.
//		Removed version of GetFullName() that takes fileName, since the result
//			cannot be guaranteed to be unique.
//		Changed remaining version of GetFullName() to return JBoolean.
//		Added option to pass relative paths to AddFile().  It accepts them,
//			and when GetFullName() is called, the virtual function
//			ResolveFullName() is called to obtain the full path.
//		If two files with the same name are added, the unique portions of their
//			paths are displayed so the user can tell the difference.
//		Promoted RowIndexToFileIndex() to public.
//	JXUNIXDirTable:
//		Renamed to JXDirTable.
//		Now uses a single column to simplify the code.
//		Entries of type "unknown" are selectable.  Visibility is controlled via
//			the JUNIXDirInfo object.
//	JXChooseSaveFile:
//		Renamed SetChooseFileContentFilter() to SetChooseFileFilters() because
//			it now controls all the visibility filters.
//		Changed prototype of CreateChooseFileDialog() to match JXChooseFileDialog::Create().
//		Added version of ChooseFile() and ChooseFiles() that accepts wildcard filter.
//		Inherits from JPrefObject.  Call SetPrefInfo() to activate this feature.
//		Added IsCharacterInWord() for use by any object that works with file names.
//	JXChooseFileDialog:
//		Added Create() and demoted constructor and BuildWindow().
//		Scrolls to the file when an initial file name is specified.
//		Enforces standard item shortcuts.
//		Added argument JXCurrentPathMenu* to SetObjects().
//	JXSaveFileDialog:
//		Added Create() and demoted constructor and BuildWindow().
//		Undo is no longer cleared in file name input field.
//		Enforces standard item shortcuts.
//		Fixed bug that caused crash when direct-save in sub-directory.
//			(e.g. drop on folder in System G)
//		Added argument JXCurrentPathMenu* to SetObjects().
//	JXChoosePathDialog:
//		Added Create() and demoted constructor and BuildWindow().
//		Enforces standard item shortcuts.
//		Added argument JXCurrentPathMenu* to SetObjects().
//	JXFileDocument:
//		Added argument origName to SaveCopyInNewFile().
//		Added FileModifiedByOthers() and CheckIfModifiedByOthers().
//		SafetySave() calls CheckIfModifiedByOthers().
//		SaveInCurrentFile() no longer checks itsSavedFlag.
//		Promoted SaveInCurrentFile(), SaveInNewFile(), SaveCopyInNewFile() to public.
//		SaveInCurrentFile() allows user to try saving even if write protected.
//		SaveBeforeClosePrompt and OKToRevertPrompt use $name instead of %f.
//	JXMenuBar:
//		Renamed kJXStdMenuBarHeight to kJXDefaultMenuBarHeight.
//		Renamed InsertMenuAhead() to InsertMenuBefore() for consistency with JPtrArray.
//	JXMenu:
//		Added Get/SetStyle().  This global setting controls all menus created by JX.
//		Added GetItemShortcuts().
//		Renamed DeleteItem() to RemoveItem() for consistency with RemoveAllItems().
//		SetTitle() allows %h to specify shortcuts.
//		Merged SetTitleImage() into SetTitle() to allow text and image together
//			in menu title.  Added new SetTitleText() and SetTitleImage() functions.
//		Renamed GetTitle() to GetTitleText() for consistency.
//		Fixed GetTitleImage() so it returns JBoolean.
//		Added 2nd version of GetTitleText() that returns JBoolean.
//		Added 3rd ctor that takes text and image.
//		Automatically uses the same cursor as the menu bar or owning menu.
//		Now draws menu *entirely* above menu bar if there is enough space above
//			but not enough space below.
//	JXMenuTable:
//		Automatically uses the same cursor as the JXMenu widget.
//		No longer asserts that it grabbed the pointer or keyboard.
//		Fixed bug in HandleKeyPress() so it no longer dies if you press modifier key.
//		Fixed dragging code so submenu stays open as long as mouse is moving toward it,
//			even if path crosses other items on parent menu.
//	JXTextMenu:
//		Added Insert/Prepend/AppendMenuItems() to correspond to SetMenuItems().
//		Added functions to get/set default item font.
//		Fixed bug so popup choice menus whose initial title is empty now work correctly.
//		No longer draws separator after the last item, even if one is requested.
//		Text and shortcuts of each item can be stored in string database.
//			See documentation for InsertMenuItems().
//		Shortcuts can include all the available modifiers.  Also added more strings
//			that are converted, e.g., "insert".  See JXTextMenuData::ParseNMShortcut()
//		If another menu item is already using the nm shortcut, it is not displayed.
//	JXTextMenuData:
//		Added Insert/Prepend/AppendMenuItems() to correspond to SetMenuItems().
//		Changed prototype of GetNMShortcut() to take "const JString**".
//		Fixed bug so SetDefaultFontSize() and SetDefaultFontStyle() now work.
//	JXStyleMenu and derived classes:
//		Removed menuStyle argument from constructor since it is now a
//			global value stored by JXMenu.
//	JXImageMenu:
//		Added version of SetItemImage() that takes JXPM.
//	JXDocumentManager:
//		Added new safety save reason:  kKillSignal
//			This is necessary because SIGTERM is sent when the system shuts down.
//		Removed first ctor argument because style comes from JXMenu::GetStyle().
//		Removed second ctor argument "useShortcutZero" because shortcuts are
//			now numbered 1, .., 9, 0 to match ordering on standard keyboards.
//		Added ctor argument to tell whether or not to use shortcuts at all.
//	JXDocumentMenu:
//		Unsaved files are now dark red instead of bright red to make the file names
//			easier to read.
//		Updates correctly if JXDocument::GetMenuIcon() changes what it returns.
//	JXApplication:
//		Added arguments to constructor for initializing JStringManager.
//		Catches SIGTERM and calls Abort().
//		Quits if all directors are deactivated.
//		Promoted CheckACEReactor() to public so networking code can call it
//			during clean-up.
//		Automatically works with session managers when you override
//			CleanUpBeforeSuddenDeath().
//		Added GetSignature().
//		HandleOneEventForWindow() is now private since it is so dangerous.
//		HandleCustomEventWhileBlocking() now returns JBoolean.
//	JXMDIServer:
//		Constructor no longer requires argument because it calls
//			JXApplication::GetSignature().
//	Created JXPrefsManager.  Always derive from this instead of JUNIXPrefsManager.
//		#** This uses the application signature for the file name, so if you used
//			to use something else, be sure to rename the file at startup if
//			the old one exists and the new one doesn't.
//	JXDisplay:
//		Fixed GetMouseContainer() to return JBoolean.
//		Broadcasts events for unknown X windows via BroadcastWithFeedback().
//		Added KeysymToModifier().
//	JXWindow:
//		Fixed GetMouseContainer() to return JBoolean.
//		Fixed destructor to free window icon pixmap and mask.
//		SetIcon() now takes ownership of the JXImage, so directors don't have to save
//			it themselves.
//		Automatically calls Update() after dispatching mouse and key events.
//			(What took me so long???)
//		Fixed SetTitle() so window whose name ends in "lock" still gets title bar
//			under fvwm.
//		Shortcuts can be any KeySym, not only normal characters.
//		No longer ignores kMod2KeyIndex through kMod5KeyIndex when checking shortcuts
//			because NumLock and ScrollLock are safely ignored.
//		Added second version of SetIcon() that uses an X window, so you can
//			accept drops when the main window is iconified.
//		To avoid window drift when using fvwm2, create ~/.jx/wm_offset to
//			contain "1 1" (without quotes).  In general, this file can offset
//			any amount of drift caused by window manager bugs.
//			The program must be restarted for a change in wm_offset to take effect.
//		Added SkipGeometry().
//		Fixed bug so additional calls to Activate() before window is mapped do
//			not shift window position by border width.
//		Swapped arguments to SetWMClass().  If you were calling this the way
//			our code was, don't change your code, because it is now correct.
//		Added HideFromTaskbar().
//	Created JXIconDirector and JXWindowIcon to support drops on iconified windows.
//	JXContainer:
//		Added SetVisible() and SetActive().  (What took me so long?)
//		Added GetDefaultCursor().
//		GetEnclosure() now returns non-const pointer so it can be passed to
//			widget constructors.
//	jXUtil:
//		Fixed bug in JXUnpackFileNames() so it doesn't crash if a URL is empty.
//		Moved JXFileNameToURL() and JXURLToFileName() to jFileUtil.
//		Added JXReportUnreachableHosts().  Call this after a drop to notify user
//			of urlList returned by JXUnpackFileNames().  It checks for urlList
//			being empty, so you can always call it.
//	JXScrollbar:
//		StepPage() now takes JFloat to allow fractions of a page.
//		Added StripMoved() and StripsSwapped() for updating scrolltabs.
//	JXScrollableWidget:
//		Shift-Page_Up/Down scrolls by 1/2 page.
//		Added SetHoriz/VertJumpToScrolltabModifiers().  If derived classes don't
//			catch the keypress, [1-9] will jump to a scrolltab.
//	JXPSPrinter:
//		Added second argument "changed" to EndUserPrintSetup().
//	JXPSPageSetupDialog:
//		Added Create() and demoted constructor and BuildWindow().
//	JXPSPrintSetupDialog:
//		Added Create() and demoted constructor and BuildWindow().
//		Uses JXFileInput instead of JXFileNameDisplay.
//		Enforces standard item shortcuts.
//		Print command accepts ~ notation.
//	JXEPSPrinter:
//		Added second argument "changed" to EndUserPrintSetup().
//	JXEPSPrintSetupDialog:
//		Added Create() and demoted constructor and BuildWindow().
//		SetObjects() requires JXFileInput instead of JXFileNameDisplay.
//		Enforces standard item shortcuts.
//		Print command accepts ~ notation.
//	JXPTPrinter:
//		Added option to print pages in reverse order in JXPTPageSetupDialog.
//		Modified prototype of CreatePageSetupDialog().
//		Added second argument "changed" to EndUserPrintSetup().
//	JXPTPageSetupDialog:
//		Added Create() and demoted constructor and BuildWindow().
//		Modified prototype of SetObjects().
//	JXPTPrintSetupDialog:
//		Added Create() and demoted constructor and BuildWindow().
//		Uses JXFileInput instead of JXFileNameDisplay.
//		Enforces standard item shortcuts.
//		Print command accepts ~ notation.
//	JXDirector:
//		Added GetSubdirectors() and CloseAllSubdirectors().
//		Promoted IsClosing() to public.
//	JXFontManager:
//		Fixed bug in GetFontID(xFontStr) so it now checks the existing fonts
//			before allocating a new one.
//	JXFontNameMenu:
//		GetFontName() returns JString instead of JString&.
//		Provides choice of available character sets.
//	JXXFontMenu:
//		GetFontName() returns JString instead of JString&.
//	JXChooseMonoFont:
//		Provides choice of available character sets.
//	JXInputField:
//		Passes Home/End and Page Up/Down to table.
//	Created JXWebBrowser as global object to encapsulate interface to
//		Netscape and Arrow.
//	JXHelpManager:
//		Added IsLocalURL().
//		Removed menuStyle argument from constructor since it is now a
//			global value stored by JXMenu.
//		Removed WriteSetup() because JX now stores the settings automatically,
//			so nobody else should store them.
//		Uses JStringManager.  RegisterSection() only requires names.
//			*** Titles should be in <head> section of actual text.
//			To convert your files, use:
//				(^[ \t]*")|([ \t]*(\\n)?"$)  ->  <blank>
//				\\n                          ->  \n
//		Moved ConvertVarNames() and URL and mail commands to JXWebBrowser.
//	JXHelpDirector:
//		Removed menuStyle argument from constructor since it is now a
//			global value stored by JXMenu.
//	JXHelpText:
//		Displays URL for link if it isn't local.
//		Prints <title> info at top of every page.
//	jXGlobals:
//		Removed menuStyle argument from JXInitHelp() since it is now a
//			global value stored by JXMenu.
//		Removed title and text array arguments from JXInitHelp() since
//			JXHelpManager no longer wants them.
//		Added JXGetSharedPrefsManager().
//		Added arguments to JXCreateGlobals() for initializing JStringManager.
//		Added JXGetWebBrowser().
//		Added JXGetInvisibleWindowClass().
//	JXTextEditor:
//		Removed menuStyle argument from constructor since it is now a
//			global value stored by JXMenu.
//	JXTextEditorSet:
//		Removed menuStyle argument from constructor since it is now a
//			global value stored by JXMenu.
//	JXUserNotification:
//		Adjusts width of dialog windows so they are never taller than the screen.
//		Added AcceptLicense().
//	Created JXAcceptLicenseDialog.
//	JXGC:
//		Turned off GraphicsExpose and NoExpose events to reduce network traffic.
//		Added SetSubwindowMode().
//	Removed jXCSFIcons.h and JXUserNotificationIcons.h.  Replaced them with:
//		jx_plain_file_small.xpm, jx_source_file_small.xpm,
//		jx_image_file_small.xpm, jx_library_file_small.xpm,
//		jx_folder_*.xpm, jx_executable_small.xpm,
//		jx_unknown_file_small.xpm, jx_un_message.xpm, jx_un_warning.xpm,
//		jx_un_error.xpm
//	Created jx_binary_file_small.xpm (thanks to Glenn Bach)
//	Created selected versions of all file icons (thanks to Glenn Bach)
//	Created JXProgressTask.
//	Created JXDeleteObjectTask and JXCloseDirectorTask.
//	JXIdleTask:
//		Added CheckIfTimeToPerform() so derived classes can override Perform()
//			to do additional work.
//	JXKeyModifiers:
//		All constructors and functions that take a state bit field now require
//			a JXDisplay* so the bits can be properly interpreted.
//		Added numLock(), scrollLock(), alt(), super(), hyper(), and
//			the corresponding kJX*KeyIndex values.
//		Added Available() so one can check which modifiers are mapped on the keyboard.
//	JXPathInput:
//		Added ChoosePath().
//		Added HasBasePath() and modified GetBasePath() to return JBoolean.
//	JXFileInput:
//		Added ChooseFile() and SaveFile().
//		Added HasBasePath() and modified GetBasePath() to return JBoolean.
//	JXFileNameDisplay:
//		If text is scrolled, entire text is displayed in hint.
//	JXStaticText:
//		Automatically parses as HTML if text begins with "<html>".
//	JXProgressDisplay:
//		AppendToMessageWindow() is now virtual protected so it can be overridden.
//		Added message CancelRequested.  This is broadcast if the process is in
//			the background and the Cancel button is pushed.
//	JXImageButton, JXImageCheckbox, JXImageRadioButton:
//		Added SetImage(const JXPM&,...).
//	JXSaveFileInput:
//		Added option to translate space into _, since space causes so much trouble
//			on cmd line and in scripts.
//	JXDNDManager:
//		Added cursors for dragging files and directories.
//		Generic Get*DND*Cursor() functions now take arguments and return the
//			appropriate cursor for the standard situation.  Created static utility
//			function GetDNDCursor() to choose the appropriate cursor if you define
//			a custom appearance.
//		Fixed bug so it no longer crashes when source or target widget is deleted
//			during a drag.
//	jXPainterUtil:
//		Added JXDrawDNDBorder().
//	JXHistoryMenuBase:
//		Added GetItemText() and UpdateInputField() for convenience.
//	JXFileHistoryMenu:
//		Added version of GetFile() that takes JBroadcaster::Message.
//	JXTextCheckbox:
//		Added GetLabel(), GetFontName(), GetFontSize(), GetFontStyle().
//	JXTextRadioButton:
//		Added GetLabel(), GetFontName(), GetFontSize(), GetFontStyle().
//	Created JXCheckModTimeTask.
//	jXConstants.h:
//		Added kJXDebounceWidth.
//	Added Create() or other work-arounds for the egcs thunks bug to:
//		JXFileListTable

// version 1.5.3:
//	JXMenu:
//		Removed "Meta-click keeps it open" because this requires the menu to be
//			updated while it is open.  A lot more machinery is required before
//			this can be implemented.
//	JXFileInput:
//		Added GetTextForChooseFile().
//	JXHelpManager:
//		Fixed code so everything except jxhelp and mailto are passed to openURL().
//			This avoids the need to specify each URL prefix.
//	JXPasswordInput:
//		Draws text in red to warn when Caps Lock is on.
//		Draws caret/selection.  Caret is always at end of text.  Selection always
//			includes the entire text.
//	JXSelectionManager:
//		Fixed bug in INCR data transfer protocol.
//	JXCSFDialogBase:
//		Fixed bug so it no longer complains about invalid path when Cancel
//			button is pushed.
//	JXDialogDirector:
//		Added GetOKButton() and GetCancelButton().
//	JXChooseMonoFont:
//		Now includes all NxM fonts, not only those with N <= 9.

// version 1.5.2:
//	JXMenu:
//		Meta-click when opening a menu (but not a submenu) will leave the
//			menu open after you select an item.  This is especially useful
//			for a menu of checkboxes.
//	JXFileListSet:
//		Added GetScrollbarSet() and SetTable() so derived classes can use
//			a derived class of JXFileListTable.
//	JXFileListTable:
//		Added GetFullNameDataList() so derived classes can store extra
//			information about each file.
//		Added RemoveFiles().
//		Added second argument to AddFile() to return insertion index.
//	JXHelpManager:
//		Added support for https: URL's.

// version 1.5.1:
//	Created jx_file_revert_to_saved.xpm
//	JXPathInput:
//		Fixed GetTextColor() so it doesn't crash if the path is empty.
//	JXInputField:
//		Holding down the Meta key lets you drag text from the input field.
//			(You can release the Meta key once DND has started.)
//	JXScrollbar:
//		Thumb size scales as (page size / document size) * available length
//			(Thanks to Gavin Wood for the patch.)

// version 1.1.23:
//	JXFileDocument:
//		Optimized FileDocumentIsOpen().
//	JXGetNewDirDialog:
//		Added basePath argument to constructor so it checks the current
//			file chooser directory, not the working directory.
//	JXFontManager:
//		Added support for character sets.
//	JXMenu:
//		Added ItemIDToIndex().  (what took me so long?)
//	JXTextMenu:
//		Added version of SetItemImage() that takes JXPM, for use with the
//			new menu_image repository.
//	JXPathInput:
//		Added option to require a writable directory.
//		Added requireWrite argument to GetTextColor().
//	Created JXFileInput.
//	JXPTPrinter:
//		Added printLineNumbers argument to CreatePrintSetupDialog().
//	JXPTPrintSetupDialog:
//		Added printLineNumbersCB and printLineNumbers arguments to SetObjects().
//	Added Create() or other work-arounds for the egcs thunks bug to:
//		JXRow/ColHeaderWidget
//	jXActions:
//		Added actions to correspond to the new menu image repository.
//	JXDocumentManager:
//		Promoted CloseDocuments() to public.
//	JXChooseSaveFile:
//		Added origName argument to CreateChooseFileDialog().
//	JXChooseFileDialog:
//		Added origName argument to BuildWindow() and SetObjects().
//			(Removed default value of BuildWindow() arguments to force
//			 compiler to remind you.)
//	JXSaveFileDialog:
//		Added draggable icon to implement local saving via the new XDS protocol.
//			http://www.newplanetsoftware.com/xds/
//	jXUtil.h:
//		JXURLToFileName() now returns kJTrue as long as the URL is on the
//			same machine, even if the file or directory doesn't exist.
//			It is now the caller's problem to check if it exists.
//		JXUnpackFileNames() now returns all items on the local machine
//			in fileNameList, even if they don't exist.  It is now the
//			caller's problem to check each one.
//	JXTEBase:
//		Added option to use Windows/Motif action for Home/End keys.
//	JXApplication:
//		It is now safe for urgent tasks to invoke the subsidiary event loop.
//	JXWindow:
//		Fixed Place() so it autodetects the window manager's behavior.

// version 1.1.22:
//	JXStyleTable:
//		Added virtual functions FontChanged() and CellStyleChanged() so
//			derived classes can update themselves, if necessary.
//	JXApplication:
//		Added PrepareForBlockingWindow() and BlockingWindowFinished()
//			to allow idle tasks to work in blocking windows.
//		HandleOneEventForWindow() no longer takes taskList argument.
//	JXChooseSaveFile:
//		When path input has focus, button now says "Go to".
//	JXPathInput:
//		Fixed bug that caused crash when a relative path was set
//			before SetBasePath() was called.

// version 1.1.21:
//	JXScrollableWidget:
//		Supports scrolling via a wheel mouse.  To activate this, call
//			ScrollForWheel() if you don't handle the mouse click yourself.
//			(http://www.inria.fr/koala/colas/mouse-wheel-scroll/)
//	JXScrollbar:
//		Added support for wheel mouse.  Point to the widget and dial to change
//			the value.
//		Added IsHorizontal() and IsVertical().
//	JXSliderBase:
//		Added support for wheel mouse.  Point to the widget and dial to change
//			the value.
//	JXMenuTable:
//		Added support for wheel mouse.
//	JXWindow:
//		The iconic hint for the window manager is cleared when the window is mapped.
//	JXPathInput:
//		Accepts '~' as the first character.
//		Added Get/Set/ClearBasePath() to allow relative paths.
//	JXPathHistoryMenu:
//		Added Get/Set/ClearBasePath() to allow relative paths.
//	JXCSFDialogBase:
//		Path input accepts paths relative to the current one.
//	JXWidget:
//		Added virtual DNDFinish().  You will almost certainly want to override this.
//	Renamed JXCSFIcons.h to jXCSFIcons.h for consistency.
//	JXTable:
//		Added convenience functions to interface to JTable selection utilities.
//	JXRadioGroupDialog:
//		Added option to provide shortcuts for each radio button.
//		Added SelectItem() so one can set the initial choice to something other
//			than the first item.
//	JXMenuBar:
//		Added GetMenu().
//	JXMenu:
//		Added functions to safely downcast to JXTextMenu and JXImageMenu.
//		Added IsChecked() and GetSubmenu().
//	JXMenuData:
//		Added IsChecked().
//	JXContainer:
//		Added SetHint() to provide a hint for a widget.
//	Created JXHintManager and JXHintDirector to implement hints.  These
//		can be used to implement hints for part of a widget.
//	JXDNDManager:
//		The initial value of the action passed to ChooseDropAction()
//			determines the default setting in the dialog.
//	Added Create() or other work-arounds for the egcs thunks bug to:
//		JXFontNameMenu, JXFontSizeMenu, JXXFontMenu, JXDisplayMenu,
//		JXStyleMenu, JXStyleTableMenu, JXTEStyleMenu, JXStyleTable,
//		JXStringList

// version 1.1.20:
//	Created JXPTPrinter for printing plain text.
//	JXPSPrinter:
//		It now listens to the dialogs, so BeginUserPage/PrintSetup() no longer
//			return JXDialogDirector*.  Instead, it broadcasts
//			Page/PrintSetupFinished when the dialogs are closed.
//		BeginUserPage/PrintSetup() no longer take JXDirector* argument,
//			because the dialogs must suspend the entire application since
//			printer objects can and should be shared.
//	JEXPSPrinter:
//		It now listens to the dialog, so BeginUserPrintSetup() no longer
//			returns JXDialogDirector*.  Instead, it broadcasts
//			PrintSetupFinished when the dialog is closed.
//		BeginUserPrintSetup() no longer takes JXDirector* argument,
//			because the dialog must suspend the entire application since
//			printer objects can and should be shared.
//	JXEPSPrintSetupDialog:
//		Changed shortcut of "Choose file" button to Meta-O for consistency with
//			JXPS/PTPrintSetupDialog.
//	JXTEBase:
//		Changed CreatePrinter() to SetPSPrinter().  Client now owns the printer
//			object, because it should be shared by all text editors that use the
//			same colormap in most cases.
//		Renamed:
//			HandlePageSetup() -> HandlePSPageSetup()
//			HandlePrinting()  -> PrintPS()
//		Removed Read/WritePrintSetup().  To read the data:
//			Read JFileVersion and ignore it.
//			Call JXPSPrinter::ReadXPSSetup().
//		Added Get/SetPSPrintFileName() since this is stored separately for each
//			text editor.
//		Added SetPTPrinter(JXPTPrinter*).
//		Renamed HandlePrintPlainText() to PrintPT() and added HandlePTPageSetup().
//		Removed Read/WritePrintPlainTextSetup().  To read the data:
//			Read JFileVersion and ignore it.
//			Read JString and call JXPTPrinter::SetPrintCmd().
//			Read JSize and call JXPTPrinter::SetPageHeight().
//			Call JIgnoreUntil(std::istream&, '\1');
//		Added Get/SetPTPrintFileName() since this is stored separately for each
//			text editor.
//		Removed all JBroadcaster messages since JPrinter has its own:
//			PSPageSetupFinished
//			PrintPSFinished
//			PrintPlainTextFinished
//	JXTextEditor:
//		No longer creates a printer object.
//	JXHelpManager:
//		Incremented setup version.  You must increment the versions of all files
//			that store this information.
//	JXDocumentManager:
//		Fixed bug that caused menus to be permanently disabled if shortcuts
//			were turned off.
//	JXFontManager:
//		Fixed bug in GetMonospaceFontNames() to avoid crash if XLoadQueryFont()
//			returns nullptr.

// version 1.1.19:
//	Removed JX*TableInput and moved functionality into JXInputField so now
//		any input field can be used in a table.
//	Removed JXStringInput since it was only wasting space.  jxlayout will
//		correctly rewrite your window layout code to use JXInputField, and
//		you can use Code Crusader's new multi-file replace to change all
//		occurrences of "(Get|Set)String\(" to "$1Text\(".
//	Created JXCharInput to accept only a single character.
//	JXStringTable:
//		CreateStringTableInput() now returns JXInputField so any input field
//			can be used in JXStringTables.  Also removed font arguments
//			because they are not needed in the constructor.
//		GetInputField() now takes JXInputField**.
//	JXFloatTable:
//		CreateStringTableInput() now returns JXFloatInput so any input field
//			can be used in JXStringTables.  Also removed font arguments
//			because they are not needed in the constructor.
//		GetInputField() now takes JXFloatInput**.
//	JXPathInput:
//		Renamed:
//			IsAllowingInvalidPath() -> WillAllowInvalidPath()
//			AllowInvalidPath()      -> ShouldAllowInvalidPath()
//	JXInputField:
//		If it is a table input, calls EndEditing() when it loses focus.
//	JXEditTable:
//		Background remains white when an input field has focus.
//		Input field's focus color automatically set to table's focus color.
//		Meta-Return ends editing without moving input field.
//		BeginEditing() fails gracefully if the input field cannot get focus.
//		Automatically calls SetTable() after CreateXInputField().
//		Renamed WantsKey() to WantsInputFieldKey() and made it virtual.
//		Removed UpdateClipboard() because JXDisplay provides it.
//	JXSaveFileDialog:
//		When the file list has focus but no selection, the Open button is deactivated.
//	JXApplication:
//		Fixed bug that caused crash when exiting if idle/urgent tasks were not
//			cleaned up by their creators.  (It is still a bug and a memory leak if
//			tasks are not deleted by their owners.)
//		If object is suspended, SIGQUIT is ignored.
//	JXDNDManager:
//		Doesn't fake middle-click if widget doesn't support text/plain.
//		GetURLXAtom() returns atom for "text/uri-list".
//		IsDragging() returns kJFalse once the mouse is released, so it is safe
//			to use blocking dialog window like JXChooseSaveFile in HandleDNDDrop().
//			(i.e. Escape will trigger the Cancel button correctly.)
//	JXFileListTable:
//		AddFile() does nothing if the file is already in the list.
//		Added RemoveFile() and RemoveSelectedFiles().
//		Added BackspaceShouldRemoveSelectedFiles().
//	JXContainer:
//		Fixed Show/Hide() and Activate/Deactivate() so one can listen to a table
//			selection and activate/deactivate widgets based on the messages.
//			(e.g. JXChooseFileDialog and JXSaveFileDialog)
//	JXWindow:
//		Added RecalcMouseContainer() and CheckForMapOrExpose().
//		Added HasFocus() to tell whether or not the window has focus.
//		Renamed RequestInputFocus() to RequestFocus() for consistency.
//	JXMenu:
//		Adjusts popup choice menu title before Broadcast() so listener's Receive()
//			can futher adjust the title.
//		Added IsEmpty().  (what took me so long?)
//		The "enable" state of an item with a submenu is tied to the "active" state
//			of the widget displaying the submenu.
//		Empty menus are forced to be disabled.  When an item is added, the menu
//			reactivates if it was previously active before becoming empty.
//		Added concept of internal ID for each item.
//	JXMenuData:
//		Changed prototype of GetItemShortcuts() to return JBoolean.
//		Added support for internal ID for each item.
//	Created jXActions.h and jXActionDefs.h.
//	jXUtil:
//		JXPack/UnpackFileNames() uses text/uri-list format.
//		JXUnpackFileNames() unpacks anything, not just files.
//		Generalized JXPack/UnpackStrings() to use arbitrary separator.
//			The default is nullptr.
//	JXScrollableWidget:
//		Meta-End, Meta-Page-Up/Down scroll horizontally.
//		Added Read/WriteScrollSetup() and SkipScrollSetup().
//	JXScrollbar:
//		Saves scroll position in setup.
//	JXHelpManager:
//		Added Get/SetViewURLCmd() and Get/SetSendMailCmd().
//		Changed variable marker from % to $
//	JXHelpDirector:
//		Added menu item to save default window size.
//	JXHelpText:
//		Space pages down, and Backspace pages up, just like in Netscape.
//	JXChooseSaveFile:
//		All dialogs expand to avoid splitting single words across lines in the
//			message text at the top of the window.
//	Created JXNewDirButton.
//	JXCSFDialogBase:
//		SetObjects() requires JXNewDirButton instead of plain JXTextButton.
//	JXDocument:
//		Added pure virtual function GetMenuIcon().
//	JXFileDocument:
//		Added Save() and ShouldAutosaveBeforeClosing().
//		SaveCopyInNewFile() returns JBoolean and can accept JString* if caller
//			wants to know the name of the file that was created.
//		Implements GetMenuIcon() to show default file icon.
//		Generalized the concept of a file from only a text file to any
//			type of file.
//		Renamed WriteFile() to WriteTextFile().  This is no longer pure virtual
//			because it won't always be needed.  It will be called only if
//			the new WriteFile() is not overridden.
//		Created new WriteFile() that must be overridden if the file is not
//			a plain text file.
//		Since there are no longer any pure virtual functions, the constructor
//			is now protected.
//		Fixed SaveInCurrentFile() to give the user the option to cancel the
//			operation if a backup file cannot be created.
//	JXDocumentManager:
//		DocumentCreated() is now virtual.
//	JXDocumentMenu:
//		No longer crashes if document closes while menu is open.
//	JXCursor:
//		Created JXGetDragDirectoryCursor() and JXGetDragFileAndDirectoryCursor().
//	JXTEBase:
//		Paste automatically converts from DOS and Mac format.
//		Meta-Control-left/right-arrow moves by partial word. (also works with Shift)
//		Meta-Control-double-click selects partial words.
//		Added menu item "Coerce paragraph margins" after the existing
//			"Clean paragraph margins".  This calls CleanRightMargin(kJTrue).
//		AppendEditMenu() and ShareEditMenu() now return the menu widget.
//		All 3 Finished messages are now always broadcast.  They include a JBoolean
//			to indicate whether or not the action was successful.
//		Added second version of ShareEditMenu() to accept menu from anywhere.
//	Created JXTEBlinkCaretTask to make JXTEBase caret blink.
//	Created JXHistoryMenuBase
//	JXStringHistoryMenu and JXFileHistoryMenu both inherit from JXHistoryMenuBase
//		*** Increment the version number of all files that store history menus
//			because the new data format cannot be read by older code.
//	JXKeyModifiers:
//		The enum for the key indices is now named JXModifierKey, and the
//			values use the kJX prefix.  Multifile replace regex is
//			k([a-z0-9]+)KeyIndex  ->  kJX$1KeyIndex
//	Created JXRadioGroupDialog.
//	JXStaticText:
//		Fixed constructors so code for width and height of zero
//			will shrink the frame if the text requires less than 100x100.
//	jXConstants.h:
//		Added kJXDoubleClickTime.
//	jXGlobals:
//		Removed excessing convenience #includes.
//	JXProgressIndicator:
//		Added IncrementValue().
//	Created JXRegexInput and JXRegexReplaceInput.
//	JXFontManager:
//		Added GetMonospaceFontNames().
//		Fixed GetXFontNames() to take JRegex& instead of JCharacter*.
//	JXXFontMenu:
//		Fixed Create() to take JRegex& instead of JCharacter*.
//	JXChooseMonoFont:
//		Now broadcasts when the font or size changes.
//	JXPSPrinter:
//		Added virtual functions CreatePageSetupDialog() and CreatePrintSetupDialog()
//			so derived classes can extend the Page Setup and Print Setup dialogs.
//	JXPSPageSetupDialog, JXPSPrintSetupDialog:
//		Restructured them so one can create derived classes with additional
//			features.
//	JXEPSPrinter:
//		Added virtual function CreatePrintSetupDialog() so derived classes
//			can extend the Print Setup dialog.
//	JXEPSPrintSetupDialog:
//		Restructured it so one can create derived classes with additional
//			features.
//	Renamed JXIconMenu to JXImageMenu for consistency.
//	JXDisplay:
//		Added GetTextClipboard() and UpdateTextClipboard().  JXTEBase
//			automatically uses this so copied text is never lost.
//	JXTextButton:
//		Added GetLabel(), GetFontName(), GetFontSize(), GetFontStyle().

// version 1.1.18:
//	JXDNDManager:
//		Implements XDND version 3.
//	JXTEBase:
//		Renamed GetEditMenuCmd() to EditMenuIndexToCmd().
//		Added EditMenuCmdToIndex().
//		Fixed bug that ignored changes in action when dropping on itself.
//	JXCursor:
//		Added JXGetDragFileCursor().
//	jXUtil:
//		Added JXPack/UnpackFileNames() for converting to and from url/url.
//		Added JXFileNameToURL(), and JXURLToFileName().
//	JXTextMenuData:
//		The translated strings (e.g. tab, return) in menu shortcuts are
//			now case-insensitive.
//		Added "backspace" to list of translated strings in menu shortcuts.
//	JXInputField:
//		Added Get/SetMinLength() and SetLengthLimits().
//		SetIsRequired() is deprecated because SetMinLength(1) should be used instead.
//	JXColHeaderWidget:
//		SetColTitle() now takes JCharacter* instead of JString&.
//	JXWindow:
//		Added version of Read/WriteGeometry() that takes JString.
//	JXStyleMenu:
//		Demoted GetSelectedColor() to protected.
//		Added constructor argument allowChooseCustomColors.
//	Created JXChooseColorDialog.
//	JXUserNotification, JXStandAlonePG:
//		All dialogs expand to avoid splitting single words across lines.
//	JXUserNotificationIcons:
//		Converted constants into access functions.
//	JXHelpManager:
//		Added support for Table of Contents.
//		Added SetTOCSectionName().
//	jXGlobals:
//		Prepended argument tocSectionName to JXInitHelp().
//	JXStyleTable:
//		Allocates/deallocates colors used in cells.
//	JXChooseSaveFile:
//		Added SetChooseFileContentFilter().
//	JXUNIXDirTable:
//		Fixed bug that would crash when something was selected and one set a
//			wildcard filter that excluded everything.
//	Created JXFileListTable and JXFileListSet.
//	JXStringTable, JXFloatTable:
//		Added GetInputField().

// version 1.1.17:
//	libXPM is now included in libjx.
//	JXColorManager:
//		Added color approximation and set of functions to control how it is done.
//	JXDNDManager:
//		Upgraded to XDND protocol version 2 which supports arbitrary drop actions.
//		Added GetDNDAction*XAtom() which define the default actions.
//		Added GetDefaultDND*Cursor() which provide cursors for visual feedback
//			 of the accepted action.
//		Added GetAskActions() to get the possible actions when the specified action is Ask.
//		Added ChooseDropAction() to ask the user which action to perform.
//	JXContainer:
//		Changed prototypes of WillAcceptDrop(), HandleDNDHere(), and HandleDNDDrop()
//			to include action and provide JXWidget* instead of dropOnSelf.  The JXWidget*
//			provides a way to recognize drags within a compound document.
//	JXWidget:
//		Added GetDNDAction(), GetDNDAskActions(), and HandleDNDResponse(),
//			all 3 of which you should at least consider overriding.
//	JXStringList:
//		Added SetStyles().
//	JXMenu:
//		Renamed ClearMenu() to RemoveAllItems().
//	JXTextMenu, JXTextMenuData:
//		Added HeightCompressed(), CompressHeight() for use on special menus where
//			packing in as many items as possible is important.
//	JXFileDocument:
//		SaveInNewFile() now correctly handles the error if the new file cannot be created.
//	JXSaveFileInput:
//		Translates / into -, since / is the UNIX directory separator.
//	JXWindow:
//		Added argument grabKeyboardFocus to Raise().  The default value is kJTrue,
//			so calling it with no arguments gives the standard behavior.
//		Added SetWMClass() to set contents of WM_CLASS property.
//		Key repeat events are flushed so a backlog can no longer form.
//	JXStandAlonePG:
//		No longer grabs focus when itsRaiseWindowFlag is set.
//		No longer displays the window if fixed length process has only 1 step.
//	JXPSPrinter:
//		Saves destination and file name as part of setup.
//	Created JXScrolltab.
//	JXScrollbar:
//		Draws incr/decr arrow pair at each end if there is space.
//		Shift-clicking on arrows scrolls 1/2 page at a time
//		Supports bookmark tabs via new JXScrolltab class.
//		Added ScrollToTab() to jump to a particular tab.
//		Added PrepareForLowerMaxValue(), PrepareForHigherMaxValue(), and
//			PrepareForScaledMaxValue() so JXScrollableWidgets can keep the
//			scrolltabs up to date.
//		Added Read/WriteSetup() to restore/save scrolltabs.
//	JXTEBase:
//		Added HasEditMenu().
//		Fixed prototype of GetEditMenu() to return JBoolean since the menu
//			doesn't always exist.
//		Automatically adjusts scrolltabs on vertical scrollbar.
//		DND supports move, copy, and ask actions.
//		Changed prototypes of TEXWillAcceptDrop() and TEXConvertDropData()
//			to include action.
//	JXEditTable:
//		Added GetEditMenuHandler().  This returns JXTEBase* that can then be
//			used for AppendEditMenu() or ShareEditMenu().
//		If the object returned by GetEditMenuHandler() has an edit menu,
//			input fields returned by CreateXInputField() automatically use it.
//	Created JXTimerTask.
//	jXUtil:
//		Added JXPackStrings(), JXUnpackStrings() for converting to and from
//			the standard X format of nullptr separation.
//	JXTextRadioButton:
//		Added GetPreferredWidth().
//	JXStaticText:
//		Fixed constructors so code for width and height of zero
//			accounts for border width and turns off word wrap.
//	JXDialogDirector:
//		Changed EndDialog() argument from cancel to success because this is much
//			more intuitive.
//	JXStringTable:
//		Added CreateStringTableInput() to allow derived classes of JXStringTableInput.
//	JXFloatTable:
//		Added CreateFloatTableInput() to allow derived classes of JXFloatTableInput.
//	JXChooseFileDialog:
//		Fixed bug so Up button is now deactivated when in the root directory.
//	JXDisplay:
//		CreateBuiltIn/CustomCursor() automatically checks if the given name
//			has already been used, so it is no longer necessary to call
//			GetCursor() first.
//	Created JXLinkText as a base class for hypertext.
//	Created JXHelpText to provide hypertext help.
//	JXHelpDirector:
//		Removed SetContents().
//		Removed Format enum.  All text must be HTML.
//	JXHelpManager:
//		Provides hypertext help and sends external URLs to netscape.
//		Added Read/WriteSetup() so applications can save preferences.
//	jXGlobals:
//		Removed format argument from JXInitHelp().

// version 1.1.16:
//	JXStaticText:
//		The width and height passed to the constructors can be zero.
//			(If you want word wrap, width cannot be zero.)
//	JXUNIXDirTable:
//		Added AllowDblClickInactive() and augmented FileDblClicked message.
//		Added second argument "allowMultiple" to AllowSelectFiles().
//		Re-wrote selection interface.
//	JXSaveFileDialog:
//		Double clicking a file copies the name to the JXSaveFileInput.
//	JXChooseFileDialog:
//		Allows user to select more than one file if ChooseMultipleFiles() was called.
//		Added argument "selectAllButton" to SetObjects().
//	JXFileHistoryMenu:
//		Added GetFile(), SetDefaultIcon(), and virtual ShowIcon().
//	JXWindow:
//		Added UsePixmapAsBackground() for faster redraws after expose events.
//	JXPathInput:
//		Added AllowInvalidPath().
//	JXTextMenu:
//		On Linux-Intel machines, displays non-menu shortcuts using Alt instead of Meta,
//			since the key is labelled Alt.

// version 1.1.15
//	Created JXFileNameDisplay.
//	JXCSFIcons.h:
//		Converted global variables to functions.
//	JXPathInput:
//		Text is red if path is not valid.

// version 1.1.14:
//	*** Removed JIPC suite since ACE is fully integrated.
//	JXWindow:
//		*** Close() performs itsCloseAction instead of always calling itsDirector->Close().
//		Fixed RequestInputFocus() so it will never cause another XSetInputFocus() error.
//		Raise() again sets input focus to the window.
//	JXDisplay:
//		Fixed X error handler to ignore XSetInputFocus() errors.
//	jXPainterUtil:
//		Added JXDrawFlatDiamond().
//	JXButton, JXCheckbox, JXRadioButton:
//		Drawn flat when deactivated to indicate that they are not pliant.
//	JXScrollableWidget:
//		Added UpdateScrollbars() so one can perform the urgent task sooner,
//			but only use this if it is safe!
//	JXApplication:
//		Added HandleCustomEvent() and HandleCustomEventWhileBlocking() to allow
//			real-time applications to process their own events.
//		Made Quit() virtual.
//	JXImage:
//		Added CreateFromFile(), CreateFromGIF(), CreateFromXPM(), WriteXPM().
//		Renamed GetColormap() to GetXColormap() to make way for new
//			JImage::GetColormap().
//		Added HasMask().
//		Promoted ConvertToImage(), ConvertToPixmap() to protected.
//		Made GetImage(), ConvertToImage(), GetPixmap(), ConvertToPixmap() const.
//	JXImageMask:
//		Added CreateFromXBM(), WriteXBM().
//	JXImageWidget:
//		Added GetImage().
//	Created JXCheckboxGroup, JXAtLeastOneCBGroup.
//	Created JXFileHistoryMenu.

// version 1.1.13:
//	JXApplication:
//		Run() returns when the event loop quits normally.
//		Quit() returns void because it only sets a flag, which invokes
//			Close() after the client code returns.
//		Selection works while a blocking window is active.
//		WM_DELETE_WINDOW messages are no longer buffered while a blocking
//			window is active.
//		Added HideAllWindows().
//	JXWindow:
//		Now translates the following keys:
//			XK_Left,  XK_KP_Left	->	kJLeftArrow
//			XK_Up,    XK_KP_Up		->	kJUpArrow
//			XK_Right, XK_KP_Right	->	kJRightArrow
//			XK_Down,  XK_KP_Down	->	kJDownArrow
//		Remember to always check modifiers.control() when you get
//			a control key in HandleKeyPress().  As an example,
//			JXCtrl('_') and kJDownArrow return the same value, so you
//			can't tell the difference without checking modifiers.control().
//		Ignores kMod2KeyIndex through kMod5KeyIndex when checking shortcuts
//			because NumLock and ScrollLock are in there somewhere.  Is there a
//			better way to filter out these modifiers?
//		Raise() no longer sets input focus to the window.
//	JXDNDManager:
//		Any Drag-And-Drop operation can be cancelled by pressing the Escape key.
//		Upgraded to Version 1 of the XDND protocol, which requires passing a
//			timestamp through to the JXSelectionManager.
//	JXSelectionManager:
//		Added timestamp argument to GetSelectionOwner(), GetAvailableTypes(), and
//			GetSelectionData().
//	JXContainer:
//		Added timestamp argument to WillAcceptDrop() and HandleDNDDrop().
//	JXTEBase:
//		Added timestamp argument to TEXWillAcceptDrop() and TEXConvertDropData().
//	JXDialogDirector:
//		Added UseModalPlacement().
//	JXCSFDialogBase:
//		Window geometry and file browser scroll position are now saved as part
//			of the setup.
//		Renamed UpdateButtons() to UpdateDisplay().
//	JXChooseSaveFile:
//		Removed ".." from file list and added "Up" button.
//		Added GetDirInfo().
//		All custom JXChooseSaveFile objects automatically share the same
//			JUNIXDirInfo and dialog state in order to present the simplest
//			model to the user.
//	Created JXPathHistoryMenu.
//	JXCSFDialogBase and derived classes:
//		Added upButton argument to SetObjects().
//		Changed prototype of SetObjects() so that pathHistory argument is of
//			type JXPathHistoryMenu*.
//	JXCardFile:
//		Added functions so you can use derived classes of JXWidgetSet.
//	JXTextClipboard:
//		Added SetClipboard().
//		Added functions to change whether or not styles are copied.
//	JXSelectionManager:
//		Fixed HandleSelectionRequest() so that it always sends SelectionNotify,
//			even if application doesn't own the selection.
//	JXScrollbar:
//		Shift-clicking on the scroll arrows moves by a page at a time.
//	JX*TableInput:
//		Now draws 1 pixel black border.  It is recommended that the table
//			itself remain white when a cell is being edited.
//		Scrolls the table to make the input field visible when the user
//			begins typing.
//	JXPasswordInput:
//		Draws # for each character entered.
//	JXColorManager:
//		Added GetSystemColorIndex().  This is only guaranteed to work when
//			using PseudoColor.  In general, you should use GetXPixel() instead.
//	JXWidget:
//		HandleFocusEvent() and HandleUnfocusEvent() now call Refresh().
//	JXHelpDirector:
//		Removed first argument of constructor (supervisor) since this must
//			always be the result of JXGetHelpManager().
//	JXDisplay:
//		Added RaiseAllWindows() and HideAllWindows().
//		Removed GetCurrentWindow().
//	JXCheckbox, JXRadioButton:
//		Created DrawChecked() to tell whether or not the object should be
//			drawn in the "on" state.  It is no longer correct for derived
//			classes to call IsChecked().
//	JXPathInput:
//		Scrolls to display end of string when it doesn't have focus.
//	JXDocument:
//		Added pure virtual protected function DiscardChanges().
//		Calls DiscardChanges() if user ok's closing without saving and
//			another document needs us to stay open.
//		Calls DiscardChanges() if user ok's deactivating without saving.
//	JXMenuBar:
//		Renamed kStdMenuBarHeight to kJXStdMenuBarHeight.

// version 1.1.12:
//	JXTEBase:
//		FilterSelectionData() now removes illegal characters such as nullptr's
//			via JTextEditor::RemoveIllegalChars().
//		Now treats keypad arrows the same as the "normal" arrows.
//	JXWindow:
//		Added LockCurrentMinSize() and LockCurrentMaxSize().
//	JXUNIXDirTable:
//		Now treats keypad arrows the same as the "normal" arrows.
//	JXMenu:
//		No longer crashes when displaying an empty submenu.
//		The number of items on the menu is only required to be non-zero
//			-after- NeedsUpdate is broadcast, not before.

// version 1.1.11:
//	Created JXCardFile.
//	JXDocument:
//		SafetySave() now takes argument of type JXDocumentManager::SafetySaveReason.
//	JXFileDocument:
//		SafetySave() saves in #...# if the X server dies and ##...## if assert() fires.
//	JXApplication:
//		Created Abort().  This is called when the X server dies or when assert()
//			decides suicide is appropriate.  It triggers both SafetySave() and
//			CleanUpBeforeSuddenDeath().
//		CleanUpBeforeSuddenDeath() now takes argument of type
//			JXDocumentManager::SafetySaveReason.
//	JXUNIXDirTable:
//		Up and down arrow keys no longer wrap selection around the end of the table.
//	JXFontManager:
//		Finally switched from xlsfonts to XListFonts().
//	JXContainer:
//		Added IsAncestor().
//	JXWindow:
//		KillFocus() generates a call to JXWidget::HandleUnfocusEvent().
//		Added SwitchFocusToFirstWidgetWithAncestor().
//		GetULShortcutIndex() is much smarter about finding the appropriate
//			character to underline.
//		InstallShortcut() automatically applies JXCtrl() to the given key
//			if the Control modifier is on.  To avoid getting Ctrl-Shift-A
//			when you specify Ctrl-A, you must pass in 'A' instead of JXCtrl('A').
//		Added OKToUnfocusCurrentWidget().
//	JXMenu:
//		The string passed to SetItemShortcuts() is no longer case sensitive,
//			so strings like "AaEe" can now be shortened to "ae".  Combined with
//			the improved JXWindow::GetULShortcutIndex(), the appropriate
//			character will almost always get underlined.
//	JXTextMenu:
//		"Ctrl-Shift-" and "Meta-Shift-" are allowed only for alphabetic and control
//			characters because of differences between keyboards.
//	JXButtonStates, JXKeyModifiers:
//		Added ToggleState() and more versions of GetState().
//	JXStandAlonePG:
//		Windows are stacked vertically instead of being placed on top of eachother.

// version 1.1.10:
//	Created JXTextClipboard to provide a way to preserve text on the
//		clipboard after the widget that performed Copy() is deleted.
//	JXEditTable:
//		Uses JXTextClipboard to preserve the copied text after EndEditing()
//			is called.
//	JXWindow:
//		SetSize() and AdjustSize() now keep the requested size within the
//			range set with SetMinSize() and SetMaxSize().
//	JXTable:
//		Added WantsKey().  JXTEBase shortcuts now work in TableInput widgets.
//	JXFileDocument:
//		Renamed ReadSetup() to ReadJXFDSetup() and WriteSetup() to
//			WriteJXFDSetup().  These functions are no longer virtual because
//			it is too dangerous.
//		Added WillMakeNewBackupEveryOpen(), ShouldMakeNewBackupEveryOpen().
//			This controls whether a backup file is created only if none exists
//			or whenever the file is first saved after being opened.
//			The default is for this flag to be kJTrue.
//	JXPSPrinter:
//		Renamed ReadSetup() to ReadXPSSetup() and WriteSetup() to WriteXPSSetup().
//			These functions are no longer virtual because it is too dangerous.
//	JXEPSPrinter:
//		Created ReadXEPSSetup() and WriteXEPSSetup().
//	JXTEBase:
//		Added ReadPrintSetup(), WritePrintSetup(), ReadPrintPlainTextSetup(),
//			WritePrintPlainTextSetup().
//		In HandleMouseDown(), clickCount is ignored for middle-click.
//	JXHelpManager:
//		Added PrintAll(), CloseAll().
//	JXHelpDirector:
//		Renamed Print menu to File menu and added items to save, print all
//		help sections, and close windows.
//	JXApplication:
//		Urgent tasks are now guaranteed to execute in the order that they
//			are registered.

// version 1.1.9:
//	JXContainer:
//		Draw() is clipped to aperture instead of bounds.
//	JXTable:
//		Draws far lower right row and column borders if Bounds is smaller than
//			Aperture.
//	JXTEBase:
//		Changed "clean right margin" flags in AppendEditMenu(),ShareEditMenu() to
//			"adjust margins".  This now controls the group of three menu items:
//			"Clean right margins | Shift left | Shift right".
//		No longer hides the cursor when you type menu shortcuts or other
//			keystrokes with modifiers.
//	JXWindow:
//		Meta-tab can now be used to tab out of an input fields that grabs the
//			tab key.
//	JXWidget:
//		WantInput() nows lets you request three categories:
//			regular input, unmodified tabs, and modified tabs
//	JXTextMenu:
//		Now recognizes "return" and "tab" as valid menu item shortcuts.
//			(e.g. "Meta-return" will be parsed correctly.)
//			Note that it is a bad idea to use "return" as a shortcut if the
//			window contains a JXTextEditor.  "Meta-tab" is not available,
//			since JXWindow catches it.
//	JXScrollableWidget:
//		Only calls WantInput(kJTrue,kJFalse) if the scrollbar set is not nullptr.
//			If you need input and you don't have a scrollbar set, you now
//			have to call WantInput() yourself.

// version 1.1.8:
//	JXSelectionManager:
//		Now supports INCR protocol for transferring unlimited quantities of data.
//			(Note that this is a bad idea, since it is so slow!)
//	JXTEBase:
//		Made AppendEditMenu(), ShareEditMenu(), GetMenuStyle(), and
//			GetEditMenu() public.
//		Added GetEditMenuCmd() to convert menu index to JTextEditor::CmdIndex.
//		AppendEditMenu() takes a flags to request "Clean right margin" and
//			"Read only" menu items.
//		ShareEditMenu() takes flags to decide the use of "Clean right margin"
//			and "Read only" menu items.
//		Added TEXWillAcceptDrop() and TEXConvertDropData() so derived classes
//			can accept other data types and paste them in.
//			(To paste other formats from the clipboard,
//			 override TEGetExternalClipboard().)
//		Implements TEDisplayBusyCursor() to display the busy cursor in all
//			windows.
//	JXHelpDirector:
//		Now appends Edit menu to the menu bar to provide Copy and Select All.
//		Now allows dragging text from it to other windows.
//	JXMenu:
//		Renamed GetArrowPosition() to GetPopupArrowPosition().
//		Renamed SetArrowPosition() to SetPopupArrowPosition().
//	JXMenuBar:
//		Added InsertMenuAhead(), InsertMenuAfter().
//	JXDisplay:
//		Added SendXEvent(), DisplayCursorInAllWindows().
//	JXApplication:
//		Added DisplayBusyCursor(), DisplayInactiveCursor();
//	JXUserNotification:
//		Displays inactive cursor in all other windows.
//	JXChooseSaveFile:
//		ReadSetup(), WriteSetup() are no longer virtual.
//		Displays inactive cursor in all other windows.
//	JXProgressDisplay:
//		For foreground processes, displays busy cursor in all other windows.
//	JXMDIServer:
//		Calls JXSetMDIServer() automatically.
//	JXDocumentManager:
//		Calls JXSetDocumentManager() automatically.
//	JXFontManager:
//		GetXFontNames() now accepts sorting function as a third parameter.
//	JXScrollbar:
//		Added IsAtMin(), ScrollToMin(), IsAtMax(), ScrollToMax().

// version 1.1.7:
//	You must register your application signatures (for prefs, MDI, etc)
//		with jx-registry@newplanetsoftware.com to ensure that they
//		are unique.
//	Moved all global variables behind the function interface in jXGlobals.h:
//		gApplication, gXChooseSaveFile, gXAssert,
//		document manager, help manager, MDI server
//	Moved font names from JXFontStyle.h to functions in jXGlobals.h:
//		kJXHelveticaFontName, kJXTimesFontName,
//		kJXCourierFontName, kJXSymbolFontName
//	Moved kJXDefaultFontSize to jXConstants.h
//	Removed JXFontStyle.h
//	JXDisplay:
//		Renamed GetDisplayName() to GetName().
//		Added mechanism to convert XErrors into BroadcastWithFeedback() messages.
//			Check JXDisplay.doc::XErrorHandler for details.
//	jXUtil.h:
//		Added JXRegionIsRectangle(), JXGetRegionBounds().
//		Optimized JXIntersection(region,pixmap).
//		JXIntersection now returns kJFalse if the result is empty.
//	JXImage:
//		Optimized drawing of images that have masks.
//	Removed jXColors.h.
//		All color indices must now be obtained from the colormap.
//		Each color constant has been converted to a function in JXColorManager.
//		The values of the colors are no longer assured, so any code that
//			assumed particular values for black,red,etc is now invalid!!!
//	JXImageMask:
//		Removed kPixelOn.
//	JXColorManager:
//		Overloaded AllocateStaticColor() to accept a valid X pixel value.
//		Now inherits from JXColorManager.
//		Added more pre-allocated colors.
//		All JX colors are now as Netscape-compatible as possible.
//		Added GetDisplay() and GetXVisualInfo().
//	JXWindowPainter:
//		Added GetXColormap().
//	JXMenu:
//		Fixed top level menus so that shortcuts will not be triggered if
//			the menu is deactivated or invisible.
//		Added SetToHiddenPopupMenu() so that invisible popup menus will still
//			accept shortcuts even though they are invisible.
//	JXMenuBar:
//		Overloaded DeleteMenu() and RemoveMenu() to accept JXMenu*.
//			Both return JBoolean to indicate whether or not the menu was found.
//	JXDocument:
//		Added pure virtual function SafetySave().
//		GetName() is no longer required to return a unique name.
//	JXFileDocument:
//		Implemented SafetySave() to work like emacs.
//		WriteFile() prototype changed to include safetySave flag.
//		GetName() now returns the file name, instead of the full path.
//		DataModified() now does nothing if itsSavedFlag is already kJFalse.
//	Created JXDocumentManager.
//		Automatically calls SafetySave() on all open documents at a settable interval.
//	Created JXMDIServer.
//	JXApplication:
//		Urgent tasks are now performed after idle tasks, so idle tasks can
//			install urgent tasks.
//		Initializes JThisProcess to convert signals into Broadcaster messages.
//			Signals are broadcast as an urgent task to avoid reentrancy problems.
//		Calls Quit() when JThisProcess reports SIGTERM, SIGQUIT.
//		Added mechanism to catch server crashes.  This calls SafetySave() on
//			all open documents and then calls the new virtual function
//			CleanUpBeforeSuddenDeath().  Derived classes can override this
//			to save preferences, etc.
//		Moved all document handling functions to JXDocumentManager.  Derived
//			classes must now create JXDocumentManager (or a derived class) and call
//			JXSetDocumentManager() if the program uses JXDocument.
//			(JXCreateDefaultDocumentManager() can be used by programs that
//			don't need a derived class.)
//		Removed last two arguments from constructor.
//		Moved MDI code to JMDIServer.  Derived classes must now create
//			a derived class of JXMDIServer and call JXSetMDIServer(),
//			instead of calling BecomeMDIServer().
//		Moved InitHelp() to jXGlobals.h
//		Removed SearchSubdirs() because JSearchSubdirs() now creates
//			a JProgressDisplay object if you don't give it one.
//	JXGC:
//		Added functions for drawing dashed lines.
//	JXDNDManager:
//		Now uses protocol described at http://www.newplanetsoftware.com/xdnd/
//	JXContainer:
//		Changed prototype of WillAcceptDrop(), HandleDNDDrop().
//	JXWidget:
//		Changed prototype of ConvertSelection().
//		Added GetSelectionTargets() and GetSelectionTimeStamp().
//		Scroll() and ScrollTo() now return JBoolean to indicate whether or
//			not the widget actually scrolled.
//		Added HandleWindowFocusEvent() and HandleWindowUnfocusEvent().
//			These are sent to the widget with focus when the X window
//			gets or loses focus.
//	JXSelectionManager:
//		Renamed kClipboardName to kJXClipboardName.
//		Optimized transfer of selection data when ownership is local.
//		Changed prototype of GetSelectionData().
//		*** GetSelectionData() must be followed by DeleteSelectionData().
//	Created JXStyleMenu.  JXTEStyleMenu now derives from this.
//	Created JXStyleTable.  JXStringTable and JXFloatTable now derive from this.
//	Created JXStyleTableMenu to work with JXStyleTable.
//	JXChooseSaveFile:
//		Removed showHidden from Create*Dialog().
//	JXCSFDialogBase and derived classes:
//		Removed showHidden from constructor, SetObjects(), and BuildWindow().
//	JXUNIXDirTable:
//		Now remembers selection after the directory is updated.
//	JXCSFIcons:
//		Renamed constants to start with kJX.
//	JXStringList:
//		Does not allow JOrderedSetT::Sorted message.
//	JXRowHeaderWidget:
//		Now broadcasts NeedsToBeWidened() if numbers get too wide.
//	JXTEBase:
//		Hides the caret and outlines the selection when the window loses focus.
//		Added AskForLine().
//	Renamed JXCEPrintDialog to JXPrintPlainTextDialog.
//	Renamed JXCEGoToLineDialog to JXGoToLineDialog.
//	Removed JXCodeEditor.  All its features are now either in JTextEditor
//		or JXTEBase.
//	JXScrollbar:
//		Added AdjustValue(), StepLine(), StepPage().
//	JXScrollableWidget:
//		If the derived class doesn't catch the arrow keys,
//			it scrolls by one line in the appropriate direction.

// version 1.1.6:
//	JXApplication:
//		Added InitHelp() to support JXHelpManager system.
//	JXImagePainter:
//		It is now safe to delete the JXImage before the JXImagePainter.

// version 1.1.5:
//	Created JXStringHistoryMenu.
//	JXChooseSaveFile:
//		Added JXStringHistoryMenus to all three dialog windows to store
//			previous paths and filters.
//		Added virtual functions ReadSetup() and WriteSetup() so programs can
//			store state of object in preferences file.
//	JXCSFDialogBase:
//		Added two JXStringHistoryMenus called pathHistory and filterHistory as
//			arguments to SetObjects().
//	JXCursorIndex renamed to JCursorIndex, in preparation for JCursorManager.
//	JXApplication:
//		Fixed HandleOneEventForWindow() so menus can be used in blocking windows.
//	JXInputField, JXStringInput:
//		Added second constructor to allow for multi-line input fields.
//	Created JXHelpManager to encapsulate creation of JXHelpDirectors.
//		This will also be the foundation of hypertext help.
//	JXHelpDirector:
//		Changed constructor prototype to include the window title and
//			allow for an arbitrary number of formats.
//		Merged SetTitle() and SetText() into SetContents().
//	JXTEBase:
//		Added ShareEditMenu() to allow text editors to share an Edit menu.
//	JXStringList:
//		Added GetStringList().
//	JXTextButton:
//		If the first shortcut is Control-M, and the button label contains an 'm',
//			this is no longer underlined since the black border shows that
//			the button responds to the return key.

// version 1.1.4:
//	JXCursor:
//		Added JXGetHandCursor().
//	JXTEBase:
//		Deallocates custom colors when deleted.
//	JXHelpDirector:
//		Inserted 3rd argument into constructor: isHTML.
//	JXFileDocument:
//		Added SaveCopyInNewFile(), ReadSetup(), WriteSetup().
//	JXCodeEditor:
//		Added ReadSetup(), WriteSetup().

// version 1.1.3:
//	JXPGMessageDirector.cc:
//		Added AddMessageString().
//		Now uses 6x13 font, when available.
//		Added Print button.
//	JXCodeEditor:
//		Print() moved to JXTEBase::HandlePrintPlainText().
//	JXTEBase:
//		Added HandlePrintPlainText().
//	JXWarningDialog:
//		Shortcuts are now Meta-Y and Meta-N to conform to the rest of JX.
//	JXOKToCloseDialog:
//		Shortcuts are now Meta-S (Save), Meta-N (Don't save), and Esc (Cancel)
//			to conform to the rest of JX.  I changed from Meta-O to Meta-N because
//			"N" is closer to "No" as in "No, don't save".

// version 1.1.2:
//	JXWindow:
//		Added RequestInputFocus() and Lower().
//		Added ReadGeometry() and WriteGeometry().
//	Replaced all calls to system() with calls to JExecute().
//	jXEventUtil.h:
//		Added JXDisplay* as second argument to JXGetButtonAndModifierStates().
//	JXMenu:
//		Now uses assert() to check that menu is not empty when it is opened.
//			This means that the programmer is responsible for disabling
//			empty menus.  It only checks after the NeedsUpdate message, however,
//			so you can build the menu on the fly.
//		Added SetItemEnable().
//		Popup menus (not in a menu bar) now draw a down arrow to the right
//			of the menu title.
//		Added PopUp() to allow (but not encourage!) hidden popup menus inside
//			other widgets.
//	JXTextMenu:
//		Removed the ability to show a different menu item when the Shift key
//			is pressed.  I don't normally like to remove features, but this
//			has turned out to be just too stupid.  It is totally invisible,
//			which contradicts the fundamental principle of GUI design that
//			everything should be visible and easy to find.
//	JXMenuData:
//		Renamed IsActive() to IsEnabled() for consistency.
//	JXTextMenuData:
//		Changed prototypes of GetImage() and GetNMShortcut().
//	JXGC, JXFontManager:
//		Long strings are now split into pieces so the maximum packet size
//			to the X server is never exceeded.
//	assert():
//		JXApplication now installs JXAssert to ensure that all display, mouse,
//			and keyboard grabs are cancelled before the assert message is
//			displayed.
//		JXAssert provides the function UnlockDisplays() which can be called
//			before a breakpoint when debugging.
//	JXTEBase:
//		Caret is now red.
//	JXTable:
//		Implemented HitSamePart() to return kJTrue only if the two points
//			are in the same cell.
//	JXColHeaderWidget:
//		Added functions to change the column titles.
//	Renamed:
//		JXPrinter          -> JXPSPrinter
//		JXPageSetupDialog  -> JXPSPageSetupDialog
//		JXPrintSetupDialog -> JXPSPrintSetupDialog
//	Added JXEPSPrinter and JXEPSPrintSetupDialog.
//	JXFileDocument:
//		Now has option to pre-allocate space for *** in the window's iconfied
//			state so fvwm will never truncate the file name when the window
//			is iconified.
//		Added fifth constructor argument defaultFileNameSuffix.
//			(This can be the empty string.)
//	Created JXSaveFileInput.  When it gets focus, it selects the file name,
//		but not the file extension.
//	JXSaveFileDialog:
//		SetObjects() now requires object of type JXSaveFileInput*.
//	JXWidget:
//		Renamed Unfocus() to NotifyFocusLost().  Only JXWindow should call this.
//		Created new Unfocus() that actually unfocuses the Widget.
//	JXApplication:
//		Idle loop calls JIPCLink::CheckAllStatus() so JIPC socket connections
//			will broadcast "magically".
//	JXGC:
//		Added DrawLines().
//	JXWindowPainter:
//		Optimized Polygon().
//	jXPainterUtil:
//		Optimized DrawUpFrame() and DrawDownFrame().
//	JXCSFDialogBase:
//		After creating a new directory, the user is placed there automatically.

// version 1.1.1:
//	Renamed all global functions to start with JX for consistency with JCore
//		naming convention.  All the functions in the following files were renamed:
//		JXCursor.h, jXPainterUtil.h, jXEventUtil.h, jXUtil.h
//	jxctrl() renamed to JXCtrl() for consistency.
//	JXFileDocument:
//		Automatically prepends asterisks to window title if document needs to
//			be saved.  (This is as close as I can get to NeXT's system.)
//		Created CanRevert() to tell whether it makes sense to enable the
//			"Revert to saved" menu item.
//		OKToRevert() is now virtual.
//	JXMenu:
//		SetTitleColor() replaced by SetTitleFontStyle().  You only have to change
//			the name of the function, because SetTitleFontStyle() will accept a
//			color index.
//		Added SetTitleFontName() and SetTitleFontSize().
//	JXSelectionManager & JXWidget:
//		Generalized selection mechanism to support named selections.
//		kClipboardName (in JXSelectionManager.h) is the name of the original clipboard.
//		JXDNDManager provides GetDNDSelectionName() to get the name of the
//			selection via which dropped data is transferred.
//		The following functions now require that the name of the selection be
//			passed in:
//			JXSelectionManager:
//				GetSelectionOwner(), GetAvailableTypes(), GetSelectionData()
//			JXWidget:
//				AddSelectionTarget(), RemoveSelectionTarget(),
//				ConvertSelection(), LostSelectionOwnership(),
//				BecomeSelectionOwner(), OwnsSelection()
//		JXSelectionManager::GetAvailableTypes() no longer notifies the user if
//			the selection is empty.
//	Drag-And-Drop:
//		Added JXContainer::WillAcceptDrop() so derived classes can decide
//			whether or not they want to become the target, be hilighted,
//			and receive DND messages.
//		Data is now transferred via a separate selection type.  See above.
//		JXDNDAck contains a flag to tell whether or not the current target
//			will accept the drop.
//	JXDNDManager:
//		JXDND messages are only sent to a window if it has the JXDNDAware property.
//		(type XA_STRING)  All JXWindows have this property set automatically.
//	JXTEBase:
//		Implemented copying, pasting, and dragging styled text.
//		Only JXTextEditor allows pasting styled text, however, because
//			JXInputField and JXCodeEditor should have a single style.
//		Rewired modifier keys so that Meta means start/end of line and
//			Ctrl means forward/backward one word.
//	JXInputField:
//		Return key automatically translated to space to enforce single line input.
//	Created JXAnimationTask to provide structure for animating JXImageWidget.
//	JXUpRect, JXDownRect:
//		Fixed constructor so the border width starts out equal to
//			kJXDefaultBorderWidth.
//	Blocking dialog windows now work correctly with the clipboard and DND.
//	Created JXPasswordInput.
//	Document windows no longer grab keyboard focus.  Now, only dialog
//		windows do.  To change the behavior of a particular window, use
//		JXWindow::SetFocusWhenShow().
//	JXFontManager::GetXFontNames() no longer returns duplicate font names,
//		even if xlsfonts does.
//	Added JXScrollbar::SetScrollDelay() to control scrolling speed.
//	Scrolling text is much faster.

// version 1.1.0:
//	JXApplication:
//		Added SearchSubdirs() as a convenience function to automatically
//		create a JProgressDisplay object and pass it to jdirUtil::JSearchSubdirs().
//	JXDisplay:
//		Both versions of FindMouseContainer() can now return an X window id.
//	Drag-And-Drop:
//		Initial release.  Refer to JXDNDManager and JXContainer for information.
//		Each JXDisplay now owns a JXDNDManager, accessible directly and via
//		JXContainer.  Data is transferred via the X selection mechanism.

// version 1.0.10:
//	JXApplication:
//		Added HadBlockingWindow() so widgets can detect when the secondary event
//			loop was invoked.
//	JXWindow:
//		Uses JXApplication::HadBlockingWindow() to clean itself up if a mouse
//			event triggered the secondary event loop.

// version 1.0.9:
//	Created jXKeySym.h to define keysyms not included in X11R5 headers.

// version 1.0.8:
//	JXWindow:
//		Fixed key press handler to handle 8-bit characters correctly.

// version 1.0.7:
//	JXWindow:
//		Fixed subtle bug that produced "pure virtual method called" error
//			when destructing window containing a widget containing
//			mutiple widgets that could accept focus.

// version 1.0.6:
//	JXImage:
//		Fixed bug that could cause crash in ConvertToImage() while constructing
//			from JXPM on 8-bit displays.

// version 1.0.5:
//	JXDialogDirector:
//		Fixed bug in Receive() that could be responsible for random,
//			irreproducible crashes when closing dialog windows.

// version 1.0.4:
//	Various minor changes to make the library compile on 64-bit machines.
//	JXPGMessageDirector:
//		Fixed sizing for Save and Close buttons.
//	JXFontManager:
//		Now works correctly on multiple displays by using the "-display" option.
//	JXWindow:
//		Fixed Update() so it is now safe for widgets to call Refresh() inside Draw().
//		Fixed Iconify()/Deiconify() so they always update XWMHints.
//		Update() returns immediately if the window is iconified.
//	JXApplication:
//		Run() calls Quit() before exiting.
//		No longer hogs all the CPU cycles when HandleOneEventForWindow()
//			is called.
//	JXButton:
//		Added Push() to allow the program to push its own buttons.
//	JXStandAlonePG:
//		Using the window manager's Quit function on a progress display window
//			activates the Cancel button, if there is one.
//		JXApplication will not quit while the process is running.

// version 1.0.3:
//	Shift-tab now works correctly with the latest version of the XFree server.

// version 1.0.2:
//	Changes made to JCore.

// version 1.0.1:
//	JXPartition:
//		Darkened bars in the strips between the compartments.
//	JXMenuBar:
//		Added GetMenuCount().

#endif
