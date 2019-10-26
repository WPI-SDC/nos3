/*******************************************************************************
** File: patch_app.c
**
** Purpose:
**   This file contains the source code for the Patch App.
**
*******************************************************************************/

/*
**   Include Files:
*/

#include "patch_app.h"
#include "patch_perfids.h"
#include "patch_msgids.h"
#include "patch_msg.h"
#include "patch_events.h"
#include "patch_version.h"

/*
** global data
*/

patch_hk_tlm_t    PATCH_HkTelemetryPkt;
CFE_SB_PipeId_t    PATCH_CommandPipe;
CFE_SB_MsgPtr_t    PATCHMsgPtr;

CFS_MODULE_DECLARE_APP(patch, 250, 4086);

static CFE_EVS_BinFilter_t  PATCH_EventFilters[] =
       {  /* Event ID    mask */
          {PATCH_STARTUP_INF_EID,       0x0000},
          {PATCH_COMMAND_ERR_EID,       0x0000},
          {PATCH_COMMANDNOP_INF_EID,    0x0000},
          {PATCH_COMMANDRST_INF_EID,    0x0000},
          {PATCH_INVALID_MSGID_ERR_EID, 0x0000},
          {PATCH_LEN_ERR_EID,           0x0000},
          {PATCH_PATCH_LEN_ERR_EID,     0x0000},
          {PATCH_PATCH_APPLIED_INF_EID, 0x0000},
          {PATCH_PATCH_APPLY_ERR_EID,   0x0000},
          {PATCH_INVALID_CC_ERR_EID,    0x0000}
       };

/** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* patch_Main() -- Application entry point and main process loop          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * *  * * * * **/
void patch_Main( void )
{
    int32  status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    CFE_ES_PerfLogEntry(PATCH_PERF_ID);

    PATCH_AppInit();

    /*
    ** PATCH Runloop
    */
    while (CFE_ES_RunLoop(&RunStatus) == TRUE)
    {
        CFE_ES_PerfLogExit(PATCH_PERF_ID);

        /* Pend on receipt of command packet -- timeout set to 500 millisecs */
        status = CFE_SB_RcvMsg(&PATCHMsgPtr, PATCH_CommandPipe, 500);
        
        CFE_ES_PerfLogEntry(PATCH_PERF_ID);

        if (status == CFE_SUCCESS)
        {
            PATCH_ProcessCommandPacket();
        }

    }

    CFE_ES_ExitApp(RunStatus);

} /* End of patch_Main() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */
/*                                                                            */
/* PATCH_AppInit() --  initialization                                         */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void PATCH_AppInit(void)
{
    /*
    ** Register the app with Executive services
    */
    CFE_ES_RegisterApp() ;

    /*
    ** Register the events
    */ 
    CFE_EVS_Register(PATCH_EventFilters,
                     sizeof(PATCH_EventFilters)/sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    /*
    ** Create the Software Bus command pipe and subscribe to housekeeping
    **  messages
    */
    CFE_SB_CreatePipe(&PATCH_CommandPipe, PATCH_PIPE_DEPTH,"PATCH_CMD_PIPE");
    CFE_SB_Subscribe(PATCH_CMD_MID, PATCH_CommandPipe);
    CFE_SB_Subscribe(PATCH_SEND_HK_MID, PATCH_CommandPipe);

    PATCH_ResetCounters();

    CFE_SB_InitMsg(&PATCH_HkTelemetryPkt,
                   PATCH_HK_TLM_MID,
                   PATCH_HK_TLM_LNGTH, TRUE);

    CFE_EVS_SendEvent (PATCH_STARTUP_INF_EID, CFE_EVS_INFORMATION,
               "PATCH App Initialized. Version %d.%d.%d.%d",
                PATCH_MAJOR_VERSION,
                PATCH_MINOR_VERSION, 
                PATCH_REVISION, 
                PATCH_MISSION_REV);
				
} /* End of PATCH_AppInit() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  PATCH_ProcessCommandPacket                                        */
/*                                                                            */
/*  Purpose:                                                                  */
/*     This routine will process any packet that is received on the PATCH */
/*     command pipe.                                                          */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void PATCH_ProcessCommandPacket(void)
{
    CFE_SB_MsgId_t  MsgId;

    MsgId = CFE_SB_GetMsgId(PATCHMsgPtr);

    switch (MsgId)
    {
        case PATCH_CMD_MID:
            PATCH_ProcessGroundCommand();
            break;

        case PATCH_SEND_HK_MID:
            PATCH_ReportHousekeeping();
            break;

        default:
            PATCH_HkTelemetryPkt.patch_command_error_count++;
            CFE_EVS_SendEvent(PATCH_COMMAND_ERR_EID,CFE_EVS_ERROR,
			"PATCH: invalid command packet,MID = 0x%x", MsgId);
            break;
    }

    return;

} /* End PATCH_ProcessCommandPacket */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* PATCH_ProcessGroundCommand() -- PATCH ground commands                    */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/

void PATCH_ProcessGroundCommand(void)
{
    uint16 CommandCode;

    CommandCode = CFE_SB_GetCmdCode(PATCHMsgPtr);

    /* Process "known" PATCH app ground commands */
    switch (CommandCode)
    {
        case PATCH_NOOP_CC:
            PATCH_HkTelemetryPkt.patch_command_count++;
            CFE_EVS_SendEvent(PATCH_COMMANDNOP_INF_EID,CFE_EVS_INFORMATION,
			"PATCH: NOOP command");
            break;

        case PATCH_RESET_COUNTERS_CC:
            PATCH_ResetCounters();
            break;
        
        case PATCH_APPLY_PATCH_CC:
            PATCH_HkTelemetryPkt.patch_command_count++;
            PATCH_ApplyPatch();
            break;

        /* default case already found during FC vs length test */
        default:
            CFE_EVS_SendEvent(PATCH_INVALID_CC_ERR_EID, CFE_EVS_ERROR, "PATCH: invalid CC received. CC=%d", CommandCode);
            break;
    }
    return;

} /* End of PATCH_ProcessGroundCommand() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  PATCH_ReportHousekeeping                                              */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function is triggered in response to a task telemetry request */
/*         from the housekeeping task. This function will gather the Apps     */
/*         telemetry, packetize it and send it to the housekeeping task via   */
/*         the software bus                                                   */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void PATCH_ReportHousekeeping(void)
{
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &PATCH_HkTelemetryPkt);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &PATCH_HkTelemetryPkt);
    return;

} /* End of PATCH_ReportHousekeeping() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  PATCH_ResetCounters                                               */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function resets all the global counter variables that are     */
/*         part of the task telemetry.                                        */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * *  * *  * * * * */
void PATCH_ResetCounters(void)
{
    /* Status of commands processed by the PATCH App */
    PATCH_HkTelemetryPkt.patch_command_count         = 0;
    PATCH_HkTelemetryPkt.patch_command_error_count   = 0;
    PATCH_HkTelemetryPkt.patch_patches_applied_count = 0;

    CFE_EVS_SendEvent(PATCH_COMMANDRST_INF_EID, CFE_EVS_INFORMATION,
		"PATCH: RESET command");
    return;

} /* End of PATCH_ResetCounters() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*  Name:  PATCH_ApplyPatch                                                   */
/*                                                                            */
/*  Purpose:                                                                  */
/*         This function applies patches sent via ground command.             */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
void PATCH_ApplyPatch(void)
{
    uint16 ExpectedLength = sizeof(PATCH_ApplyPatchCmd_t);
    PATCH_ApplyPatchCmd_t   *CmdPtr = NULL;
    int32   OS_Status;

    /*
     * Verify command packet length
     */
    if(PATCH_VerifyCmdLength(PATCHMsgPtr, ExpectedLength))
    {
        CmdPtr = ((PATCH_ApplyPatchCmd_t *) PATCHMsgPtr);

        if (PATCH_MAX_PATCH_LENGTH < (CmdPtr -> PatchLength))
        {
            CFE_EVS_SendEvent(PATCH_PATCH_LEN_ERR_EID, CFE_EVS_ERROR, 
                    "PATCH: invalid patch length. Len=%d, max=%d", 
                    (CmdPtr -> PatchLength), PATCH_MAX_PATCH_LENGTH);
            PATCH_HkTelemetryPkt.patch_command_error_count++;
        }
        else
        {
            OS_Status = OS_ApplyPatch( CmdPtr -> PatchData, 
                    CmdPtr -> PatchLength, 
                    CmdPtr -> PatchLocation);
            if (OS_SUCCESS == OS_Status)
            {
                // Send INFO EVS, increment patches_applied_count
                CFE_EVS_SendEvent(PATCH_PATCH_APPLIED_INF_EID, CFE_EVS_INFORMATION,
                        "PATCH: APPLY PATCH command");
                PATCH_HkTelemetryPkt.patch_patches_applied_count++;
            }
            else
            {
                // Send ERROR EVS, increment command_error_count
                CFE_EVS_SendEvent(PATCH_PATCH_APPLY_ERR_EID, CFE_EVS_ERROR,
                        "PATCH: patch application failed. Error=%d", OS_Status);
                PATCH_HkTelemetryPkt.patch_command_error_count++;
            }
        }
    }
    
    return;
} /* End of PATCH_ApplyPatch() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
/*                                                                            */
/* PATCH_VerifyCmdLength() -- Verify command packet length                   */
/*                                                                            */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * **/
boolean PATCH_VerifyCmdLength(CFE_SB_MsgPtr_t msg, uint16 ExpectedLength)
{     
    boolean result = TRUE;

    uint16 ActualLength = CFE_SB_GetTotalMsgLength(msg);

    /*
    ** Verify the command packet length.
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_SB_MsgId_t MessageID   = CFE_SB_GetMsgId(msg);
        uint16         CommandCode = CFE_SB_GetCmdCode(msg);

        CFE_EVS_SendEvent(PATCH_LEN_ERR_EID, CFE_EVS_ERROR,
           "Invalid msg length: ID = 0x%X,  CC = %d, Len = %d, Expected = %d",
              MessageID, CommandCode, ActualLength, ExpectedLength);
        result = FALSE;
        PATCH_HkTelemetryPkt.patch_command_error_count++;
    }

    return(result);

} /* End of PATCH_VerifyCmdLength() */

