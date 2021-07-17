/******************************************************************************
 GLFitManager.cpp

	BASE CLASS = <NONE>

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#include <GLFitManager.h>
#include "GLPolyFitDescription.h"
#include "GLBuiltinFitDescription.h"
#include "GLNonLinearFitDescription.h"
#include "GLModuleFitDescription.h"
#include "GLDLFitModule.h"
#include "GLGlobals.h"
#include "GLPrefsMgr.h"
#include "GLPlotApp.h"
#include <JDirInfo.h>
#include <jDirUtil.h>
#include <jAssert.h>

const JFileVersion	kCurrentPrefsVersion = 1;

const JString kFitDlDirName("fitdlmodule", JString::kNoCopy);

const JUtf8Byte* GLFitManager::kFitsChanged	= "kFitsChanged::GLFitManager";

/******************************************************************************
 Constructor

 *****************************************************************************/

GLFitManager::GLFitManager()
	:
	JPrefObject(GLGetPrefsMgr(), kFitManagerID),
	itsIsInitialized(false)
{
	itsFitDescriptions	= jnew JPtrArray<GLFitDescription>(JPtrArrayT::kDeleteAll);
	assert(itsFitDescriptions != nullptr);

	itsFitDescriptions->SetCompareFunction(GLFitDescription::CompareFits);

	JPrefObject::ReadPrefs();

	InitializeList();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GLFitManager::~GLFitManager()
{
	JPrefObject::WritePrefs();
	
	itsFitDescriptions->DeleteAll();
	jdelete itsFitDescriptions;
}

/******************************************************************************
 GetFitCount (public)

 ******************************************************************************/

JSize
GLFitManager::GetFitCount()
	const
{
	return itsFitDescriptions->GetElementCount();
}

/******************************************************************************
 GetFitDescription (public)

 ******************************************************************************/

const GLFitDescription&
GLFitManager::GetFitDescription
	(
	const JIndex index
	)
	const
{
	assert(itsFitDescriptions->IndexValid(index));
	return *(itsFitDescriptions->GetElement(index));
}

GLFitDescription&
GLFitManager::GetFitDescription
	(
	const JIndex index
	)
{
	assert(itsFitDescriptions->IndexValid(index));
	return *(itsFitDescriptions->GetElement(index));
}

/******************************************************************************
 AddFitDescription (public)

 ******************************************************************************/

void
GLFitManager::AddFitDescription
	(
	const GLFitDescription& fit
	)
{
//	GLFitDescription* fd	= jnew GLFitDescription(fit);
//	assert(fd != nullptr);

//	itsFitDescriptions->InsertSorted(fd);
}

void
GLFitManager::AddFitDescription
	(
	GLFitDescription* fit
	)
{
	itsFitDescriptions->InsertSorted(fit);
	Broadcast(FitsChanged());
}

/******************************************************************************
 NewFitDescription (public)

 ******************************************************************************/

void
GLFitManager::NewFitDescription
	(
	const GLFitDescription::FitType type
	)
{
//	GLFitDescription* fd	= jnew GLFitDescription(type);
//	assert(fd != nullptr);

//	itsFitDescriptions->InsertSorted(fd);
}

/******************************************************************************
 RemoveFitDescription (public)

 ******************************************************************************/

void
GLFitManager::RemoveFitDescription
	(
	const JIndex index
	)
{
	assert(itsFitDescriptions->IndexValid(index));
	if (FitIsRemovable(index))
		{
		itsFitDescriptions->DeleteElement(index);
		Broadcast(FitsChanged());
		}
}

/******************************************************************************
 ReadPrefs (virtual protected)

 ******************************************************************************/

void
GLFitManager::ReadPrefs
	(
	std::istream& input
	)
{
	itsIsInitialized	= true;
	
	JFileVersion version;
	input >> version;
	
	if (version > kCurrentPrefsVersion)
		{
		return;
		}

	if (version > 0)
		{
		JSize count;
		input >> count;
		for (JIndex i = 1; i <= count; i++)
			{
			GLFitDescription* fd;
			if (GLFitDescription::Create(input, &fd))
				{
				itsFitDescriptions->InsertSorted(fd);
				}
			}
		}
}

/******************************************************************************
 WritePrefs (virtual protected)

 ******************************************************************************/

void
GLFitManager::WritePrefs
	(
	std::ostream& output
	)
	const
{
	output << ' ' << kCurrentPrefsVersion << ' ';
	const JSize count	= itsFitDescriptions->GetElementCount();
	JSize rCount		= 0;
	for (JIndex i = 1; i <= count; i++)
		{
		GLFitDescription* fd	= itsFitDescriptions->GetElement(i);
		if (fd->GetType() == GLFitDescription::kPolynomial ||
			fd->GetType() == GLFitDescription::kNonLinear)
			{
			rCount++;
			}
		}
	output << ' ' << rCount << ' ';
	for (JIndex i = 1; i <= count; i++)
		{
		GLFitDescription* fd	= itsFitDescriptions->GetElement(i);
		if (fd->GetType() == GLFitDescription::kPolynomial ||
			fd->GetType() == GLFitDescription::kNonLinear)
			{
			fd->WriteSetup(output);
			}
		}
}

/******************************************************************************
 InitializeList (private)

 ******************************************************************************/

void
GLFitManager::InitializeList()
{
	GLBuiltinFitDescription* bd = 
		jnew GLBuiltinFitDescription(GLFitDescription::kBLinear);
	assert(bd != nullptr);
	itsFitDescriptions->InsertSorted(bd);

	bd = 
		jnew GLBuiltinFitDescription(GLFitDescription::kBExp);
	assert(bd != nullptr);
	itsFitDescriptions->InsertSorted(bd);

	bd = 
		jnew GLBuiltinFitDescription(GLFitDescription::kBPower);
	assert(bd != nullptr);
	itsFitDescriptions->InsertSorted(bd);

	const JPtrArray<JString>& paths	= GLGetApplication()->GetModulePath();
	const JSize count	= paths.GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		const JString& path	= *(paths.GetElement(i));
		JString fitPath	= JCombinePathAndName(path, kFitDlDirName);
		JDirInfo* dir;
		if (JDirInfo::Create(fitPath, &dir))
			{
			dir->SetWildcardFilter(JString("*.so", JString::kNoCopy));
			const JSize count	= dir->GetEntryCount();
			for (JIndex i = 1; i <= count; i++)
				{
				const JDirEntry& entry = dir->GetEntry(i);
				GLDLFitModule* fit;
				if (!entry.IsDirectory() && GLDLFitModule::Create(entry.GetFullName(), &fit))
					{
					GLModuleFitDescription* md	= 
						jnew GLModuleFitDescription(fit);
					assert(md != nullptr);
					itsFitDescriptions->InsertSorted(md);
					}
				}
			jdelete dir;
			}
		}
}

/******************************************************************************
 FitIsRemovable (public)

 ******************************************************************************/

bool
GLFitManager::FitIsRemovable
	(
	const JIndex index
	)
{
	assert(itsFitDescriptions->IndexValid(index));
	GLFitDescription* fd	= itsFitDescriptions->GetElement(index);
	if (fd->GetType() == GLFitDescription::kPolynomial ||
		fd->GetType() == GLFitDescription::kNonLinear)
		{
		return true;
		}
	return false;
}
