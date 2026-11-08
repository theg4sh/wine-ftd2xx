typedef struct _EVENT_HANDLE
{
    pthread_cond_t eCondVar;
    pthread_mutex_t eMutex;
    int iVar;
} EVENT_HANDLE;

struct ft_handle_entry
{
    UINT32 token;
    FT_HANDLE native;
    UINT32 references;
    BOOL closing;
    struct ft_handle_entry *next;
};

struct native_event
{
    UINT64 id;
    EVENT_HANDLE event;
    BOOL closing;
    struct native_event *next;
};

static pthread_mutex_t handles_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t events_mutex = PTHREAD_MUTEX_INITIALIZER;
static struct ft_handle_entry *handles;
static struct native_event *events;
static UINT32 next_token = 1;
static UINT64 next_event_id = 1;

static struct ft_handle_entry *handle_acquire(UINT32 token)
{
    struct ft_handle_entry *handle;

    pthread_mutex_lock(&handles_mutex);
    for (handle = handles; handle; handle = handle->next)
    {
        if (handle->token == token && !handle->closing)
        {
            handle->references++;
            break;
        }
    }
    pthread_mutex_unlock(&handles_mutex);
    return handle;
}

static void handle_release(struct ft_handle_entry *handle)
{
    BOOL destroy = FALSE;

    pthread_mutex_lock(&handles_mutex);
    if (!--handle->references && handle->closing) destroy = TRUE;
    pthread_mutex_unlock(&handles_mutex);
    if (destroy) free(handle);
}

static UINT32 handle_insert(FT_HANDLE native)
{
    struct ft_handle_entry *handle;
    UINT32 token;

    if (!native || !(handle = calloc(1, sizeof(*handle)))) return 0;
    handle->native = native;
    handle->references = 1;

    pthread_mutex_lock(&handles_mutex);
    do
    {
        token = next_token++;
        if (!next_token) next_token = 1;
    } while (!token);
    handle->token = token;
    handle->next = handles;
    handles = handle;
    pthread_mutex_unlock(&handles_mutex);

    handle_release(handle);
    return token;
}

static UINT32 handle_token_for_native(FT_HANDLE native)
{
    struct ft_handle_entry *handle;
    UINT32 token = 0;

    if (!native) return 0;
    pthread_mutex_lock(&handles_mutex);
    for (handle = handles; handle; handle = handle->next)
    {
        if (handle->native == native && !handle->closing)
        {
            token = handle->token;
            break;
        }
    }
    pthread_mutex_unlock(&handles_mutex);
    return token ? token : handle_insert(native);
}

static UINT32 handle_close(UINT32 token)
{
    struct ft_handle_entry **link, *handle = NULL;
    FT_STATUS status;

    pthread_mutex_lock(&handles_mutex);
    for (link = &handles; *link; link = &(*link)->next)
    {
        if ((*link)->token == token && !(*link)->closing)
        {
            handle = *link;
            *link = handle->next;
            handle->closing = TRUE;
            handle->references++;
            break;
        }
    }
    pthread_mutex_unlock(&handles_mutex);
    if (!handle) return FT_INVALID_HANDLE;

    status = pFT_Close(handle->native);
    handle_release(handle);
    return status;
}

static struct native_event *event_find(UINT64 id)
{
    struct native_event *event;

    for (event = events; event; event = event->next)
        if (event->id == id) return event;
    return NULL;
}

static NTSTATUS wrap_event_wait(void *args)
{
    struct event_wait_params *params = args;
    struct native_event *event;

    params->signaled = 0;
    pthread_mutex_lock(&events_mutex);
    event = event_find(params->registration);
    if (!event)
    {
        pthread_mutex_unlock(&events_mutex);
        return STATUS_INVALID_HANDLE;
    }
    pthread_mutex_lock(&event->event.eMutex);
    pthread_mutex_unlock(&events_mutex);

    while (!event->event.iVar && !event->closing)
        pthread_cond_wait(&event->event.eCondVar, &event->event.eMutex);
    if (event->event.iVar && !event->closing)
    {
        event->event.iVar = 0;
        params->signaled = 1;
    }
    pthread_mutex_unlock(&event->event.eMutex);
    return STATUS_SUCCESS;
}

static NTSTATUS wrap_event_stop(void *args)
{
    struct event_registration_params *params = args;
    struct native_event *event;

    pthread_mutex_lock(&events_mutex);
    event = event_find(params->registration);
    if (event)
    {
        pthread_mutex_lock(&event->event.eMutex);
        event->closing = TRUE;
        pthread_cond_broadcast(&event->event.eCondVar);
        pthread_mutex_unlock(&event->event.eMutex);
    }
    pthread_mutex_unlock(&events_mutex);
    return event ? STATUS_SUCCESS : STATUS_INVALID_HANDLE;
}

static NTSTATUS wrap_event_destroy(void *args)
{
    struct event_registration_params *params = args;
    struct native_event **link, *event = NULL;

    pthread_mutex_lock(&events_mutex);
    for (link = &events; *link; link = &(*link)->next)
    {
        if ((*link)->id == params->registration)
        {
            event = *link;
            *link = event->next;
            break;
        }
    }
    pthread_mutex_unlock(&events_mutex);
    if (!event) return STATUS_INVALID_HANDLE;

    pthread_cond_destroy(&event->event.eCondVar);
    pthread_mutex_destroy(&event->event.eMutex);
    free(event);
    return STATUS_SUCCESS;
}

static NTSTATUS wrap_FT_Open(void *args)
{
    struct FT_Open_params *params = args;
    FT_HANDLE native = NULL;

    params->result = pFT_Open(params->deviceNumber, &native);
    if (params->result == FT_OK)
    {
        *params->pHandle = handle_insert(native);
        if (!*params->pHandle)
        {
            pFT_Close(native);
            params->result = FT_INSUFFICIENT_RESOURCES;
        }
    }
    return STATUS_SUCCESS;
}

static NTSTATUS wrap_FT_OpenEx(void *args)
{
    struct FT_OpenEx_params *params = args;
    FT_HANDLE native = NULL;

    params->result = pFT_OpenEx(params->pvArg1, params->dwFlags, &native);
    if (params->result == FT_OK)
    {
        *params->pHandle = handle_insert(native);
        if (!*params->pHandle)
        {
            pFT_Close(native);
            params->result = FT_INSUFFICIENT_RESOURCES;
        }
    }
    return STATUS_SUCCESS;
}

static NTSTATUS wrap_FT_Close(void *args)
{
    struct FT_Close_params *params = args;

    params->result = handle_close(params->ftHandle);
    return STATUS_SUCCESS;
}

static NTSTATUS wrap_FT_ListDevices(void *args)
{
    struct FT_ListDevices_params *params = args;
    DWORD mode = params->dwFlags & FT_OPEN_MASK;

    if (!pFT_ListDevices)
    {
        params->result = FT_NOT_SUPPORTED;
        return STATUS_SUCCESS;
    }

    if ((params->dwFlags & FT_LIST_ALL) && mode != FT_OPEN_BY_LOCATION)
    {
        DWORD i, count;
        char **buffers;

        if (!params->pvArg1 || !params->pvArg2)
        {
            params->result = FT_INVALID_PARAMETER;
            return STATUS_SUCCESS;
        }
        count = *(DWORD *)params->pvArg2;
        if (count > 65536 || !(buffers = calloc(count + 1, sizeof(*buffers))))
        {
            params->result = FT_INSUFFICIENT_RESOURCES;
            return STATUS_SUCCESS;
        }
        for (i = 0; i < count; i++)
            buffers[i] = ULongToPtr(((UINT32 *)params->pvArg1)[i]);
        params->result = pFT_ListDevices(buffers, params->pvArg2, params->dwFlags);
        free(buffers);
    }
    else params->result = pFT_ListDevices(params->pvArg1, params->pvArg2, params->dwFlags);
    return STATUS_SUCCESS;
}

static NTSTATUS wrap_FT_GetDeviceInfoList(void *args)
{
    struct FT_GetDeviceInfoList_params *params = args;
    FT_DEVICE_LIST_INFO_NODE *native = NULL;
    struct ft_device_info_node32 *dest = params->pDest;
    DWORD count, i;

    if (!pFT_GetDeviceInfoList)
    {
        params->result = FT_NOT_SUPPORTED;
        return STATUS_SUCCESS;
    }
    if (!params->lpdwNumDevs)
    {
        params->result = FT_INVALID_PARAMETER;
        return STATUS_SUCCESS;
    }
    count = *params->lpdwNumDevs;
    if (dest && count)
    {
        if (!(native = calloc(count, sizeof(*native))))
        {
            params->result = FT_INSUFFICIENT_RESOURCES;
            return STATUS_SUCCESS;
        }
    }

    params->result = pFT_GetDeviceInfoList(native, params->lpdwNumDevs);
    if (params->result == FT_OK && dest)
    {
        if (*params->lpdwNumDevs < count) count = *params->lpdwNumDevs;
        for (i = 0; i < count; i++)
        {
            dest[i].Flags = native[i].Flags;
            dest[i].Type = native[i].Type;
            dest[i].ID = native[i].ID;
            dest[i].LocId = native[i].LocId;
            memcpy(dest[i].SerialNumber, native[i].SerialNumber, sizeof(dest[i].SerialNumber));
            memcpy(dest[i].Description, native[i].Description, sizeof(dest[i].Description));
            dest[i].ftHandle = handle_token_for_native(native[i].ftHandle);
        }
    }
    free(native);
    return STATUS_SUCCESS;
}

static NTSTATUS wrap_FT_GetDeviceInfoDetail(void *args)
{
    struct FT_GetDeviceInfoDetail_params *params = args;
    FT_HANDLE native = NULL;

    if (!pFT_GetDeviceInfoDetail)
    {
        params->result = FT_NOT_SUPPORTED;
        return STATUS_SUCCESS;
    }
    params->result = pFT_GetDeviceInfoDetail(params->dwIndex, params->lpdwFlags,
            params->lpdwType, params->lpdwID, params->lpdwLocId,
            params->lpSerialNumber, params->lpDescription, &native);
    if (params->result == FT_OK)
        *params->pftHandle = handle_token_for_native(native);
    return STATUS_SUCCESS;
}

static NTSTATUS wrap_FT_SetEventNotification(void *args)
{
    struct FT_SetEventNotification_params *params = args;
    struct ft_handle_entry *handle;
    struct native_event *event;

    if (!pFT_SetEventNotification)
    {
        params->result = FT_NOT_SUPPORTED;
        return STATUS_SUCCESS;
    }
    handle = handle_acquire(params->ftHandle);
    if (!handle)
    {
        params->result = FT_INVALID_HANDLE;
        return STATUS_SUCCESS;
    }
    if (!(event = calloc(1, sizeof(*event))))
    {
        handle_release(handle);
        params->result = FT_INSUFFICIENT_RESOURCES;
        return STATUS_SUCCESS;
    }

    pthread_mutex_init(&event->event.eMutex, NULL);
    pthread_cond_init(&event->event.eCondVar, NULL);
    pthread_mutex_lock(&events_mutex);
    event->id = next_event_id++;
    if (!next_event_id) next_event_id = 1;
    pthread_mutex_unlock(&events_mutex);

    params->result = pFT_SetEventNotification(handle->native, params->dwEventMask, &event->event);
    if (params->result == FT_OK)
    {
        pthread_mutex_lock(&events_mutex);
        event->next = events;
        events = event;
        pthread_mutex_unlock(&events_mutex);
        params->registration = event->id;
    }
    else
    {
        pthread_cond_destroy(&event->event.eCondVar);
        pthread_mutex_destroy(&event->event.eMutex);
        free(event);
    }
    handle_release(handle);
    return STATUS_SUCCESS;
}

static void custom_detach(void)
{
    struct native_event *event;
    struct ft_handle_entry *handle;

    while ((event = events))
    {
        events = event->next;
        pthread_cond_destroy(&event->event.eCondVar);
        pthread_mutex_destroy(&event->event.eMutex);
        free(event);
    }
    while ((handle = handles))
    {
        handles = handle->next;
        handle->closing = TRUE;
        if (pFT_Close) pFT_Close(handle->native);
        handle_release(handle);
    }
}
