/******************************************************************************
 test_JPrefsFile.cpp

	Program to test JPrefsFile classes.

	Written by John Lindal.

 ******************************************************************************/

#include <jx-af/jcore/JTestManager.h>
#include <jx-af/jcore/JPrefsFile.h>
#include <jx-af/jcore/jFileUtil.h>
#include <sstream>
#include <jx-af/jcore/jAssert.h>

const JFileVersion kVersionOfExistingFile = 5;

static const JString kTestFileName("PrefsFileTest", JString::kNoCopy);

int main()
{
	return JTestManager::Execute();
}

JTEST(NewFile)
{
	const JString s1 = "~/." + kTestFileName + ".pref";
	JString s2;
	JAssertTrue(JExpandHomeDirShortcut(s1, &s2));

	if (JNameUsed(s2))
	{
		JAssertOK(JRemoveFile(s2));
	}
	JAssertFalse(JNameUsed(s2));

	JPrefsFile* prefsFile;
	JAssertOK(JPrefsFile::Create(kTestFileName, &prefsFile));

	JAssertEqual(0, prefsFile->GetElementCount());
	JAssertFalse(prefsFile->GetVersion() == kVersionOfExistingFile);

	prefsFile->SetVersion(kVersionOfExistingFile);
	JAssertEqual(kVersionOfExistingFile, prefsFile->GetVersion());

{
	std::ostringstream dataStream;
	dataStream << JString("This was the first element that was added to the file.", JString::kNoCopy);
	prefsFile->SetData(3, dataStream);
}

{
	std::ostringstream dataStream;
	dataStream << JString("This was the second element that was added to the file.", JString::kNoCopy);
	prefsFile->SetData(5, dataStream);
}

	prefsFile->RemoveData(3);

{
	std::ostringstream dataStream;
	dataStream << JString("This was the third element that was added to the file", JString::kNoCopy);
	prefsFile->SetData(7, dataStream);
}

	JAssertEqual(2, prefsFile->GetElementCount());

{
	std::string data;
	prefsFile->GetData(7, &data);

	std::istringstream dataStream(data);
	JString elementData;
	dataStream >> elementData;
	JAssertStringsEqual("This was the third element that was added to the file", elementData);
}

{
	std::ostringstream dataStream;
	dataStream << JString("1st element shortended", JString::kNoCopy);
	prefsFile->SetData(5, dataStream);
}

{
	std::ostringstream dataStream;
	dataStream << JString("2nd element shortended", JString::kNoCopy);
	prefsFile->SetData(7, dataStream);
}

{
	std::ostringstream dataStream;
	dataStream << JString("Now the first element is really, really long!!!", JString::kNoCopy);
	prefsFile->SetData(5, dataStream);
}

{
	std::ostringstream dataStream;
	dataStream << JString("Now the second element is also very much longer!!!", JString::kNoCopy);
	prefsFile->SetData(7, dataStream);
}

	jdelete prefsFile;
}

JTEST(OldFile)
{
	JPrefsFile* prefsFile;
	JAssertOK(JPrefsFile::Create(kTestFileName, &prefsFile));

	JAssertEqual(kVersionOfExistingFile, prefsFile->GetVersion());
	JAssertEqual(2, prefsFile->GetElementCount());

	JString elementData;

	JPrefID id;
	JAssertTrue(prefsFile->IndexToID(1, &id));
	JAssertEqual(5, id.GetID());
{
	std::string data;
	prefsFile->GetData(id, &data);

	std::istringstream dataStream(data);
	dataStream >> elementData;
	JAssertStringsEqual("Now the first element is really, really long!!!", elementData);
}

	JAssertTrue(prefsFile->IndexToID(2, &id));
	JAssertEqual(7, id.GetID());
{
	std::string data;
	prefsFile->GetData(id, &data);

	std::istringstream dataStream(data);
	dataStream >> elementData;
	JAssertStringsEqual("Now the second element is also very much longer!!!", elementData);
}

	jdelete prefsFile;

	JString fullName;
	JAssertTrue(JExpandHomeDirShortcut(JString("~/." + kTestFileName + ".pref", JString::kNoCopy), &fullName));
	JAssertOK(JRemoveFile(fullName));
}
