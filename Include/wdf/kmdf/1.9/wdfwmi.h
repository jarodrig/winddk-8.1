/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

_WdfVersionBuild_

Module Name:

    WdfWMI.h

Abstract:

    This is the C interface for WMI support

Environment:

    kernel mode only

Revision History:

--*/

#ifndef _WDFWMI_1_9_H_
#define _WDFWMI_1_9_H_



#if (NTDDI_VERSION >= NTDDI_WIN2K)

typedef enum _WDF_WMI_PROVIDER_CONTROL {
    WdfWmiControlInvalid = 0,
    WdfWmiEventControl,
    WdfWmiInstanceControl,
} WDF_WMI_PROVIDER_CONTROL;

// 
// WdfWmiProviderExpensive:
// The provider's operations are expensive in terms of resources
// 
// WdfWmiProviderReadOnly:
// The provider is read only. No set or set data item callbacks will be
// made on all instances of this provider.
// 
// WdfWmiProviderEventOnly:
// Data item is being used to fire events only.  It will not receive any
// callbacks on it to get / set / execute buffers.
// 
typedef enum _WDF_WMI_PROVIDER_FLAGS {
    WdfWmiProviderEventOnly = 0x0001,
    WdfWmiProviderExpensive = 0x0002,
    WdfWmiProviderTracing =   0x0004,
    WdfWmiProviderValidFlags = WdfWmiProviderEventOnly | WdfWmiProviderExpensive | WdfWmiProviderTracing,
} WDF_WMI_PROVIDER_FLAGS;



typedef
_Function_class_(EVT_WDF_WMI_INSTANCE_QUERY_INSTANCE)
_IRQL_requires_same_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
EVT_WDF_WMI_INSTANCE_QUERY_INSTANCE(
    _In_
    WDFWMIINSTANCE WmiInstance,
    _In_
    ULONG OutBufferSize,
    _Out_writes_bytes_to_(OutBufferSize, *BufferUsed)
    PVOID OutBuffer,
    _Out_
    PULONG BufferUsed
    );

typedef EVT_WDF_WMI_INSTANCE_QUERY_INSTANCE *PFN_WDF_WMI_INSTANCE_QUERY_INSTANCE;

typedef
_Function_class_(EVT_WDF_WMI_INSTANCE_SET_INSTANCE)
_IRQL_requires_same_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
EVT_WDF_WMI_INSTANCE_SET_INSTANCE(
    _In_
    WDFWMIINSTANCE WmiInstance,
    _In_
    ULONG InBufferSize,
    _In_reads_bytes_(InBufferSize)
    PVOID InBuffer
    );

typedef EVT_WDF_WMI_INSTANCE_SET_INSTANCE *PFN_WDF_WMI_INSTANCE_SET_INSTANCE;

typedef
_Function_class_(EVT_WDF_WMI_INSTANCE_SET_ITEM)
_IRQL_requires_same_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
EVT_WDF_WMI_INSTANCE_SET_ITEM(
    _In_
    WDFWMIINSTANCE WmiInstance,
    _In_
    ULONG DataItemId,
    _In_
    ULONG InBufferSize,
    _In_reads_bytes_(InBufferSize)
    PVOID InBuffer
    );

typedef EVT_WDF_WMI_INSTANCE_SET_ITEM *PFN_WDF_WMI_INSTANCE_SET_ITEM;

typedef
_Function_class_(EVT_WDF_WMI_INSTANCE_EXECUTE_METHOD)
_IRQL_requires_same_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
EVT_WDF_WMI_INSTANCE_EXECUTE_METHOD(
    _In_
    WDFWMIINSTANCE WmiInstance,
    _In_
    ULONG MethodId,
    _In_
    ULONG InBufferSize,
    _In_
    ULONG OutBufferSize,
    _When_(InBufferSize >= OutBufferSize, _Inout_updates_bytes_(InBufferSize))
    _When_(InBufferSize < OutBufferSize, _Inout_updates_bytes_(OutBufferSize))
    PVOID Buffer,
    _Out_
    PULONG BufferUsed
    );

typedef EVT_WDF_WMI_INSTANCE_EXECUTE_METHOD *PFN_WDF_WMI_INSTANCE_EXECUTE_METHOD;

typedef
_Function_class_(EVT_WDF_WMI_PROVIDER_FUNCTION_CONTROL)
_IRQL_requires_same_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
EVT_WDF_WMI_PROVIDER_FUNCTION_CONTROL(
    _In_
    WDFWMIPROVIDER WmiProvider,
    _In_
    WDF_WMI_PROVIDER_CONTROL Control,
    _In_
    BOOLEAN Enable
    );

typedef EVT_WDF_WMI_PROVIDER_FUNCTION_CONTROL *PFN_WDF_WMI_PROVIDER_FUNCTION_CONTROL;

typedef struct _WDF_WMI_PROVIDER_CONFIG {
    //
    // Size of this structure in bytes
    //
    ULONG Size;

    //
    // The GUID being registered
    //
    GUID Guid;

    //
    // Combination of values from the enum WDF_WMI_PROVIDER_FLAGS
    //
    ULONG Flags;

    //
    // Minimum expected buffer size for query and set instance requests.
    // Ignored if WdfWmiProviderEventOnly is set in Flags.
    //
    ULONG MinInstanceBufferSize;

    //
    // Callback when caller is opening a provider which ha been marked as
    // expensive or when a caller is interested in events.
    //
    PFN_WDF_WMI_PROVIDER_FUNCTION_CONTROL EvtWmiProviderFunctionControl;

} WDF_WMI_PROVIDER_CONFIG, *PWDF_WMI_PROVIDER_CONFIG;

VOID
FORCEINLINE
WDF_WMI_PROVIDER_CONFIG_INIT(
    _Out_ PWDF_WMI_PROVIDER_CONFIG Config,
    _In_ CONST GUID* Guid
    )
{
    RtlZeroMemory(Config, sizeof(WDF_WMI_PROVIDER_CONFIG));

    Config->Size = sizeof(WDF_WMI_PROVIDER_CONFIG);
    RtlCopyMemory(&Config->Guid, Guid, sizeof(GUID));
}

typedef struct _WDF_WMI_INSTANCE_CONFIG {
    //
    // Size of the structure in bytes
    //
    ULONG Size;

    //
    // Optional parameter.  If NULL, ProviderConfig must be set to a valid pointer
    // value.   If specified, indicates the provider to create an instance for.
    //
    WDFWMIPROVIDER Provider;

    //
    // Optional parameter.  If NULL, Provider must be set to a valid handle
    // value.  If specifeid, indicates the configuration for a provider to be
    // created and for this instance to be associated with.
    //
    PWDF_WMI_PROVIDER_CONFIG ProviderConfig;

    //
    // If the Provider is configured as read only and this field is set to TRUE,
    // the EvtWmiInstanceQueryInstance is ignored and WDF will blindly copy the
    // context associated with this instance (using RtlCopyMemory, with no locks
    // held) into the query buffer.
    //
    BOOLEAN UseContextForQuery;

    //
    // If TRUE, the instance will be registered as well as created.
    //
    BOOLEAN Register;

    //
    // Callback when caller wants to query the entire data item's buffer.
    //
    PFN_WDF_WMI_INSTANCE_QUERY_INSTANCE EvtWmiInstanceQueryInstance;

    //
    // Callback when caller wants to set the entire data item's buffer.
    //
    PFN_WDF_WMI_INSTANCE_SET_INSTANCE EvtWmiInstanceSetInstance;

    //
    // Callback when caller wants to set a single field in the data item's buffer
    //
    PFN_WDF_WMI_INSTANCE_SET_ITEM EvtWmiInstanceSetItem;

    //
    // Callback when caller wants to execute a method on the data item.
    //
    PFN_WDF_WMI_INSTANCE_EXECUTE_METHOD EvtWmiInstanceExecuteMethod;

} WDF_WMI_INSTANCE_CONFIG, *PWDF_WMI_INSTANCE_CONFIG;

VOID
FORCEINLINE
WDF_WMI_INSTANCE_CONFIG_INIT_PROVIDER(
    _Out_ PWDF_WMI_INSTANCE_CONFIG Config,
    _In_ WDFWMIPROVIDER Provider
    )
{
    RtlZeroMemory(Config, sizeof(WDF_WMI_INSTANCE_CONFIG));
    Config->Size = sizeof(WDF_WMI_INSTANCE_CONFIG);

    Config->Provider = Provider;
}

VOID
FORCEINLINE
WDF_WMI_INSTANCE_CONFIG_INIT_PROVIDER_CONFIG(
    _Out_ PWDF_WMI_INSTANCE_CONFIG Config,
    _In_ PWDF_WMI_PROVIDER_CONFIG ProviderConfig
    )
{
    RtlZeroMemory(Config, sizeof(WDF_WMI_INSTANCE_CONFIG));
    Config->Size = sizeof(WDF_WMI_INSTANCE_CONFIG);

    Config->ProviderConfig = ProviderConfig;
}

NTSTATUS
FORCEINLINE
WDF_WMI_BUFFER_APPEND_STRING(
    _Out_writes_bytes_(BufferLength) PVOID Buffer,
    _In_ ULONG BufferLength,
    _In_ PCUNICODE_STRING String,
    _Out_ PULONG RequiredSize
    )
{
    //
    // Compute the length of buffer we need to use.  Upon error the caller can
    // use this length to report the required length.  On success, the caller
    // can use this length to know how many bytes were written.
    //
    *RequiredSize = String->Length + sizeof(USHORT);

    //
    // UNICODE_STRING.Length is the length of the string in bytes, not characters
    //

    // First check to see if there is enough space
    // 1)  to store the length of the string
    // 2)  to store the string itself
    //
    if (BufferLength < (String->Length + sizeof(USHORT))) {
        //
        // Not enough room in the string, report back how big a buffer is
        // required.
        //
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // Store the length of the string
    //
    *(USHORT *) Buffer = String->Length;

    //
    // Copy the string to the buffer
    //
    RtlCopyMemory(WDF_PTR_ADD_OFFSET(Buffer, sizeof(USHORT)),
                  String->Buffer,
                  String->Length);

    return STATUS_SUCCESS;
}

//
// WDF Function: WdfWmiProviderCreate
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFWMIPROVIDERCREATE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PWDF_WMI_PROVIDER_CONFIG WmiProviderConfig,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES ProviderAttributes,
    _Out_
    WDFWMIPROVIDER* WmiProvider
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfWmiProviderCreate(
    _In_
    WDFDEVICE Device,
    _In_
    PWDF_WMI_PROVIDER_CONFIG WmiProviderConfig,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES ProviderAttributes,
    _Out_
    WDFWMIPROVIDER* WmiProvider
    )
{
    return ((PFN_WDFWMIPROVIDERCREATE) WdfFunctions[WdfWmiProviderCreateTableIndex])(WdfDriverGlobals, Device, WmiProviderConfig, ProviderAttributes, WmiProvider);
}

//
// WDF Function: WdfWmiProviderGetDevice
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDFDEVICE
(*PFN_WDFWMIPROVIDERGETDEVICE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFWMIPROVIDER WmiProvider
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFDEVICE
FORCEINLINE
WdfWmiProviderGetDevice(
    _In_
    WDFWMIPROVIDER WmiProvider
    )
{
    return ((PFN_WDFWMIPROVIDERGETDEVICE) WdfFunctions[WdfWmiProviderGetDeviceTableIndex])(WdfDriverGlobals, WmiProvider);
}

//
// WDF Function: WdfWmiProviderIsEnabled
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
BOOLEAN
(*PFN_WDFWMIPROVIDERISENABLED)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFWMIPROVIDER WmiProvider,
    _In_
    WDF_WMI_PROVIDER_CONTROL ProviderControl
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
BOOLEAN
FORCEINLINE
WdfWmiProviderIsEnabled(
    _In_
    WDFWMIPROVIDER WmiProvider,
    _In_
    WDF_WMI_PROVIDER_CONTROL ProviderControl
    )
{
    return ((PFN_WDFWMIPROVIDERISENABLED) WdfFunctions[WdfWmiProviderIsEnabledTableIndex])(WdfDriverGlobals, WmiProvider, ProviderControl);
}

//
// WDF Function: WdfWmiProviderGetTracingHandle
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
ULONGLONG
(*PFN_WDFWMIPROVIDERGETTRACINGHANDLE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFWMIPROVIDER WmiProvider
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
ULONGLONG
FORCEINLINE
WdfWmiProviderGetTracingHandle(
    _In_
    WDFWMIPROVIDER WmiProvider
    )
{
    return ((PFN_WDFWMIPROVIDERGETTRACINGHANDLE) WdfFunctions[WdfWmiProviderGetTracingHandleTableIndex])(WdfDriverGlobals, WmiProvider);
}

//
// WDF Function: WdfWmiInstanceCreate
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFWMIINSTANCECREATE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFDEVICE Device,
    _In_
    PWDF_WMI_INSTANCE_CONFIG InstanceConfig,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES InstanceAttributes,
    _Out_opt_
    WDFWMIINSTANCE* Instance
    );

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
NTSTATUS
FORCEINLINE
WdfWmiInstanceCreate(
    _In_
    WDFDEVICE Device,
    _In_
    PWDF_WMI_INSTANCE_CONFIG InstanceConfig,
    _In_opt_
    PWDF_OBJECT_ATTRIBUTES InstanceAttributes,
    _Out_opt_
    WDFWMIINSTANCE* Instance
    )
{
    return ((PFN_WDFWMIINSTANCECREATE) WdfFunctions[WdfWmiInstanceCreateTableIndex])(WdfDriverGlobals, Device, InstanceConfig, InstanceAttributes, Instance);
}

//
// WDF Function: WdfWmiInstanceRegister
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFWMIINSTANCEREGISTER)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFWMIINSTANCE WmiInstance
    );

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
FORCEINLINE
WdfWmiInstanceRegister(
    _In_
    WDFWMIINSTANCE WmiInstance
    )
{
    return ((PFN_WDFWMIINSTANCEREGISTER) WdfFunctions[WdfWmiInstanceRegisterTableIndex])(WdfDriverGlobals, WmiInstance);
}

//
// WDF Function: WdfWmiInstanceDeregister
//
typedef
_IRQL_requires_max_(PASSIVE_LEVEL)
WDFAPI
VOID
(*PFN_WDFWMIINSTANCEDEREGISTER)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFWMIINSTANCE WmiInstance
    );

_IRQL_requires_max_(PASSIVE_LEVEL)
VOID
FORCEINLINE
WdfWmiInstanceDeregister(
    _In_
    WDFWMIINSTANCE WmiInstance
    )
{
    ((PFN_WDFWMIINSTANCEDEREGISTER) WdfFunctions[WdfWmiInstanceDeregisterTableIndex])(WdfDriverGlobals, WmiInstance);
}

//
// WDF Function: WdfWmiInstanceGetDevice
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDFDEVICE
(*PFN_WDFWMIINSTANCEGETDEVICE)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFWMIINSTANCE WmiInstance
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFDEVICE
FORCEINLINE
WdfWmiInstanceGetDevice(
    _In_
    WDFWMIINSTANCE WmiInstance
    )
{
    return ((PFN_WDFWMIINSTANCEGETDEVICE) WdfFunctions[WdfWmiInstanceGetDeviceTableIndex])(WdfDriverGlobals, WmiInstance);
}

//
// WDF Function: WdfWmiInstanceGetProvider
//
typedef
_IRQL_requires_max_(DISPATCH_LEVEL)
WDFAPI
WDFWMIPROVIDER
(*PFN_WDFWMIINSTANCEGETPROVIDER)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFWMIINSTANCE WmiInstance
    );

_IRQL_requires_max_(DISPATCH_LEVEL)
WDFWMIPROVIDER
FORCEINLINE
WdfWmiInstanceGetProvider(
    _In_
    WDFWMIINSTANCE WmiInstance
    )
{
    return ((PFN_WDFWMIINSTANCEGETPROVIDER) WdfFunctions[WdfWmiInstanceGetProviderTableIndex])(WdfDriverGlobals, WmiInstance);
}

//
// WDF Function: WdfWmiInstanceFireEvent
//
typedef
_Must_inspect_result_
_IRQL_requires_max_(APC_LEVEL)
WDFAPI
NTSTATUS
(*PFN_WDFWMIINSTANCEFIREEVENT)(
    _In_
    PWDF_DRIVER_GLOBALS DriverGlobals,
    _In_
    WDFWMIINSTANCE WmiInstance,
    _In_opt_
    ULONG EventDataSize,
    _In_reads_bytes_opt_(EventDataSize)
    PVOID EventData
    );

_Must_inspect_result_
_IRQL_requires_max_(APC_LEVEL)
NTSTATUS
FORCEINLINE
WdfWmiInstanceFireEvent(
    _In_
    WDFWMIINSTANCE WmiInstance,
    _In_opt_
    ULONG EventDataSize,
    _In_reads_bytes_opt_(EventDataSize)
    PVOID EventData
    )
{
    return ((PFN_WDFWMIINSTANCEFIREEVENT) WdfFunctions[WdfWmiInstanceFireEventTableIndex])(WdfDriverGlobals, WmiInstance, EventDataSize, EventData);
}



#endif // (NTDDI_VERSION >= NTDDI_WIN2K)


#endif // _WDFWMI_1_9_H_
