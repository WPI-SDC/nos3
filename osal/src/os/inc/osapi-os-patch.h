/*
 * File: osapi-os-patch.h
 *
 * Author: Samuel Goldman
 *
 * Purpose: contains prototype function defintions for the OS Abstraction Layer
 * 	    Patch OS module
 *
 */

#ifndef _osapi_patch_
#define _osapi_patch_

#include "common_types.h"

/*
 * Function: OS_ApplyPatch
 */
int32 OS_ApplyPatch( uint8 *data, uint32 length, uint32 location);

#endif
