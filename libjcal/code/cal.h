/******************************************************************************
 cal.h

 ******************************************************************************/

#ifndef _H_GCal
#define _H_GCal

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

void	day_array(int month, int year, int* days);
int		day_in_week (int day, int month, int year);

#endif
