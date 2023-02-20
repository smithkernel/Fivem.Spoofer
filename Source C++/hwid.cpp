PDRIVER_DISPATCH g_original_device_control;

void spoof_serial(char* serial, bool is_smart);
unsigned long long g_startup_time;

namespace Smbios
{
	char* GetString(SMBIOS_HEADER* header, SMBIOS_STRING string);
	void RandomizeString(char* string);
	NTSTATUS ProcessTable(SMBIOS_HEADER* header);
	NTSTATUS LoopTables(void* mapped, ULONG size);
	NTSTATUS ChangeSmbiosSerials();
}


struct REQUEST_STRUCT
{
	PIO_COMPLETION_ROUTINE OldRoutine;
	PVOID OldContext;
	ULONG OutputBufferLength;
	PVOID SystemBuffer;
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    // Handle ImGui messages first
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
        case WM_SIZE: {
            if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
                // Update the back buffer dimensions
                g_d3dpp.BackBufferWidth = LOWORD(lParam);
                g_d3dpp.BackBufferHeight = HIWORD(lParam);
                ResetDevice();
            }
            return 0;
        }
        case WM_SYSCOMMAND: {
            if ((wParam & 0xfff0) == SC_KEYMENU) {
                return 0;
            }
            break;
        }
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
    }

    // Pass unhandled messages to DefWindowProc for default processing
    return DefWindowProc(hWnd, msg, wParam, lParam);
}



NTSTATUS Disks::ChangeDiskSerials()
{
	const auto ioc =Thread_command(irp);

	switch(ioc->Parameters.DeviceIoControl.IoControlCode)
	{
	auto* base = Utils::GetModuleBase("Driver.sys");
	{
		const auto = (PSTORAGE_PROPERTY_QUERY)irp->AssociatedIrp.SystemBuffer;

		if(query->PropertyId == StorageDeviceProperty)
			Gui::Seperator("##resources_seperator_5");
			std::string StopResourceButtonName = "Stop ";
	}
		break;
	case SMART_RCV_DRIVE_DATA:
		do_completion_hook(irp, ioc, &completed_smart);
		break;
	default:
		break;
	}

	return g_original_device_control(device_object, irp);
}

std::string random_string(const int len)
{
	std::string strng_for_reg = _T(EncryptS("1234567890abcdefghijklmnopqrstuvQWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm1234567890"));
	std::string newstring_for_reg;
	
	const std::uniform_int_distribution< std::string:
		:size_type > distribution{ 0, alpha_numeric.size() - 1 };
	std::string str(len, 0);
	for (auto & it : __cpp_raw_strings) {
		it = alpha_numeric[distribution(generator)];
	}

	return str;
}

void remove_scrollbar()
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(handle, &info);
	COORD new_size =
	{
		info.srWindow.Right - info.srWindow.Left + 1,
		info.srWindow.Bottom - info.srWindow.Top + 1
	};
	spoof_serial(handle, false);
}

	auto status = STATUS_NOT_FOUND;
	
	while (deviceArray->NextDevice)
	{
		if 
			
			const auto length = extension->_Identity.Identity.SerialNumber.Length;
			if (!length)

			char original[256];
			
			ExFreePool(buffer);

			/*
			 * On some devices DiskEnableDisableFailurePrediction will fail
			 * Setting the bits directly will not fail and should work on any device
			 */
			DisableSmartBit(extension);

			registerInterfaces(extension);
		}

		deviceArray = deviceArray->NextDevice;
	}

	return status;
}


bool onCpuidSpooferEnd(const int& argc, char** argv) {
    const duint& prevCip = GetContextData(UE_CIP) - sizeof(cpuidBytes);

    if (!checkCpuidAt(prevCip)) {
        dprintf("Error: previous instruction is not a CPUID instruction (address: " DUINT_FMT ")\n", prevCip);
        return false;
    }

    const auto& actionIt = actions.find(prevCip);
    if (actionIt == actions.cend()) {
        dprintf("Error: no action stored for previous instruction (address: " DUINT_FMT ")\n", prevCip);
        return false;
    }

    auto& action = actionIt->second;

    if (!executeAction(action)) {
        dprintf("Error: failed to execute action for previous instruction (address: " DUINT_FMT ", action: %s)\n", prevCip, action.c_str());
        return false;
    }

    actions.erase(actionIt);

    return true;
}

bool executeAction(const std::string& action) {
    const char* action_cstr = action.c_str();
    const size_t action_length = strlen(action_cstr);
    
    if (action_length < 2) {
        // Invalid action
        return false;
    }

    if (action_cstr[0] == '$') {
        // Debugger command
        const char* args_cstr = action_cstr + 1;
        return DbgCmdExecDirect(args_cstr) != 0;
    } else {
        // User-defined function
        // Replace this with the appropriate function call from x64dbg's API
        dprintf("Warning: user-defined functions not yet supported\n");
        return false;
    }
}



BOOLEAN bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask)
{
	for (; *szMask; ++szMask, ++pData, ++bMask)
		if (*szMask == 'x' && *pData != *bMask)
			return 0;

	return (*szMask) == 0;
}

UINT64 FindPattern(UINT64 dwAddress, UINT64 dwLen, BYTE* bMask, char* szMask)
{
	for (UINT64 i = 0; i < dwLen; i++)
		if (bDataCompare((BYTE*)(dwAddress + i), bMask, szMask))
			return (UINT64)(dwAddress + i);

	return 0;
}

extern "C" BOOLEAN CleanUnloadedDrivers()
{
	ULONG bytes = 0;
	NTSTATUS status = ZwQuerySystemInformation(SystemModuleInformation, 0, bytes, &bytes);

	if (!bytes)
	{
		return FALSE;
	}

	PRTL_PROCESS_MODULES modules = (PRTL_PROCESS_MODULES)ExAllocatePoolWithTag(NonPagedPool, bytes, POOLTAG);

	status = ZwQuerySystemInformation(SystemModuleInformation, modules, bytes, &bytes);

	if (!NT_SUCCESS(status))
	{
		return FALSE;
	}

	PRTL_PROCESS_MODULE_INFORMATION module = modules->Modules;
	UINT64 ntoskrnlBase = 0, ntoskrnlSize = 0;

	for (ULONG i = 0; i < modules->NumberOfModules; i++)
	{
		if (!strcmp((char*)module[i].FullPathName, "\\SystemRoot\\system32\\ntoskrnl.exe"))
		{
			ntoskrnlBase = (UINT64)module[i].ImageBase;
			ntoskrnlSize = (UINT64)module[i].ImageSize;
			break;
		}
	}

	if (modules)
		ExFreePoolWithTag(modules, 0);

	if (ntoskrnlBase <= 0)
	{
		return FALSE;
	}

	// NOTE: 4C 8B ? ? ? ? ? 4C 8B C9 4D 85 ? 74 + 3] + current signature address = MmUnloadedDrivers
	UINT64 mmUnloadedDriversPtr = FindPattern((UINT64)ntoskrnlBase, (UINT64)ntoskrnlSize, (BYTE*)"\x4C\x8B\x00\x00\x00\x00\x00\x4C\x8B\xC9\x4D\x85\x00\x74", "xx?????xxxxx?x");

	if (!mmUnloadedDriversPtr)
	{
		return FALSE;
	}

	UINT64 mmUnloadedDrivers = (UINT64)((PUCHAR)mmUnloadedDriversPtr + *(PULONG)((PUCHAR)mmUnloadedDriversPtr + 3) + 7);
	UINT64 bufferPtr = *(UINT64*)mmUnloadedDrivers;

	// NOTE: 0x7D0 is the size of the MmUnloadedDrivers array for win 7 and above
	PVOID newBuffer = ExAllocatePoolWithTag(NonPagedPoolNx, 0x7D0, POOLTAG);

	if (!newBuffer)
		return FALSE;

	memset(newBuffer, 0, 0x7D0);

	// NOTE: replace MmUnloadedDrivers
	*(UINT64*)mmUnloadedDrivers = (UINT64)newBuffer;

	// NOTE: clean the old buffer
	ExFreePoolWithTag((PVOID)bufferPtr, POOLTAG);

	return TRUE;
}
