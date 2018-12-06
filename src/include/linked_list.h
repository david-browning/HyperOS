#pragma once

#include "annotations.h"

template<typename T>
struct ll_node_t
{
   ll_node_t<T>* Next;
   T Data;

   ll_node_t(const T* data_p)
   {
      Data = *data_p;
      Next = nullptr;
   }
};

template<typename T>
inline result_t LLInsertLast(ll_node_t<T>* const head, const T* data_p)
{
   ll_node_t<T>* newNode = new ll_node_t<T>(data_p);
   if (!head || newNode == nullptr)
   {
      return RES_OUT_OF_MEMORY;
   }

   //Search for the last node.
   ll_node_t<T>* curNode = head;
   while (curNode->Next)
   {
      curNode = curNode->Next;
   }

   curNode->Next = newNode;

   return RES_SUCCESS;
}

template<typename T>
inline void LLRemove(ll_node_t<T>* head, T* toRemove)
{
   ll_node_t<T>* cur = head;
   ll_node_t<T>* prev = nullptr;

   for (cur = head; cur != nullptr; prev = cur, cur = cur->Next)
   {
      if (cur->Data == *toRemove)
      {
         if (prev == nullptr)
         {
            //Found
            head = cur->Next;
         }
         else
         {
            prev->Next = cur->Next;
            delete cur;
         }
      }
   }
}

template<typename T>
inline void LLFree(ll_node_t<T>* head)
{
   ll_node_t<T>* lastNode = head;
   ll_node_t<T>* curNode = head;

   while (curNode)
   {
      lastNode = curNode;
      curNode = curNode->Next;
      delete lastNode;
   }
}


