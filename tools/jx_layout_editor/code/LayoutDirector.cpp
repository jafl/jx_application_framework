/******************************************************************************
 LayoutDirector.cpp

	BASE CLASS = public JXWindowDirector

	Copyright (C) 2023 by John Lindal.

 *****************************************************************************/

#include "LayoutDirector.h"
#include "MainDocument.h"
#include "BaseWidget.h"
#include "CustomWidget.h"
#include "TextButton.h"
#include "globals.h"
#include "actionDefs.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXToolBar.h>
#include <jx-af/jx/JXBorderRect.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXGetStringDialog.h>
#include <jx-af/jx/JXWebBrowser.h>
#include <jx-af/jx/JXMacWinPrefsDialog.h>
#include <jx-af/jcore/JPtrArray-JString.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JColorManager.h>
#include <jx-af/jcore/jStreamUtil.h>
#include <jx-af/jcore/jAssert.h>

const JFileVersion kCurrentDataVersion = 0;		// remember to increment MainDocument version

// File menu

static const JUtf8Byte* kFileMenuStr =
	"    Edit name...           %i" kEditLayoutNameAction
	"  | Close        %k Meta-W %i" kJXCloseWindowAction;

enum
{
	kEditLayoutNameCmd = 1,
	kCloseCmd
};

// Preferences menu

static const JUtf8Byte* kPrefsMenuStr =
	"    Edit tool bar..."
	"  | File manager & web browser..."
	"  | Mac/Win/X emulation...";

enum
{
	kEditToolBarCmd = 1,
	kWebBrowserCmd,
	kEditMacWinPrefsCmd
};

/******************************************************************************
 Constructor

 *****************************************************************************/

LayoutDirector::LayoutDirector
	(
	MainDocument* doc
	)
	:
	JXWindowDirector(doc),
	itsDoc(doc),
	itsLayoutName(JGetString("DefaultLayoutName::LayoutDirector"))
{
	BuildWindow();
	UpdateWindowTitle();
}

LayoutDirector::LayoutDirector
	(
	MainDocument*	doc,
	std::istream&	input
	)
	:
	JXWindowDirector(doc),
	itsDoc(doc)
{
	BuildWindow();
	ReadLayout(input);
	UpdateWindowTitle();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

LayoutDirector::~LayoutDirector()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "layout_window_icon.xpm"

void
LayoutDirector::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 500,300, JString::empty);

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 500,30);
	assert( menuBar != nullptr );

	itsToolBar =
		jnew JXToolBar(GetPrefsManager(), kLayoutDirToolBarID, menuBar, window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 500,270);
	assert( itsToolBar != nullptr );

// end JXLayout

	window->SetWMClass(GetWMClassInstance(), GetLayoutDirectorClass());
	window->SetMinSize(200, 100);

	JXImage* image = jnew JXImage(GetDisplay(), layout_window_icon);
	assert( image != nullptr );
	window->SetIcon(image);

	itsLayoutContainer =
		jnew JXBorderRect(itsToolBar->GetWidgetEnclosure(),
						  JXWidget::kHElastic,JXWidget::kVElastic,
						  0,0, 100,100);
	itsLayoutContainer->FitToEnclosure();
	itsLayoutContainer->SetBorderWidth(10);

	// menus

	itsFileMenu = menuBar->AppendTextMenu(JGetString("FileMenuTitle::JXGlobal"));
	itsFileMenu->SetMenuItems(kFileMenuStr, "LayoutDirector");
	itsFileMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsFileMenu->AttachHandlers(this,
		&LayoutDirector::UpdateFileMenu,
		&LayoutDirector::HandleFileMenu);

	itsPrefsMenu = menuBar->AppendTextMenu(JGetString("PrefsMenuTitle::JXGlobal"));
	itsPrefsMenu->SetMenuItems(kPrefsMenuStr, "LayoutDirector");
	itsPrefsMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsPrefsMenu->AttachHandler(this, &LayoutDirector::HandlePrefsMenu);

	JXTextMenu* helpMenu = GetApplication()->CreateHelpMenu(menuBar, "LayoutDirector", "MainHelp");

	// must be done after creating widgets

	itsToolBar->LoadPrefs();
	if (itsToolBar->IsEmpty())
	{
		itsToolBar->AppendButton(itsFileMenu, kCloseCmd);
		GetApplication()->AppendHelpMenuToToolBar(itsToolBar, helpMenu);
	}
}

/******************************************************************************
 UpdateWindowTitle (private)

 ******************************************************************************/

void
LayoutDirector::UpdateWindowTitle()
{
	GetWindow()->SetTitle(itsDoc->GetName() + ": " + itsLayoutName);
}

/******************************************************************************
 ReadLayout (private)

 ******************************************************************************/

void
LayoutDirector::ReadLayout
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	assert( vers <= kCurrentDataVersion );

	input >> itsLayoutName;
	GetWindow()->SetTitle(itsLayoutName);

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
 WriteLayout

 ******************************************************************************/

void
LayoutDirector::WriteLayout
	(
	std::ostream& output
	)
	const
{
	output << kCurrentDataVersion << std::endl;
	output << itsLayoutName << std::endl;
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
LayoutDirector::ImportFDesignLayout
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

	itsLayoutName = ReadFDesignString(input, kFormNameMarker);
	GetWindow()->SetTitle(itsLayoutName);

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
LayoutDirector::ReadFDesignString
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
LayoutDirector::ReadFDesignNumber
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
LayoutDirector::ParseFDesignGravity
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
LayoutDirector::GetTempFDesignVarName
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
LayoutDirector::GetFDesignEnclosure
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
LayoutDirector::SplitFDesignClassNameAndArgs
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
 DataModified

 ******************************************************************************/

void
LayoutDirector::DataModified()
{
	itsDoc->DataModified();
}

/******************************************************************************
 SetLayoutSize

 ******************************************************************************/

void
LayoutDirector::SetLayoutSize
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
LayoutDirector::GetSelectedWidgets
	(
	JPtrArray<BaseWidget>* list
	)
	const
{
	list->CleanOut();

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
 ClearSelection

 ******************************************************************************/

void
LayoutDirector::ClearSelection()
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
LayoutDirector::UpdateFileMenu()
{
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
LayoutDirector::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kEditLayoutNameCmd)
	{
		auto* dlog = jnew JXGetStringDialog(
			JGetString("EditLayoutNameWindowTitle::LayoutDirector"),
			JGetString("EditLayoutNamePrompt::LayoutDirector"),
			itsLayoutName);

		if (dlog->DoDialog())
		{
			itsLayoutName = dlog->GetString();
			UpdateWindowTitle();
		}
	}

	else if (index == kCloseCmd)
	{
		Close();
	}
}

/******************************************************************************
 HandlePrefsMenu (private)

 ******************************************************************************/

void
LayoutDirector::HandlePrefsMenu
	(
	const JIndex index
	)
{
	if (index == kEditToolBarCmd)
	{
		itsToolBar->Edit();
	}
	else if (index == kWebBrowserCmd)
	{
		JXGetWebBrowser()->EditPrefs();
	}
	else if (index == kEditMacWinPrefsCmd)
	{
		JXMacWinPrefsDialog::EditPrefs();
	}
}
