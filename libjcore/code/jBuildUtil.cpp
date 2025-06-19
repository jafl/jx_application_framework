/******************************************************************************
 jBuildUtil.cpp

	Routines to help with build artifacts.

	Copyright (C) 2025 John Lindal.

 ******************************************************************************/

#include "jBuildUtil.h"
#include "JString.h"
#include "jAssert.h"

static const JString kMavenDirName("target");
static const JString kNodeDirName ("node_modules");

/******************************************************************************
 JIsBuildArtifactDirectory

 ******************************************************************************/

bool
JIsBuildArtifactDirectory
	(
	const JString& name
	)
{
	return (name == kMavenDirName ||
			name == kNodeDirName);
}
