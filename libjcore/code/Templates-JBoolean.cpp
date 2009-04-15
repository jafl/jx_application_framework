// This is not part of JBooleanIO.cc because Dustin Laurence needs to avoid
// dragging in all of JCore when linking his test programs.

#define JTemplateType JBoolean
#include <JCoreStdInc.h>
#include <JArray.tmpl>
#include <JRunArray.tmpls>
#include <JAuxTableData.tmpls>
#undef JTemplateType
