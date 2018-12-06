#include "kernel_driver_manager.h"
#include "mem.h"

DriverManager::DriverManager()
{
   _numDrivers = 0;
   _driversArraySize = 0;
   _drivers = nullptr;
   _stacks_p = nullptr;
}

result_t DriverManager::Initialize(_IN_POINTER_ StackAllocator* stackAllocator_p)
{
   _stacks_p = stackAllocator_p;
   _driversArraySize = DriverManager::_initialDriverCount;

   //Allocate memory for the list of drivers.
   _drivers = MemoryAllocate<IDeviceDriver*>(sizeof(IDeviceDriver) * _driversArraySize);
   if (_drivers == nullptr)
   {
      return RES_OUT_OF_MEMORY;
   }

   _isLoaded = true;
   return RES_SUCCESS;
}

result_t DriverManager::InstallDriver(_IN_POINTER_ IDeviceDriver* driver_p,
                                      _IN_SIZE_ size_t driverSize,
                                      _OUT_HANDLE_ handle_t* devHandle_p)
{
   result_t res = RES_SUCCESS;
   if (_numDrivers == _driversArraySize)
   {
      return RES_OUT_OF_MEMORY;
   }

   auto i = 0;
   while (_drivers[i])
   {
      i++;
   }










   _numDrivers++;
   return RES_SUCCESS;

cleanup1:




   return res;
}

result_t DriverManager::RemoveDriver(_IN_HANDLE_ handle_t devHandle)
{
   result_t res = _drivers[devHandle]->Unload();
   _drivers[devHandle] = nullptr;
   _numDrivers--;

   return res;
}

result_t DriverManager::SwapIn(_IN_POINTER_ kernel_swap_state_t* state_p)
{
   result_t res = RES_SUCCESS;

   //Call swap in on all the drivers.
   for (auto i = 0; i < _numDrivers; i++)
   {
      kernel_swap_state_t state;
      if (_drivers[i])
      {
         if (ResultFailed(res = _drivers[i]->SwapIn(&state)))
         {
            return res;
         }
      }
   }

   return RES_SUCCESS;
}

result_t DriverManager::SwapOut(_IN_POINTER_ kernel_swap_state_t * state_p)
{
   result_t res = RES_SUCCESS;

   //Call swap out on all the drivers.
   for (auto i = 0; i < _numDrivers; i++)
   {
      kernel_swap_state_t state;
      if (_drivers[i])
      {
         if (ResultFailed(res = _drivers[i]->SwapOut(&state)))
         {
            return res;
         }
      }
   }

   return RES_SUCCESS;
}
