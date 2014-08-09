/***************************************************************************
* File		mscuser.c
* Brief		Mass Storage Class Custom User Module
* Date		27.03.2013
* Author	"Other Technologies", sole proprietorship Pashkova E.A.
*
* Copyright(C) 2013, "Other Technologies", sole proprietorship Pashkova E.A.
* All rights reserved.
***************************************************************************/

#include <string.h>
#include "lpc18xx.h"
#include "lpc_types.h"

#include "usb.h"
#include "msc.h"
#include "usbcfg.h"
#include "usbhw.h"
#include "usbcore.h"
#include "mscuser.h"

#include <wait.h>

#if defined   (  __GNUC__  )
#define __align(x) __attribute__((aligned(x)))
#endif

#define MSC_BUF_SIZE (16*1024)
#define MSC_BUF_ADDR (0x2000C000)
//__align(4096) uint8_t msc_buf[MSC_BUF_SIZE];//(uint8_t*)MSC_BUF_ADDR;
uint8_t* msc_buf = (uint8_t*)MSC_BUF_ADDR;
uint32_t msc_iblock = 0;
uint32_t msc_nblocks = 0;
volatile uint8_t  msc_wr = 0;

extern uint32_t DevStatusFS2HS;

uint32_t  MemOK;                   /* Memory OK */

uint32_t Offset;                  /* R/W Offset */
uint32_t Length;                  /* R/W Length */

uint32_t OffsetBase;                  /* R/W Offset */

/* If it's a USB HS, the max packet is 512, if it's USB FS,
the max packet is 64. Use 512 for both HS and FS. */
#ifdef __ICCARM__
#pragma data_alignment=4096
uint8_t  BulkBuf[MSC_HS_MAX_PACKET]; /* Bulk In/Out Buffer */
#pragma data_alignment=4
#else
//__align(4096) uint8_t  BulkBuf[MSC_HS_MAX_PACKET*8]; /* Bulk In/Out Buffer */
//uint8_t* WriteBulkBuf = BulkBuf;
uint8_t* BulkBuf = (uint8_t*)(0x20003000);
uint8_t* WriteBulkBuf = (uint8_t*)(0x20003000);
#endif



uint8_t  BulkStage;               /* Bulk Stage */
MSC_CBW CBW;                   /* Command Block Wrapper */
MSC_CSW CSW;                   /* Command Status Wrapper */
uint32_t  BulkLen;                 /* Bulk In/Out Length */

/*
 *  Set Stall for MSC Endpoint
 *    Parameters:      EPNum: Endpoint Number
 *                       EPNum.0..3: Address
 *                       EPNum.7:    Dir
 *    Return Value:    None
 */

void MSC_SetStallEP (uint32_t EPNum) {          /* set EP halt status according stall status */
  USB_SetStallEP(EPNum);
  USB_EndPointHalt  |=  (EPNum & 0x80) ? ((1 << 16) << (EPNum & 0x0F)) : (1 << EPNum);
}

/*
 *  MSC Mass Storage Reset Request Callback
 *   Called automatically on Mass Storage Reset Request
 *    Parameters:      None (global SetupPacket and EP0Buf)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

uint32_t MSC_Reset (void) {

  USB_EndPointStall = 0x00000000;          /* EP must stay stalled */
  CSW.dSignature = 0;                      /* invalid signature */

  BulkStage = MSC_BS_CBW;
  return (TRUE);
}


/*
 *  MSC Get Max LUN Request Callback
 *   Called automatically on Get Max LUN Request
 *    Parameters:      None (global SetupPacket and EP0Buf)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

uint32_t MSC_GetMaxLUN (void) {

  EP0Buf[0] = 0;               /* No LUN associated with this device */
  return (TRUE);
}


/*
 *  MSC Memory Read Callback
 *   Called automatically on Memory Read Event
 *    Parameters:      None (global variables)
 *    Return Value:    None
 */

void MSC_MemoryRead (void) 
{
  uint32_t n, n2, i;
  uint32_t nBlock; //Block number

#ifdef DEBUG_MSC
  printf("USB: MSC: Read %d %d\r\n", Offset, Length);
#endif

  	if (Length > MSC_HS_MAX_PACKET) {
        n = MSC_HS_MAX_PACKET;
  	} else {
        n = Length;
  	}

    uint32_t queue_size = 1;
    uint32_t max_queue_size = DevStatusFS2HS == FALSE ? 1 : 8;
    uint32_t iBlock = OffsetBase + Offset/MSC_BlockSize;
    uint32_t nBlocks = Length/MSC_BlockSize;
    uint32_t delay = 12000;

      if(Length/MSC_BlockSize > 1) queue_size = Length/MSC_BlockSize > max_queue_size ? max_queue_size : Length/MSC_BlockSize;

      USB_WriteEP(MSC_EP_IN, (uint8_t *)(&msc_buf[Offset%MSC_BUF_SIZE]), MSC_BlockSize*queue_size);

      Offset += MSC_BlockSize*queue_size;
      Length -= MSC_BlockSize*queue_size;

      CSW.dDataResidue -= MSC_BlockSize*queue_size;

  if (Length == 0) {
    BulkStage = MSC_BS_DATA_IN_LAST;
  }

msc_read_exit:
  if (BulkStage != MSC_BS_DATA_IN) {
    CSW.bStatus = CSW_CMD_PASSED;
  }
}

/*
 *  MSC Memory Write Callback
 *   Called automatically on Memory Write Event
 *    Parameters:      None (global variables)
 *    Return Value:    None
 */

void MSC_MemoryWrite (void) 
{
  #ifdef DEBUG_MSC
  printf("USB: MSC: Write %d %d\r\n", Offset, Length);
  #endif
  uint32_t iBlock = OffsetBase + Offset/MSC_BlockSize;
  uint32_t nBlocks = Length/MSC_BlockSize;
  uint32_t writed, transfered;

  Offset += BulkLen;
  Length -= BulkLen;

  WriteBulkBuf = (uint8_t*)&msc_buf[Offset%MSC_BUF_SIZE];

  CSW.dDataResidue -= BulkLen;

  if((Length == 0) || (BulkStage == MSC_BS_CSW))
  {
      CSW.bStatus = CSW_CMD_PASSED;
      MSC_SetCSW();
  }
  else
  {
      MSC_PrimeBulkOut();
  }
}


/*
 *  MSC Memory Verify Callback
 *   Called automatically on Memory Verify Event
 *    Parameters:      None (global variables)
 *    Return Value:    None
 */

void MSC_MemoryVerify (void)
{
  uint32_t n;

  if ((Offset + BulkLen) > MSC_MemorySize) {
    BulkLen = MSC_MemorySize - Offset;
    BulkStage = MSC_BS_CSW;
    MSC_SetStallEP(MSC_EP_OUT);
  }

  Offset += BulkLen;
  Length -= BulkLen;

  CSW.dDataResidue -= BulkLen;

  if ((Length == 0) || (BulkStage == MSC_BS_CSW)) {
    CSW.bStatus = (MemOK) ? CSW_CMD_PASSED : CSW_CMD_FAILED;
    MSC_SetCSW();
  }
}


/*
 *  MSC SCSI Read/Write Setup Callback
 *    Parameters:      None (global variables)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

uint32_t MSC_RWSetup (void) {
  uint32_t nblocks, iblock;


  /* Logical Block Address of First Block */
  iblock = (CBW.CB[2] << 24) |
           (CBW.CB[3] << 16) |
           (CBW.CB[4] <<  8) |
           (CBW.CB[5] <<  0);

  Offset = 0;
  OffsetBase = iblock;

  /* Number of Blocks to transfer */
  switch (CBW.CB[0]) {
    case SCSI_READ10:
    case SCSI_WRITE10:
    case SCSI_VERIFY10:
      nblocks = (CBW.CB[7] <<  8) |
          (CBW.CB[8] <<  0);
      break;

    case SCSI_READ12:
    case SCSI_WRITE12:
      nblocks = (CBW.CB[6] << 24) |
          (CBW.CB[7] << 16) |
          (CBW.CB[8] <<  8) |
          (CBW.CB[9] <<  0);
      break;
  }

  Length = nblocks * MSC_BlockSize;

      if(CBW.CB[0] == SCSI_READ10 || CBW.CB[0] == SCSI_READ12)
      {
          #ifdef DEBUG_MSC
          printf("{ Start read: %d, %d\r\n", OffsetBase, Length);
          #endif
          uint32_t len = Length;
          msc_iblock = iblock;
          msc_nblocks = len/MSC_BlockSize;
      }
      else if(CBW.CB[0] == SCSI_WRITE10 || CBW.CB[0] == SCSI_WRITE12)
      {
              #ifdef DEBUG_MSC
              printf("{ Start write: %d, %d\r\n", OffsetBase, Length);
              #endif
              uint32_t len = Length;
              msc_iblock = iblock;
              msc_nblocks = len/MSC_BlockSize;

              WriteBulkBuf = (uint8_t*)&msc_buf[0];
      }

  if (CBW.dDataLength == 0) {              /* host requests no data */
    CSW.bStatus = CSW_CMD_FAILED;
    MSC_SetCSW();
    return (FALSE);
  }

  if (CBW.dDataLength != Length) {
    if ((CBW.bmFlags & 0x80) != 0) {       /* stall appropriate EP */
      MSC_SetStallEP(MSC_EP_IN);
    } else {
      MSC_SetStallEP(MSC_EP_OUT);
    }

    CSW.bStatus = CSW_CMD_FAILED;
    MSC_SetCSW();
    return (FALSE);
  }

  return (TRUE);
}


/*
 *  Check Data IN Format
 *    Parameters:      None (global variables)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

uint32_t DataInFormat (void) {

  if (CBW.dDataLength == 0) {
    CSW.bStatus = CSW_PHASE_ERROR;
    MSC_SetCSW();
    return (FALSE);
  }
  if ((CBW.bmFlags & 0x80) == 0) {
    MSC_SetStallEP(MSC_EP_OUT);
    CSW.bStatus = CSW_PHASE_ERROR;
    MSC_SetCSW();
    return (FALSE);
  }
  return (TRUE);
}


/*
 *  Perform Data IN Transfer
 *    Parameters:      None (global variables)
 *    Return Value:    TRUE - Success, FALSE - Error
 */

void DataInTransfer (void) {

  if (BulkLen >= CBW.dDataLength) {
    BulkLen = CBW.dDataLength;
    BulkStage = MSC_BS_DATA_IN_LAST;
  }
  else {
    BulkStage = MSC_BS_DATA_IN_LAST_STALL; /* short or zero packet */
  }

  USB_WriteEP(MSC_EP_IN, BulkBuf, BulkLen);

  CSW.dDataResidue -= BulkLen;
  CSW.bStatus = CSW_CMD_PASSED;
}


/*
 *  MSC SCSI Test Unit Ready Callback
 *    Parameters:      None (global variables)
 *    Return Value:    None
 */

void MSC_TestUnitReady (void) {

  if (CBW.dDataLength != 0) {
    if ((CBW.bmFlags & 0x80) != 0) {
      MSC_SetStallEP(MSC_EP_IN);
    } else {
      MSC_SetStallEP(MSC_EP_OUT);
    }
  }
  
  #ifdef DEBUG_MSC
  printf("TestUnitReady\r\n");
  #endif

  CSW.bStatus = CSW_CMD_PASSED;
  MSC_SetCSW();
}


/*
 *  MSC SCSI Request Sense Callback
 *    Parameters:      None (global variables)
 *    Return Value:    None
 */

void MSC_RequestSense (void) {

  if (!DataInFormat()) return;
  
  BulkBuf[ 0] = 0x70;                      /* Response Code */
  BulkBuf[ 1] = 0x00;

  BulkBuf[ 2] = 0x00;

  BulkBuf[ 3] = 0x00;
  BulkBuf[ 4] = 0x00;
  BulkBuf[ 5] = 0x00;
  BulkBuf[ 6] = 0x00;
  BulkBuf[ 7] = 0x0A;                      /* Additional Length */
  BulkBuf[ 8] = 0x00;
  BulkBuf[ 9] = 0x00;
  BulkBuf[10] = 0x00;
  BulkBuf[11] = 0x00;

      
  BulkBuf[12] = 0x00;

  BulkBuf[13] = 0x00;                      /* ASCQ */
  BulkBuf[14] = 0x00;
  BulkBuf[15] = 0x00;
  BulkBuf[16] = 0x00;
  BulkBuf[17] = 0x00;

  #ifdef DEBUG_MSC
  printf("RequestSense\r\n");
  #endif

  BulkLen = 18;
  DataInTransfer();
}


/*
 *  MSC SCSI Inquiry Callback
 *    Parameters:      None (global variables)
 *    Return Value:    None
 */

void MSC_Inquiry (void)
{
  uint32_t lunNameSize, lunVendorSize;
  char *lunName, *lunVendor;

  if (!DataInFormat()) return;

  BulkBuf[ 0] = 0x00;                      /* Direct Access Device */
  BulkBuf[ 1] = 0x80;                      /* RMB = 1: Non Removable Medium */
  BulkBuf[ 2] = 0x00;                      /* Version: No conformance claim to standard */
  BulkBuf[ 3] = 0x00;

  BulkBuf[ 4] = 93;                       /* Additional Length */
  BulkBuf[ 5] = 0x00;
  BulkBuf[ 6] = 0x00;
  BulkBuf[ 7] = 0x60;

  memset(&BulkBuf[8],  0, 8);
  memset(&BulkBuf[16], 0, 16);

  memcpy(&BulkBuf[8], "MCS51", 5);
  memcpy(&BulkBuf[16], "LUN", 3);

  memset(&BulkBuf[32], 0, 65);

  BulkLen = 97;

  DataInTransfer();
}


/*
 *  MSC SCSI Mode Sense (6-Byte) Callback
 *    Parameters:      None (global variables)
 *    Return Value:    None
 */

void MSC_ModeSense6 (void) {

  if (!DataInFormat()) return;

      BulkBuf[ 0] = 0x03;
      BulkBuf[ 1] = 0x00;

      BulkBuf[ 2] = 0x00;

      BulkBuf[ 3] = 0x00;

  #ifdef DEBUG_MSC
  printf("ModeSense(6) %d\r\n", CBW.CB[2]);
  #endif

  BulkLen = 4;
  DataInTransfer();
}


/*
 *  MSC SCSI Mode Sense (10-Byte) Callback
 *    Parameters:      None (global variables)
 *    Return Value:    None
 */

void MSC_ModeSense10 (void) {

  if (!DataInFormat()) return;

  BulkBuf[ 0] = 0x00;
  BulkBuf[ 1] = 0x06;
  BulkBuf[ 2] = 0x00;
  BulkBuf[ 3] = 0x00;
  BulkBuf[ 4] = 0x00;
  BulkBuf[ 5] = 0x00;
  BulkBuf[ 6] = 0x00;
  BulkBuf[ 7] = 0x00;

  #ifdef DEBUG_MSC
  printf("ModeSense(10)\r\n");
  #endif

  BulkLen = 8;
  DataInTransfer();
}


/*
 *  MSC SCSI Read Capacity Callback
 *    Parameters:      None (global variables)
 *    Return Value:    None
 */

void MSC_ReadCapacity (void)
{
  uint32_t sz, c;

  if (!DataInFormat()) return;

  #ifdef DEBUG_MSC
  printf("ReadCapacity %d\r\n", CBW.bLUN);
  #endif

  c = 512 - 1;

  sz = 512;


  /* Last Logical Block */
  BulkBuf[ 0] = ((c) >> 24) & 0xFF;
  BulkBuf[ 1] = ((c) >> 16) & 0xFF;
  BulkBuf[ 2] = ((c) >>  8) & 0xFF;
  BulkBuf[ 3] = ((c) >>  0) & 0xFF;

  /* Block Length */
  BulkBuf[ 4] = (sz >> 24) & 0xFF;
  BulkBuf[ 5] = (sz >> 16) & 0xFF;
  BulkBuf[ 6] = (sz >>  8) & 0xFF;
  BulkBuf[ 7] = (sz >>  0) & 0xFF;

  BulkLen = 8;
  DataInTransfer();
}


/*
 *  MSC SCSI Read Format Capacity Callback
 *    Parameters:      None (global variables)
 *    Return Value:    None
 */

void MSC_ReadFormatCapacity (void)
{
  uint32_t sz, c = 0;

  if (!DataInFormat()) return;

  #ifdef DEBUG_MSC
  printf("ReadFormatCapacity\r\n");
  #endif

  c = 512;

  sz = 512;

  BulkBuf[ 0] = 0x00;
  BulkBuf[ 1] = 0x00;
  BulkBuf[ 2] = 0x00;
  BulkBuf[ 3] = 0x08;                      /* Capacity List Length */

  /* Block Count */
  BulkBuf[ 4] = (c >> 24) & 0xFF;
  BulkBuf[ 5] = (c >> 16) & 0xFF;
  BulkBuf[ 6] = (c >>  8) & 0xFF;
  BulkBuf[ 7] = (c >>  0) & 0xFF;

  /* Block Length */
  BulkBuf[ 8] = 0x02;                      /* Descriptor Code: Formatted Media */
  BulkBuf[ 9] = (sz >> 16) & 0xFF;
  BulkBuf[10] = (sz >>  8) & 0xFF;
  BulkBuf[11] = (sz >>  0) & 0xFF;

  BulkLen = 12;
  DataInTransfer();
}

void MSC_MediaRemoval(void)
{
    CSW.bStatus = CSW_CMD_PASSED;
    MSC_SetCSW();
}

void MSC_Read(void)
{
    if(!MSC_RWSetup()) return;

    if((CBW.bmFlags & 0x80) != 0)
    {
        BulkStage = MSC_BS_DATA_IN;
        MSC_MemoryRead();
    }
    else
    {
        /* direction mismatch */
        MSC_SetStallEP(MSC_EP_OUT);
        CSW.bStatus = CSW_PHASE_ERROR;
        MSC_SetCSW();
    }
}

void MSC_Write(void)
{
    if(!MSC_RWSetup()) return;

    if((CBW.bmFlags & 0x80) == 0)
    {
        BulkStage = MSC_BS_DATA_OUT;
        MSC_PrimeBulkOut();
    }

    if((CBW.bmFlags & 0x80) != 0)
    {
        /* direction mismatch */
        MSC_SetStallEP(MSC_EP_IN);
        CSW.bStatus = CSW_PHASE_ERROR;
        MSC_SetCSW();
    }
}

void MSC_Verify(void)
{
    if ((CBW.CB[1] & 0x02) == 0)
    {
        // BYTCHK = 0 -> CRC Check (not implemented)
        CSW.bStatus = CSW_CMD_PASSED;
        MSC_SetCSW();
        return;
    }

    if(!MSC_RWSetup()) return;

    if((CBW.bmFlags & 0x80) == 0) BulkStage = MSC_BS_DATA_OUT;
    if((CBW.bmFlags & 0x80) != 0)
    {
        MSC_SetStallEP(MSC_EP_IN);
        CSW.bStatus = CSW_PHASE_ERROR;
        MSC_SetCSW();
    }
    else MemOK = TRUE;
}


/*
 *  MSC Get Command Block Wrapper Callback
 *    Parameters:      None (global variables)
 *    Return Value:    None
 */

void MSC_GetCBW (void)
{
    uint32_t n;

    memcpy(&CBW, BulkBuf, BulkLen);

    if((BulkLen != sizeof(CBW)) || (CBW.dSignature != MSC_CBW_Signature))
    {
        /* Invalid CBW */
        MSC_SetStallEP(MSC_EP_IN);
        
        /* set EP to stay stalled */
        USB_EndPointStall |=  (1 << (16 + (MSC_EP_IN  & 0x0F)));
        MSC_SetStallEP(MSC_EP_OUT);
        
        /* set EP to stay stalled */
        USB_EndPointStall |=  (1 << MSC_EP_OUT);
        BulkStage = MSC_BS_ERROR;

        return;
    }

    /* Valid CBW */
    CSW.dTag = CBW.dTag;
    CSW.dDataResidue = CBW.dDataLength;

    if( (CBW.bCBLength <  1) ||
        (CBW.bCBLength > 16)   )
    {
fail:
        CSW.bStatus = CSW_CMD_FAILED;
        MSC_SetCSW();

        return;
    }
    
    switch (CBW.CB[0])
    {
    case SCSI_TEST_UNIT_READY:        MSC_TestUnitReady();      break;
    case SCSI_REQUEST_SENSE:          MSC_RequestSense();       break;
    case SCSI_INQUIRY:                MSC_Inquiry();            break;
    case SCSI_MODE_SENSE6:            MSC_ModeSense6();         break;
    case SCSI_MODE_SENSE10:           MSC_ModeSense10();        break;
    case SCSI_READ_FORMAT_CAPACITIES: MSC_ReadFormatCapacity(); break;
    case SCSI_READ_CAPACITY:          MSC_ReadCapacity();       break;
    case SCSI_MEDIA_REMOVAL:          MSC_MediaRemoval();       break;

    case SCSI_FORMAT_UNIT:     goto fail;
    case SCSI_START_STOP_UNIT: goto fail;
    case SCSI_MODE_SELECT6:    goto fail;
    case SCSI_MODE_SELECT10:   goto fail;
    default:                   goto fail;
 
    case SCSI_READ10:
    case SCSI_READ12:   MSC_Read();   break;

    case SCSI_WRITE10:
    case SCSI_WRITE12:  MSC_Write();  break;

    case SCSI_VERIFY10: MSC_Verify(); break;
    }

}


/*
 *  MSC Set Command Status Wrapper Callback
 *    Parameters:      None (global variables)
 *    Return Value:    None
 */

void MSC_SetCSW (void)
{
    CSW.dSignature = MSC_CSW_Signature;
    USB_WriteEP(MSC_EP_IN, (uint8_t *)&CSW, sizeof(CSW));
    BulkStage = MSC_BS_CSW;
}


/*
 *  MSC Bulk In Callback
 *    Parameters:      None (global variables)
 *    Return Value:    None
 */

void MSC_BulkIn (void)
{
  switch (BulkStage) {
    case MSC_BS_DATA_IN:
      switch (CBW.CB[0]) {
        case SCSI_READ10:
        case SCSI_READ12:
          MSC_MemoryRead();
          break;
      }
      break;
    case MSC_BS_DATA_IN_LAST:
      MSC_SetCSW();
      break;
    case MSC_BS_DATA_IN_LAST_STALL:
      MSC_SetStallEP(MSC_EP_IN);
      MSC_SetCSW();
      break;
    case MSC_BS_CSW:
      BulkStage = MSC_BS_CBW;
      break;
  }
}

/*
 *  MSC Bulk Out Nak Callback
 *    Parameters:      None (global variables)
 *    Return Value:    None
 */

void MSC_PrimeBulkOut(void)
{
          uint32_t len = Length > 4096 ? 4096 : Length;
            USB_ReadReqEP(MSC_EP_OUT, WriteBulkBuf, len);
      led_set(0,0);
}

void MSC_BulkOutNak(void)
{
  int res;

  #ifdef DEBUG_MSC
  printf("n");
  #endif

  switch (BulkStage)
  {
    case MSC_BS_DATA_OUT:
        break;
    case MSC_BS_CBW:
        USB_ReadReqEP(MSC_EP_OUT, BulkBuf, 32);
        break;
    case MSC_BS_CSW:
        // TODO
        USB_ReadReqEP(MSC_EP_OUT, BulkBuf, MSC_HS_MAX_PACKET);
        break;
    default:
        // TODO
        ;
  }
}

/*
 *  MSC Bulk Out Callback
 *    Parameters:      None (global variables)
 *    Return Value:    None
 */

void MSC_BulkOut (void)
{
    BulkLen = USB_ReadEP(MSC_EP_OUT, BulkBuf);
    
    switch (BulkStage)
    {
    case MSC_BS_CBW: MSC_GetCBW(); break;

    case MSC_BS_DATA_OUT:
        switch (CBW.CB[0])
        {
        case SCSI_WRITE10:
        case SCSI_WRITE12:
            MSC_MemoryWrite();
            break;
        case SCSI_VERIFY10:
            MSC_MemoryVerify();
            break;
        }
        break;

    case MSC_BS_CSW: break;

    default:
        MSC_SetStallEP(MSC_EP_OUT);
        CSW.bStatus = CSW_PHASE_ERROR;
        MSC_SetCSW();
        break;
    }
}

