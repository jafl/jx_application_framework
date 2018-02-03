/******************************************************************************
 JXWMainDialog.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#include "JXWMainDialog.h"
#include "jxwGlobals.h"
#include "jxwHelpText.h"
#include <JXApplication.h>
#include <JXWindow.h>
#include <JXTextButton.h>
#include <JXStaticText.h>
#include <JXPathInput.h>
#include <JXTextCheckbox.h>
#include <JXPathHistoryMenu.h>
#include <JXHelpManager.h>
#include <JRegex.h>
#include <JDirInfo.h>
#include <jProcessUtil.h>
#include <jFStreamUtil.h>
#include <jFileUtil.h>
#include <jVCSUtil.h>
#include <jAssert.h>

const JSize kHistoryLength = 20;

static const JCharacter* kDefaultURLText     = "http://";
static const JCharacter* kDefaultProjDir     = JX_PATH "programs/my_project/";
static const JCharacter* kDefaultTemplateDir = JX_PATH "programs/jx_project_wizard/app_template/";
static const JCharacter* kDefaultOpenCmd     = "jcc $f";

static const JCharacter* kFilePrefixTag   = "_pre_";
static const JCharacter* kBinaryPrefixTag = "_Binary_";

const JSize kFilePrefixTagLength   = 5;
const JSize kBinaryPrefixTagLength = 8;

static const JCharacter* kTag[] =
{
	"<pre>",
	"<Program>",
	"<Version>",
	"<Author>",
	"<Company>",
	"<Year>",
	"<EMail>",
	"<URL>",
	"<MDI_DEF>",
	"<Binary>",
	"<PROJ_DIR>",
	"<JX_PATH>"
};

const JSize kPrefixTagLength = 5;

enum
{
	kPrefix,
	kProgram,
	kVersion,
	kAuthor,
	kCompany,
	kYear,
	kEmail,
	kURL,
	kMDI,
	kBinary,
	kProjDir,
	kJXPath,

	kTagCount
};

const JFileVersion kCurrentPrefsVersion	= 0;

// string ID's

static const JCharacter* kWindowTitleID           = "WindowTitle::JXWMainDialog";
static const JCharacter* kSrcPrefixErrorID        = "SrcPrefixError::JXWMainDialog";
static const JCharacter* kUnableToCreateProjDirID = "UnableToCreateProjDir::JXWMainDialog";
static const JCharacter* kInvalidTmplDirErrorID   = "InvalidTmplDirError::JXWMainDialog";

/******************************************************************************
 Constructor

 ******************************************************************************/

JXWMainDialog::JXWMainDialog
	(
	JXDirector*		supervisor,
	const JSize		argc,
	char**			argv
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(JXWGetPrefsManager(), kJXWMainDlogID)
{
	BuildWindow(argc, argv);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXWMainDialog::~JXWMainDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
JXWMainDialog::BuildWindow
	(
	const JSize		argc,
	char**			argv
	)
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 500,370, "");
	assert( window != NULL );

	itsCreateButton =
		jnew JXTextButton(JGetString("itsCreateButton::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 360,340, 60,20);
	assert( itsCreateButton != NULL );
	itsCreateButton->SetShortcuts(JGetString("itsCreateButton::JXWMainDialog::shortcuts::JXLayout"));

	itsQuitButton =
		jnew JXTextButton(JGetString("itsQuitButton::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 90,340, 60,20);
	assert( itsQuitButton != NULL );

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 225,340, 60,20);
	assert( itsHelpButton != NULL );
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::JXWMainDialog::shortcuts::JXLayout"));

	JXStaticText* nameLabel =
		jnew JXStaticText(JGetString("nameLabel::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,20, 120,20);
	assert( nameLabel != NULL );
	nameLabel->SetToLabel();

	JXStaticText* versionLabel =
		jnew JXStaticText(JGetString("versionLabel::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,40, 120,20);
	assert( versionLabel != NULL );
	versionLabel->SetToLabel();

	itsProgramName =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,20, 350,20);
	assert( itsProgramName != NULL );

	itsProgramVersion =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,40, 350,20);
	assert( itsProgramVersion != NULL );

	JXStaticText* authorLabel =
		jnew JXStaticText(JGetString("authorLabel::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 120,20);
	assert( authorLabel != NULL );
	authorLabel->SetToLabel();

	itsDevName =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,70, 350,20);
	assert( itsDevName != NULL );

	JXStaticText* emailLabel =
		jnew JXStaticText(JGetString("emailLabel::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,90, 120,20);
	assert( emailLabel != NULL );
	emailLabel->SetToLabel();

	itsDevEmail =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,90, 350,20);
	assert( itsDevEmail != NULL );

	JXStaticText* urlLabel =
		jnew JXStaticText(JGetString("urlLabel::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,130, 120,20);
	assert( urlLabel != NULL );
	urlLabel->SetToLabel();

	JXStaticText* srcPrefixLabel =
		jnew JXStaticText(JGetString("srcPrefixLabel::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,180, 120,20);
	assert( srcPrefixLabel != NULL );
	srcPrefixLabel->SetToLabel();

	itsNeedsMDICB =
		jnew JXTextCheckbox(JGetString("itsNeedsMDICB::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,210, 130,20);
	assert( itsNeedsMDICB != NULL );
	itsNeedsMDICB->SetShortcuts(JGetString("itsNeedsMDICB::JXWMainDialog::shortcuts::JXLayout"));

	JXStaticText* directoryLabel =
		jnew JXStaticText(JGetString("directoryLabel::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,240, 120,20);
	assert( directoryLabel != NULL );
	directoryLabel->SetToLabel();

	itsChooseProjDirButton =
		jnew JXTextButton(JGetString("itsChooseProjDirButton::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 420,240, 60,20);
	assert( itsChooseProjDirButton != NULL );

	JXStaticText* templateLabel =
		jnew JXStaticText(JGetString("templateLabel::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,270, 120,20);
	assert( templateLabel != NULL );
	templateLabel->SetToLabel();

	itsChooseTmplDirButton =
		jnew JXTextButton(JGetString("itsChooseTmplDirButton::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 390,270, 60,20);
	assert( itsChooseTmplDirButton != NULL );

	JXStaticText* programNameLabel =
		jnew JXStaticText(JGetString("programNameLabel::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,160, 120,20);
	assert( programNameLabel != NULL );
	programNameLabel->SetToLabel();

	JXStaticText* companyLabel =
		jnew JXStaticText(JGetString("companyLabel::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,110, 120,20);
	assert( companyLabel != NULL );
	companyLabel->SetToLabel();

	itsCompanyName =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,110, 350,20);
	assert( itsCompanyName != NULL );

	itsDevURL =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,130, 350,20);
	assert( itsDevURL != NULL );

	itsBinaryName =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,160, 350,20);
	assert( itsBinaryName != NULL );

	itsSrcPrefix =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,180, 350,20);
	assert( itsSrcPrefix != NULL );

	itsProjectDir =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,240, 290,20);
	assert( itsProjectDir != NULL );

	itsTemplateDir =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,270, 260,20);
	assert( itsTemplateDir != NULL );

	itsTmplDirHistory =
		jnew JXPathHistoryMenu(kHistoryLength, "", window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 450,270, 30,20);
	assert( itsTmplDirHistory != NULL );

	JXStaticText* openCmdLabel =
		jnew JXStaticText(JGetString("openCmdLabel::JXWMainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,300, 120,20);
	assert( openCmdLabel != NULL );
	openCmdLabel->SetToLabel();

	itsOpenCmd =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,300, 350,20);
	assert( itsOpenCmd != NULL );

// end JXLayout

	const JCharacter* map[] =
		{
		"vers", JXWGetVersionNumberStr()
		};
	const JString title = JGetString(kWindowTitleID, map, sizeof(map));
	window->SetTitle(title);
	window->ShouldFocusWhenShow(kJTrue);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();

	ListenTo(itsCreateButton);
	ListenTo(itsQuitButton);
	ListenTo(itsHelpButton);
	ListenTo(itsChooseProjDirButton);
	ListenTo(itsChooseTmplDirButton);
	ListenTo(itsTmplDirHistory);

	itsProgramVersion->SetText("1.0.0");
	itsDevURL->SetText(kDefaultURLText);
	itsNeedsMDICB->SetState(kJTrue);

	itsProjectDir->ShouldAllowInvalidPath();
	itsProjectDir->ShouldRequireWritable();
	itsProjectDir->SetText(kDefaultProjDir);

	itsTemplateDir->ShouldAllowInvalidPath();
	itsTemplateDir->SetText(kDefaultTemplateDir);

	itsOpenCmd->SetText(kDefaultOpenCmd);

	JPrefObject::ReadPrefs();

	// after read prefs to overwrite itsProjectDir

	if (argc == 3)
		{
		itsProjectDir->SetText(argv[1]);
		itsProgramName->SetText(argv[2]);
		itsBinaryName->SetText(argv[2]);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
JXWMainDialog::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsCreateButton && message.Is(JXButton::kPushed))
		{
		if (WriteTemplate())
			{
			JPrefObject::WritePrefs();
			(JXGetApplication())->Quit();
			}
		}
	else if (sender == itsQuitButton && message.Is(JXButton::kPushed))
		{
		(JXGetApplication())->Quit();
		}
	else if (sender == itsHelpButton && message.Is(JXButton::kPushed))
		{
		(JXGetHelpManager())->ShowSection(kJXWMainHelpName);
		}

	else if (sender == itsChooseProjDirButton && message.Is(JXButton::kPushed))
		{
		itsProjectDir->ChoosePath("");
		}
	else if (sender == itsChooseTmplDirButton && message.Is(JXButton::kPushed))
		{
		itsTemplateDir->ChoosePath("");
		}

	else if (sender == itsTmplDirHistory && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != NULL );
		itsTemplateDir->SetText(
			itsTmplDirHistory->JXTextMenu::GetItemText(selection->GetIndex()));
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 WriteTemplate (private)

 ******************************************************************************/

JBoolean
JXWMainDialog::WriteTemplate()
{
	// validate inputs -- itsOpenCmd can be empty

	JXInputField* input[] =
		{
		itsProgramName, itsProgramVersion,
		itsDevName, itsDevEmail, itsDevURL,
		itsBinaryName, itsSrcPrefix
		};
	const JSize inputCount = sizeof(input)/sizeof(JXInputField*);

	for (JIndex i=0; i<inputCount; i++)
		{
		input[i]->SetIsRequired(kJTrue);
		const JBoolean ok = input[i]->InputValid();
		input[i]->SetIsRequired(kJFalse);
		if (!ok)
			{
			input[i]->Focus();
			return kJFalse;
			}
		}

	// can't have whitespace in source prefix

	if ((itsSrcPrefix->GetText()).Contains(" "))
		{
		(JGetUserNotification())->ReportError(JGetString(kSrcPrefixErrorID));
		itsSrcPrefix->Focus();
		return kJFalse;
		}

	// check template directory

	itsTemplateDir->ShouldAllowInvalidPath(kJFalse);
	JBoolean tmplDirOK = itsTemplateDir->InputValid();
	itsTemplateDir->ShouldAllowInvalidPath(kJTrue);
	if (!tmplDirOK)
		{
		itsTemplateDir->Focus();
		return kJFalse;
		}

	JString templateDir;
	tmplDirOK = itsTemplateDir->GetPath(&templateDir);
	assert( tmplDirOK );

	const JString testFile = JCombinePathAndName(templateDir, "_Binary_.fd");
	if (!JFileExists(testFile))
		{
		(JGetUserNotification())->ReportError(JGetString(kInvalidTmplDirErrorID));
		itsTemplateDir->Focus();
		return kJFalse;
		}

	// try to create project directory if it doesn't exist

	JString projectDir;
	if (JIsRelativePath(itsProjectDir->GetText()) ||
		!JExpandHomeDirShortcut(itsProjectDir->GetText(), &projectDir))
		{
		itsProjectDir->ShouldAllowInvalidPath(kJFalse);
		itsProjectDir->InputValid();
		itsProjectDir->ShouldAllowInvalidPath(kJTrue);
		itsProjectDir->Focus();
		return kJFalse;
		}

	const JError err = JCreateDirectory(projectDir);
	if (!err.OK())
		{
		(JGetStringManager())->ReportError(kUnableToCreateProjDirID, err);
		return kJFalse;
		}

	// warnings should be after all errors so user doesn't have to answer yes
	// after fixing each error

		// nothing to warn about at the moment

	// generate code

	const time_t t = time(NULL);
	struct tm* ts  = gmtime(&t);
	const JString currentYear(1900 + ts->tm_year, 0);

	JString f = projectDir, p, dirName;
	JStripTrailingDirSeparator(&f);
	JSplitPathAndName(f, &p, &dirName);

	JString jxPath = JX_PATH;
	if (projectDir.BeginsWith(jxPath))
		{
		jxPath = JConvertToRelativePath(jxPath, projectDir);
		JStripTrailingDirSeparator(&jxPath);
		}

	const JCharacter* value[kTagCount];
	value[kPrefix]  = itsSrcPrefix->GetText();
	value[kProgram] = itsProgramName->GetText();
	value[kVersion] = itsProgramVersion->GetText();
	value[kAuthor]  = itsDevName->GetText();
	value[kCompany] = itsCompanyName->GetText();
	value[kYear]    = currentYear;
	value[kEmail]   = itsDevEmail->GetText();
	value[kURL]     = itsDevURL->GetText();
	value[kMDI]     = (itsNeedsMDICB->IsChecked() ? "-DUSE_MDI" : "");
	value[kBinary]  = itsBinaryName->GetText();
	value[kProjDir] = dirName;
	value[kJXPath]  = jxPath;

	CopyAndAdjustTemplateFiles(templateDir, projectDir, value);

	// open the project

	if (!itsOpenCmd->IsEmpty())
		{
		JString projFile = itsBinaryName->GetText() + ".jcc";
		projFile         = JCombinePathAndName(projectDir, projFile);
		projFile         = JPrepArgForExec(projFile);

		const JCharacter* map[] =
			{
			"f", projFile
			};
		JString cmd = itsOpenCmd->GetText();
		(JGetStringManager())->Replace(&cmd, map, sizeof(map));

		pid_t pid;
		JExecute(cmd, &pid);
		}

	// save template directory

	itsTmplDirHistory->AddString(templateDir);
	return kJTrue;
}

/******************************************************************************
 CopyAndAdjustTemplateFiles (private, recursive)

 ******************************************************************************/

void
JXWMainDialog::CopyAndAdjustTemplateFiles
	(
	const JCharacter*	sourceDir,
	const JCharacter*	targetDir,
	const JCharacter**	value
	)
{
	JDirInfo* info;
	if (!JDirInfo::Create(sourceDir, &info))
		{
		return;
		}
	info->SetWildcardFilter("*~ #*#", kJTrue);

	const JSize prefixLength = strlen(value[kPrefix]);

	const JSize count = info->GetEntryCount();
	JString prefix;
	for (JIndex i=1; i<=count; i++)
		{
		const JDirEntry& entry = info->GetEntry(i);
		if (entry.IsDirectory() && !JIsVCSDirectory(entry.GetName()))
			{
			const JString& name = entry.GetName();
			const JString d1    = JCombinePathAndName(sourceDir, name);
			const JString d2    = JCombinePathAndName(targetDir, name);
			const JError err    = JCreateDirectory(d2);
			if (err.OK())
				{
				CopyAndAdjustTemplateFiles(d1, d2, value);
				}
			else
				{
				err.ReportIfError();
				}
			}
		else if (entry.IsFile())
			{
			JString name = entry.GetName();
			if (name.BeginsWith(kFilePrefixTag, kJFalse))
				{
				prefix = name.GetSubstring(2, kFilePrefixTagLength-1);
				name.RemoveSubstring(1, kFilePrefixTagLength);
				name.Prepend(value[kPrefix]);
				name.MatchCase(prefix, JIndexRange(1, prefix.GetLength()),
							   JIndexRange(1, prefixLength));
				}
			else if (name.BeginsWith(kBinaryPrefixTag))
				{
				name.ReplaceSubstring(1, kBinaryPrefixTagLength, value[kBinary]);
				}

			JString data;
			JReadFile(entry.GetFullName(), &data);

			for (JIndex j=0; j<kTagCount; j++)
				{
				const JBoolean isPrefix = JI2B( j == kPrefix );

				JIndex fIndex = 1;
				while (data.LocateNextSubstring(kTag[j], !isPrefix, &fIndex))
					{
					if (isPrefix)
						{
						prefix = data.GetSubstring(fIndex+1, fIndex+kPrefixTagLength-2);
						}
					data.ReplaceSubstring(fIndex, fIndex + strlen(kTag[j])-1,
										  value[j]);
					if (isPrefix)
						{
						data.MatchCase(prefix, JIndexRange(1, prefix.GetLength()),
									   JIndexRange(fIndex, fIndex+prefixLength-1));
						}
					}
				}

			const JString target = JCombinePathAndName(targetDir, name);
			std::ofstream os(target);
			data.Print(os);
			}
		}

	jdelete info;
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
JXWMainDialog::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers <= kCurrentPrefsVersion)
		{
		GetWindow()->ReadGeometry(input);

		JString s;
		input >> s;
		itsProgramVersion->SetText(s);

		input >> s;
		itsDevName->SetText(s);

		input >> s;
		itsDevEmail->SetText(s);

		input >> s;
		itsCompanyName->SetText(s);

		input >> s;
		itsDevURL->SetText(s);

		JBoolean checked;
		input >> checked;
		itsNeedsMDICB->SetState(checked);

		input >> s;
		if (!s.IsEmpty())
			{
			UpdatePath(&s);
			itsProjectDir->SetText(s);
			}

		input >> s;
		if (!s.IsEmpty())
			{
			UpdatePath(&s);
			itsTemplateDir->SetText(s);
			}

		itsTmplDirHistory->ReadSetup(input);

		input >> s;
		itsOpenCmd->SetText(s);
		}
}

/******************************************************************************
 UpdatePath (private)

 ******************************************************************************/

static const JRegex jxVersPattern = "JX-([0-9.]+)";

void
JXWMainDialog::UpdatePath
	(
	JString* s
	)
{
	if (!s->IsEmpty())
		{
		JArray<JIndexRange> matchList;
		JIndexRange r;
		while (jxVersPattern.MatchAfter(*s, r, &matchList))
			{
			s->ReplaceSubstring(matchList.GetElement(2), JXWGetVersionNumberStr(), &r);
			r = matchList.GetElement(1);
			}
		}
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

static const JRegex hostPattern = "[^/]+://[^/]+/";
static const JRegex projPattern = ".+/JX-[0-9.]+/+programs/";

void
JXWMainDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentPrefsVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ' << itsProgramVersion->GetText();
	output << ' ' << itsDevName->GetText();
	output << ' ' << itsDevEmail->GetText();
	output << ' ' << itsCompanyName->GetText();

	// save host name from url

	output << ' ';
	const JString& url = itsDevURL->GetText();
	JIndexRange r;
	if (hostPattern.Match(url, &r))
		{
		output << url.GetSubstring(r);
		}
	else
		{
		output << JString(kDefaultURLText);
		}

	output << ' ' << itsNeedsMDICB->IsChecked();

	// save ".../JX-<vers>/programs/" from project directory

	output << ' ';
	const JString& projDir = itsProjectDir->GetText();
	if (projPattern.Match(projDir, &r))
		{
		output << projDir.GetSubstring(r);
		}
	else
		{
		output << JString();
		}

	output << ' ' << itsTemplateDir->GetText();

	output << ' ';
	itsTmplDirHistory->WriteSetup(output);

	output << ' ' << itsOpenCmd->GetText();
	output << ' ';
}
