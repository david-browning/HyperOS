#pragma once

#include "linked_list.h"
#include "kernel_context.h"

class ProcessBlocker
{
   public:
   ProcessBlocker();

   virtual ~ProcessBlocker();

   virtual void Block(_IN_POINTER_ context_t* proc_p);

   virtual void UnblockFirst();

   virtual void UnblockAll();

   private:
   ll_node_t<context_t*>* _blockedProcs_p;
};
