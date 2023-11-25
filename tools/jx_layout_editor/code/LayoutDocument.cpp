/******************************************************************************
 LayoutDocument.cpp

	BASE CLASS = public JXFileDocument

	Copyright (C) 2023 by John Lindal.

 *****************************************************************************/

#include "LayoutDocument.h"
#include "FileHistoryMenu.h"
#include "LayoutContainer.h"
#include "CustomWidget.h"
#include "TextButton.h"
#include "MDIServer.h"
#include "globals.h"
#include "actionDefs.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXGetStringDialog.h>
#include <jx-af/jx/JXWebBrowser.h>
#include <jx-af/jx/JXMacWinPrefsDialog.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

static const JUtf8Byte* kFileSignature = "jx_layout_editor";
const JFileVersion kCurrentFileVersion = 0;

// File menu

static const JUtf8Byte* kFileMenuStr =
	"    New...            %k Meta-N       %i" kNewLayoutAction
	"%l| Open...           %k Meta-O       %i" kOpenLayoutAction
	"  | Recent"
	"%l| Save              %k Meta-S       %i" kSaveLayoutAction
	"  | Save as...        %k Ctrl-S       %i" kSaveLayoutAsAction
	"  | Save a copy as... %k Ctrl-Shift-S %i" kSaveLayoutCopyAsAction
	"  | Revert to saved                   %i" kRevertLayoutsAction
	"  | Save all          %k Meta-Shift-S %i" kSaveAllLayoutsAction
	"%l| Show in file manager              %i" kShowInFileMgrAction
	"%l| Close             %k Meta-W       %i" kJXCloseWindowAction
	"%l| Quit              %k Meta-Q       %i" kJXQuitAction;

enum
{
	kNewCmd = 1,
	kOpenCmd, kRecentMenuCmd,
	kSaveCmd, kSaveAsCmd, kSaveCopyAsCmd, kRevertCmd, kSaveAllCmd,
	kShowInFileMgrCmd,
	kCloseCmd,
	kQuitCmd
};

// Edit menu

static const JUtf8Byte* kEditMenuStr =
	"    Undo       %k Meta-Z       %i" kJXUndoAction
	"  | Redo       %k Meta-Shift-Z %i" kJXRedoAction
	"%l| Cut        %k Meta-X       %i" kJXCutAction
	"  | Copy       %k Meta-C       %i" kJXCopyAction
	"  | Paste      %k Meta-V       %i" kJXPasteAction
	"  | Delete                     %i" kJXClearAction
	"%l| Select all %k Meta-A       %i" kJXSelectAllAction;

enum
{
	kUndoCmd = 1, kRedoCmd,
	kCutCmd, kCopyCmd, kPasteCmd, kClearCmd,
	kSelectAllCmd
};

// Preferences menu

static const JUtf8Byte* kPrefsMenuStr =
	"    Edit tool bar..."
	"  | File manager..."
	"  | Mac/Win/X emulation...";

enum
{
	kEditToolBarCmd = 1,
	kFilePrefsCmd,
	kEditMacWinPrefsCmd
};

/******************************************************************************
 Create (static)

	Create an empty layout.

 *****************************************************************************/

bool
LayoutDocument::Create
	(
	LayoutDocument** doc
	)
{
	auto* dlog = jnew JXGetStringDialog(
		JGetString("NewLayoutNameWindowTitle::LayoutDocument"),
		JGetString("EditLayoutNamePrompt::LayoutDocument"));

	if (dlog->DoDialog())
	{
		*doc = jnew LayoutDocument(dlog->GetString(), false);
		(**doc).Activate();
		return true;
	}

	return false;
}

/******************************************************************************
 Create (static)

	Load layout(s) from a file.

 *****************************************************************************/

bool
LayoutDocument::Create
	(
	const JString& fullName
	)
{
	std::ifstream input(fullName.GetBytes());
	JFileVersion vers;

	FileStatus status = DefaultCanReadASCIIFile(input, "Magic:", 15000, &vers);
	if (status == kFileReadable)
	{
		ImportFDesignFile(input);
		return true;
	}
	else if (status == kNeedNewerVersion)
	{
		const JString v((JUInt64) vers);
		const JUtf8Byte* map[] =
		{
			"v", v.GetBytes()
		};
		const JString msg = JGetString("UnsupportedFDesignVersion::LayoutDocument", map, sizeof(map));
		JGetUserNotification()->ReportError(msg);
		return false;
	}

	JXFileDocument* fdoc;
	if (JXGetDocumentManager()->FileDocumentIsOpen(fullName, &fdoc))
	{
		fdoc->Activate();
		return true;
	}

	status = DefaultCanReadASCIIFile(input, kFileSignature, kCurrentFileVersion, &vers);
	if (status == kFileReadable)
	{
		auto* doc = jnew LayoutDocument(fullName, true);
		doc->ReadFile(input);
		doc->Activate();
		return true;
	}
	else if (status == kNeedNewerVersion)
	{
		JGetUserNotification()->ReportError(JGetString("NeedNewerVersion::LayoutDocument"));
		return false;
	}
	else
	{
		JGetUserNotification()->ReportError(JGetString("UnknownFile::LayoutDocument"));
		return false;
	}
}

/******************************************************************************
 Constructor (protected)

 *****************************************************************************/

LayoutDocument::LayoutDocument
	(
	const JString&	fullName,
	const bool		onDisk
	)
	:
	JXFileDocument(JXGetApplication(), fullName, onDisk, false, ".jxl")
{
	BuildWindow();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

LayoutDocument::~LayoutDocument()
{
	bool onDisk;
	const JString fullName = GetFullName(&onDisk);
	if (onDisk)
	{
		GetDocumentManager()->AddToFileHistoryMenu(fullName);
	}
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "main_window_icon.xpm"
#include <jx-af/image/jx/jx_file_new.xpm>
#include <jx-af/image/jx/jx_file_open.xpm>
#include <jx-af/image/jx/jx_file_save.xpm>
#include <jx-af/image/jx/jx_file_revert_to_saved.xpm>
#include <jx-af/image/jx/jx_file_save_all.xpm>
#include <jx-af/image/jx/jx_edit_undo.xpm>
#include <jx-af/image/jx/jx_edit_redo.xpm>
#include <jx-af/image/jx/jx_edit_cut.xpm>
#include <jx-af/image/jx/jx_edit_copy.xpm>
#include <jx-af/image/jx/jx_edit_paste.xpm>
#include <jx-af/image/jx/jx_edit_clear.xpm>

void
LayoutDocument::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 500,300, JString::empty);

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 500,30);
	assert( menuBar != nullptr );

	itsToolBar =
		jnew JXToolBar(GetPrefsManager(), kLayoutDocToolBarID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 500,270);
	assert( itsToolBar != nullptr );

// end JXLayout

	AdjustWindowTitle();
	window->SetCloseAction(JXWindow::kCloseDirector);
	window->SetWMClass(GetWMClassInstance(), GetLayoutDocumentClass());
	window->SetMinSize(200, 100);

	JXImage* image = jnew JXImage(GetDisplay(), main_window_icon);
	window->SetIcon(image);

	itsLayoutContainer =
		jnew LayoutContainer(this, itsToolBar->GetWidgetEnclosure(),
							 JXWidget::kHElastic,JXWidget::kVElastic,
							 0,0, 100,100);
	itsLayoutContainer->FitToEnclosure();

	// menus

	itsFileMenu = menuBar->PrependTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "LayoutDocument");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&LayoutDocument::UpdateFileMenu,
		&LayoutDocument::HandleFileMenu);

	itsFileMenu->SetItemImage(kNewCmd,     jx_file_new);
	itsFileMenu->SetItemImage(kOpenCmd,    jx_file_open);
	itsFileMenu->SetItemImage(kSaveCmd,    jx_file_save);
	itsFileMenu->SetItemImage(kRevertCmd,  jx_file_revert_to_saved);
	itsFileMenu->SetItemImage(kSaveAllCmd, jx_file_save_all);

	jnew FileHistoryMenu(itsFileMenu, kRecentMenuCmd, menuBar);

	itsEditMenu = menuBar->AppendTextMenu(JGetString("EditMenuTitle::JXGlobal"));
	itsEditMenu->SetMenuItems(kEditMenuStr, "LayoutDocument");
	itsEditMenu->SetUpdateAction(JXMenu::kDisableAll);
	itsEditMenu->AttachHandlers(this,
		&LayoutDocument::UpdateEditMenu,
		&LayoutDocument::HandleEditMenu);

	itsEditMenu->SetItemImage(kUndoCmd,  jx_edit_undo);
	itsEditMenu->SetItemImage(kRedoCmd,  jx_edit_redo);
	itsEditMenu->SetItemImage(kCutCmd,   jx_edit_cut);
	itsEditMenu->SetItemImage(kCopyCmd,  jx_edit_copy);
	itsEditMenu->SetItemImage(kPasteCmd, jx_edit_paste);
	itsEditMenu->SetItemImage(kClearCmd, jx_edit_clear);

	itsPrefsMenu = menuBar->AppendTextMenu(JGetString("PrefsMenuTitle::JXGlobal"));
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "LayoutDocument");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPrefsMenu->AttachHandler(this, &LayoutDocument::HandlePrefsMenu);

	JXTextMenu* helpMenu = GetApplication()->CreateHelpMenu(menuBar, "LayoutDocument", "MainHelp");

	// must be done after creating widgets

	itsToolBar->LoadPrefs();
	if (itsToolBar->IsEmpty())
	{
		itsToolBar->AppendButton(itsFileMenu, kNewCmd);
		itsToolBar->AppendButton(itsFileMenu, kOpenCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsFileMenu, kSaveCmd);
		itsToolBar->AppendButton(itsFileMenu, kSaveAllCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsEditMenu, kUndoCmd);
		itsToolBar->AppendButton(itsEditMenu, kRedoCmd);
		itsToolBar->NewGroup();
		itsToolBar->AppendButton(itsEditMenu, kCutCmd);
		itsToolBar->AppendButton(itsEditMenu, kCopyCmd);
		itsToolBar->AppendButton(itsEditMenu, kPasteCmd);

		GetApplication()->AppendHelpMenuToToolBar(itsToolBar, helpMenu);
	}
}

/******************************************************************************
 GetName (virtual)

	Override of JXDocument::GetName().

 ******************************************************************************/

const JString&
LayoutDocument::GetName()
	const
{
	JString root, suffix;
	JSplitRootAndSuffix(GetFileName(), &root, &suffix);
	itsDocName = root;

	return itsDocName;
}

/******************************************************************************
 ReadFile (private)

 ******************************************************************************/

void
LayoutDocument::ReadFile
	(
	std::istream& input
	)
{
	input.ignore(strlen(kFileSignature));

	JFileVersion vers;
	input >> vers;
	assert( vers <= kCurrentFileVersion );

	jdelete itsLayoutContainer;
	itsLayoutContainer =
		jnew LayoutContainer(this, itsToolBar->GetWidgetEnclosure(),
							 JXWidget::kHElastic,JXWidget::kVElastic,
							 0,0, 100,100);
	itsLayoutContainer->FitToEnclosure();

	JCoordinate w,h;
	input >> w >> h;
	SetLayoutSize(w,h);

	JPtrArray<JXWidget> widgetList(JPtrArrayT::kForgetAll);

	JString className, varName;
	JRect frame;
	while (!input.eof() && !input.fail())
	{
		bool keepGoing;
		input >> keepGoing;
		if (!keepGoing)
		{
			break;
		}

		JIndex enclosureIndex;
		int hs, vs;
		input >> enclosureIndex >> className >> hs >> vs >> frame;

		JXWidget* e =
			enclosureIndex == 0 ? itsLayoutContainer :
			widgetList.GetItem(enclosureIndex);

		const JXWidget::HSizingOption hS = (JXWidget::HSizingOption) hs;
		const JXWidget::VSizingOption vS = (JXWidget::VSizingOption) vs;

		const JCoordinate x = frame.left;
		const JCoordinate y = frame.top;
		const JCoordinate w = frame.width();
		const JCoordinate h = frame.height();

		JXWidget* widget = nullptr;
		if (className == "TextButton")
		{
			widget = jnew TextButton(this, input, e, hS,vS, x,y,w,h);
		}
		else
		{
			assert( className == "CustomWidget" );
			widget = jnew CustomWidget(this, input, e, hS,vS, x,y,w,h);
		}

		widgetList.Append(widget);
	}
}

/******************************************************************************
 WriteTextFile (virtual protected)

	This must be overridden if WriteFile() is not overridden.

 ******************************************************************************/

void
LayoutDocument::WriteTextFile
	(
	std::ostream&	output,
	const bool		safetySave
	)
	const
{
	output << kFileSignature << ' ' << kCurrentFileVersion << std::endl;
	output << itsLayoutContainer->GetApertureWidth() << std::endl;
	output << itsLayoutContainer->GetApertureHeight() << std::endl;

	JPtrArray<JXWidget> widgetList(JPtrArrayT::kForgetAll);

	itsLayoutContainer->ForEach([&output, &widgetList](const JXContainer* obj)
	{
		const BaseWidget* widget = dynamic_cast<const BaseWidget*>(obj);
		if (widget == nullptr)
		{
			return;		// used for rendering
		}

		const JXWidget* encl = dynamic_cast<const JXWidget*>(obj->GetEnclosure());
		assert( encl != nullptr );

		JIndex enclosureIndex;
		widgetList.Find(encl, &enclosureIndex);		// zero if not found

		output << true << std::endl;
		output << enclosureIndex << std::endl;
		widget->StreamOut(output);

		widgetList.Append(const_cast<BaseWidget*>(widget));
	},
	true);

	output << false << std::endl;
}

/******************************************************************************
 DiscardChanges (virtual protected)

 ******************************************************************************/

void
LayoutDocument::DiscardChanges()
{
	bool onDisk;
	const JString fullName = GetFullName(&onDisk);
	assert( onDisk );

	std::ifstream input(fullName.GetBytes());
	JFileVersion vers;
	const FileStatus status = DefaultCanReadASCIIFile(input, kFileSignature, kCurrentFileVersion, &vers);
	if (status == kFileReadable)
	{
		ReadFile(input);
		DataReverted();
	}
	else
	{
		JGetUserNotification()->ReportError(JGetString("UnknownFile::LayoutDocument"));
	}
}

/******************************************************************************
 ImportFDesignFile (static private)

 ******************************************************************************/

void
LayoutDocument::ImportFDesignFile
	(
	std::istream& input
	)
{
	while (!input.eof() && !input.fail())
	{
		auto* doc = jnew LayoutDocument(JString::empty, false);
		if (doc->ImportFDesignLayout(input))
		{
			doc->DataReverted();
			doc->Activate();
		}
		else
		{
			doc->DataReverted();
			doc->Close();
		}
	}
}

/******************************************************************************
 ImportFDesignLayout

 ******************************************************************************/

// Form fields

static const JUtf8Byte* kBeginFormLine      = "=============== FORM ===============";
static const JUtf8Byte* kFormNameMarker     = "Name:";
static const JUtf8Byte* kFormWidthMarker    = "Width:";
static const JUtf8Byte* kFormHeightMarker   = "Height:";
static const JUtf8Byte* kFormObjCountMarker = "Number of Objects:";

// Object fields

static const JUtf8Byte* kBeginObjLine      = "--------------------";
static const JUtf8Byte* kObjClassMarker    = "class:";
static const JUtf8Byte* kObjTypeMarker     = "type:";
static const JUtf8Byte* kObjRectMarker     = "box:";
static const JUtf8Byte* kObjBoxTypeMarker  = "boxtype:";
static const JUtf8Byte* kObjColorsMarker   = "colors:";
static const JUtf8Byte* kObjLAlignMarker   = "alignment:";
static const JUtf8Byte* kObjLStyleMarker   = "style:";
static const JUtf8Byte* kObjLSizeMarker    = "size:";
static const JUtf8Byte* kObjLColorMarker   = "lcol:";
static const JUtf8Byte* kObjLabelMarker    = "label:";
static const JUtf8Byte* kObjShortcutMarker = "shortcut:";
static const JUtf8Byte* kObjGravityMarker  = "gravity:";
static const JUtf8Byte* kObjNameMarker     = "name:";
static const JUtf8Byte* kObjCBArgMarker    = "argument:";

bool
LayoutDocument::ImportFDesignLayout
	(
	std::istream& input
	)
{
	JString s;
	do
	{
		s = JReadLine(input);
	}
		while (s != kBeginFormLine && !input.eof() && !input.fail());

	if (input.eof() || input.fail())
	{
		return false;
	}

	// name

	s = ReadFDesignString(input, kFormNameMarker);
	FileChanged(s, false);

	// window size

	SetLayoutSize(
		ReadFDesignNumber(input, kFormWidthMarker),
		ReadFDesignNumber(input, kFormHeightMarker));

	// object count (marker contains whitespace)

	JString flClass, flType, boxType, color1, color2,
		label, labelAlign, labelStyle, labelSize, labelColor,
		shortcuts, gravity, varName, argument, className, argList;
	JArray<JRect> rectList;
	JPtrArray<JString> objNames(JPtrArrayT::kDeleteAll), tmp(JPtrArrayT::kDeleteAll);
	JPtrArray<BaseWidget> widgetList(JPtrArrayT::kForgetAll);

	const JSize itemCount = ReadFDesignNumber(input, kFormObjCountMarker);
	for (JIndex i=1; i<=itemCount; i++)
	{
		// check for start-of-object

		input >> std::ws;
		s = JReadLine(input);
		assert( s == kBeginObjLine );

		flClass = ReadFDesignString(input, kObjClassMarker);
		flType  = ReadFDesignString(input, kObjTypeMarker);

		JCoordinate x = ReadFDesignNumber(input, kObjRectMarker);
		JCoordinate y,w,h;
		input >> y >> w >> h;

		boxType = ReadFDesignString(input, kObjBoxTypeMarker);
		if (flClass == "FL_BOX")	// for actual boxes, use boxType instead of type
		{
			flType = boxType;
		}

		s = ReadFDesignString(input, kObjColorsMarker);
		s.Split(" ", &tmp);
		assert( tmp.GetItemCount() == 2 );
		color1 = *tmp.GetItem(1);
		color2 = *tmp.GetItem(2);

		labelAlign = ReadFDesignString(input, kObjLAlignMarker);
		labelStyle = ReadFDesignString(input, kObjLStyleMarker);
		labelSize  = ReadFDesignString(input, kObjLSizeMarker);
		labelSize  = ReadFDesignString(input, kObjLColorMarker);
		label      = ReadFDesignString(input, kObjLabelMarker);

		shortcuts = ReadFDesignString(input, kObjShortcutMarker);

		JIgnoreLine(input);		// resizing

		s = ReadFDesignString(input, kObjGravityMarker);
		s.Split(" ", &tmp);
		gravity = *tmp.GetItem(1);

		varName = ReadFDesignString(input, kObjNameMarker);

		JIgnoreLine(input);		// callback

		argument = ReadFDesignString(input, kObjCBArgMarker);

		do
		{
			s = JReadLine(input);
			s.TrimWhitespace();
		}
			while (!s.IsEmpty());

		// don't bother to generate code for initial box
		// if it is FL_BOX, FL_FLAT_BOX, FL_COL1

		if (i == 1 && flClass == "FL_BOX" && flType == "FL_FLAT_BOX" && color1 == "FL_COL1")
		{
			continue;
		}

		bool varIsInstance = false;
		if (varName.IsEmpty())
		{
			varName = GetTempFDesignVarName(objNames);
		}
		else if (varName.GetFirstCharacter() == '(' &&
				  varName.GetLastCharacter()  == ')')
		{
			JStringIterator iter(&varName);
			iter.SkipNext();
			iter.RemoveAllPrev();
			iter.MoveTo(JStringIterator::kStartAtEnd, 0);
			iter.SkipPrev();
			iter.RemoveAllNext();
		}
		else
		{
			varIsInstance = true;
		}

		JRect frame(y, x, y+h, x+w);
		JIndex enclIndex;
		JXWidget* enclosure;
		JRect localFrame = frame;
		if (GetFDesignEnclosure(frame, rectList, &enclIndex))
		{
			enclosure = widgetList.GetItem(enclIndex);

			const JRect enclFrame = rectList.GetItem(enclIndex);
			localFrame.Shift(-enclFrame.topLeft());
		}
		else
		{
			enclosure = itsLayoutContainer;
		}

		JXWidget::HSizingOption hS;
		JXWidget::VSizingOption vS;
		ParseFDesignGravity(gravity, &hS, &vS);

		x = localFrame.left;
		y = localFrame.top;
		w = localFrame.width();
		h = localFrame.height();

		BaseWidget* widget;
		if (flClass == "FL_BUTTON")
		{
			widget = jnew TextButton(this, label, enclosure, hS,vS, x,y,w,h);

			if (flType == "FL_RETURN_BUTTON")
			{
//				widget->SetShortcuts("^M");
			}
		}
		else // if (flClass == "FL_BOX" && flType == "FL_NO_BOX" && !label->IsEmpty())
		{
			SplitFDesignClassNameAndArgs(label, &className, &argList);

			widget = jnew CustomWidget(this, className, argList, false,
										enclosure, hS,vS, x,y,w,h);
		}

		if (widget != nullptr)
		{
			widget->SetVarName(varName, varIsInstance);

			rectList.AppendItem(frame);
			objNames.Append(varName);
			widgetList.Append(widget);
		}
	}

	return true;
}

/******************************************************************************
 ReadFDesignString (static private)

 ******************************************************************************/

JString
LayoutDocument::ReadFDesignString
	(
	std::istream&		input,
	const JUtf8Byte*	marker
	)
{
	JString s;
	input >> std::ws;
	s = JReadUntil(input, ':') + ":";
	assert( s == marker );

	s = JReadLine(input);
	s.TrimWhitespace();
	return s;
}

/******************************************************************************
 ReadFDesignNumber (static private)

 ******************************************************************************/

JCoordinate
LayoutDocument::ReadFDesignNumber
	(
	std::istream&		input,
	const JUtf8Byte*	marker
	)
{
	JString s;
	input >> std::ws;
	s = JReadUntil(input, ':') + ":";
	assert( s == marker );

	JCoordinate v;
	input >> v;
	return v;
}

/******************************************************************************
 ParseFDesignGravity (static private)

 ******************************************************************************/

struct GravityMap
{
	const JUtf8Byte* gravity;
	const JXWidget::HSizingOption hSizing;
	const JXWidget::VSizingOption vSizing;
};

static const GravityMap kGravityMap[] =
{
	{ "FL_NoGravity", JXWidget::kHElastic,   JXWidget::kVElastic    },	// default
	{ "FL_North",     JXWidget::kHElastic,   JXWidget::kFixedTop    },
	{ "FL_NorthEast", JXWidget::kFixedRight, JXWidget::kFixedTop    },
	{ "FL_East",      JXWidget::kFixedRight, JXWidget::kVElastic    },
	{ "FL_SouthEast", JXWidget::kFixedRight, JXWidget::kFixedBottom },
	{ "FL_South",     JXWidget::kHElastic,   JXWidget::kFixedBottom },
	{ "FL_SouthWest", JXWidget::kFixedLeft,  JXWidget::kFixedBottom },
	{ "FL_West",      JXWidget::kFixedLeft,  JXWidget::kVElastic    },
	{ "FL_NorthWest", JXWidget::kFixedLeft,  JXWidget::kFixedTop    }
};

const JSize kGravityCount = sizeof(kGravityMap) / sizeof(GravityMap);

void
LayoutDocument::ParseFDesignGravity
	(
	const JString&				gravity,
	JXWidget::HSizingOption*	hSizing,
	JXWidget::VSizingOption*	vSizing
	)
{
	for (JUnsignedOffset i=0; i<kGravityCount; i++)
	{
		if (gravity == kGravityMap[i].gravity)
		{
			*hSizing = kGravityMap[i].hSizing;
			*vSizing = kGravityMap[i].vSizing;
			return;
		}
	}

	*hSizing = kGravityMap[0].hSizing;
	*vSizing = kGravityMap[0].vSizing;
}

/******************************************************************************
 GetTempFDesignVarName (static private)

	Return a variable name that is not in the given list.

	We ignore the possibility of not finding a valid name because the
	code we are writing will run out of memory long before we run out
	of possibilities.

 ******************************************************************************/

JString
LayoutDocument::GetTempFDesignVarName
	(
	const JPtrArray<JString>& objNames
	)
{
	JString varName;
	for (JIndex i=1; i<=INT_MAX; i++)
	{
		varName = "obj" + JString((JUInt64) i);

		bool unique = true;
		for (const auto* usedName : objNames)
		{
			if (varName == *usedName)
			{
				unique = false;
				break;
			}
		}
		if (unique)
		{
			break;
		}
	}

	return varName;
}

/******************************************************************************
 GetFDesignEnclosure

	Returns true if it finds a rectangle that encloses the rectangle
	at the specified index.  If it finds more than one enclosing rectangle,
	it returns the smallest one.

	If no enclosure is found, returns false, and sets *enclIndex to zero.

 ******************************************************************************/

bool
LayoutDocument::GetFDesignEnclosure
	(
	const JRect&			frame,
	const JArray<JRect>&	rectList,
	JIndex*					enclIndex
	)
{
	bool found    = false;
	*enclIndex    = 0;
	JSize minArea = 0;

	const JSize count = rectList.GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		const JRect r = rectList.GetItem(i);
		const JSize a = r.area();
		if (r.Contains(frame) && (a < minArea || minArea == 0))
		{
			minArea    = a;
			found      = true;
			*enclIndex = i;
		}
	}

	return found;
}

/******************************************************************************
 SplitFDesignClassNameAndArgs (static private)

	Returns false if there is no class name.

 ******************************************************************************/

void
LayoutDocument::SplitFDesignClassNameAndArgs
	(
	const JString&	str,
	JString*		name,
	JString*		args
	)
{
	JPtrArray<JString> list(JPtrArrayT::kDeleteAll);
	str.Split("(", &list, 2);

	const bool hasArgs = list.GetItemCount() > 1;
	if (hasArgs &&
		!list.GetFirstItem()->IsEmpty() &&
		!list.GetLastItem()->IsEmpty())
	{
		*name = *list.GetFirstItem();
		*args = *list.GetLastItem();

		name->TrimWhitespace();
		args->TrimWhitespace();
		return;
	}

	if (hasArgs)
	{
		*name = *list.GetFirstItem();
	}
	else
	{
		*name = str;
	}
	name->TrimWhitespace();
	args->Clear();
}

/******************************************************************************
 SetLayoutSize

 ******************************************************************************/

void
LayoutDocument::SetLayoutSize
	(
	const JCoordinate w,
	const JCoordinate h
	)
{
	GetWindow()->AdjustSize(
		w - itsLayoutContainer->GetApertureWidth(),
		h - itsLayoutContainer->GetApertureHeight());
}

/******************************************************************************
 GetSelectedWidgets

 ******************************************************************************/

void
LayoutDocument::GetSelectedWidgets
	(
	JPtrArray<BaseWidget>* list
	)
	const
{
	list->CleanOut();
	list->SetCleanUpAction(JPtrArrayT::kForgetAll);

	itsLayoutContainer->ForEach([&list](JXContainer* obj)
	{
		BaseWidget* widget = dynamic_cast<BaseWidget*>(obj);
		if (widget != nullptr && widget->IsSelected())
		{
			list->Append(widget);
		}
	},
	true);
}

/******************************************************************************
 SelectAllWidgets

 ******************************************************************************/

void
LayoutDocument::SelectAllWidgets()
{
	itsLayoutContainer->ForEach([](JXContainer* obj)
	{
		BaseWidget* widget = dynamic_cast<BaseWidget*>(obj);
		if (widget != nullptr)
		{
			widget->SetSelected(true);
		}
	},
	true);
}

/******************************************************************************
 ClearSelection

 ******************************************************************************/

void
LayoutDocument::ClearSelection()
{
	itsLayoutContainer->ForEach([](JXContainer* obj)
	{
		BaseWidget* widget = dynamic_cast<BaseWidget*>(obj);
		if (widget != nullptr)
		{
			widget->SetSelected(false);
		}
	},
	true);
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
LayoutDocument::UpdateFileMenu()
{
	itsFileMenu->SetItemEnabled(kSaveCmd, NeedsSave());
	itsFileMenu->SetItemEnabled(kRevertCmd, CanRevert());
	itsFileMenu->SetItemEnabled(kSaveAllCmd, JXGetDocumentManager()->DocumentsNeedSave());

	itsFileMenu->SetItemEnabled(kShowInFileMgrCmd, ExistsOnDisk());
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
LayoutDocument::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kNewCmd)
	{
		LayoutDocument* doc;
		Create(&doc);
	}
	else if (index == kOpenCmd)
	{
		MDIServer::ChooseFiles();
	}

	else if (index == kSaveCmd)
	{
		SaveInCurrentFile();
	}
	else if (index == kSaveAsCmd)
	{
		bool onDisk;
		const JString fullName = GetFullName(&onDisk);
		if (onDisk)
		{
			GetDocumentManager()->AddToFileHistoryMenu(fullName);
		}

		SaveInNewFile();
	}
	else if (index == kSaveCopyAsCmd)
	{
		JString fullName;
		if (SaveCopyInNewFile(JString::empty, &fullName))
		{
			GetDocumentManager()->AddToFileHistoryMenu(fullName);
		}
	}
	else if (index == kRevertCmd)
	{
		RevertToSaved();
	}
	else if (index == kSaveAllCmd)
	{
		JXGetDocumentManager()->SaveAllFileDocuments(true);
	}

	else if (index == kShowInFileMgrCmd)
	{
		bool onDisk;
		const JString fullName = GetFullName(&onDisk);
		assert( onDisk );
		JXGetWebBrowser()->ShowFileLocation(fullName);
	}

	else if (index == kCloseCmd)
	{
		Close();
	}

	else if (index == kQuitCmd)
	{
		GetApplication()->Quit();
	}
}

/******************************************************************************
 UpdateEditMenu (private)

 ******************************************************************************/

void
LayoutDocument::UpdateEditMenu()
{
	itsEditMenu->EnableItem(kSelectAllCmd);
}

/******************************************************************************
 HandleEditMenu (private)

 ******************************************************************************/

void
LayoutDocument::HandleEditMenu
	(
	const JIndex index
	)
{
	if (index == kUndoCmd)
	{
	}
	else if (index == kRedoCmd)
	{
	}

	else if (index == kCutCmd)
	{
	}
	else if (index == kCopyCmd)
	{
	}
	else if (index == kPasteCmd)
	{
	}
	else if (index == kClearCmd)
	{
	}

	else if (index == kSelectAllCmd)
	{
		SelectAllWidgets();
	}
}

/******************************************************************************
 HandlePrefsMenu (private)

 ******************************************************************************/

void
LayoutDocument::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kEditToolBarCmd)
	{
		itsToolBar->Edit();
	}
	else if (index == kFilePrefsCmd)
	{
		JXGetWebBrowser()->EditPrefs();
	}
	else if (index == kEditMacWinPrefsCmd)
	{
		JXMacWinPrefsDialog::EditPrefs();
	}
}
