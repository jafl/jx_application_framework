/******************************************************************************
 jTestUtil.h

   Copyright (C) 2018 by John Lindal.

 *****************************************************************************/

#ifndef _H_jTextUtil
#define _H_jTextUtil

#include "JList.h"

class JString;

template <class T>
void JSetList(const JUtf8Byte* data, JList<T>* list);

template <class T>
JString JPrintList(const JList<T>& list);

#include "jTestUtil.tmpl"

#endif
