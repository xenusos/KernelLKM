/*
    Purpose: 
    Author: Reece W.
    License: All Rights Reserved J. Reece Wilson
*/

#include "common.h"

#include <linux/string.h>

#include "file_helper.h"
#include "pe_headers.h"
#include "bootstrapper.h"

int init_os_struct(linux_info_ref info)
{
	info->LINUX_THREAD_SIZE_ORDER          = THREAD_SIZE_ORDER;
	info->LINUX_THREAD_SIZE                = THREAD_SIZE;
	info->LINUX_PAGE_SHIFT                 = PAGE_SHIFT;
	info->LINUX_PAGE_SIZE                  = PAGE_SIZE;
	info->LINUX_PAGE_MASK                  = PAGE_MASK;
	info->LINUX_PMD_PAGE_SIZE              = PMD_PAGE_SIZE;
	info->LINUX_PMD_PAGE_MASK              = PMD_PAGE_MASK;
	info->LINUX_PUD_PAGE_SIZE              = PUD_PAGE_SIZE;
	info->LINUX_PUD_PAGE_MASK              = PUD_PAGE_MASK;
	info->LINUX___PHYSICAL_MASK            = __PHYSICAL_MASK;
	info->LINUX___VIRTUAL_MASK             = __VIRTUAL_MASK;
	info->LINUX_PHYSICAL_PAGE_MASK         = PHYSICAL_PAGE_MASK;
	info->LINUX_PHYSICAL_PMD_PAGE_MASK     = PHYSICAL_PMD_PAGE_MASK;
	info->LINUX_PHYSICAL_PUD_PAGE_MASK     = PHYSICAL_PUD_PAGE_MASK;
	info->LINUX_HPAGE_SHIFT                = HPAGE_SHIFT;
	info->LINUX_HPAGE_SIZE                 = HPAGE_SIZE;
	info->LINUX_HPAGE_MASK                 = HPAGE_MASK;
	info->LINUX_HUGETLB_PAGE_ORDER         = HUGETLB_PAGE_ORDER;
	info->LINUX_HUGE_MAX_HSTATE            = HUGE_MAX_HSTATE;
	info->LINUX_PAGE_OFFSET                = PAGE_OFFSET;
	info->LINUX___PHYSICAL_START           = __PHYSICAL_START;
	info->LINUX___START_KERNEL             = __START_KERNEL;
}
