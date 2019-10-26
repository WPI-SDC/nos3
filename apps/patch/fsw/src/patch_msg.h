/*******************************************************************************
** File:
**   patch_msg.h 
**
** Purpose: 
**  Define PATCH App  Messages and info
**
** Notes:
**
**
*******************************************************************************/
#ifndef _patch_msg_h_
#define _patch_msg_h_

/*
** PATCH App command codes
*/
#define PATCH_NOOP_CC                 1
#define PATCH_RESET_COUNTERS_CC       2
#define PATCH_APPLY_PATCH_CC          3


/*
 * Constants
 */
#define PATCH_MAX_PATCH_LENGTH (1024)


/*************************************************************************/
/*
** Type definition (generic "no arguments" command)
*/
typedef struct
{
   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];

} PATCH_NoArgsCmd_t;

/*************************************************************************/
/*
 * Type definition - apply patch command
 */
typedef struct {
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint32  PatchLength;
    uint32  PatchLocation;
    uint8   PatchData[PATCH_MAX_PATCH_LENGTH];
} PATCH_ApplyPatchCmd_t;

/*************************************************************************/
/*
** Type definition (PATCH App housekeeping)
*/
typedef struct 
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8              patch_command_error_count;
    uint8              patch_command_count;
    uint8              patch_patches_applied_count;
    uint8              spare[1];

}   OS_PACK patch_hk_tlm_t  ;

#define PATCH_HK_TLM_LNGTH   sizeof ( patch_hk_tlm_t )

#endif /* _patch_msg_h_ */

/************************/
/*  End of File Comment */
/************************/
