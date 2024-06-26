/******************************************************************************
 MainDialog.cpp

	BASE CLASS = JXWindowDirector

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#include "MainDialog.h"
#include "globals.h"
#include <jx-af/jx/JXApplication.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXTextButton.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXPathInput.h>
#include <jx-af/jx/JXTextCheckbox.h>
#include <jx-af/jx/JXPathHistoryMenu.h>
#include <jx-af/jx/JXHelpManager.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JDirInfo.h>
#include <jx-af/jcore/jProcessUtil.h>
#include <jx-af/jcore/jFStreamUtil.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jVCSUtil.h>
#include <jx-af/jcore/jAssert.h>

const JSize kHistoryLength = 20;

static const JString kDefaultURLText("https://");
static const JString kDefaultOpenCmd("jcc $f");

static const JUtf8Byte* kDefaultTemplateDir = TEMPLATE_ROOT;
static const JUtf8Byte* kAppTemplateDir     = "app_template";

static const JString kBinaryPrefixTag("_Binary_");

static const JUtf8Byte* kTag[] =
{
	"<Program>",
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

enum
{
	kProgram,
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

const JFileVersion kCurrentPrefsVersion	= 1;

// version  1 removed program version

/******************************************************************************
 Constructor

 ******************************************************************************/

MainDialog::MainDialog
	(
	JXDirector*		supervisor,
	const JSize		argc,
	char**			argv
	)
	:
	JXWindowDirector(supervisor),
	JPrefObject(GetPrefsManager(), kMainDlogID)
{
	BuildWindow(argc, argv);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

MainDialog::~MainDialog()
{
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
MainDialog::BuildWindow
	(
	const JSize		argc,
	char**			argv
	)
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 500,330, JString::empty);

	auto* nameLabel =
		jnew JXStaticText(JGetString("nameLabel::MainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,20, 120,20);
	nameLabel->SetToLabel(false);

	auto* authorLabel =
		jnew JXStaticText(JGetString("authorLabel::MainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,50, 120,20);
	authorLabel->SetToLabel(false);

	auto* emailLabel =
		jnew JXStaticText(JGetString("emailLabel::MainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,70, 120,20);
	emailLabel->SetToLabel(false);

	auto* companyLabel =
		jnew JXStaticText(JGetString("companyLabel::MainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,90, 120,20);
	companyLabel->SetToLabel(false);

	auto* urlLabel =
		jnew JXStaticText(JGetString("urlLabel::MainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,110, 120,20);
	urlLabel->SetToLabel(false);

	auto* programNameLabel =
		jnew JXStaticText(JGetString("programNameLabel::MainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,140, 120,20);
	programNameLabel->SetToLabel(false);

	itsNeedsMDICB =
		jnew JXTextCheckbox(JGetString("itsNeedsMDICB::MainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,170, 130,20);
	itsNeedsMDICB->SetShortcuts(JGetString("itsNeedsMDICB::shortcuts::MainDialog::JXLayout"));

	auto* directoryLabel =
		jnew JXStaticText(JGetString("directoryLabel::MainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,200, 120,20);
	directoryLabel->SetToLabel(false);

	itsChooseProjDirButton =
		jnew JXTextButton(JGetString("itsChooseProjDirButton::MainDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 420,200, 60,20);

	auto* templateLabel =
		jnew JXStaticText(JGetString("templateLabel::MainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,230, 120,20);
	templateLabel->SetToLabel(false);

	itsChooseTmplDirButton =
		jnew JXTextButton(JGetString("itsChooseTmplDirButton::MainDialog::JXLayout"), window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 390,230, 60,20);

	itsTmplDirHistory =
		jnew JXPathHistoryMenu(kHistoryLength, window,
					JXWidget::kFixedRight, JXWidget::kFixedTop, 450,230, 30,20);

	auto* openCmdLabel =
		jnew JXStaticText(JGetString("openCmdLabel::MainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 10,260, 120,20);
	openCmdLabel->SetToLabel(false);

	itsQuitButton =
		jnew JXTextButton(JGetString("itsQuitButton::MainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 90,300, 60,20);

	itsHelpButton =
		jnew JXTextButton(JGetString("itsHelpButton::MainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 225,300, 60,20);
	itsHelpButton->SetShortcuts(JGetString("itsHelpButton::shortcuts::MainDialog::JXLayout"));

	itsCreateButton =
		jnew JXTextButton(JGetString("itsCreateButton::MainDialog::JXLayout"), window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 360,300, 60,20);
	itsCreateButton->SetShortcuts(JGetString("itsCreateButton::shortcuts::MainDialog::JXLayout"));

	itsProgramName =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,20, 350,20);

	itsDevName =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,50, 350,20);

	itsDevEmail =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,70, 350,20);

	itsCompanyName =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,90, 350,20);

	itsDevURL =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,110, 350,20);

	itsBinaryName =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,140, 350,20);

	itsProjectDir =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,200, 290,20);
	itsProjectDir->SetIsRequired(true);
	itsProjectDir->ShouldAllowInvalidPath(true);
	itsProjectDir->ShouldRequireWritable(true);

	itsTemplateDir =
		jnew JXPathInput(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,230, 260,20);
	itsTemplateDir->SetIsRequired(true);
	itsTemplateDir->ShouldAllowInvalidPath(true);
	itsTemplateDir->ShouldRequireWritable(false);

	itsOpenCmd =
		jnew JXInputField(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 130,260, 350,20);

// end JXLayout

	const JUtf8Byte* map[] =
	{
		"vers", GetVersionNumberStr()
	};
	const JString title = JGetString("WindowTitle::MainDialog", map, sizeof(map));
	window->SetTitle(title);
	window->ShouldFocusWhenShow(true);
	window->PlaceAsDialogWindow();
	window->LockCurrentMinSize();

	ListenTo(itsCreateButton, std::function([this](const JXButton::Pushed&)
	{
		if (WriteTemplate())
		{
			JPrefObject::WritePrefs();
			JXGetApplication()->Quit();
		}
	}));

	ListenTo(itsQuitButton, std::function([](const JXButton::Pushed&)
	{
		JXGetApplication()->Quit();
	}));

	ListenTo(itsHelpButton, std::function([](const JXButton::Pushed&)
	{
		JXGetHelpManager()->ShowSection("MainHelp");
	}));

	ListenTo(itsChooseProjDirButton, std::function([this](const JXButton::Pushed&)
	{
		itsProjectDir->ChoosePath();
	}));

	ListenTo(itsChooseTmplDirButton, std::function([this](const JXButton::Pushed&)
	{
		itsTemplateDir->ChoosePath();
	}));

	ListenTo(itsTmplDirHistory, std::function([this](const JXMenu::ItemSelected& msg)
	{
		itsTemplateDir->GetText()->SetText(
			itsTmplDirHistory->GetItemText(msg.GetIndex()));
	}));

	itsDevURL->GetText()->SetText(kDefaultURLText);
	itsNeedsMDICB->SetState(true);

	JString defaultTemplateDir(kDefaultTemplateDir);
	if (!JIsAbsolutePath(defaultTemplateDir))	// it's the app signature
	{
		JString s;
		JGetDataDirectories(kDefaultTemplateDir, kAppTemplateDir, &defaultTemplateDir, &s);
	}

	itsTemplateDir->GetText()->SetText(defaultTemplateDir);
	itsOpenCmd->GetText()->SetText(kDefaultOpenCmd);

	JPrefObject::ReadPrefs();

	// after read prefs to overwrite itsProjectDir

	if (argc == 3)
	{
		itsProjectDir->GetText()->SetText(JString(argv[1], JString::kNoCopy));
		itsProgramName->GetText()->SetText(JString(argv[2], JString::kNoCopy));
		itsBinaryName->GetText()->SetText(JString(argv[2], JString::kNoCopy));
	}
}

/******************************************************************************
 WriteTemplate (private)

 ******************************************************************************/

bool
MainDialog::WriteTemplate()
{
	// validate inputs -- itsOpenCmd can be empty

	JXInputField* input[] =
	{
		itsProgramName,
		itsDevName, itsDevEmail, itsDevURL,
		itsBinaryName
	};

	for (auto* i : input)
	{
		i->SetIsRequired(true);
		const bool ok = i->InputValid();
		i->SetIsRequired(false);
		if (!ok)
		{
			i->Focus();
			return false;
		}
	}

	// check template directory

	itsTemplateDir->ShouldAllowInvalidPath(false);
	bool tmplDirOK = itsTemplateDir->InputValid();
	itsTemplateDir->ShouldAllowInvalidPath(true);
	if (!tmplDirOK)
	{
		itsTemplateDir->Focus();
		return false;
	}

	JString templateDir;
	tmplDirOK = itsTemplateDir->GetPath(&templateDir);
	assert( tmplDirOK );

	const JString testFile = JCombinePathAndName(templateDir, "_Binary_.jcc");
	if (!JFileExists(testFile))
	{
		JGetUserNotification()->ReportError(JGetString("InvalidTmplDirError::MainDialog"));
		itsTemplateDir->Focus();
		return false;
	}

	// try to create project directory if it doesn't exist

	JString projectDir;
	if (JIsRelativePath(itsProjectDir->GetText()->GetText()) ||
		!JExpandHomeDirShortcut(itsProjectDir->GetText()->GetText(), &projectDir))
	{
		itsProjectDir->ShouldAllowInvalidPath(false);
		itsProjectDir->InputValid();
		itsProjectDir->ShouldAllowInvalidPath(true);
		itsProjectDir->Focus();
		return false;
	}

	JError err = JNoError();
	if (!JCreateDirectory(projectDir, &err))
	{
		JGetStringManager()->ReportError("UnableToCreateProjDir::MainDialog", err);
		return false;
	}

	// warnings should be after all errors so user doesn't have to answer yes
	// after fixing each error

		// nothing to warn about at the moment

	// generate code

	tm ts;
	const time_t t = time(nullptr);
	gmtime_r(&t, &ts);
	const JString currentYear(1900 + ts.tm_year);

	JString f = projectDir, p, dirName;
	JStripTrailingDirSeparator(&f);
	JSplitPathAndName(f, &p, &dirName);

	JString jxPath(JX_PATH, JString::kNoCopy);
	if (projectDir.StartsWith(jxPath))
	{
		jxPath = JConvertToRelativePath(jxPath, projectDir);
	}
	JStripTrailingDirSeparator(&jxPath);

	JPtrArray<JString> value(JPtrArrayT::kDeleteAll);
	value.Append(itsProgramName->GetText()->GetText());
	value.Append(itsDevName->GetText()->GetText());
	value.Append(itsCompanyName->GetText()->GetText());
	value.Append(currentYear);
	value.Append(itsDevEmail->GetText()->GetText());
	value.Append(itsDevURL->GetText()->GetText());
	value.Append(itsNeedsMDICB->IsChecked() ? "-DUSE_MDI" : JString::empty);
	value.Append(itsBinaryName->GetText()->GetText());
	value.Append(dirName);
	value.Append(jxPath);

	CopyAndAdjustTemplateFiles(templateDir, projectDir, value);

	// open the project

	if (!itsOpenCmd->GetText()->IsEmpty())
	{
		JString projFile = itsBinaryName->GetText()->GetText() + ".jcc";
		projFile         = JCombinePathAndName(projectDir, projFile);
		projFile         = JPrepArgForExec(projFile);

		const JUtf8Byte* map[] =
		{
			"f", projFile.GetBytes()
		};
		JString cmd = itsOpenCmd->GetText()->GetText();
		JGetStringManager()->Replace(&cmd, map, sizeof(map));

		pid_t pid;
		JExecute(cmd, &pid);
	}

	// save template directory

	itsTmplDirHistory->AddString(templateDir);
	return true;
}

/******************************************************************************
 CopyAndAdjustTemplateFiles (private, recursive)

 ******************************************************************************/

void
MainDialog::CopyAndAdjustTemplateFiles
	(
	const JString&				sourceDir,
	const JString&				targetDir,
	const JPtrArray<JString>&	value
	)
{
	JDirInfo* info;
	if (!JDirInfo::Create(sourceDir, &info))
	{
		return;
	}
	info->SetWildcardFilter("*~ #*#", true);

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

			JError err = JNoError();
			if (JCreateDirectory(d2, &err))
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
			if (name.StartsWith(kBinaryPrefixTag))
			{
				JStringIterator iter(&name);
				iter.BeginMatch();
				iter.SkipNext(kBinaryPrefixTag.GetCharacterCount());
				iter.FinishMatch();
				iter.ReplaceLastMatch(*value.GetItem(kBinary+1));
			}

			JString data;
			JReadFile(entry.GetFullName(), &data);

			JStringIterator iter(&data);
			for (JIndex j=1; j<=kTagCount; j++)
			{
				iter.MoveTo(JStringIterator::kStartAtBeginning, 0);
				while (iter.Next(kTag[j-1]))
				{
					iter.ReplaceLastMatch(*value.GetItem(j));
				}
			}

			const JString target = JCombinePathAndName(targetDir, name);
			std::ofstream os(target.GetBytes());
			data.Print(os);
		}
	}

	jdelete info;
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
MainDialog::ReadPrefs
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

		if (vers == 0)
		{
			input >> s;		// program version
		}

		input >> s;
		itsDevName->GetText()->SetText(s);

		input >> s;
		itsDevEmail->GetText()->SetText(s);

		input >> s;
		itsCompanyName->GetText()->SetText(s);

		input >> s;
		itsDevURL->GetText()->SetText(s);

		bool checked;
		input >> JBoolFromString(checked);
		itsNeedsMDICB->SetState(checked);

		input >> s;
		if (!s.IsEmpty())
		{
			UpdatePath(&s);
			itsProjectDir->GetText()->SetText(s);
		}

		input >> s;
		if (!s.IsEmpty())
		{
			UpdatePath(&s);
			itsTemplateDir->GetText()->SetText(s);
		}

		itsTmplDirHistory->ReadSetup(input);

		input >> s;
		itsOpenCmd->GetText()->SetText(s);
	}
}

/******************************************************************************
 UpdatePath (private)

 ******************************************************************************/

static const JRegex jxVersPattern("JX-([0-9.]+)");

void
MainDialog::UpdatePath
	(
	JString* s
	)
{
	JStringIterator iter(s);
	const JString v = "JX-" + JString(GetVersionNumberStr(), JString::kNoCopy);
	while (iter.Next(jxVersPattern))
	{
		iter.ReplaceLastMatch(v);
	}
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

static const JRegex hostPattern("[^/]+://[^/]+/");
static const JRegex projPattern(".+/JX-[0-9.]+/+programs/");

void
MainDialog::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentPrefsVersion;

	output << ' ';
	GetWindow()->WriteGeometry(output);

	output << ' ' << itsDevName->GetText()->GetText();
	output << ' ' << itsDevEmail->GetText()->GetText();
	output << ' ' << itsCompanyName->GetText()->GetText();

	// save host name from url

	output << ' ';
	const JStringMatch m1 = hostPattern.Match(itsDevURL->GetText()->GetText(), JRegex::kIgnoreSubmatches);
	if (!m1.IsEmpty())
	{
		output << m1.GetString();
	}
	else
	{
		output << JString(kDefaultURLText, JString::kNoCopy);
	}

	output << ' ' << JBoolToString(itsNeedsMDICB->IsChecked());

	// save ".../JX-<vers>/programs/" from project directory

	output << ' ';
	const JStringMatch m2 = projPattern.Match(itsProjectDir->GetText()->GetText(), JRegex::kIgnoreSubmatches);
	if (!m2.IsEmpty())
	{
		output << m2.GetString();
	}
	else
	{
		output << JString();
	}

	output << ' ' << itsTemplateDir->GetText()->GetText();

	output << ' ';
	itsTmplDirHistory->WriteSetup(output);

	output << ' ' << itsOpenCmd->GetText()->GetText();
	output << ' ';
}
