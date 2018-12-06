#include "kernel_debug.h"

void WaitForDebugger(_IN_POINTER_ VGADriver * const vgaDrvr)
{
	if (vgaDrvr)
	{
		vgaDrvr->WriteString(CONSOLE_ERROR_ATTRIBUTE, "\nWAITING FOR DEBUGGER.\n");
		vgaDrvr->WriteString(CONSOLE_ERROR_ATTRIBUTE, "Set \"wait\" to zero.\n");
	}
	else
	{
		char* str = "Waiting for Debugger Set \"wait\" to zero.";
		static constexpr uint16_t* mem = (uint16_t*)0xB80A0;
		for (auto i = 0; i < 40; i++)
		{
			mem[i] = vgaMakeChar(str[i], CONSOLE_ERROR_ATTRIBUTE);
		}
	}

	int wait = 1;
	while (wait);
}
