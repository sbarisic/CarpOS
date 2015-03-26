#pragma once
#include "CarpSDK.h"

typedef struct {
   uint Present : 1; // Page present in memory
   uint Writable : 1; // Read-only if clear, readwrite if set
   uint User : 1; // Supervisor level only if clear
   uint Accessed : 1; // Has the page been accessed since last refresh?
   uint Dirty : 1; // Has the page been written to since last refresh?
   uint Unused : 7; // Amalgamation of unused and reserved bits
   uint Frame : 20; // Frame address (shifted right 12 bits)
} Page;


typedef struct {
	Page Pages[1024];
} PageTable;


typedef struct {
	PageTable* Tables[1024];
	uint TablesPhysical[1024];
	uint Physical;
} PageDirectory;



class Paging {
public:
	static PageDirectory* CurrentDirectory;
	static PageDirectory* KernelDirectory;

	static void Map(void* Virtual, void* Physical);
	static void Init(uint MemLen);
	static void Enable();
	static void Disable();

	static void SetPageDir(PageDirectory* PageDir);

};