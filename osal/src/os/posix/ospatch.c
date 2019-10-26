/*
 * File: ospatch.c
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#include "common_types.h"
#include "osapi.h"
#include "osapi-os-patch.h"


// First address in the executable
extern char __executable_start;


/*
 * Function: OS_ApplyPatch
 */
int32 OS_ApplyPatch( uint8 *data, uint32 length, uint32 location) {
    
    // Find page size for this system.
    size_t pagesize = sysconf(_SC_PAGESIZE);

    // Calculate start and end address for the write
    uintptr_t start = &__executable_start + location;
    uintptr_t end   = start + length;

    // Calculate start of page for mprotect
    uintptr_t pagestart = start & -pagesize;

    // Change memory protection to allow for write
    if (mprotect((void *) pagestart, end - pagestart, PROT_READ | PROT_WRITE | PROT_EXEC)) {
        return OS_ERR_MPROTECT;
    }

    // Copy the new values
    memcpy((void *) start, data, length);

    // Restore standard protections
    mprotect((void *) pagestart, end - pagestart, PROT_READ | PROT_EXEC);
    
    // Success
    return OS_SUCCESS;
}
