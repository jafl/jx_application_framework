#include <JCoreStdInc.h>
#include <ace/SOCK_Stream.h>
#define JTemplateType ACE_SOCK_STREAM
#include "evt_handlers.tmpls"
#undef JTemplateType

template class ACE_Task<ACE_NULL_SYNCH>;
template class ACE_Message_Queue<ACE_NULL_SYNCH>;
template class ACE_Module<ACE_NULL_SYNCH>;
template class ACE_Thru_Task<ACE_NULL_SYNCH>;
template class ACE_Unbounded_Set<int>;
template class ACE_Unbounded_Set_Iterator<int>;
template class ACE_Unbounded_Set_Ex<int, ACE_Unbounded_Set_Default_Comparator<int> >;
template class ACE_Node<int, ACE_Unbounded_Set_Default_Comparator<int> >;
template class ACE_Unbounded_Set_Ex_Iterator<int, ACE_Unbounded_Set_Default_Comparator<int> >;
template class ACE_Unbounded_Set_Ex_Const_Iterator<int, ACE_Unbounded_Set_Default_Comparator<int> >;
