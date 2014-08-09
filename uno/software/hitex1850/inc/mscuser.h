/***************************************************************************
* File		mscuser.h
* Brief		
* Date		27.03.2013
* Author	"Other Technologies", sole proprietorship Pashkova E.A.
*
* Copyright(C) 2013, "Other Technologies", sole proprietorship Pashkova E.A.
* All rights reserved.
***************************************************************************/

#ifndef __MSCUSER_H__
#define __MSCUSER_H__


/* Mass Storage Memory Layout */
/* MSC Disk Image Definitions */
/* Mass Storage Memory Layout */
#define DIR_ENTRY           32
#define BLOCKS_PER_CLUSTER  16
#define MSC_BlockSize       512
#define BOOT_SECT_SIZE 	    MSC_BlockSize
#define ROOT_DIR_SIZE       (MSC_BlockSize * 1)
#define FAT_SIZE	        (MSC_BlockSize * 2)

#define EMMC_MSC_OFF (1024*1024/512)

#define MSC_ImageSize	    ((uint32_t)(32 * 1024))//(BOOT_SECT_SIZE + FAT_SIZE + ROOT_DIR_SIZE + MSC_BlockSize)
#define MSC_MemorySize      ((uint32_t)(3957325824U - EMMC_MSC_OFF)) //( BOOT_SECT_SIZE + FAT_SIZE + ROOT_DIR_SIZE + MAX_FILE_SZ )
//#define MSC_BlockCount      (MSC_MemorySize / MSC_BlockSize)
#define NO_OF_CLUSTERS 	    (MSC_BlockCount/BLOCKS_PER_CLUSTER)


/* Max In/Out Packet Size */
#define MSC_FS_MAX_PACKET  512 
#define MSC_HS_MAX_PACKET  512 /* < 256 work */

/* MSC In/Out Endpoint Address */
#define MSC_EP_IN       0x81
#define MSC_EP_OUT      0x01

/* MSC Requests Callback Functions */
extern uint32_t MSC_Reset(void);
extern uint32_t MSC_GetMaxLUN (void);

/* MSC Bulk Callback Functions */
extern void MSC_GetCBW (void);
extern void MSC_SetCSW (void);
extern void MSC_BulkIn (void);
extern void MSC_BulkOut(void);
extern void MSC_BulkOutNak(void);

#endif  /* __MSCUSER_H__ */
