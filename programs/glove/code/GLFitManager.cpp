/******************************************************************************
 GLFitManager.cpp

	BASE CLASS = <NONE>

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.
	
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

const JFileVersion	kCurrentPrefsVersion		= 1;

const JCharacter* kFitDlDirName					= "fitdlmodule";

const JCharacter* GLFitManager::kFitsChanged	= "kFitsChanged::GLFitManager";

/******************************************************************************
 Constructor

 *****************************************************************************/

GLFitManager::GLFitManager()
	:
	JPrefObject(GetPrefsMgr(), kFitManagerID),
	itsIsInitialized(kJFalse)
{
	itsFitDescriptions	= new JPtrArray<GLFitDescription>(JPtrArrayT::kDeleteAll);
	assert(itsFitDescriptions != NULL);

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
	delete itsFitDescriptions;
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
	return *(itsFitDescriptions->NthElement(index));
}

GLFitDescription&
GLFitManager::GetFitDescription
	(
	const JIndex index
	)
{
	assert(itsFitDescriptions->IndexValid(index));
	return *(itsFitDescriptions->NthElement(index));
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
//	GLFitDescription* fd	= new GLFitDescription(fit);
//	assert(fd != NULL);

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
//	GLFitDescription* fd	= new GLFitDescription(type);
//	assert(fd != NULL);

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
	istream& input
	)
{
	itsIsInitialized	= kJTrue;
	
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
	ostream& output
	)
	const
{
	output << ' ' << kCurrentPrefsVersion << ' ';
	const JSize count	= itsFitDescriptions->GetElementCount();
	JSize rCount		= 0;
	for (JIndex i = 1; i <= count; i++)
		{
		GLFitDescription* fd	= itsFitDescriptions->NthElement(i);
		if (fd->GetType() == GLFitDescription::kPolynomial ||
			fd->GetType() == GLFitDescription::kNonLinear)
			{
			rCount++;
			}
		}
	output << ' ' << rCount << ' ';
	for (JIndex i = 1; i <= count; i++)
		{
		GLFitDescription* fd	= itsFitDescriptions->NthElement(i);
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
		new GLBuiltinFitDescription(GLFitDescription::kBLinear);
	assert(bd != NULL);
	itsFitDescriptions->InsertSorted(bd);

	bd = 
		new GLBuiltinFitDescription(GLFitDescription::kBExp);
	assert(bd != NULL);
	itsFitDescriptions->InsertSorted(bd);

	bd = 
		new GLBuiltinFitDescription(GLFitDescription::kBPower);
	assert(bd != NULL);
	itsFitDescriptions->InsertSorted(bd);

	const JPtrArray<JString>& paths	= GLGetApplication()->GetModulePath();
	const JSize count	= paths.GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		const JString& path	= *(paths.NthElement(i));
		JString fitPath	= JCombinePathAndName(path, kFitDlDirName);
		JDirInfo* dir;
		if (JDirInfo::Create(fitPath, &dir))
			{
			dir->SetWildcardFilter("*.so");
			const JSize count	= dir->GetEntryCount();
			for (JIndex i = 1; i <= count; i++)
				{
				const JDirEntry& entry = dir->GetEntry(i);
				GLDLFitModule* fit;
				if (!entry.IsDirectory() && GLDLFitModule::Create(entry.GetFullName(), &fit))
					{
					GLModuleFitDescription* md	= 
						new GLModuleFitDescription(fit);
					assert(md != NULL);
					itsFitDescriptions->InsertSorted(md);
					}
				}
			delete dir;
			}
		}
}

/******************************************************************************
 FitIsRemovable (public)

 ******************************************************************************/

JBoolean
GLFitManager::FitIsRemovable
	(
	const JIndex index
	)
{
	assert(itsFitDescriptions->IndexValid(index));
	GLFitDescription* fd	= itsFitDescriptions->NthElement(index);
	if (fd->GetType() == GLFitDescription::kPolynomial ||
		fd->GetType() == GLFitDescription::kNonLinear)
		{
		return kJTrue;
		}
	return kJFalse;
}
