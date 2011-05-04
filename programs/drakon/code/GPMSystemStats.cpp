/******************************************************************************
 GPMSystemStats.cpp

	Displays the CPU history and memory usage.

	BASE CLASS = JXWidget

	Copyright © 2011 by John Lindal. All rights reserved.

 ******************************************************************************/

#include "GPMSystemStats.h"
#include "GPMProcessList.h"
#include "gpmGlobals.h"
#include <JXWindowPainter.h>
#include <JColormap.h>
#include <JRegex.h>
#include <jStreamUtil.h>
#include <jAssert.h>

const JCoordinate kCPUHistoryBarWidth    = 2;
const JCoordinate kCPUHistoryMarginWidth = 1;
const JCoordinate kDisplayMarginWidth    = 5;

/******************************************************************************
 Constructor

 ******************************************************************************/

GPMSystemStats::GPMSystemStats
	(
	GPMProcessList*		processList,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h),
	itsUID(getuid()),
	itsProcessList(processList),
	itsMaxCPU(100)
{
	itsCPUHistory = new JArray<CPU>();
	assert( itsCPUHistory != NULL );

	ListenTo(itsProcessList);
}

/******************************************************************************
 Destructor

	We don't deallocate our colors because we always need the same ones
	and we will probably be used again later.

 ******************************************************************************/

GPMSystemStats::~GPMSystemStats()
{
	delete itsCPUHistory;
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
GPMSystemStats::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
GPMSystemStats::Draw
	(	
	JXWindowPainter&	p,
	const JRect&		clip
	)
{
	// update stats

	CPU cpuPercentage;
	JFloat userMemoryPercentage, otherMemoryPercentage;
	ComputeStats(&cpuPercentage.user, &cpuPercentage.other,
				 &userMemoryPercentage, &otherMemoryPercentage);

	itsCPUHistory->AppendElement(cpuPercentage);
	itsCPUHistory->RemoveElement(1);

	// detect multiple CPU's by looking for > 100% usages

	while (cpuPercentage.user + cpuPercentage.other > itsMaxCPU + 5)
		{
		itsMaxCPU += 100;
		}

	// render memory usage

	const JColormap* cmap = p.GetColormap();

	const JColorIndex otherMem = cmap->GetDarkRedColor();
	const JColorIndex userMem  = cmap->GetRedColor();
	const JColorIndex freeMem  = cmap->GetGreenColor();
	const JColorIndex cpuBack  = cmap->GetBlackColor();
	const JColorIndex userCPU  = cmap->GetGreenColor();
	const JColorIndex otherCPU = cmap->GetRedColor();

	const JRect bounds = GetBounds();

	JRect r = bounds;
	r.left  = r.right - bounds.height();
	r.Shift(-kDisplayMarginWidth, 0);

	const JFloat angle1 = 360 * otherMemoryPercentage / 100;
	const JFloat angle2 = 360 * userMemoryPercentage / 100;

	p.SetFilling(kJTrue);
	p.SetPenColor(otherMem);
	p.JPainter::Arc(r, 90, -angle1);
	p.SetPenColor(userMem);
	p.JPainter::Arc(r, 90 - angle1, -angle2);
	p.SetPenColor(freeMem);
	p.JPainter::Arc(r, 90 - angle1 - angle2, - 360 + angle1 + angle2);

	// render cpu history

	r        = bounds;
	r.left  += kDisplayMarginWidth;
	r.right -= bounds.height() + 2*kDisplayMarginWidth;

	p.SetPenColor(cpuBack);
	p.JPainter::Rect(r);

	r.Shrink(1,1);

	const JSize historyCount = r.width() / (kCPUHistoryBarWidth + kCPUHistoryMarginWidth);
	if (itsCPUHistory->GetElementCount() > historyCount)
		{
		itsCPUHistory->RemoveNextElements(1, itsCPUHistory->GetElementCount() - historyCount);
		}
	else
		{
		CPU blank;
		blank.user = blank.other = 0;
		while (itsCPUHistory->GetElementCount() < historyCount)
			{
			itsCPUHistory->PrependElement(blank);
			}
		}

	r.left              = r.right - kCPUHistoryBarWidth;
	const JCoordinate h = r.height();

	for (JIndex i=historyCount; i>=1; i--)
		{
		const CPU cpu = itsCPUHistory->GetElement(i);

		p.SetPenColor(userCPU);
		r.top = r.bottom - JRound(h * (cpu.user + cpu.other) / itsMaxCPU);
		p.JPainter::Rect(r);

		p.SetPenColor(otherCPU);
		r.top = r.bottom - JRound(h * cpu.other / itsMaxCPU);
		p.JPainter::Rect(r);

		r.Shift(-kCPUHistoryBarWidth-kCPUHistoryMarginWidth, 0);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GPMSystemStats::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsProcessList && message.Is(GPMProcessList::kListChanged))
		{
		Refresh();
		}
	else
		{
		JXWidget::Receive(sender, message);
		}
}

/******************************************************************************
 ComputeStats (private)

 ******************************************************************************/

static const JRegex freeMemoryPattern   = "^MemFree:\\s*([0-9]+)";
static const JRegex bufferMemoryPattern = "^Buffers:\\s*([0-9]+)";
static const JRegex cacheMemoryPattern  = "^Cached:\\s*([0-9]+)";

void
GPMSystemStats::ComputeStats
	(
	JFloat* userCPUPercentage,
	JFloat* otherCPUPercentage,
	JFloat* userMemoryPercentage,
	JFloat* otherMemoryPercentage
	)
	const
{
	*userCPUPercentage     = 0;
	*otherCPUPercentage    = 0;
	*userMemoryPercentage  = 0;
	*otherMemoryPercentage = 0;

	JSize count = itsProcessList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const GPMProcessEntry* e = itsProcessList->GetProcessEntry(i);
		if (e->GetUID() == itsUID)
			{
			*userCPUPercentage += e->GetPercentCPU();
			}
		else
			{
			*otherCPUPercentage += e->GetPercentCPU();
			}
		}

	const JPtrArray<GPMProcessEntry>& hidden = itsProcessList->GetHiddenProcesses();
	count = hidden.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const GPMProcessEntry* e = hidden.NthElement(i);
		*otherCPUPercentage     += e->GetPercentCPU();
		}

#ifdef _J_HAS_PROC
	{
	JSize totalMem, freeMem = 0, otherMem = 0, foundCount = 0;
	if (GPMGetSystemMemory(&totalMem))
		{
		ifstream ms("/proc/meminfo");
		JArray<JIndexRange> matchList;
		while (ms.good() && !ms.eof())
			{
			JString line = JReadLine(ms);
			if (freeMemoryPattern.Match(line, &matchList))
				{
				const JString s = line.GetSubstring(matchList.GetElement(2));
				s.ConvertToUInt(&freeMem);	// usually kB
				foundCount++;
				}
			else if (bufferMemoryPattern.Match(line, &matchList) ||
					 cacheMemoryPattern.Match(line, &matchList))
				{
				const JString s = line.GetSubstring(matchList.GetElement(2));
				JSize mem;
				s.ConvertToUInt(&mem);	// usually kB
				otherMem += mem;
				foundCount++;
				}

			if (foundCount >= 3)
				{
				break;
				}
			}

		*userMemoryPercentage  = 100 * (1 - (freeMem + otherMem) / JFloat(totalMem));
		*otherMemoryPercentage = 100 * otherMem / JFloat(totalMem);
		}
	}
#endif
}

#define JTemplateType GPMSystemStats::CPU
#include <JArray.tmpls>
#undef JTemplateType
