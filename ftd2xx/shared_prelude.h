#ifndef __FTD2XX_UNIXLIB_H
#define __FTD2XX_UNIXLIB_H

#include "wine/unixlib.h"

typedef PVOID FT_HANDLE;
typedef ULONG FT_STATUS;
typedef ULONG FT_DEVICE;

enum
{
    FT_OK,
    FT_INVALID_HANDLE,
    FT_DEVICE_NOT_FOUND,
    FT_DEVICE_NOT_OPENED,
    FT_IO_ERROR,
    FT_INSUFFICIENT_RESOURCES,
    FT_INVALID_PARAMETER,
    FT_INVALID_BAUD_RATE,
    FT_DEVICE_NOT_OPENED_FOR_ERASE,
    FT_DEVICE_NOT_OPENED_FOR_WRITE,
    FT_FAILED_TO_WRITE_DEVICE,
    FT_EEPROM_READ_FAILED,
    FT_EEPROM_WRITE_FAILED,
    FT_EEPROM_ERASE_FAILED,
    FT_EEPROM_NOT_PRESENT,
    FT_EEPROM_NOT_PROGRAMMED,
    FT_INVALID_ARGS,
    FT_NOT_SUPPORTED,
    FT_OTHER_ERROR,
    FT_DEVICE_LIST_NOT_READY
};

#define FT_OPEN_BY_SERIAL_NUMBER 1
#define FT_OPEN_BY_DESCRIPTION   2
#define FT_OPEN_BY_LOCATION      4
#define FT_OPEN_MASK             (FT_OPEN_BY_SERIAL_NUMBER | FT_OPEN_BY_DESCRIPTION | FT_OPEN_BY_LOCATION)

#define FT_LIST_NUMBER_ONLY 0x80000000
#define FT_LIST_BY_INDEX    0x40000000
#define FT_LIST_ALL         0x20000000
#define FT_LIST_MASK        (FT_LIST_NUMBER_ONLY | FT_LIST_BY_INDEX | FT_LIST_ALL)

typedef struct _FTTIMEOUTS
{
    DWORD ReadIntervalTimeout;
    DWORD ReadTotalTimeoutMultiplier;
    DWORD ReadTotalTimeoutConstant;
    DWORD WriteTotalTimeoutMultiplier;
    DWORD WriteTotalTimeoutConstant;
} FTTIMEOUTS, *LPFTTIMEOUTS;

typedef struct _FTCOMSTAT FTCOMSTAT, *LPFTCOMSTAT;
typedef struct _FTDCB FTDCB, *LPFTDCB;
typedef struct ft_program_data FT_PROGRAM_DATA, *PFT_PROGRAM_DATA;

typedef struct _ft_device_list_info_node
{
    ULONG Flags;
    ULONG Type;
    ULONG ID;
    DWORD LocId;
    char SerialNumber[16];
    char Description[64];
    FT_HANDLE ftHandle;
} FT_DEVICE_LIST_INFO_NODE;

struct ft_device_info_node32
{
    UINT32 Flags;
    UINT32 Type;
    UINT32 ID;
    UINT32 LocId;
    char SerialNumber[16];
    char Description[64];
    UINT32 ftHandle;
};

C_ASSERT(sizeof(struct ft_device_info_node32) == 100);

struct event_wait_params
{
    UINT64 registration;
    UINT32 signaled;
};

struct event_registration_params
{
    UINT64 registration;
};

