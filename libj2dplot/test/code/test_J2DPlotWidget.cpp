/******************************************************************************
 test_J2DPlotWidget.cpp

	Tests for J2DPlotWidget.

	Written by John Lindal.

 *****************************************************************************/

#include <JTestManager.h>
#include <J2DPlotWidget.h>
#include <jAssert.h>

int main()
{
	return JTestManager::Execute();
}

JTEST(FindRange)
{
	JFloat start, end, inc;
	J2DPlotWidget::FindRange(0.0, 10.0, &start, &end, &inc);
	JAssertEqual(0.0, start);
	JAssertEqual(10.0, end);
	JAssertEqual(2.0, inc);

	J2DPlotWidget::FindRange(0.0, 11.0, &start, &end, &inc);
	JAssertEqual(0.0, start);
	JAssertEqual(12.0, end);
	JAssertEqual(2.0, inc);

	J2DPlotWidget::FindRange(-1.083642, 1.05732, &start, &end, &inc);
	JAssertEqual(-2.0, start);
	JAssertEqual(2.0, end);
	JAssertEqual(1.0, inc);

	J2DPlotWidget::FindRange(-15.378924, 22.846282234, &start, &end, &inc);
	JAssertEqual(-20.0, start);
	JAssertEqual(30.0, end);
	JAssertEqual(10.0, inc);
}

JTEST(FindLogRange)
{
	JFloat start, end, inc;
	J2DPlotWidget::FindRange(0.0, 1.0e5, &start, &end, &inc);
	JAssertEqual(0.0, start);
	JAssertEqual(1.0e5, end);
	JAssertEqual(2.0e4, inc);

	J2DPlotWidget::FindRange(1.29834e-7, 35.8348652e8, &start, &end, &inc);
	JAssertEqual(0.0, start);
	JAssertEqual(4.0e9, end);
	JAssertEqual(1.0e9, inc);

	J2DPlotWidget::FindRange(1.29834e-7, 35.8348652e-4, &start, &end, &inc);
	JAssertEqual(0.0, start);
	JAssertEqual(4.0e-3, end);
	JAssertEqual(1.0e-3, inc);
}
