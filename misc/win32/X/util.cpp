/******************************************************************************
 util.cpp
 
	General X functions.

	Copyright (C) 2005 by John Lindal.

 ******************************************************************************/

#include "XStdInc.h"
#include <stdlib.h>
#include <assert.h>

/******************************************************************************
 XBell

 ******************************************************************************/

int
XBell
	(
	Display*	display,
	int			percent
	)
{
	return MessageBeep(-1);
}

/******************************************************************************
 XFree

 ******************************************************************************/

int
XFree
	(
	void* data
	)
{
	free(data);
	return TRUE;
}

/******************************************************************************
 XFree

 ******************************************************************************/

Status
XStringListToTextProperty
	(
	char**			list,
	int				count,
	XTextProperty*	text_prop_return
	)
{
	long length = 0;
	for (int i=0; i<count; i++)
		{
		length += strlen(list[i]) + 1;	// include nullptr
		}
	length++;	// extra nullptr

	unsigned char* value = (unsigned char*) malloc(length);
	if (value != nullptr)
		{
		char* p = (char*) value;
		for (int i=0; i<count; i++)
			{
			strcpy(p, list[i]);
			p += strlen(list[i]) + 1;
			}
		*p = '\0';

		text_prop_return->value    = value;
		text_prop_return->encoding = XA_STRING;
		text_prop_return->format   = 8;
		text_prop_return->nitems   = count;
		return TRUE;
		}
	else
		{
		return FALSE;
		}
}
