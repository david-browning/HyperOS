#pragma once
#include "annotations.h"
#include "ischeduler.h"
#include "kernel_context.h"
#include "mem.h"
#include "linked_list.h"

template<typename T>
class RRScheduler : public IScheduler<T>
{
   public:
   virtual result_t Initialize()
   {
      _numProcs = 32;
      this->_listHead = _curPtr = nullptr;
      _blockedProcs = new ll_node_t<_blockedObject>*[_numProcs];
      return RES_SUCCESS;
   }

   virtual T* GetNext()
   {
      ll_node_t<T>* ret = _curPtr;
      if ((_curPtr = _curPtr->Next) == nullptr)
      {
         _curPtr = _listHead;
      }

      return &ret->Data;
   }

   virtual void Insert(T* obj)
   {
      if (!_listHead)
      {
         _curPtr = _listHead = new ll_node_t<T>(obj);
      }
      else
      {
         LLInsertLast(_listHead, obj);
      }
   }

   virtual void Remove(T* obj)
   {
      LLRemove(_listHead, obj);
   }

   virtual result_t SwapIn(_IN_POINTER_ kernel_swap_state_t* state_p)
   {
      return RES_SUCCESS;
   }

   virtual result_t SwapOut(_IN_POINTER_ kernel_swap_state_t* state_p)
   {
      return RES_SUCCESS;
   }

   virtual bool IsEmpty()
   {
      return _listHead == nullptr;
   }

   private:

   struct _blockedObject
   {
      T* Object;
      bool IsBlocked;
   };

   ll_node_t<T>* _getLast()
   {
      if (!_listHead)
      {
         return nullptr;
      }

      ll_node_t<T>* cur = _listHead;
      while (cur->Next)
      {
         cur = cur->Next;
      }

      return cur;
   }

   ll_node_t<T>* _listHead;
   ll_node_t<T>* _curPtr;
   ll_node_t<_blockedObject>** _blockedProcs;
   size_t _numProcs;


};