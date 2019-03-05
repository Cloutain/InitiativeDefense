#include "ntddk.h" 
#include <stdio.h> 
#include <string.h>
#include <windef.h>
#include <wchar.h>


#define EVENT_NAME L"\\BaseNamedObjects\\MemEvent"  //������֪ͨ�¼�
#define APP_EVENT_NAME L"\\BaseNamedObjects\\MenAppEvent" //Ӧ�ò��Ƿ��¼�

// Length of process name (rounded up to next DWORD)
#define PROCNAMELEN     20
// Maximum length of NT process name
#define NT_PROCNAMELEN 16
#define MAXPATHLEN 256
//��������
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject,IN PUNICODE_STRING theRegistryPath);
NTSTATUS DispatchDeviceControl(IN PDEVICE_OBJECT  DeviceObject,IN PIRP  Irp);
VOID OnUnload(IN PDRIVER_OBJECT DriverObject);



#pragma pack(1)	//SSDT��Ľṹ
typedef struct ServiceDescriptorEntry {
	unsigned int *ServiceTableBase;
	unsigned int *ServiceCounterTableBase; //Used only in checked build
	unsigned int NumberOfServices;
	unsigned char *ParamTableBase;
} ServiceDescriptorTableEntry_t, *PServiceDescriptorTableEntry_t;
#pragma pack()

PMDL m_MDL;
PVOID *m_Mapped;
__declspec(dllimport) ServiceDescriptorTableEntry_t KeServiceDescriptorTable;	//�������ǲ��ܱ��,��Ϊ�Ǵ��ⲿ����
#define SYSTEMSERVICE(_function)  KeServiceDescriptorTable.ServiceTableBase[ *(PULONG)((PUCHAR)_function+1)]
#define SYSCALL_INDEX(_Function) *(PULONG)((PUCHAR)_Function+1)
#define HOOK_SYSCALL(_Function, _Hook, _Orig )  \
       _Orig = (PVOID) InterlockedExchange( (PLONG) &m_Mapped[SYSCALL_INDEX(_Function)], (LONG) _Hook)

NTSYSAPI NTSTATUS NTAPI ZwOpenProcess(OUT PHANDLE ProcessHandle,IN ACCESS_MASK DesiredAccess,IN POBJECT_ATTRIBUTES ObjectAttributes,IN PCLIENT_ID ClientId OPTIONAL);
NTSYSAPI NTSTATUS NTAPI ZwTerminateProcess(IN HANDLE ProcessHandle OPTIONAL,IN NTSTATUS ExitStatus);
typedef NTSTATUS (*ZWOPENPROCESS)(OUT PHANDLE ProcessHandle,IN ACCESS_MASK DesiredAccess,IN POBJECT_ATTRIBUTES ObjectAttributes,IN PCLIENT_ID ClientId OPTIONAL);
typedef NTSTATUS (*ZWTERMINATEPROCESS)(IN HANDLE ProcessHandle OPTIONAL,IN NTSTATUS ExitStatus);
NTSTATUS NewZwOpenProcess(OUT PHANDLE ProcessHandle,IN ACCESS_MASK DesiredAccess,IN POBJECT_ATTRIBUTES ObjectAttributes,IN PCLIENT_ID ClientId OPTIONAL);
NTSTATUS NewZwTerminateProcess(IN HANDLE ProcessHandle OPTIONAL,IN NTSTATUS ExitStatus);

NTSTATUS PsLookupProcessByProcessId(IN ULONG ulProcId, OUT PEPROCESS *pEProcess);



typedef struct _CallbackInfo
{
	CHAR    ProcName[256];	//������
	CHAR    ProcFullPath[256];          //����·��
	CHAR    Info1[256];					//��Ϣ1
	CHAR    Info2[256];					//��Ϣ2
 } CALLBACK_INFO, *PCALLBACK_INFO;


typedef struct _DEVICE_EXTENSION
{
    HANDLE             MemHandle;        // �¼�������
    PKEVENT            MemEvent;          // �û����ں�ͨ�ŵ��¼�����ָ��
	CHAR	*			ProcName[256];					//������
    CHAR	*			ProcFullPath[256];         //����·��
	CHAR   *			Info1[256];					//��Ϣ1
	CHAR	*			Info2[256];					//��Ϣ2
}DEVICE_EXTENSION, *PDEVICE_EXTENSION;



//�¼�
NTSYSAPI NTSTATUS NTAPI
  ZwCreateSection(
    OUT PHANDLE  SectionHandle,
    IN ACCESS_MASK  DesiredAccess,
    IN POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
    IN PLARGE_INTEGER  MaximumSize OPTIONAL,
    IN ULONG  SectionPageProtection,
    IN ULONG  AllocationAttributes,
    IN HANDLE  FileHandle OPTIONAL
    ); 	

NTSTATUS 
  HookZwCreateSection(
    OUT PHANDLE  SectionHandle,
    IN ACCESS_MASK  DesiredAccess,
    IN POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
    IN PLARGE_INTEGER  MaximumSize OPTIONAL,
    IN ULONG  SectionPageProtection,
    IN ULONG  AllocationAttributes,
    IN HANDLE  FileHandle OPTIONAL
    ); 	

 
typedef  (*ZWCREATESECTION)(
    OUT PHANDLE  SectionHandle,
    IN ACCESS_MASK  DesiredAccess,
    IN POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
    IN PLARGE_INTEGER  MaximumSize OPTIONAL,
    IN ULONG  SectionPageProtection,
    IN ULONG  AllocationAttributes,
    IN HANDLE  FileHandle OPTIONAL
    ); 		
	
ZWCREATESECTION RealZwCreateSection=NULL;	
ZWCREATESECTION  OldZwCreateSetion=NULL;