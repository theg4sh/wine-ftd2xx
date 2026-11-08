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


struct FT_Open_params
{
    INT32 deviceNumber;
    UINT32 * pHandle;
    UINT32 result;
};

struct FT_Close_params
{
    UINT32 ftHandle;
    UINT32 result;
};

struct FT_Read_params
{
    UINT32 ftHandle;
    LPVOID lpBuffer;
    UINT32 dwBytesToRead;
    LPDWORD lpdwBytesReturned;
    UINT32 result;
};

struct FT_Write_params
{
    UINT32 ftHandle;
    LPVOID lpBuffer;
    UINT32 dwBytesToWrite;
    LPDWORD lpdwBytesWritten;
    UINT32 result;
};

struct FT_IoCtl_params
{
    UINT32 ftHandle;
    UINT32 dwIoControlCode;
    LPVOID lpInBuf;
    UINT32 nInBufSize;
    LPVOID lpOutBuf;
    UINT32 nOutBufSize;
    LPDWORD lpBytesReturned;
    LPOVERLAPPED lpOverlapped;
    UINT32 result;
};

struct FT_ResetDevice_params
{
    UINT32 ftHandle;
    UINT32 result;
};

struct FT_SetBaudRate_params
{
    UINT32 ftHandle;
    UINT32 dwBaudRate;
    UINT32 result;
};

struct FT_SetDataCharacteristics_params
{
    UINT32 ftHandle;
    UINT8 uWordLength;
    UINT8 uStopBits;
    UINT8 uParity;
    UINT32 result;
};

struct FT_SetFlowControl_params
{
    UINT32 ftHandle;
    UINT16 usFlowControl;
    UINT8 uXonChar;
    UINT8 uXoffChar;
    UINT32 result;
};

struct FT_SetDtr_params
{
    UINT32 ftHandle;
    UINT32 result;
};

struct FT_ClrDtr_params
{
    UINT32 ftHandle;
    UINT32 result;
};

struct FT_SetRts_params
{
    UINT32 ftHandle;
    UINT32 result;
};

struct FT_ClrRts_params
{
    UINT32 ftHandle;
    UINT32 result;
};

struct FT_GetModemStatus_params
{
    UINT32 ftHandle;
    ULONG * lpdwModemStatus;
    UINT32 result;
};

struct FT_SetChars_params
{
    UINT32 ftHandle;
    UINT8 uEventChar;
    UINT8 uEventCharEnabled;
    UINT8 uErrorChar;
    UINT8 uErrorCharEnabled;
    UINT32 result;
};

struct FT_Purge_params
{
    UINT32 ftHandle;
    UINT32 ulMask;
    UINT32 result;
};

struct FT_SetTimeouts_params
{
    UINT32 ftHandle;
    UINT32 dwReadTimeout;
    UINT32 dwWriteTimeout;
    UINT32 result;
};

struct FT_GetQueueStatus_params
{
    UINT32 ftHandle;
    DWORD * lpdwAmountInRxQueue;
    UINT32 result;
};

struct FT_SetEventNotification_params
{
    UINT32 ftHandle;
    UINT32 dwEventMask;
    PVOID pvArg;
    UINT64 registration;
    UINT32 result;
};

struct FT_GetEventStatus_params
{
    UINT32 ftHandle;
    DWORD * dwEventDWord;
    UINT32 result;
};

struct FT_GetStatus_params
{
    UINT32 ftHandle;
    DWORD * lpdwAmountInRxQueue;
    DWORD * lpdwAmountInTxQueue;
    DWORD * lpdwEventStatus;
    UINT32 result;
};

struct FT_SetBreakOn_params
{
    UINT32 ftHandle;
    UINT32 result;
};

struct FT_SetBreakOff_params
{
    UINT32 ftHandle;
    UINT32 result;
};

struct FT_SetWaitMask_params
{
    UINT32 ftHandle;
    UINT32 Mask;
    UINT32 result;
};

struct FT_WaitOnMask_params
{
    UINT32 ftHandle;
    DWORD * Mask;
    UINT32 result;
};

struct FT_SetDivisor_params
{
    UINT32 ftHandle;
    UINT16 usDivisor;
    UINT32 result;
};

struct FT_OpenEx_params
{
    PVOID pvArg1;
    UINT32 dwFlags;
    UINT32 * pHandle;
    UINT32 result;
};

struct FT_ListDevices_params
{
    PVOID pvArg1;
    PVOID pvArg2;
    UINT32 dwFlags;
    UINT32 result;
};

struct FT_SetLatencyTimer_params
{
    UINT32 ftHandle;
    UINT8 ucLatency;
    UINT32 result;
};

struct FT_GetLatencyTimer_params
{
    UINT32 ftHandle;
    PUCHAR pucLatency;
    UINT32 result;
};

struct FT_SetBitMode_params
{
    UINT32 ftHandle;
    UINT8 ucMask;
    UINT8 ucEnable;
    UINT32 result;
};

struct FT_GetBitMode_params
{
    UINT32 ftHandle;
    PUCHAR pucMode;
    UINT32 result;
};

struct FT_SetUSBParameters_params
{
    UINT32 ftHandle;
    UINT32 ulInTransferSize;
    UINT32 ulOutTransferSize;
    UINT32 result;
};

struct FT_EraseEE_params
{
    UINT32 ftHandle;
    UINT32 result;
};

struct FT_ReadEE_params
{
    UINT32 ftHandle;
    UINT32 dwWordOffset;
    LPWORD lpwValue;
    UINT32 result;
};

struct FT_WriteEE_params
{
    UINT32 ftHandle;
    UINT32 dwWordOffset;
    UINT16 wValue;
    UINT32 result;
};

struct FT_EE_Program_params
{
    UINT32 ftHandle;
    PFT_PROGRAM_DATA pData;
    UINT32 result;
};

struct FT_EE_Read_params
{
    UINT32 ftHandle;
    PFT_PROGRAM_DATA pData;
    UINT32 result;
};

struct FT_EE_UARead_params
{
    UINT32 ftHandle;
    PUCHAR pucData;
    UINT32 dwDataLen;
    LPDWORD lpdwBytesRead;
    UINT32 result;
};

struct FT_EE_UASize_params
{
    UINT32 ftHandle;
    LPDWORD lpdwSize;
    UINT32 result;
};

struct FT_EE_UAWrite_params
{
    UINT32 ftHandle;
    PUCHAR pucData;
    UINT32 dwDataLen;
    UINT32 result;
};

struct FT_W32_CreateFile_params
{
    LPCSTR lpszName;
    UINT32 dwAccess;
    UINT32 dwShareMode;
    LPSECURITY_ATTRIBUTES lpSecurityAttributes;
    UINT32 dwCreate;
    UINT32 dwAttrsAndFlags;
    HANDLE hTemplate;
    UINT32 result;
};

struct FT_W32_CloseHandle_params
{
    UINT32 ftHandle;
    UINT32 result;
};

struct FT_W32_ReadFile_params
{
    UINT32 ftHandle;
    LPVOID lpBuffer;
    UINT32 nBufferSize;
    LPDWORD lpdwBytesReturned;
    LPOVERLAPPED lpOverlapped;
    UINT32 result;
};

struct FT_W32_WriteFile_params
{
    UINT32 ftHandle;
    LPVOID lpBuffer;
    UINT32 nBufferSize;
    LPDWORD lpdwBytesWritten;
    LPOVERLAPPED lpOverlapped;
    UINT32 result;
};

struct FT_W32_GetOverlappedResult_params
{
    UINT32 ftHandle;
    LPOVERLAPPED lpOverlapped;
    LPDWORD lpdwBytesTransferred;
    INT32 bWait;
    UINT32 result;
};

struct FT_W32_ClearCommBreak_params
{
    UINT32 ftHandle;
    UINT32 result;
};

struct FT_W32_ClearCommError_params
{
    UINT32 ftHandle;
    LPDWORD lpdwErrors;
    LPFTCOMSTAT lpftComstat;
    UINT32 result;
};

struct FT_W32_EscapeCommFunction_params
{
    UINT32 ftHandle;
    UINT32 dwFunc;
    UINT32 result;
};

struct FT_W32_GetCommModemStatus_params
{
    UINT32 ftHandle;
    LPDWORD lpdwModemStatus;
    UINT32 result;
};

struct FT_W32_GetCommState_params
{
    UINT32 ftHandle;
    LPFTDCB lpftDcb;
    UINT32 result;
};

struct FT_W32_GetCommTimeouts_params
{
    UINT32 ftHandle;
    FTTIMEOUTS * pftTimeouts;
    UINT32 result;
};

struct FT_W32_GetLastError_params
{
    UINT32 ftHandle;
    UINT32 result;
};

struct FT_W32_PurgeComm_params
{
    UINT32 ftHandle;
    UINT32 dwMask;
    UINT32 result;
};

struct FT_W32_SetCommBreak_params
{
    UINT32 ftHandle;
    UINT32 result;
};

struct FT_W32_SetCommMask_params
{
    UINT32 ftHandle;
    UINT32 ulEventMask;
    UINT32 result;
};

struct FT_W32_SetCommState_params
{
    UINT32 ftHandle;
    LPFTDCB lpftDcb;
    UINT32 result;
};

struct FT_W32_SetCommTimeouts_params
{
    UINT32 ftHandle;
    FTTIMEOUTS * pftTimeouts;
    UINT32 result;
};

struct FT_W32_SetupComm_params
{
    UINT32 ftHandle;
    UINT32 dwReadBufferSize;
    UINT32 dwWriteBufferSize;
    UINT32 result;
};

struct FT_W32_WaitCommEvent_params
{
    UINT32 ftHandle;
    PULONG pulEvent;
    LPOVERLAPPED lpOverlapped;
    UINT32 result;
};

struct FT_GetDeviceInfo_params
{
    UINT32 ftHandle;
    FT_DEVICE * lpftDevice;
    LPDWORD lpdwID;
    PCHAR pcSerialNumber;
    PCHAR pcDescription;
    LPVOID pvDummy;
    UINT32 result;
};

struct FT_W32_CancelIo_params
{
    UINT32 ftHandle;
    UINT32 result;
};

struct FT_StopInTask_params
{
    UINT32 ftHandle;
    UINT32 result;
};

struct FT_RestartInTask_params
{
    UINT32 ftHandle;
    UINT32 result;
};

struct FT_SetResetPipeRetryCount_params
{
    UINT32 ftHandle;
    UINT32 dwCount;
    UINT32 result;
};

struct FT_ResetPort_params
{
    UINT32 ftHandle;
    UINT32 result;
};

struct FT_EE_ProgramEx_params
{
    UINT32 ftHandle;
    PFT_PROGRAM_DATA pData;
    char * Manufacturer;
    char * ManufacturerId;
    char * Description;
    char * SerialNumber;
    UINT32 result;
};

struct FT_EE_ReadEx_params
{
    UINT32 ftHandle;
    PFT_PROGRAM_DATA pData;
    char * Manufacturer;
    char * ManufacturerId;
    char * Description;
    char * SerialNumber;
    UINT32 result;
};

struct FT_CyclePort_params
{
    UINT32 ftHandle;
    UINT32 result;
};

struct FT_CreateDeviceInfoList_params
{
    LPDWORD lpdwNumDevs;
    UINT32 result;
};

struct FT_GetDeviceInfoList_params
{
    void * pDest;
    LPDWORD lpdwNumDevs;
    UINT32 result;
};

struct FT_GetDeviceInfoDetail_params
{
    UINT32 dwIndex;
    LPDWORD lpdwFlags;
    LPDWORD lpdwType;
    LPDWORD lpdwID;
    LPDWORD lpdwLocId;
    LPVOID lpSerialNumber;
    LPVOID lpDescription;
    UINT32 * pftHandle;
    UINT32 result;
};

struct FT_SetDeadmanTimeout_params
{
    UINT32 ftHandle;
    UINT32 ulDeadmanTimeout;
    UINT32 result;
};

struct FT_GetDriverVersion_params
{
    UINT32 ftHandle;
    LPDWORD lpdwDriverVersion;
    UINT32 result;
};

struct FT_GetLibraryVersion_params
{
    LPDWORD lpdwDLLVersion;
    UINT32 result;
};

struct FT_W32_GetCommMask_params
{
    UINT32 ftHandle;
    LPDWORD lpdwEventMask;
    UINT32 result;
};

struct FT_Rescan_params
{
    UINT32 result;
};

struct FT_Reload_params
{
    UINT16 wVID;
    UINT16 wPID;
    UINT32 result;
};

struct FT_GetComPortNumber_params
{
    UINT32 ftHandle;
    LPLONG lplComPortNumber;
    UINT32 result;
};

struct FT_EE_ReadConfig_params
{
    UINT32 ftHandle;
    UINT8 ucAddress;
    PUCHAR pucValue;
    UINT32 result;
};

struct FT_EE_WriteConfig_params
{
    UINT32 ftHandle;
    UINT8 ucAddress;
    UINT8 ucValue;
    UINT32 result;
};

struct FT_EE_ReadECC_params
{
    UINT32 ftHandle;
    UINT8 ucOption;
    LPWORD lpwValue;
    UINT32 result;
};

struct FT_GetQueueStatusEx_params
{
    UINT32 ftHandle;
    DWORD * dwRxBytes;
    UINT32 result;
};

struct FT_EEPROM_Read_params
{
    UINT32 ftHandle;
    void * eepromData;
    UINT32 eepromDataSize;
    char * Manufacturer;
    char * ManufacturerId;
    char * Description;
    char * SerialNumber;
    UINT32 result;
};

struct FT_EEPROM_Program_params
{
    UINT32 ftHandle;
    void * eepromData;
    UINT32 eepromDataSize;
    char * Manufacturer;
    char * ManufacturerId;
    char * Description;
    char * SerialNumber;
    UINT32 result;
};

struct FT_ComPortIdle_params
{
    UINT32 ftHandle;
    UINT32 result;
};

struct FT_ComPortCancelIdle_params
{
    UINT32 ftHandle;
    UINT32 result;
};

struct FT_VendorCmdGet_params
{
    UINT32 ftHandle;
    UINT8 Request;
    UCHAR * Buf;
    UINT16 Len;
    UINT32 result;
};

struct FT_VendorCmdSet_params
{
    UINT32 ftHandle;
    UINT8 Request;
    UCHAR * Buf;
    UINT16 Len;
    UINT32 result;
};

struct FT_VendorCmdGetEx_params
{
    UINT32 ftHandle;
    UINT16 wValue;
    UCHAR * Buf;
    UINT16 Len;
    UINT32 result;
};

struct FT_VendorCmdSetEx_params
{
    UINT32 ftHandle;
    UINT16 wValue;
    UCHAR * Buf;
    UINT16 Len;
    UINT32 result;
};

enum ftd2xx_unix_funcs
{
    unix_attach,
    unix_detach,
    unix_event_wait,
    unix_event_stop,
    unix_event_destroy,
    unix_FT_Open,
    unix_FT_Close,
    unix_FT_Read,
    unix_FT_Write,
    unix_FT_IoCtl,
    unix_FT_ResetDevice,
    unix_FT_SetBaudRate,
    unix_FT_SetDataCharacteristics,
    unix_FT_SetFlowControl,
    unix_FT_SetDtr,
    unix_FT_ClrDtr,
    unix_FT_SetRts,
    unix_FT_ClrRts,
    unix_FT_GetModemStatus,
    unix_FT_SetChars,
    unix_FT_Purge,
    unix_FT_SetTimeouts,
    unix_FT_GetQueueStatus,
    unix_FT_SetEventNotification,
    unix_FT_GetEventStatus,
    unix_FT_GetStatus,
    unix_FT_SetBreakOn,
    unix_FT_SetBreakOff,
    unix_FT_SetWaitMask,
    unix_FT_WaitOnMask,
    unix_FT_SetDivisor,
    unix_FT_OpenEx,
    unix_FT_ListDevices,
    unix_FT_SetLatencyTimer,
    unix_FT_GetLatencyTimer,
    unix_FT_SetBitMode,
    unix_FT_GetBitMode,
    unix_FT_SetUSBParameters,
    unix_FT_EraseEE,
    unix_FT_ReadEE,
    unix_FT_WriteEE,
    unix_FT_EE_Program,
    unix_FT_EE_Read,
    unix_FT_EE_UARead,
    unix_FT_EE_UASize,
    unix_FT_EE_UAWrite,
    unix_FT_W32_CreateFile,
    unix_FT_W32_CloseHandle,
    unix_FT_W32_ReadFile,
    unix_FT_W32_WriteFile,
    unix_FT_W32_GetOverlappedResult,
    unix_FT_W32_ClearCommBreak,
    unix_FT_W32_ClearCommError,
    unix_FT_W32_EscapeCommFunction,
    unix_FT_W32_GetCommModemStatus,
    unix_FT_W32_GetCommState,
    unix_FT_W32_GetCommTimeouts,
    unix_FT_W32_GetLastError,
    unix_FT_W32_PurgeComm,
    unix_FT_W32_SetCommBreak,
    unix_FT_W32_SetCommMask,
    unix_FT_W32_SetCommState,
    unix_FT_W32_SetCommTimeouts,
    unix_FT_W32_SetupComm,
    unix_FT_W32_WaitCommEvent,
    unix_FT_GetDeviceInfo,
    unix_FT_W32_CancelIo,
    unix_FT_StopInTask,
    unix_FT_RestartInTask,
    unix_FT_SetResetPipeRetryCount,
    unix_FT_ResetPort,
    unix_FT_EE_ProgramEx,
    unix_FT_EE_ReadEx,
    unix_FT_CyclePort,
    unix_FT_CreateDeviceInfoList,
    unix_FT_GetDeviceInfoList,
    unix_FT_GetDeviceInfoDetail,
    unix_FT_SetDeadmanTimeout,
    unix_FT_GetDriverVersion,
    unix_FT_GetLibraryVersion,
    unix_FT_W32_GetCommMask,
    unix_FT_Rescan,
    unix_FT_Reload,
    unix_FT_GetComPortNumber,
    unix_FT_EE_ReadConfig,
    unix_FT_EE_WriteConfig,
    unix_FT_EE_ReadECC,
    unix_FT_GetQueueStatusEx,
    unix_FT_EEPROM_Read,
    unix_FT_EEPROM_Program,
    unix_FT_ComPortIdle,
    unix_FT_ComPortCancelIdle,
    unix_FT_VendorCmdGet,
    unix_FT_VendorCmdSet,
    unix_FT_VendorCmdGetEx,
    unix_FT_VendorCmdSetEx,
    unix_funcs_count
};

#endif /* __FTD2XX_UNIXLIB_H */
