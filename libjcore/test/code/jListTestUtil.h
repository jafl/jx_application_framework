/******************************************************************************
 jListTestUtil.h

   Copyright (C) 2018 by John Lindal.

 *****************************************************************************/

#ifndef _H_jListTestUtil
#define _H_jListTestUtil

#include "JList.h"

class JString;

template <class T>
void JSetList(const JUtf8Byte* data, JList<T>* list);

template <class T>
JString JPrintList(const JList<T>& list);

#include "jListTestUtil.tmpl"

#endif
