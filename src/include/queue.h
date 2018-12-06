#pragma once

#include "annotations.h"

template<typename T>
class Queue
{
   public:
   Queue()
   {
      _head = _tail = 0;
      _objs = new T[32];
      _count = 32;
   }

   virtual ~Queue()
   {
      delete[] _objs;
   }


   void Pop(T* front_p)
   {
      *front_p = _objs[_head++];
      if (_head == _count)
      {
         _head = 0;
      }
   }

   void Push(T* back_p)
   {
      _objs[_tail++] = *back_p;
      if (_tail == _count)
      {
         _tail = 0;
      }
   }

   bool IsEmpty()
   {
      return _head == _tail;
   }

   private:
   size_t _count;
   T* _objs;

   size_t _head;
   size_t _tail;
};