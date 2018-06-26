/******************************************************************************
 JXSharedPrefsManager.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_JXSharedPrefsManager
#define _H_JXSharedPrefsManager

#include <JBroadcaster.h>

class JXSharedPrefObject;

class JXSharedPrefsManager : virtual public JBroadcaster
{
public:

	JXSharedPrefsManager();

	virtual ~JXSharedPrefsManager();

	void	ReadPrefs(JXSharedPrefObject* obj);
	void	WritePrefs(const JXSharedPrefObject* obj);

	// JBroadcaster messages

	static const JUtf8Byte* kRead;
};

JXSharedPrefsManager*	JXGetSharedPrefsManager();

#endif
