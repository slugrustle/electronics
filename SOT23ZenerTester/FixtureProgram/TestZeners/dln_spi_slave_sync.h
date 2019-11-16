#ifndef DLN_SPI_SLAVE_SYNC_H
#define DLN_SPI_SLAVE_SYNC_H
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


#define DLN_PIN_ROLE_SPI_SLAVE_SYNC_MISO    ((DLN_PIN_ROLE)1)
#define DLN_PIN_ROLE_SPI_SLAVE_SYNC_MOSI    ((DLN_PIN_ROLE)0)
#define DLN_PIN_ROLE_SPI_SLAVE_SYNC_SCLK    ((DLN_PIN_ROLE)0)
#define DLN_PIN_ROLE_SPI_SLAVE_SYNC_SS      ((DLN_PIN_ROLE)0)


#define DLN_MSG_ID_SPI_SLAVE_SYNC_GET_PORT_COUNT    DLN_BUILD_MSG_ID(0x00, DLN_MODULE_SPI_SLAVE_SYNC)

#define DLN_MSG_ID_SPI_SLAVE_SYNC_ENABLE            DLN_BUILD_MSG_ID(0x01, DLN_MODULE_SPI_SLAVE_SYNC)
#define DLN_MSG_ID_SPI_SLAVE_SYNC_DISABLE           DLN_BUILD_MSG_ID(0x02, DLN_MODULE_SPI_SLAVE_SYNC)
#define DLN_MSG_ID_SPI_SLAVE_SYNC_IS_ENABLED        DLN_BUILD_MSG_ID(0x03, DLN_MODULE_SPI_SLAVE_SYNC)

#define DLN_MSG_ID_SPI_SLAVE_SYNC_SET_MODE          DLN_BUILD_MSG_ID(0x04, DLN_MODULE_SPI_SLAVE_SYNC)
#define DLN_MSG_ID_SPI_SLAVE_SYNC_GET_MODE          DLN_BUILD_MSG_ID(0x05, DLN_MODULE_SPI_SLAVE_SYNC)
#define DLN_MSG_ID_SPI_SLAVE_SYNC_SET_CPHA          DLN_BUILD_MSG_ID(0x06, DLN_MODULE_SPI_SLAVE_SYNC)
#define DLN_MSG_ID_SPI_SLAVE_SYNC_GET_CPHA          DLN_BUILD_MSG_ID(0x07, DLN_MODULE_SPI_SLAVE_SYNC)
#define DLN_MSG_ID_SPI_SLAVE_SYNC_SET_CPOL          DLN_BUILD_MSG_ID(0x08, DLN_MODULE_SPI_SLAVE_SYNC)
#define DLN_MSG_ID_SPI_SLAVE_SYNC_GET_CPOL          DLN_BUILD_MSG_ID(0x09, DLN_MODULE_SPI_SLAVE_SYNC)
#define DLN_MSG_ID_SPI_SLAVE_SYNC_SET_FRAME_SIZE    DLN_BUILD_MSG_ID(0x0A, DLN_MODULE_SPI_SLAVE_SYNC)
#define DLN_MSG_ID_SPI_SLAVE_SYNC_GET_FRAME_SIZE    DLN_BUILD_MSG_ID(0x0B, DLN_MODULE_SPI_SLAVE_SYNC)

#define DLN_MSG_ID_SPI_SLAVE_SYNC_CLEAR_QUEUE       DLN_BUILD_MSG_ID(0x10, DLN_MODULE_SPI_SLAVE_SYNC)
#define DLN_MSG_ID_SPI_SLAVE_SYNC_ENQUEUE_TRANSFER  DLN_BUILD_MSG_ID(0x11, DLN_MODULE_SPI_SLAVE_SYNC)

#define DLN_MSG_ID_SPI_SLAVE_SYNC_ENABLE_EVENT      DLN_BUILD_MSG_ID(0x20, DLN_MODULE_SPI_SLAVE_SYNC)
#define DLN_MSG_ID_SPI_SLAVE_SYNC_DISABLE_EVENT     DLN_BUILD_MSG_ID(0x21, DLN_MODULE_SPI_SLAVE_SYNC)
#define DLN_MSG_ID_SPI_SLAVE_SYNC_IS_EVENT_ENABLED  DLN_BUILD_MSG_ID(0x22, DLN_MODULE_SPI_SLAVE_SYNC)
#define DLN_MSG_ID_SPI_SLAVE_SYNC_TRANSFER_EV       DLN_BUILD_MSG_ID(0xFF, DLN_MODULE_SPI_SLAVE_SYNC)

#define DLN_MSG_ID_SPI_SLAVE_SYNC_GET_SUPPORTED_MODES           DLN_BUILD_MSG_ID(0x40, DLN_MODULE_SPI_SLAVE_SYNC)
#define DLN_MSG_ID_SPI_SLAVE_SYNC_GET_SUPPORTED_CPHA_VALUES     DLN_BUILD_MSG_ID(0x41, DLN_MODULE_SPI_SLAVE_SYNC)
#define DLN_MSG_ID_SPI_SLAVE_SYNC_GET_SUPPORTED_CPOL_VALUES     DLN_BUILD_MSG_ID(0x42, DLN_MODULE_SPI_SLAVE_SYNC)
#define DLN_MSG_ID_SPI_SLAVE_SYNC_GET_SUPPORTED_FRAME_SIZES     DLN_BUILD_MSG_ID(0x43, DLN_MODULE_SPI_SLAVE_SYNC)



#define SPI_SLAVE_SYNC_BUFFER_SIZE          256

#define SPI_SLAVE_SYNC_ENABLED              1
#define SPI_SLAVE_SYNC_DISABLED             0

#define DLN_SPI_SLAVE_SYNC_CPHA_BIT         (1 << 0)
#define DLN_SPI_SLAVE_SYNC_CPHA_0           (0 << 0)
#define DLN_SPI_SLAVE_SYNC_CPHA_1           (1 << 0)
#define DLN_SPI_SLAVE_SYNC_CPOL_BIT         (1 << 1)
#define DLN_SPI_SLAVE_SYNC_CPOL_0           (0 << 1)
#define DLN_SPI_SLAVE_SYNC_CPOL_1           (1 << 1)

#define DLN_SPI_SLAVE_SYNC_FRAME_SIZE_8     8
#define DLN_SPI_SLAVE_SYNC_FRAME_SIZE_9     9
#define DLN_SPI_SLAVE_SYNC_FRAME_SIZE_10    10
#define DLN_SPI_SLAVE_SYNC_FRAME_SIZE_11    11
#define DLN_SPI_SLAVE_SYNC_FRAME_SIZE_12    12
#define DLN_SPI_SLAVE_SYNC_FRAME_SIZE_13    13
#define DLN_SPI_SLAVE_SYNC_FRAME_SIZE_14    14
#define DLN_SPI_SLAVE_SYNC_FRAME_SIZE_15    15
#define DLN_SPI_SLAVE_SYNC_FRAME_SIZE_16    16

#define DLN_SPI_SLAVE_SYNC_EVENT_ENABLED    1
#define DLN_SPI_SLAVE_SYNC_EVENT_DISABLED   0


typedef struct
{
    DLN_MSG_HEADER header;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_GET_PORT_COUNT_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
    uint8_t count;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_GET_PORT_COUNT_RSP;


typedef struct
{
    DLN_MSG_HEADER header;
    uint8_t port;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_ENABLE_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_ENABLE_RSP;


typedef struct
{
    DLN_MSG_HEADER header;
    uint8_t port;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_DISABLE_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_DISABLE_RSP;


typedef struct
{
    DLN_MSG_HEADER header;
    uint8_t port;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_IS_ENABLED_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
    uint8_t enabled;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_IS_ENABLED_RSP;


typedef struct
{
    DLN_MSG_HEADER header;
    uint8_t port;
    uint8_t mode;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_SET_MODE_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_SET_MODE_RSP;


typedef struct
{
    DLN_MSG_HEADER header;
    uint8_t port;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_GET_MODE_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
    uint8_t mode;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_GET_MODE_RSP;


typedef struct
{
    DLN_MSG_HEADER header;
    uint8_t port;
    uint8_t cpha;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_SET_CPHA_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_SET_CPHA_RSP;


typedef struct
{
    DLN_MSG_HEADER header;
    uint8_t port;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_GET_CPHA_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
    uint8_t cpha;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_GET_CPHA_RSP;


typedef struct
{
    DLN_MSG_HEADER header;
    uint8_t port;
    uint8_t cpol;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_SET_CPOL_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_SET_CPOL_RSP;


typedef struct
{
    DLN_MSG_HEADER header;
    uint8_t port;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_GET_CPOL_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
    uint8_t cpol;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_GET_CPOL_RSP;


typedef struct
{
    DLN_MSG_HEADER header;
    uint8_t port;
    uint8_t frameSize;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_SET_FRAME_SIZE_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_SET_FRAME_SIZE_RSP;


typedef struct
{
    DLN_MSG_HEADER header;
    uint8_t port;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_GET_FRAME_SIZE_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
    uint8_t frameSize;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_GET_FRAME_SIZE_RSP;


typedef struct
{
    DLN_MSG_HEADER header;
    uint8_t port;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_CLEAR_QUEUE_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_CLEAR_QUEUE_RSP;


typedef struct
{
    DLN_MSG_HEADER header;
    uint8_t port;
    uint16_t size;
    uint8_t attribute;  // reserved
    uint8_t buffer[DLN_SPI_SLAVE_SYNC_BUFFER_SIZE];
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_ENQUEUE_TRANSFER_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_ENQUEUE_TRANSFER_RSP;


typedef struct
{
    DLN_MSG_HEADER header;
    uint8_t port;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_ENABLE_EVENT_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_ENABLE_EVENT_RSP;


typedef struct
{
    DLN_MSG_HEADER header;
    uint8_t port;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_DISABLE_EVENT_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_DISABLE_EVENT_RSP;


typedef struct
{
    DLN_MSG_HEADER header;
    uint8_t port;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_IS_EVENT_ENABLED_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
    uint8_t enabled;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_IS_EVENT_ENABLED_RSP;


typedef struct
{
    DLN_MSG_HEADER header;
    uint16_t eventCount;
    uint8_t port;
    uint16_t size;
    uint8_t attribute;
    uint8_t buffer[DLN_SPI_SLAVE_SYNC_BUFFER_SIZE];
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_TRANSFER_EV;


typedef struct
{
    uint8_t count;
    uint8_t values[4];
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_MODE_VALUES;

typedef struct
{
    DLN_MSG_HEADER header;
    uint8_t port;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_GET_SUPPORTED_MODES_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
    DLN_SPI_SLAVE_SYNC_MODE_VALUES values;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_GET_SUPPORTED_MODES_RSP;


typedef struct
{
    uint8_t count;
    uint8_t values[2];
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_CPHA_VALUES;

typedef struct
{
    DLN_MSG_HEADER header;
    uint8_t port;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_GET_SUPPORTED_CPHA_VALUES_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
    DLN_SPI_SLAVE_SYNC_CPHA_VALUES values;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_GET_SUPPORTED_CPHA_VALUES_RSP;


typedef struct
{
    uint8_t count;
    uint8_t values[2];
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_CPOL_VALUES;

typedef struct
{
    DLN_MSG_HEADER header;
    uint8_t port;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_GET_SUPPORTED_CPOL_VALUES_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
    DLN_SPI_SLAVE_SYNC_CPOL_VALUES values;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_GET_SUPPORTED_CPOL_VALUES_RSP;


typedef struct
{
    uint8_t count;
    uint8_t frameSizes[36];
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_FRAME_SIZES;

typedef struct
{
    DLN_MSG_HEADER header;
    uint8_t port;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_GET_SUPPORTED_FRAME_SIZES_CMD;

typedef struct
{
    DLN_MSG_HEADER header;
    DLN_RESULT result;
    DLN_SPI_SLAVE_SYNC_FRAME_SIZES supportedSizes;
} __PACKED_ATTR DLN_SPI_SLAVE_SYNC_GET_SUPPORTED_FRAME_SIZES_RSP;










#ifdef __ICCARM__
        #pragma pack()
#elif defined(WIN32)
        #pragma pack(pop)
#endif

#ifdef __cplusplus
}
#endif

#endif // DLN_SPI_SLAVE_SYNC_H
