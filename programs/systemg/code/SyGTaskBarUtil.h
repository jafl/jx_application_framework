/******************************************************************************
 SyGTaskBarUtil.h

	Copyright (C) 2001 by John Lindal.

 ******************************************************************************/

#ifndef _H_SyGTaskBarUtil
#define _H_SyGTaskBarUtil

#include <jMountUtil.h>
#include <JPtrArray-JString.h>

class JXImage;

void	SyGCreateMountPointSmallIcons();
void	SyGDeleteMountPointSmallIcons();

JBoolean	SyGGetMountPointSmallIcon(const JMountType type, JXImage** image);
JXImage*	SyGGetTrashSmallIcon(const JBoolean selected = kJFalse);

JBoolean	SyGTBTrashDirectoryIsEmpty();

#if defined SYSTEM_G

	void	SyGWriteTaskBarSetup(const JPtrArray<JString>& shortcutList,
								 const JBoolean replace);

#elif defined JX_FVWM2_TASKBAR

	JBoolean	SyGReadTaskBarSetup(JBoolean* hasTrashDir, JString* trashDir,
									JPtrArray<JString>* shortcutList,
									time_t* modTime);

#endif

#endif
