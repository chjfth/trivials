#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

// [2025-01-17] Chj: NTFS sparse file related code assisted by GPT4o.

INT64 g_solidlen = 0;

void CopySparseRange(HANDLE hSource, HANDLE hDest, INT64 StartOffset, INT64 EndOffset_)
{
	INT64 rangebytes = EndOffset_ - StartOffset;
	UINT nMaxChunks = UINT(rangebytes / 65536);

	FILE_ALLOCATED_RANGE_BUFFER *outranges = new FILE_ALLOCATED_RANGE_BUFFER[nMaxChunks];

	// Query allocated ranges in the source file
	FILE_ALLOCATED_RANGE_BUFFER queryRange = {};
	queryRange.FileOffset.QuadPart = StartOffset;
	queryRange.Length.QuadPart = rangebytes;
	DWORD bytesReturned = 0;

	if (DeviceIoControl(hSource, FSCTL_QUERY_ALLOCATED_RANGES, 
		&queryRange, sizeof(queryRange),
		outranges, nMaxChunks*sizeof(FILE_ALLOCATED_RANGE_BUFFER), 
		&bytesReturned, NULL)) 
	{
		int rangeCount = bytesReturned / sizeof(FILE_ALLOCATED_RANGE_BUFFER);

		// Copy each allocated range
		for (int i = 0; i < rangeCount; ++i) 
		{
			LARGE_INTEGER offset = outranges[i].FileOffset;

			// Move the file pointer of the source & dest file
			SetFilePointerEx(hSource, offset, NULL, FILE_BEGIN);
			SetFilePointerEx(hDest, offset, NULL, FILE_BEGIN);

			char buffer[65536]; // 64KB buffer
			DWORD ThisRangeRemains = static_cast<DWORD>(outranges[i].Length.QuadPart);
			DWORD readBytes = 0, writtenBytes = 0;

			while (ThisRangeRemains > 0) {

				DWORD nowToRead = (ThisRangeRemains < sizeof(buffer)) ? ThisRangeRemains : sizeof(buffer);
				if (!ReadFile(hSource, buffer, nowToRead, &readBytes, NULL)) {
					_tprintf(_T("Failed to read from source file @%I64d, +%u. WinErr: %d\n"),
						offset.QuadPart, ThisRangeRemains,
						GetLastError());
					exit(4);
				}

				if(nowToRead!=readBytes)
				{
					_tprintf(_T("Unexpect! ReadFile() reports wrong readBytes. Expect %u, actual %u\n"), 
						ThisRangeRemains, readBytes);
					exit(4);
				}

				if (!WriteFile(hDest, buffer, readBytes, &writtenBytes, NULL)) {
					_tprintf(_T("Failed to write to destination file @%I64d, +%u. WinErr: %d\n"),
						offset.QuadPart, readBytes,
						GetLastError());
					exit(4);
				}

				if(readBytes!=writtenBytes)
				{
					_tprintf(_T("Unexpect! WritFile() reports wrong writtenBytes. Expect %u, actual %u\n"), 
						readBytes, writtenBytes);
					exit(4);
				}

				g_solidlen += writtenBytes;

				ThisRangeRemains -= writtenBytes;
			}
		}
	} else {
		_tprintf(_T("Unexpect error on FSCTL_QUERY_ALLOCATED_RANGES. WinErr: %d\n"), GetLastError());
		exit(4);
	}

	delete []outranges;
}

void CopySparseFile(const TCHAR* sourceFile, const TCHAR* destFile) {
	// Open the source file
	HANDLE hSource = CreateFile(sourceFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hSource == INVALID_HANDLE_VALUE) {
		_tprintf(_T("Failed to open source file. WinErr: %d\n"),  GetLastError());
		return;
	}

	// Get the sparse attribute of the source file
	FILE_ATTRIBUTE_TAG_INFO tagInfo;
	if (!GetFileInformationByHandleEx(hSource, FileAttributeTagInfo, &tagInfo, sizeof(tagInfo))) {
		_tprintf(_T("GetFileInformationByHandleEx() fail. WinErr: %d\n"), GetLastError());
		CloseHandle(hSource);
		return;
	}

	bool isSparse = (tagInfo.FileAttributes & FILE_ATTRIBUTE_SPARSE_FILE) != 0;

	// Create the destination file
	HANDLE hDest = CreateFile(destFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDest == INVALID_HANDLE_VALUE) {
		_tprintf(_T("Failed to create destination file. WinErr: %d\n"), GetLastError());
		CloseHandle(hSource);
		return;
	}

	// If the source file is sparse, make the destination file sparse
	if (isSparse) {
		DWORD temp;
		if (!DeviceIoControl(hDest, FSCTL_SET_SPARSE, NULL, 0, NULL, 0, &temp, NULL)) {
			_tprintf(_T("Failed to set sparse attribute on destination file. WinErr: %d\n"), GetLastError());
			CloseHandle(hSource);
			CloseHandle(hDest);
			return;
		}
	}

	const int ONE_RANGE_SIZE = 1024*1024;
	LARGE_INTEGER li = {};
	GetFileSizeEx(hSource, &li);
	INT64 filesize = li.QuadPart;
	INT64 remain = filesize;
	INT64 offset = 0;

	while(remain>0)
	{
		int nowcopys = remain<ONE_RANGE_SIZE ? (int)remain : ONE_RANGE_SIZE;

		CopySparseRange(hSource, hDest, offset, offset+nowcopys);

		offset += nowcopys;
		remain -= nowcopys;
	}

	// Clean up
	CloseHandle(hSource);
	CloseHandle(hDest);

	_tprintf(_T("Copy operation completed.\n")
		_T("File length: %I64d \n")
		_T("Solid bytes: %I64d \n")
		,
		filesize, g_solidlen);
}

int _tmain(int argc, TCHAR *argv[]) 
{
	if(argc<3)
	{
		_tprintf(_T("This program copies an NTFS file preserving its sparse attribute.\n"));
		_tprintf(_T("Usage:\n"));
		_tprintf(_T("    copy-ntfs-sparse <source-file> <destination-file>\n"));
		exit(1);
	}

	CopySparseFile(argv[1], argv[2]);
	return 0;
}
