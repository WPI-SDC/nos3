/*******************************************************************************
** File: patch_app.h
**
** Purpose:
**   This file is main hdr file for the PATCH application.
**
**
*******************************************************************************/

#ifndef _patch_app_h_
#define _patch_app_h_

/*
** Required header files.
*/
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include "osapi.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>

/***********************************************************************/

#define PATCH_PIPE_DEPTH                     32

/************************************************************************
** Type Definitions
*************************************************************************/

/****************************************************************************/
/*
** Local function prototypes.
**
** Note: Except for the entry point (PATCH_AppMain), these
**       functions are not called from any other source module.
*/
void patch_Main(void);
void PATCH_AppInit(void);
void PATCH_ProcessCommandPacket(void);
void PATCH_ProcessGroundCommand(void);
void PATCH_ReportHousekeeping(void);
void PATCH_ResetCounters(void);
void PATCH_ApplyPatch(void);

boolean PATCH_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength);

#endif /* _patch_app_h_ */
