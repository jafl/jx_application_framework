#include <JCoreStdInc.h>
#include <JTextEditor.h>
#include <JTEUndoBase.h>

#define JTemplateType JTextEditor::Font
#include <JArray.tmpl>
#include <JRunArray.tmpls>
#undef JTemplateType

#define JTemplateType JTextEditor::Font,JArray<JTextEditor::Font>
#include <JStack.tmpls>
#undef JTemplateType

#define JTemplateType JTextEditor::LineGeometry
#include <JRunArray.tmpls>
#undef JTemplateType

#define JTemplateType JTEUndoBase
#include <JPtrArray.tmpls>
#undef JTemplateType

#define JTemplateType JTextEditor::CRMRule
#include <JArray.tmpls>
#undef JTemplateType

#define JTemplateType JTextEditor::HTMLListType
#include <JArray.tmpls>
#undef JTemplateType

#define JTemplateType JTextEditor::HTMLListType,JArray<JTextEditor::HTMLListType>
#include <JStack.tmpls>
#undef JTemplateType
