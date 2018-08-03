/*
	Purpose: I don't even care anymore. Just load the fucking PE. This implementation is no longer used within the kernel itself. TODO: port the other PE loader.
	Author: Reece W.
	License: All Rights Reserved J. Reece Wilson
*/

#include <linux/types.h> 
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/string.h>

#include "pe_headers.h"
#include "pe_loader.h"

#define HIWORD(l)		((uint16_t)((((uint32_t)(l)) >> 16) & 0xFFFF))
#define LOWORD(l)		((uint16_t)(uint32_t)(l))
#define HIBYTE(w)		((uint8_t)(((uint16_t)(w) >> 8) & 0xFF))
#define LOBYTE(w)		((uint8_t)(w))
#define MAKELONG(low,hi)	((uint32_t)(((uint16_t)(low))|(((uint32_t)((uint16_t)(hi)))<<16)))
#define MAKEWORD(low,hi)	((uint16_t)(((uint8_t)(low))|(((uint16_t)((uint8_t)(hi)))<<8)))


#define RVA(type, off) (type)((uint64_t)baddr + off) 
	
void relocate_kern(
	void * baddr,
	PIMAGE_NT_HEADERS ntheader,
	uint64_t difference)
{
	size_t base;
	size_t items;
	uint16_t * map;
	size_t i;
	IMAGE_DATA_DIRECTORY dir;
	IMAGE_BASE_RELOCATION *base_relocation;
	
	dir = ntheader->OptionalHeader64.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];

	if (!(dir.Size)) return;
	
	base_relocation = RVA(PIMAGE_BASE_RELOCATION, dir.VirtualAddress);
	
	while (base_relocation->SizeOfBlock) 
	{
		map = (uint16_t *)(((uint64_t)(base_relocation)) + sizeof(IMAGE_BASE_RELOCATION)); 
		items = (base_relocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(IMAGE_RELOC);

		base = RVA(size_t, base_relocation->VirtualAddress);

		// https://github.com/abhisek/Pe-Loader-Sample/blob/master/src/PeLdr.cpp
		// TODO: rewrite
		for (i = 0; i < items; i++) 
		{
			IMAGE_RELOC reloc;
			reloc = *(PIMAGE_RELOC)&map[i];
			switch (reloc.type) 
			{
			case IMAGE_REL_BASED_DIR64:
				*((size_t *)	(base + reloc.offset)) += difference;
				break;
			case IMAGE_REL_BASED_HIGHLOW:
				*((uint32_t *)	(base + reloc.offset)) += (uint32_t) difference;
				break;
			case IMAGE_REL_BASED_HIGH:
				*((uint16_t *)	(base + reloc.offset)) += HIWORD(difference);
				break;
			case IMAGE_REL_BASED_LOW:
				*((uint16_t *)	(base + reloc.offset)) += LOWORD(difference);
				break;
			case IMAGE_REL_BASED_ABSOLUTE:
				break;
			default:
				printk("Unknown relocation type: 0x%08x \n", reloc.type);
				break;
			}
		}
		base_relocation = (PIMAGE_BASE_RELOCATION)(((size_t)base_relocation) + base_relocation->SizeOfBlock);
	}
}
#undef RVA

void * load_pe(void * data, size_t length, void **entrypoint)
{ 
	void * baddr;
	uint64_t difference;
	PDOS_HEADER dheader;
	PIMAGE_NT_HEADERS ntheader;
	uint64_t image_size;
	int x;
  
	dheader		= (PDOS_HEADER)data;
	ntheader	= (PIMAGE_NT_HEADERS)((uint64_t)data + dheader->e_lfanew);
	
	image_size	= ntheader->OptionalHeader64.SizeOfImage;
	
	baddr		= memset(__vmalloc(image_size, GFP_KERNEL, PAGE_KERNEL_EXEC), 0, image_size);
	
	if (!baddr)
		return NULL;
	
	//TODO: check pe sig
	//TODO: check mz sig

	difference = (uint64_t) baddr - (uint64_t) ntheader->OptionalHeader64.ImageBase;
	
	//Move header
	memcpy((void *)baddr, (const void *) data, ntheader->OptionalHeader64.SizeOfHeaders);
	
	//Update pointers
	dheader = (PDOS_HEADER)baddr;
	ntheader = (PIMAGE_NT_HEADERS)((uint64_t)baddr + dheader->e_lfanew);
	
	//Update PE/DOS headers 
	ntheader->OptionalHeader64.ImageBase = baddr;
	
	//Load sections
	PIMAGE_SECTION_HEADER sec;
	size_t sections;
	
	sec = (PIMAGE_SECTION_HEADER)((uint64_t)&ntheader->OptionalHeader64 + ntheader->FileHeader.SizeOfOptionalHeader);
	sections = ntheader->FileHeader.NumberOfSections;
	for (x = 0; x < sections; x++)
		memcpy((void *)((uint64_t)baddr + sec[x].VirtualAddress), (const void *) ((uint64_t)data + sec[x].PointerToRawData), sec[x].SizeOfRawData);
	
	//TODO: check length
	//TODO: check within range

	relocate_kern(baddr, ntheader, difference);																												;
   
	if (entrypoint)
		*entrypoint = baddr + ntheader->OptionalHeader.AddressOfEntryPoint;
	
	return baddr;
}
