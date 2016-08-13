/******************************************************************************
 SyGTaskBarUtil.cpp

	Functions shared by System G and JXFVWM2TaskBar.

	Copyright © 2001 John Lindal. All rights reserved.

 ******************************************************************************/

#include "SyGTaskBarUtil.h"

#if defined SYSTEM_G
#include "SyGGlobals.h"
#endif

#include <JXDisplay.h>
#include <JXImageCache.h>
#include <JXImage.h>
#include <jXGlobals.h>
#include <JDirInfo.h>
#include <jStreamUtil.h>
#include <jFileUtil.h>
#include <jProcessUtil.h>
#include <jAssert.h>

// owned by JXImageCache
static JXImage* theHDSmallIcon  = NULL;
static JXImage* theZipSmallIcon = NULL;
static JXImage* theFDSmallIcon  = NULL;
static JXImage* theCDSmallIcon  = NULL;

// owned by JXImageCache
static JXImage* theTrashEmptySmallIcon         = NULL;
static JXImage* theTrashEmptySelectedSmallIcon = NULL;
static JXImage* theTrashFullSmallIcon          = NULL;
static JXImage* theTrashFullSelectedSmallIcon  = NULL;

#if defined JX_FVWM2_TASKBAR

static JDirInfo* theTrashDirInfo = NULL;

#endif

// shared prefs

static const JCharacter* kTaskBarSetupFileName = ".jxfvwm2taskbar/systemg";

const JFileVersion kCurrentTaskBarSetupVersion = 0;

// Prototypes

JBoolean	SyGGetTaskBarSetupFileName(JString* fileName);

#if defined SYSTEM_G

/******************************************************************************
 SyGWriteTaskBarSetup

 ******************************************************************************/

void
SyGWriteTaskBarSetup
	(
	const JPtrArray<JString>&	shortcutList,
	const JBoolean				replace
	)
{
JIndex i;

	JString fileName;
	if (!SyGGetTaskBarSetupFileName(&fileName) ||
		(!replace && JFileExists(fileName)))
		{
		return;
		}

	JString p,n;
	JSplitPathAndName(fileName, &p, &n);
	if (JCreateDirectory(p) != kJNoError)
		{
		return;
		}

	JString tmpName;
	JError err = JCreateTempFile(&tmpName);
	if (!err.OK())
		{
		err.ReportIfError();
		return;
		}

	ofstream output(tmpName);
	output << kCurrentTaskBarSetupVersion << ' ';

	JString trashDir;
	if (SyGGetTrashDirectory(&trashDir, kJFalse))
		{
		output << kJTrue << ' ';
		trashDir.Print(output);
		}
	else
		{
		output << kJFalse;
		}

	output << endl;

	const JSize count = shortcutList.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		(shortcutList.NthElement(i))->Print(output);
		output << endl;
		}

	const JCharacter* argv[] = { "mv", "-f", tmpName, fileName, NULL };
	err = JExecute(argv, sizeof(argv), NULL);
	err.ReportIfError();
}

/******************************************************************************
 SyGTBTrashDirectoryIsEmpty

 ******************************************************************************/

JBoolean
SyGTBTrashDirectoryIsEmpty()
{
	return SyGTrashDirectoryIsEmpty();
}

#elif defined JX_FVWM2_TASKBAR

/******************************************************************************
 SyGReadTaskBarSetup

 ******************************************************************************/

JBoolean
SyGReadTaskBarSetup
	(
	JBoolean*			hasTrashDir,
	JString*			trashDir,
	JPtrArray<JString>*	shortcutList,
	time_t*				modTime
	)
{
	JString fileName;
	if (!SyGGetTaskBarSetupFileName(&fileName))
		{
		return kJFalse;
		}

	time_t t;
	if (modTime != NULL &&
		(JGetModificationTime(fileName, &t)).OK() &&
		t == *modTime)
		{
		return kJFalse;
		}
	if (modTime != NULL)
		{
		*modTime = t;
		}

	ifstream input(fileName);
	if (!input.good())
		{
		return kJFalse;
		}

	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentTaskBarSetupVersion)
		{
		return kJFalse;
		}

	input >> *hasTrashDir;
	if (*hasTrashDir)
		{
		*trashDir = JReadLine(input);
		JDirInfo::Create(*trashDir, &theTrashDirInfo);
		}
	else
		{
		trashDir->Clear();
		}

	shortcutList->CleanOut();

	JString path;
	while (1)
		{
		path = JReadLine(input);
		if ((input.eof() && path.IsEmpty()) || input.fail())
			{
			break;
			}

		shortcutList->Append(path);
		}

	return kJTrue;
}

/******************************************************************************
 SyGTBTrashDirectoryIsEmpty

 ******************************************************************************/

JBoolean
SyGTBTrashDirectoryIsEmpty()
{
	if (theTrashDirInfo != NULL)
		{
		theTrashDirInfo->Update();
		return theTrashDirInfo->IsEmpty();
		}
	else
		{
		return kJTrue;
		}
}

#endif

/******************************************************************************
 SyGGetTaskBarSetupFileName (local)

	Returns kJTrue if the user has a home directory.

 ******************************************************************************/

JBoolean
SyGGetTaskBarSetupFileName
	(
	JString* fileName
	)
{
	if (JGetPrefsDirectory(fileName))
		{
		*fileName = JCombinePathAndName(*fileName, kTaskBarSetupFileName);
		return kJTrue;
		}
	else
		{
		fileName->Clear();
		return kJFalse;
		}
}

/******************************************************************************
 Icons

 ******************************************************************************/

#include <jx_hard_disk_small.xpm>
#include <jx_zip_disk_small.xpm>
#include <jx_floppy_disk_small.xpm>
#include <jx_cdrom_disk_small.xpm>

#include <jx_trash_can_empty_small.xpm>
#include <jx_trash_can_empty_selected_small.xpm>
#include <jx_trash_can_full_small.xpm>
#include <jx_trash_can_full_selected_small.xpm>

void
SyGCreateMountPointSmallIcons()
{
	JXImageCache* c = (JXGetApplication())->GetDisplay(1)->GetImageCache();

	theHDSmallIcon  = c->GetImage(jx_hard_disk_small);
	theZipSmallIcon = c->GetImage(jx_zip_disk_small);
	theFDSmallIcon  = c->GetImage(jx_floppy_disk_small);
	theCDSmallIcon  = c->GetImage(jx_cdrom_disk_small);

	theTrashEmptySmallIcon         = c->GetImage(jx_trash_can_empty_small);
	theTrashEmptySelectedSmallIcon = c->GetImage(jx_trash_can_empty_selected_small);
	theTrashFullSmallIcon          = c->GetImage(jx_trash_can_full_small);
	theTrashFullSelectedSmallIcon  = c->GetImage(jx_trash_can_full_selected_small);
}

void
SyGDeleteMountPointSmallIcons()
{
}

/******************************************************************************
 SyGGetMountPointSmallIcon

	Returns kJTrue if there is an image for the given type.

 ******************************************************************************/

JBoolean
SyGGetMountPointSmallIcon
	(
	const JMountType	type,
	JXImage**			image
	)
{
	if (type == kJHardDisk)
		{
		*image = theHDSmallIcon;
		}
	else if (type == kJFloppyDisk)
		{
		*image = theFDSmallIcon;
		}
	else if (type == kJCDROM)
		{
		*image = theCDSmallIcon;
		}
	else
		{
		*image = NULL;
		}
	return JNegate(*image == NULL);
}

/******************************************************************************
 SyGGetTrashSmallIcon

 ******************************************************************************/

JXImage*
SyGGetTrashSmallIcon
	(
	const JBoolean selected
	)
{
#if defined SYSTEM_G
	if (SyGTrashDirectoryIsEmpty())
#elif defined JX_FVWM2_TASKBAR
	if (SyGTBTrashDirectoryIsEmpty())
#endif
		{
		return selected ? theTrashEmptySelectedSmallIcon : theTrashEmptySmallIcon;
		}
	else
		{
		return selected ? theTrashFullSelectedSmallIcon : theTrashFullSmallIcon;
		}
}
