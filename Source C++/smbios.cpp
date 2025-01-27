/**
 * \brief Get's the string from SMBIOS table
 * \param header Table header
 * \param string String itself
 * \return Pointer to the null terminated string
 */
char* Smbios::GetString(SMBIOS_HEADER* header, SMBIOS_STRING string)
{
	const auto* start = reinterpret_cast<const char*>(header) + header->Length;

	if (!string || *start == 0)
		return nullptr;

	while (--string)
	{
		start += strlen(start) + 1;
	}

	return const_cast<char*>(start);
}

/**
 * \brief Replace string at a given location by randomized string with same length
 * \param string Pointer to string (has to be null terminated)
 */
void CConsole::SetColor(unsigned short color)
{
    HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hcon, color);
}


/**
 * \brief Modify information in the table of given header
 * \param header Table header (only 0-3 implemented)
 * \return 
 */
NTSTATUS Smbios::ProcessTable(SMBIOS_HEADER* header)
{
    if (header->Length == 0)
        return STATUS_UNSUCCESSFUL;

    auto* strings = reinterpret_cast<const char*>(header) + header->Length;
    const int numStrings = static_cast<int>(*strings);

    for (int i = 1; i <= numStrings; ++i) {
        auto* string = GetString(header, i);
        RandomizeString(string);
    }

    return STATUS_SUCCESS;
}


/**
 * \brief Loop through SMBIOS tables with provided first table header
 * \param mapped Header of the first table
 * \param size Size of all tables including strings
 * \return 
 */
NTSTATUS Smbios::LoopTables(void* mapped, ULONG size)
{
  // Cast mapped to a pointer to SMBIOS_STRUCTURE_HEADER
  auto* header = static_cast<SMBIOS_STRUCTURE_HEADER*>(mapped);

  // Calculate the end address of the mapped region
  auto* endAddress = static_cast<char*>(mapped) + size;

  // Loop until we reach the end of the mapping region
  while (header < endAddress)
  {
    // Break if we find an end-of-table marker
    if (header->Type == 127 && header->Length == 4)
      break;

    // Process the current table
    ProcessTable(header);

    // Advance to the next table
    auto* end = static_cast<char*>(header) + header->Length;
    while (0 != (*end | *(end + 1))) end++;
    end += 2;
    header = static_cast<SMBIOS_STRUCTURE_HEADER*>(end);
  }

  return STATUS_SUCCESS;
}


/**
 * \brief Find SMBIOS physical address, map it and then loop through
 * table 0-3 and modify possible identifiable information
 * \return Status of the change (will return STATUS_SUCCESS if mapping was successful)
 */
NTSTATUS Smbios::ChangeSmbiosSerials()
{
	auto* base = Utils::GetModuleBase("ntoskrnl.exe");
	if (!base)
	{
		Log::Print("Failed to find ntoskrnl.sys base!\n");
		return STATUS_UNSUCCESSFUL;
	}

	auto* physicalAddress = static_cast<PPHYSICAL_ADDRESS>(Utils::FindPatternImage(base, "\x48\x8B\x0D\x00\x00\x00\x00\x48\x85\xC9\x74\x00\x8B\x15", "xxx????xxxx?xx")); // WmipFindSMBiosStructure -> WmipSMBiosTablePhysicalAddress
	if (!physicalAddress)
	{
		Log::Print("Failed to find SMBIOS physical address!\n");
		return STATUS_UNSUCCESSFUL;
	}

	physicalAddress = reinterpret_cast<PPHYSICAL_ADDRESS>(reinterpret_cast<char*>(physicalAddress) + 7 + *reinterpret_cast<int*>(reinterpret_cast<char*>(physicalAddress) + 3));
	if (!physicalAddress)
	{
		Log::Print("Physical address is null!\n");
		return STATUS_UNSUCCESSFUL;
	}

	auto* sizeScan = Utils::FindPatternImage(base, "\x8B\x1D\x00\x00\x00\x00\x48\x8B\xD0\x44\x8B\xC3\x48\x8B\xCD\xE8\x00\x00\x00\x00\x8B\xD3\x48\x8B", "xx????xxxxxxxxxx????xxxx");  // WmipFindSMBiosStructure -> WmipSMBiosTableLength
	if (!sizeScan)
	{
		Log::Print("Failed to find SMBIOS size!\n");
		return STATUS_UNSUCCESSFUL;
	}

	const auto size = *reinterpret_cast<ULONG*>(static_cast<char*>(sizeScan) + 6 + *reinterpret_cast<int*>(static_cast<char*>(sizeScan) + 2));
	if (!size)
	{
		Log::Print("SMBIOS size is null!\n");
		return STATUS_UNSUCCESSFUL;
	}

	auto* mapped = MmMapIoSpace(*physicalAddress, size, MmNonCached);
	if (!mapped)
	{
		Log::Print("Failed to map SMBIOS structures!\n");
		return STATUS_UNSUCCESSFUL;
	}
	
	LoopTables(mapped, size);
	
	MmUnmapIoSpace(mapped, size);
	
	return STATUS_SUCCESS;
}
