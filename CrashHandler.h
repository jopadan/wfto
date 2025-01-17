#ifndef CRASH_HANDLER_H
#define CRASH_HANDLER_H

#include <windows.h>
#include <dbghelp.h>
#include <stdio.h>

namespace utils
{
	namespace CrashHandler
	{
		void CreateMiniDump(EXCEPTION_POINTERS* pep);
		bool CALLBACK MyMiniDumpCallback(PVOID pParam, const PMINIDUMP_CALLBACK_INPUT pInput, PMINIDUMP_CALLBACK_OUTPUT pOutput); 
	
		/*typedef enum _MINIDUMP_TYPE {
			MiniDumpNormal                         = 0x00000000,
			MiniDumpWithDataSegs                   = 0x00000001,
			MiniDumpWithFullMemory                 = 0x00000002,
			MiniDumpWithHandleData                 = 0x00000004,
			MiniDumpFilterMemory                   = 0x00000008,
			MiniDumpScanMemory                     = 0x00000010,
			MiniDumpWithUnloadedModules            = 0x00000020,
			MiniDumpWithIndirectlyReferencedMemory = 0x00000040,
			MiniDumpFilterModulePaths              = 0x00000080,
			MiniDumpWithProcessThreadData          = 0x00000100,
			MiniDumpWithPrivateReadWriteMemory     = 0x00000200,
			MiniDumpWithoutOptionalData            = 0x00000400,
			MiniDumpWithFullMemoryInfo             = 0x00000800,
			MiniDumpWithThreadInfo                 = 0x00001000,
			MiniDumpWithCodeSegs                   = 0x00002000,
			MiniDumpWithoutManagedState            = 0x00004000,
		} MINIDUMP_TYPE;*/
	};
};
#endif //CRASH_HANDLER_H