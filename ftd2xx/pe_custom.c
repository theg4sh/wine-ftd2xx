struct pe_event
{
    UINT32 handle;
    UINT64 registration;
    HANDLE windows_event;
    HANDLE thread;
    struct pe_event *next;
};

static CRITICAL_SECTION pe_events_lock;
static struct pe_event *pe_events;

static void pe_stop_event(UINT32 handle)
{
    struct pe_event **link, *event = NULL;
    struct event_registration_params params;

    EnterCriticalSection(&pe_events_lock);
    for (link = &pe_events; *link; link = &(*link)->next)
    {
        if ((*link)->handle == handle)
        {
            event = *link;
            *link = event->next;
            break;
        }
    }
    LeaveCriticalSection(&pe_events_lock);
    if (!event) return;

    params.registration = event->registration;
    UNIX_CALL(event_stop, &params);
    if (event->thread)
    {
        WaitForSingleObject(event->thread, INFINITE);
        CloseHandle(event->thread);
    }
    UNIX_CALL(event_destroy, &params);
    free(event);
}

static DWORD WINAPI pe_event_thread(void *arg)
{
    struct pe_event *event = arg;
    struct event_wait_params params;

    for (;;)
    {
        params.registration = event->registration;
        params.signaled = 0;
        if (UNIX_CALL(event_wait, &params)) break;
        if (!params.signaled) break;
        if (event->windows_event) SetEvent(event->windows_event);
    }
    return 0;
}

FT_STATUS WINAPI FT_Open(int deviceNumber, FT_HANDLE *pHandle)
{
    struct FT_Open_params params = {0};
    UINT32 token = 0;

    if (!pHandle) return FT_INVALID_PARAMETER;
    params.deviceNumber = deviceNumber;
    params.pHandle = &token;
    UNIX_CALL(FT_Open, &params);
    if (params.result == FT_OK) *pHandle = ULongToPtr(token);
    return params.result;
}

FT_STATUS WINAPI FT_OpenEx(PVOID pvArg1, DWORD dwFlags, FT_HANDLE *pHandle)
{
    struct FT_OpenEx_params params = {0};
    UINT32 token = 0;

    if (!pHandle) return FT_INVALID_PARAMETER;
    params.pvArg1 = pvArg1;
    params.dwFlags = dwFlags;
    params.pHandle = &token;
    UNIX_CALL(FT_OpenEx, &params);
    if (params.result == FT_OK) *pHandle = ULongToPtr(token);
    return params.result;
}

FT_STATUS WINAPI FT_Close(FT_HANDLE ftHandle)
{
    struct FT_Close_params params = {0};
    UINT32 token = PtrToUlong(ftHandle);

    pe_stop_event(token);
    params.ftHandle = token;
    UNIX_CALL(FT_Close, &params);
    return params.result;
}

FT_STATUS WINAPI FT_ListDevices(PVOID pvArg1, PVOID pvArg2, DWORD dwFlags)
{
    struct FT_ListDevices_params params = {0};
    DWORD mode = dwFlags & FT_OPEN_MASK;

    params.pvArg1 = pvArg1;
    params.pvArg2 = pvArg2;
    params.dwFlags = dwFlags;

    if ((dwFlags & FT_LIST_ALL) && mode != FT_OPEN_BY_LOCATION)
    {
        PVOID *buffers = pvArg1;
        UINT32 *buffers32;
        DWORD i, count;

        if (!pvArg1 || !pvArg2) return FT_INVALID_PARAMETER;
        count = *(DWORD *)pvArg2;
        if (count > 65536 || !(buffers32 = calloc(count + 1, sizeof(*buffers32))))
            return FT_INSUFFICIENT_RESOURCES;
        for (i = 0; i < count; i++) buffers32[i] = PtrToUlong(buffers[i]);
        params.pvArg1 = buffers32;
        UNIX_CALL(FT_ListDevices, &params);
        free(buffers32);
    }
    else UNIX_CALL(FT_ListDevices, &params);
    return params.result;
}

FT_STATUS WINAPI FT_GetDeviceInfoList(FT_DEVICE_LIST_INFO_NODE *pDest, LPDWORD lpdwNumDevs)
{
    struct FT_GetDeviceInfoList_params params = {0};
    struct ft_device_info_node32 *nodes = NULL;
    DWORD count, i;

    if (!lpdwNumDevs) return FT_INVALID_PARAMETER;
    count = *lpdwNumDevs;
    if (pDest && count && !(nodes = calloc(count, sizeof(*nodes))))
        return FT_INSUFFICIENT_RESOURCES;

    params.pDest = nodes;
    params.lpdwNumDevs = lpdwNumDevs;
    UNIX_CALL(FT_GetDeviceInfoList, &params);
    if (params.result == FT_OK && pDest)
    {
        if (*lpdwNumDevs < count) count = *lpdwNumDevs;
        for (i = 0; i < count; i++)
        {
            pDest[i].Flags = nodes[i].Flags;
            pDest[i].Type = nodes[i].Type;
            pDest[i].ID = nodes[i].ID;
            pDest[i].LocId = nodes[i].LocId;
            memcpy(pDest[i].SerialNumber, nodes[i].SerialNumber, sizeof(pDest[i].SerialNumber));
            memcpy(pDest[i].Description, nodes[i].Description, sizeof(pDest[i].Description));
            pDest[i].ftHandle = ULongToPtr(nodes[i].ftHandle);
        }
    }
    free(nodes);
    return params.result;
}

FT_STATUS WINAPI FT_GetDeviceInfoDetail(DWORD dwIndex, LPDWORD lpdwFlags,
        LPDWORD lpdwType, LPDWORD lpdwID, LPDWORD lpdwLocId,
        LPVOID lpSerialNumber, LPVOID lpDescription, FT_HANDLE *pftHandle)
{
    struct FT_GetDeviceInfoDetail_params params = {0};
    UINT32 token = 0;

    if (!pftHandle) return FT_INVALID_PARAMETER;
    params.dwIndex = dwIndex;
    params.lpdwFlags = lpdwFlags;
    params.lpdwType = lpdwType;
    params.lpdwID = lpdwID;
    params.lpdwLocId = lpdwLocId;
    params.lpSerialNumber = lpSerialNumber;
    params.lpDescription = lpDescription;
    params.pftHandle = &token;
    UNIX_CALL(FT_GetDeviceInfoDetail, &params);
    if (params.result == FT_OK) *pftHandle = ULongToPtr(token);
    return params.result;
}

FT_STATUS WINAPI FT_SetEventNotification(FT_HANDLE ftHandle, DWORD dwEventMask, PVOID pvArg)
{
    struct FT_SetEventNotification_params params = {0};
    struct pe_event *event;
    UINT32 token = PtrToUlong(ftHandle);

    pe_stop_event(token);
    params.ftHandle = token;
    params.dwEventMask = dwEventMask;
    UNIX_CALL(FT_SetEventNotification, &params);
    if (params.result != FT_OK) return params.result;

    if (!(event = calloc(1, sizeof(*event))))
    {
        struct event_registration_params registration = { params.registration };
        UNIX_CALL(event_stop, &registration);
        UNIX_CALL(event_destroy, &registration);
        return FT_INSUFFICIENT_RESOURCES;
    }
    event->handle = token;
    event->registration = params.registration;
    event->windows_event = pvArg;

    EnterCriticalSection(&pe_events_lock);
    event->next = pe_events;
    pe_events = event;
    LeaveCriticalSection(&pe_events_lock);

    if (pvArg)
    {
        event->thread = CreateThread(NULL, 0, pe_event_thread, event, 0, NULL);
        if (!event->thread)
        {
            pe_stop_event(token);
            return FT_OTHER_ERROR;
        }
    }
    return FT_OK;
}

static BOOL load_functions(void)
{
    const char *soname = getenv("FTD2XX_LIBRARY");

    if (!soname || !*soname) soname = FALLBACK_LIBFTD2XX;
    TRACE("Loading library '%s'\n", soname);
    if (!UNIX_CALL(attach, (void *)soname)) return TRUE;
    ERR("Wine cannot find a usable libftd2xx.so; ftd2xx.dll is unavailable.\n");
    return FALSE;
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(instance);
        InitializeCriticalSection(&pe_events_lock);
        if (__wine_init_unix_call() || !load_functions())
        {
            DeleteCriticalSection(&pe_events_lock);
            return FALSE;
        }
        break;
    case DLL_PROCESS_DETACH:
        if (reserved) break;
        while (pe_events) pe_stop_event(pe_events->handle);
        UNIX_CALL(detach, NULL);
        DeleteCriticalSection(&pe_events_lock);
        break;
    }
    return TRUE;
}
