/******************************************************************************
 JISOStyler.cpp

	BASE CLASS = none

	Copyright © 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include <cmStdInc.h>
#include <JISOStyler.h>
#include <JTextEditor.h>
#include <JColormap.h>
#include <JFontStyle.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JISOStyler::JISOStyler
	(
	JTextEditor* editor
	)
{
	itsEditor    = editor;
	itsFontStyle = itsEditor->GetCurrentFontStyle();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JISOStyler::~JISOStyler()
{
}

/******************************************************************************
 FilterISO

 ******************************************************************************/

void
JISOStyler::FilterISO
	(
	const JString& text
	)
{
	itsData.Append(text);
	itsEditor->SetCurrentFontStyle(itsFontStyle);
	JIndex findex;
	while (itsData.LocateSubstring("\033", &findex))
		{
		if (findex > 1)
			{
			JString data = itsData.GetSubstring(1, findex - 1);
			itsEditor->Paste(data);
			itsData.RemoveSubstring(1, findex - 1);
			}
		// at some point, I'll need to do a better check for non ISO \033
		if (itsData.LocateSubstring("m", &findex))
			{
//			JFontStyle style = itsEditor->GetCurrentFontStyle();
			JString data = itsData.GetSubstring(1, findex - 1);
			itsData.RemoveSubstring(1, findex);
			if (!data.BeginsWith("\033["))
				{
				// This isn't ISO
				itsEditor->Paste(data);
				}
			else
				{
				data.RemoveSubstring(1, 2);
				while (data.LocateSubstring(";", &findex))
					{
					JString valStr;
					if (findex > 1)
						{
						valStr = data.GetSubstring(1, findex - 1);
						}
					data.RemoveSubstring(1, findex);
					if (findex == 1)
						{
						itsFontStyle = JFontStyle();
						}
					else
						{
						AdjustStyle(valStr);
						}
					}
				JString valStr = data;
				if (!data.IsEmpty())
					{
					AdjustStyle(valStr);
					}
				else
					{
					itsFontStyle = JFontStyle();
					}
				itsEditor->SetCurrentFontStyle(itsFontStyle);
				}
			}
		else
			{
			itsData.RemoveSubstring(1,1);
			}
		}

	if (!itsData.IsEmpty())
		{
		itsEditor->Paste(itsData);
		itsData.Clear();
		}
}

/******************************************************************************
 AdjustStyle

 ******************************************************************************/

void
JISOStyler::AdjustStyle
	(
	const JString& valStr
	)
{
	JColormap* cmap = itsEditor->TEGetColormap();

	JInteger value;
	if (!valStr.ConvertToInteger(&value))
		{
		itsFontStyle = JFontStyle();
		}
	else if (value == 0)
		{
		itsFontStyle = JFontStyle();
		}
	else if (value == 1)
		{
		itsFontStyle.bold = kJTrue;
		}
	else if (value == 3)
		{
		itsFontStyle.italic = kJTrue;
		}
	else if (value == 4)
		{
		itsFontStyle.underlineCount = 1;
		}
	else if (value == 22)
		{
		itsFontStyle.bold = kJFalse;
		}
	else if (value == 23)
		{
		itsFontStyle.italic = kJFalse;
		}
	else if (value == 24)
		{
		itsFontStyle.underlineCount = 0;
		}
	else if (value == 30)
		{
		itsFontStyle.color = cmap->GetBlackColor();
		}
	else if (value == 31)
		{
		itsFontStyle.color = cmap->GetRedColor();
		}
	else if (value == 32)
		{
		itsFontStyle.color = cmap->GetGreenColor();
		}
	else if (value == 33)
		{
		itsFontStyle.color = cmap->GetYellowColor();
		}
	else if (value == 34)
		{
		itsFontStyle.color = cmap->GetBlueColor();
		}
	else if (value == 35)
		{
		itsFontStyle.color = cmap->GetMagentaColor();
		}
	else if (value == 36)
		{
		itsFontStyle.color = cmap->GetCyanColor();
		}
	else if (value == 37)
		{
		itsFontStyle.color = cmap->GetWhiteColor();
		}
	else if (value == 38)
		{
		itsFontStyle.color = cmap->GetBlackColor();
		}
	else
		{
		itsFontStyle = JFontStyle();
		}
}
