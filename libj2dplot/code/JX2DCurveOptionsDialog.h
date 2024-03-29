/******************************************************************************
 JX2DCurveOptionsDialog.h

	Copyright (C) 1997 by Glenn W. Bach.

 ******************************************************************************/

#ifndef _H_JX2DCurveOptionsDialog
#define _H_JX2DCurveOptionsDialog

#include <jx-af/jx/JXModalDialogDirector.h>
#include "J2DCurveInfo.h"

class JString;
class JXTextButton;
class JXTextCheckbox;
class JXRadioGroup;
class JXTextRadioButton;
class JX2DCurveNameList;

class JX2DCurveOptionsDialog : public JXModalDialogDirector
{
public:

	enum
	{
		kShowPoints = 0,
		kShowSymbols,
		kShowLines,
		kShowBoth
	};

public:

	JX2DCurveOptionsDialog(const JArray<J2DCurveInfo>& array,
							const JArray<bool>& hasXErrors,
							const JArray<bool>& hasYErrors,
							const JArray<bool>& isFunction,
							const JArray<bool>& isScatter,
							const JIndex startIndex);

	~JX2DCurveOptionsDialog() override;

	const JArray<J2DCurveInfo>&	GetCurveInfoArray();

protected:

	bool	OKToDeactivate() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JArray<J2DCurveInfo>*	itsCurveInfo;
	JArray<bool>*			itsHasXErrors;
	JArray<bool>*			itsHasYErrors;
	JArray<bool>*			itsIsFunction;
	JArray<bool>*			itsIsScatter;
	JIndex					itsCurrentIndex;

// begin JXLayout

	JX2DCurveNameList* itsNameList;
	JXTextCheckbox*    itsShowCurveCB;
	JXRadioGroup*      itsCurveStyleRG;
	JXTextRadioButton* itsShowPointsRB;
	JXTextRadioButton* itsShowSymbolsRB;
	JXTextRadioButton* itsShowLinesRB;
	JXTextRadioButton* itsShowBothRB;
	JXTextCheckbox*    itsShowXErrorsCB;
	JXTextCheckbox*    itsShowYErrorsCB;
	JXTextButton*      itsCancelButton;

// end JXLayout

private:

	void	BuildWindow();
	void	SaveSettings();
	void	AdjustWidgets();
	JIndex	EncodeCurveStyle(const J2DCurveInfo& info);
	void	DecodeCurveStyle(J2DCurveInfo* info);
};


/******************************************************************************
 GetCurveInfoArray

 ******************************************************************************/

inline const JArray<J2DCurveInfo>&
JX2DCurveOptionsDialog::GetCurveInfoArray()
{
	return *itsCurveInfo;
}

#endif
