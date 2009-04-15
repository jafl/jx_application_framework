#include <JArray.tmpls>
#define JTemplateType long
#include <JLinkedList.tmpl>
#include <JLinkedListIterator.tmpl>

#undef JTemplateType
#define JTemplateType long,JArray<long>
#include <JStack.tmpls>
#include <JQueue.tmpls>
