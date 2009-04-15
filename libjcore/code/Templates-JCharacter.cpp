#include <JCoreStdInc.h>
#include <jTypes.h>
#define JTemplateType JCharacter
#include <JArray.tmpls>

#undef JTemplateType
#define JTemplateType JCharacter,JArray<JCharacter>
#include <JStack.tmpls>
