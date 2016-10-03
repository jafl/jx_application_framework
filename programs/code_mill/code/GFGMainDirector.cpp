/******************************************************************************
 GFGMainDirector.cpp

	<Description>

	BASE CLASS = public JXWindowDirector

	Copyright (C) 2002 by New Planet Software. All rights reserved.

 *****************************************************************************/

#include "GFGMainDirector.h"
#include "GFGApp.h"
#include "GFGPrefsManager.h"
#include "GFGClass.h"
#include "GFGFunctionTable.h"

#include "gfgHelpText.h"
#include "gfgGlobals.h"
#include "gfgActionDefs.h"

#include <JXChooseSaveFile.h>
#include <JXColHeaderWidget.h>
#include <JXColormap.h>
#include <JXHelpManager.h>
#include <JXImage.h>
#include <JXInputField.h>
#include <JXMenuBar.h>
#include <JXPathInput.h>
#include <JXScrollbarSet.h>
#include <JXStaticText.h>
#include <JXTextButton.h>
#include <JXTextMenu.h>
#include <JXToolBar.h>
#include <JXWindow.h>

#include <jDirUtil.h>
#include <jFileUtil.h>
#include <jProcessUtil.h>
#include <jFStreamUtil.h>
#include <jAssert.h>

const JCoordinate kColHeaderHeight		= 20;
const JCoordinate kGapHeight			= 40;
const JFileVersion kCurrentPrefsVersion	= 1;

const JCharacter* kUseLabel		= "Override";
const JCharacter* kRTLabel		= "Return Type";
const JCharacter* kFNLabel		= "Function Name";
const JCharacter* kConstLabel	= "Const";
const JCharacter* kArgsLabel	= "Arguments";

static const JCharacter* kFileMenuTitleStr = "File";
static const JCharacter* kFileMenuStr =
	"Quit %k Meta-Q %i" kJXQuitAction;

enum
{
	kQuitCmd = 1
};

static const JCharacter* kPrefsMenuTitleStr = "Preferences";
static const JCharacter* kPrefsMenuStr =
	"    Edit preferences  %i" kGFGEditPreferencesAction
	"  | Edit tool bar...  %i" kGFGEditToolBarAction;

enum
{
	kPrefsCmd = 1,
	kEditToolBarCmd
};

static const JCharacter* kHelpMenuTitleStr = "Help";
static const JCharacter* kHelpMenuStr =
	"    About"
	"%l| Table of Contents       %i" kJXHelpTOCAction
	"  | Overview"
	"  | This window       %k F1 %i" kJXHelpSpecificAction
	"%l| Changes"
	"  | Credits";

enum
{
	kAboutCmd = 1,
	kTOCCmd,
	kOverviewCmd,
	kThisWindowCmd,
	kChangesCmd,
	kCreditsCmd
};

/******************************************************************************
 Constructor

 *****************************************************************************/

GFGMainDirector::GFGMainDirector
	(
	JXDirector* supervisor,
	const JPtrArray<JString>& files
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(GFGGetPrefsManager(), kGFGMainDirectorID)
{
	itsClass	= new GFGClass();
	assert(itsClass != NULL);

	JString outputPath;

	const JSize count	= files.GetElementCount();
	JString classname, filename;
	for (JIndex i = 1; i <= count; i++)
		{
		const JString* argName = files.NthElement(i);
		if (*argName == "--output_path" && i < count)
			{
			outputPath = *(files.NthElement(i+1));
			i++;
			}
		else if (!argName->BeginsWith("-"))
			{
			ifstream is(*argName);
			if (is.good())
				{
				int version;
				is >> version;
				if (version == 0)
					{
					JSize count;
					is >> count;
					for (JIndex i = 1; i <= count; i++)
						{
						is >> classname >> filename;
						if (i == 1)
							{
							itsClass->AddBaseClass(classname, filename);
							}
						else
							{
							itsClass->AddAncestor(classname, filename);
							}
						}
					}
				}
			}
		}

	itsClass->Populate();

	BuildWindow(outputPath);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GFGMainDirector::~GFGMainDirector()
{
	JPrefObject::WritePrefs();
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

#include "gfg_main_window_icon.xpm"
#include <jx_help_specific.xpm>
#include <jx_help_toc.xpm>

void
GFGMainDirector::BuildWindow
	(
	const JCharacter* outputPath
	)
{
	const JCoordinate minWidth  = 200;
	const JCoordinate minHeight = 200;

	const JCoordinate w		= 650;
	const JCoordinate h		= 400;

// begin JXLayout

	JXWindow* window = new JXWindow(this, 650,460, "");
	assert( window != NULL );

	itsClassInput =
		new JXInputField(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 95,10, 140,20);
	assert( itsClassInput != NULL );

	JXStaticText* obj1_JXLayout =
		new JXStaticText(JGetString("obj1_JXLayout::GFGMainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 15,10, 80,20);
	assert( obj1_JXLayout != NULL );
	obj1_JXLayout->SetToLabel();

	JXStaticText* obj2_JXLayout =
		new JXStaticText(JGetString("obj2_JXLayout::GFGMainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 240,10, 110,20);
	assert( obj2_JXLayout != NULL );
	obj2_JXLayout->SetToLabel();

	itsChooseButton =
		new JXTextButton(JGetString("itsChooseButton::GFGMainDirector::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 550,10, 80,20);
	assert( itsChooseButton != NULL );
	itsChooseButton->SetShortcuts(JGetString("itsChooseButton::GFGMainDirector::shortcuts::JXLayout"));

	JXScrollbarSet* scrollbarSet =
		new JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 20,75, 610,300);
	assert( scrollbarSet != NULL );

	itsGenerateButton =
		new JXTextButton(JGetString("itsGenerateButton::GFGMainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 440,425, 70,20);
	assert( itsGenerateButton != NULL );
	itsGenerateButton->SetShortcuts(JGetString("itsGenerateButton::GFGMainDirector::shortcuts::JXLayout"));

	itsHelpButton =
		new JXTextButton(JGetString("itsHelpButton::GFGMainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 285,425, 70,20);
	assert( itsHelpButton != NULL );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::GFGMainDirector::shortcuts::JXLayout"));

	itsCancelButton =
		new JXTextButton(JGetString("itsCancelButton::GFGMainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 130,425, 70,20);
	assert( itsCancelButton != NULL );
	itsCancelButton->SetShortcuts(JGetString("itsCancelButton::GFGMainDirector::shortcuts::JXLayout"));

	itsDirInput =
		new JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 350,10, 200,20);
	assert( itsDirInput != NULL );

	JXStaticText* obj3_JXLayout =
		new JXStaticText(JGetString("obj3_JXLayout::GFGMainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 15,45, 90,20);
	assert( obj3_JXLayout != NULL );
	obj3_JXLayout->SetToLabel();

	itsBaseClassTxt =
		new JXStaticText(JGetString("itsBaseClassTxt::GFGMainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 100,45, 530,20);
	assert( itsBaseClassTxt != NULL );
    const JFontStyle itsBaseClassTxt_style(kJTrue, kJFalse, 0, kJFalse, GetColormap()->GetBlackColor());
    itsBaseClassTxt->SetFontStyle(itsBaseClassTxt_style);
	itsBaseClassTxt->SetToLabel();

	JXStaticText* obj4_JXLayout =
		new JXStaticText(JGetString("obj4_JXLayout::GFGMainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 15,390, 60,20);
	assert( obj4_JXLayout != NULL );
	obj4_JXLayout->SetToLabel();

	itsAuthorInput =
		new JXInputField(window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 75,390, 160,20);
	assert( itsAuthorInput != NULL );

	itsStringsButton =
		new JXTextButton(JGetString("itsStringsButton::GFGMainDirector::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedBottom, 550,390, 80,20);
	assert( itsStringsButton != NULL );
	itsStringsButton->SetShortcuts(JGetString("itsStringsButton::GFGMainDirector::shortcuts::JXLayout"));

	JXStaticText* obj5_JXLayout =
		new JXStaticText(JGetString("obj5_JXLayout::GFGMainDirector::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedBottom, 250,390, 70,20);
	assert( obj5_JXLayout != NULL );
	obj5_JXLayout->SetToLabel();

	itsCopyrightInput =
		new JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedBottom, 320,390, 230,20);
	assert( itsCopyrightInput != NULL );

// end JXLayout

	window->SetTitle("Code Mill");
	window->LockCurrentMinSize();

	ListenTo(itsChooseButton);
	ListenTo(itsCancelButton);
	ListenTo(itsHelpButton);
	ListenTo(itsGenerateButton);
	ListenTo(itsStringsButton);

	JString cname, fname;
	JString bases;
	const JSize count	= itsClass->GetBaseClassCount();
	for (JIndex i = 1; i <= count; i++)
		{
		itsClass->GetBaseClass(i, &cname, &fname);
		if (i > 1)
			{
			bases += ", ";
			}
		bases += cname;
		}
	
	itsBaseClassTxt->SetText(bases);

	JXImage* image = new JXImage(GetDisplay(), gfg_main_window_icon);
	assert( image != NULL );
	window->SetIcon(image);

	itsTable	= 
		GFGFunctionTable::Create(itsClass, 
			scrollbarSet, scrollbarSet->GetScrollEnclosure(),
			JXWidget::kHElastic, JXWidget::kVElastic,
			0,kColHeaderHeight, 100,
			scrollbarSet->GetScrollEnclosure()->GetBoundsHeight() - kColHeaderHeight);
	assert(itsTable != NULL);

	itsTable->FitToEnclosure(kJTrue, kJFalse);

	JXColHeaderWidget* widget =
		new JXColHeaderWidget(itsTable, 
							  scrollbarSet, scrollbarSet->GetScrollEnclosure(),
							  JXWidget::kHElastic, JXWidget::kFixedTop,
							  0,0, 100,kColHeaderHeight);
	assert(widget != NULL);
	widget->FitToEnclosure(kJTrue, kJFalse);

	widget->SetColTitle(GFGFunctionTable::kFUsed, kUseLabel);
	widget->SetColTitle(GFGFunctionTable::kFReturnType, kRTLabel);
	widget->SetColTitle(GFGFunctionTable::kFFunctionName, kFNLabel);
	widget->SetColTitle(GFGFunctionTable::kFConst, kConstLabel);
	widget->SetColTitle(GFGFunctionTable::kFArgs, kArgsLabel);

	itsClassInput->SetIsRequired();
	itsDirInput->SetText(outputPath);
	itsAuthorInput->SetText(GFGGetPrefsManager()->GetAuthor());
	itsCopyrightInput->SetText(GFGGetPrefsManager()->GetCopyright());

	JPrefObject::ReadPrefs();
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GFGMainDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsChooseButton && message.Is(JXButton::kPushed))
		{
		JString path;
		if (itsDirInput != NULL &&
			JGetChooseSaveFile()->ChooseRWPath("", NULL, itsDirInput->GetText(), &path))
			{
			itsDirInput->SetText(path);
			}
		}
	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(kGFGTOCHelpName);
		}
	else if (sender == itsCancelButton && message.Is(JXButton::kPushed))
		{
		Close();
		}
	else if (sender == itsGenerateButton && message.Is(JXButton::kPushed))
		{
		GFGGetPrefsManager()->SetAuthor(itsAuthorInput->GetText());
		GFGGetPrefsManager()->SetCopyright(itsCopyrightInput->GetText());
		
		if (Write())
			{
			//Close();
			}
		}
	else if (sender == itsStringsButton && message.Is(JXButton::kPushed))
		{
		GFGGetPrefsManager()->EditPrefs();
		}
	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}



/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
GFGMainDirector::ReadPrefs
	(
	istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentPrefsVersion)
		{
		}

	if (vers >= 1)
		{
		GetWindow()->ReadGeometry(input);
		}
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
GFGMainDirector::WritePrefs
	(
	ostream& output
	)
	const
{
	output << kCurrentPrefsVersion << ' ';
	GetWindow()->WriteGeometry(output);
}

/******************************************************************************
 Write (private)

 ******************************************************************************/

JBoolean
GFGMainDirector::Write()
{
	JString cname	= itsClassInput->GetText();
	if (cname.IsEmpty())
		{
		JGetUserNotification()->ReportError("Please provide a class name.");
		itsClassInput->Focus();
		return kJFalse;
		}
	itsClass->SetClassName(cname);

	JString dir;
	if (!itsDirInput->GetPath(&dir))
		{
		JGetUserNotification()->ReportError("Please provide a valid destination directory.");
		itsDirInput->Focus();
		return kJFalse;
		}
	JString headername	= cname + ".h";
	JString sourcename	= cname + ".cc";

	JString headerfile	= JCombinePathAndName(dir, headername);
	JString sourcefile	= JCombinePathAndName(dir, sourcename);

	JBoolean exists	= kJFalse;

	if (JFileExists(headerfile) ||
		JFileExists(sourcefile))
		{
		exists		= kJTrue;

		headername	= cname + "_tmp.h";
		sourcename	= cname + "_tmp.cc";

		headerfile	= JCombinePathAndName(dir, headername);
		sourcefile	= JCombinePathAndName(dir, sourcename);
		}

	ofstream oh(headerfile);
	if (!oh.good())
		{
		JGetUserNotification()->ReportError("There was an error creating the files.");
		return kJFalse;
		}

	JString bcname, bfname;
	JString bases;
	JString basefiles;
	JString baseconstructors;
	const JSize count	= itsClass->GetBaseClassCount();
	for (JIndex i = 1; i <= count; i++)
		{
		itsClass->GetBaseClass(i, &bcname, &bfname);
		if (i > 1)
			{
			bases += ", ";
			}
		bases += "public " + bcname;

		JString path, name;
		JSplitPathAndName(bfname, &path, &name);

		basefiles += "#include <" + name + ">";
		baseconstructors += "\t" + bcname + "()";
		if (i < count)
			{
			basefiles += "\n";
			baseconstructors += ",\n";
			}
		}

	JString s	= GFGGetPrefsManager()->GetHeaderComment(cname);
	s.Print(oh);

	const JCharacter* map[] =
		{
		"class", cname,
		"basefile", basefiles,
		"base", bases
		};
	s = JGetString("CLASS_HEADER_START", map, sizeof(map));

	s.Print(oh);

	oh << "\t";
	cname.Print(oh);
	oh << "();" << endl;

	oh << "\tvirtual ~";
	cname.Print(oh);
	oh << "();" << endl << endl;

	itsClass->WritePublic(oh, kJTrue);

	oh << endl << endl;
	oh << "protected:" << endl << endl;

	itsClass->WriteProtected(oh, kJTrue);

	oh << endl << endl << "private:" << endl << endl;

	oh << "\t";
	cname.Print(oh);
	oh << "(const ";
	cname.Print(oh);
	oh << "& source);" << endl;

	oh << "\tconst ";
	cname.Print(oh);
	oh << "& operator=(const ";
	cname.Print(oh);
	oh << "& source);" << endl;

	oh << endl << endl << "};" << endl << endl << "#endif" << endl;

	oh.close();

	ofstream os(sourcefile);
	if (!os.good())
		{
		JGetUserNotification()->ReportError("There was an error creating the files.");
		return kJFalse;
		}

	s	= GFGGetPrefsManager()->GetSourceComment(cname, bases);
	s.Print(os);

	os << endl;
	os << "#include <";
	cname.Print(os);
	os << ".h>" << endl << endl;

	s	= GFGGetPrefsManager()->GetConstructorComment();
	s.Print(os);

	const JCharacter* map2[] =
		{
		"class", cname,
		"constructs", baseconstructors
		};
		
	s = JGetString("CLASS_CONSTRUCTOR_DATA", map2, sizeof(map2));

	s.Print(os);

	s	= GFGGetPrefsManager()->GetDestructorComment();
	s.Print(os);

	const JCharacter* map3[] =
		{
		"class", cname,
		"constructs", baseconstructors
		};
		
	s = JGetString("CLASS_DESTRUCTOR_DATA", map3, sizeof(map3));

	s.Print(os);

	itsClass->WritePublic(os);
	itsClass->WriteProtected(os);

	os.close();

	JString cmd	= "jcc " + headerfile + " " + sourcefile;

	JString errStr;
	JRunProgram(cmd, &errStr);
	
	return kJTrue;
}
