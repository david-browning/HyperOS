#include "proc_terminal.h"
#include "drvr_ps2kybd.h"
#include "snakes.h"
#include "kernel_procs.h"
#include "kernel_debug.h"

static constexpr char* EXIT_COMMAND = "exit";

void ClearConsole(VGADriver* vga_p)
{
   vga_p->Clear(VGA_MODES::VGA, VGA_COLORS::Blue);
   vga_p->WriteString(CONSOLE_DEFAULT_ATTRIBUTE, "OS> ");
}

void Terminal(void* arg)
{
   const size_t MAX_STR_INPUT = 512;
   char* input = new char[MAX_STR_INPUT];
   size_t charOffset = 0;

   //Get the necessary drivers.
   PS2KeyboardDriver* kb = GetKeyboardDriverPtr();
   VGADriver* vga = GetVGADriverPtr();

   ProcessManager* procManager = (ProcessManager*)arg;

   while (MemoryCompare<char>(input, EXIT_COMMAND, MemoryLength(EXIT_COMMAND)) != 0)
   {
      ClearConsole(vga);
      if (charOffset > 0)
      {
         if (MemoryCompare<char>(input, "snakes", MemoryLength("snakes")) == 0)
         {
            proc_id_t snakesProcID = setup_snakes(1);
            procManager->StartProcess(snakesProcID);
         }
         else
         {
            vga->WriteString(CONSOLE_ERROR_ATTRIBUTE, "Unknown command.\n");
            vga->WriteString(CONSOLE_DEFAULT_ATTRIBUTE, "OS> ");
         }
      }

      charOffset = 0;
      VIRTUAL_KEYS k;
      while ((k = kb->GetNextVirtualKey()) != VIRTUAL_KEYS::KEY_RETURN)
      {
         if (VirtualKeyIsPressed(k) && VirtualKeyIsPrintable(k))
         {
            if (charOffset == MAX_STR_INPUT)
            {
               vga->WriteString(CONSOLE_ERROR_ATTRIBUTE, "Input buffer is full. Press enter to clear.\n");
            }
            else
            {
               char keychar = VirtualKeyToCharacter(k);
               vga->WriteCharacter(CONSOLE_DEFAULT_ATTRIBUTE, keychar);

               //Append the character to the input.
               input[charOffset++] = keychar;
            }
         }
      }

      //Write the newline.
      vga->WriteCharacter(CONSOLE_DEFAULT_ATTRIBUTE, '\n');
   }

   delete[] input;
   vga->WriteString(CONSOLE_SUCCESS_ATTRIBUTE, "Turn off.\n");

   while (true);
}
