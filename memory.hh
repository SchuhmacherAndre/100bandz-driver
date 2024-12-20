#ifndef MEMORY_HH
#define MEMORY_HH

#pragma once

class c_memory {
private:
	int PAGE_OFFSET_SIZE = 12;
	UINT64 PHYS_ADDR_MASK = (~0xfull << 8) & 0xfffffffffull;


public:
	NTSTATUS read_physical_addr(PVOID targetAddr, PVOID outBuffer, SIZE_T size, SIZE_T* bytesRead);
	NTSTATUS write_physical_addr(PVOID targetAddr, PVOID inBuffer, SIZE_T size, SIZE_T* bytesWritten);

	NTSTATUS read_virtual_mem(UINT64 dirBase, UINT64 addr, PUINT8 buffer, SIZE_T size, SIZE_T* bytesRead);
	NTSTATUS write_virtual_mem(UINT64 dirBase, UINT64 addr, PUINT8 buffer, SIZE_T size, SIZE_T* bytesWritten);

	UINT64 translate_linear_addr(UINT64 dirTableBase, UINT64 virtAddr);
	ULONG_PTR get_proc_cr3(PEPROCESS targetProc);

};
extern c_memory* memory;



#endif // MEMORY_HH