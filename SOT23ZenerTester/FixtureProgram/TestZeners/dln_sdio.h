#ifndef DLN_SDIO_H
#define DLN_SDIO_H
#include "dln.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__ICCARM__)
        #pragma pack(1)
        #define __PACKED_ATTR   /*nothing*/
        //#define __attribute__(...)
#elif defined(WIN32) || defined(__BORLANDC__)
        #pragma pack(push,1)
        #define __PACKED_ATTR   /*nothing*/
        //#define __attribute__(...)
#else
        #define __PACKED_ATTR __attribute__ ((packed))
#endif


////////////////////////////////////////////////////////////////////////////////
//// SDIO commands and structures
////////////////////////////////////////////////////////////////////////////////
// SDIO command list
#define DLN_MSG_ID_SDIO_GET_SLOT_COUNT      DLN_BUILD_MSG_ID(0x00, DLN_MODULE_SDIO)  
#define DLN_MSG_ID_SDIO_GET_VALID_CFG       DLN_BUILD_MSG_ID(0x01, DLN_MODULE_SDIO) 
#define DLN_MSG_ID_SDIO_SET_SLOT_CFG	    DLN_BUILD_MSG_ID(0x02, DLN_MODULE_SDIO)	
#define DLN_MSG_ID_SDIO_GET_SLOT_CFG        DLN_BUILD_MSG_ID(0x03, DLN_MODULE_SDIO)
#define DLN_MSG_ID_SDIO_INIT_CARD           DLN_BUILD_MSG_ID(0x04, DLN_MODULE_SDIO)
#define DLN_MSG_ID_SDIO_SEND_COMMAND        DLN_BUILD_MSG_ID(0x05, DLN_MODULE_SDIO)
#define DLN_MSG_ID_SDIO_SELECT_SLOT         DLN_BUILD_MSG_ID(0x06, DLN_MODULE_SDIO)
    
  
#define DLN_MSG_ID_SDIO_DEINIT_CARD         DLN_BUILD_MSG_ID(0x07, DLN_MODULE_SDIO)
#define DLN_MSG_ID_SDIO_READ_ANSWER         DLN_BUILD_MSG_ID(0x08, DLN_MODULE_SDIO)
  
#define SDIO_SLOT_A                         0x00
#define SDIO_SLOT_B                         0x01
#define SDIO_SLOT_C                         0x02
#define SDIO_SLOT_D                         0x03
   
//SDIO Bus Width    
#define SDIO_BUS_WIDTH_1_BIT                0x01
#define SDIO_BUS_WIDTH_4_BIT                0x04
    
//SDIO Transfer direction
#define SDIO_TRANSFER_DIRECTION_WRITE       0x00    
#define SDIO_TRANSFER_DIRECTION_READ        0x01    

//SDIO Transfer type
#define SDIO_TRANSFER_TYPE_BYTE             0x04    
#define SDIO_TRANSFER_TYPE_BLOCK            0x05    
    
//SDIO Specioal Command
#define SDIO_SPECIAL_COMMEND_STD            0x00    
#define SDIO_SPECIAL_COMMEND_SUSPEND        0x01    
#define SDIO_SPECIAL_COMMEND_RESUME         0x02    
typedef struct
{
    uint16_t cfg;
} __PACKED_ATTR DLN_SDIO_SLOT_CONFIG;


typedef struct
{
    DLN_MSG_HEADER header;
} __PACKED_ATTR DLN_SDIO_GET_SLOT_COUNT_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
    uint8_t count;
} __PACKED_ATTR DLN_SDIO_GET_SLOT_COUNT_RSP;


typedef struct
{
    DLN_MSG_HEADER header;
} __PACKED_ATTR DLN_SDIO_INIT_CARD_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
} __PACKED_ATTR DLN_SDIO_INIT_CARD_RSP;

typedef struct
{
    DLN_MSG_HEADER header;
} __PACKED_ATTR DLN_SDIO_DEINIT_CARD_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
} __PACKED_ATTR DLN_SDIO_DEINIT_CARD_RSP;

typedef struct 
{
    DLN_MSG_HEADER header;
    uint8_t slot;
    DLN_SDIO_SLOT_CONFIG config;
    
} __PACKED_ATTR DLN_SDIO_SET_SLOT_CFG_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
    uint16_t conflict;
} __PACKED_ATTR DLN_SDIO_SET_SLOT_CFG_RSP;

typedef struct
{
    DLN_MSG_HEADER header;
    uint8_t slot;
} __PACKED_ATTR DLN_SDIO_GET_SLOT_CFG_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
    DLN_SDIO_SLOT_CONFIG cfg;
} __PACKED_ATTR DLN_SDIO_GET_SLOT_CFG_RSP;
    
typedef struct
{
    DLN_MSG_HEADER header;
    uint8_t command;
    uint32_t data;
} __PACKED_ATTR DLN_SDIO_SEND_COMMAND_CMD;    

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
    uint8_t answer;
} __PACKED_ATTR DLN_SDIO_SEND_COMMAND_RSP;    


typedef struct
{
    DLN_MSG_HEADER header;
} __PACKED_ATTR DLN_SDIO_READ_ANSWER_CMD;    

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
    uint8_t answer;
} __PACKED_ATTR DLN_SDIO_READ_ANSWER_RSP;    


typedef struct
{
    DLN_MSG_HEADER header;
    uint8_t command;
    uint8_t slot;
} __PACKED_ATTR DLN_SDIO_SELECT_SLOT_CMD;    

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
} __PACKED_ATTR DLN_SDIO_SELECT_SLOT_RSP;   

DLN_RESULT DlnSdioSelectSlot(HDLN handle, uint8_t slot);
DLN_RESULT DlnSdioGetSlotCount(HDLN handle, uint8_t *count);
DLN_RESULT DlnSdioSetSlotCfg(HDLN handle, uint8_t slot, DLN_SDIO_SLOT_CONFIG config, uint16_t *conflict);
DLN_RESULT DlnSdioGetSlotCfg(HDLN handle, uint8_t slot, DLN_SDIO_SLOT_CONFIG *config);
DLN_RESULT DlnSdioInitCard(HDLN handle);
DLN_RESULT DlnSdioDeinitCard(HDLN handle);

DLN_RESULT DlnSdioReadAnswer(HDLN handle, uint32_t *answer);
DLN_RESULT DlnSdioSendCommand(HDLN handle, uint8_t command, uint32_t arg);





#ifdef __ICCARM__
        #pragma pack()
#elif defined(WIN32)
        #pragma pack(pop)
#endif

#ifdef __cplusplus
}
#endif


#endif // DLN_SPI_H
