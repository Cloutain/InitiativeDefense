#include "memmon.h"
// Download by http://www.51xue8xue8.com
#define NT_DEVICE_NAME      L"\\Device\\MemMon"
#define DOS_DEVICE_NAME     L"\\DosDevices\\MemMon"


HANDLE             MemAppHandle;        // �¼�������
PKEVENT            MemAppEvent;          // �û����ں�ͨ�ŵ��¼�����ָ��
CHAR LastFile[256]="LoveSuae";


//-------------------MEMmon����-------------------
#define IOCTL_GET_MEMINFO CTL_CODE(FILE_DEVICE_UNKNOWN,0x0830, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_SET_APPEVENT_OK CTL_CODE(FILE_DEVICE_UNKNOWN,0x0831, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_SET_APPEVENT_NO CTL_CODE(FILE_DEVICE_UNKNOWN,0x0832, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_TURNON_MEMMON CTL_CODE(FILE_DEVICE_UNKNOWN,0x0833, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_TURNOFF_MEMMON CTL_CODE(FILE_DEVICE_UNKNOWN,0x0834, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
int TurnOnMemMon=1;//Ĭ�Ͽ���
int MemPass=1;//Ĭ��ͨ��
//-------------------MEMmon���ƽ���-------------------


PDEVICE_OBJECT  g_pDriverObject;//����ȫ������
NTSTATUS DispatchCreate(IN PDEVICE_OBJECT DeviceObject,IN PIRP	 Irp);
NTSTATUS DispatchClose(IN PDEVICE_OBJECT DeviceObject,IN PIRP	 Irp);
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject,IN PUNICODE_STRING theRegistryPath)
{
	//������ں���
	NTSTATUS        ntStatus = STATUS_SUCCESS;
	UNICODE_STRING  ntDeviceName,linkString;
	UNICODE_STRING  DeviceLinkString;
	UNICODE_STRING	MemEventString;
	UNICODE_STRING	MemAppEventString;
	PDEVICE_OBJECT  deviceObject = NULL;
    PDEVICE_EXTENSION	deviceExtension={0};
	RtlInitUnicodeString( &ntDeviceName, NT_DEVICE_NAME );
	ntStatus = IoCreateDevice(
		DriverObject,
		sizeof(DEVICE_EXTENSION), // Ϊ�豸��չ�ṹ����ռ� 
		&ntDeviceName,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&deviceObject );

	if ( !NT_SUCCESS( ntStatus ) )
	{
		KdPrint(("[*]�޷����������豸"));
		return ntStatus;
	}

    RtlInitUnicodeString(&linkString,L"\\DosDevices\\MemMon");

    ntStatus = IoCreateSymbolicLink (&linkString, &ntDeviceName);

	if(!NT_SUCCESS(ntStatus))
	{
		return ntStatus;
	}

     deviceExtension=(PDEVICE_EXTENSION)deviceObject->DeviceExtension;   
	g_pDriverObject=deviceObject;	 
	 
	//��ʼ���¼�
	RtlInitUnicodeString(&MemEventString,EVENT_NAME);  //������
	RtlInitUnicodeString(&MemAppEventString,APP_EVENT_NAME);  //Ӧ�ò�
	deviceExtension->MemEvent=IoCreateNotificationEvent(&MemEventString,&deviceExtension->MemHandle);	
	MemAppEvent=IoCreateNotificationEvent(&MemAppEventString,&MemAppHandle);
	//����Ϊ������״̬
	KeClearEvent(deviceExtension->MemEvent);	
	KeClearEvent(MemAppEvent);	
	
	
	
	DriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDeviceControl;
	DriverObject->DriverUnload = OnUnload;

	m_MDL = MmCreateMdl(NULL,KeServiceDescriptorTable.ServiceTableBase,KeServiceDescriptorTable.NumberOfServices*4);
	if(!m_MDL)
		return STATUS_UNSUCCESSFUL;

	//�Ƿ�ҳ�ڴ�
	MmBuildMdlForNonPagedPool(m_MDL);

	m_MDL->MdlFlags = m_MDL->MdlFlags | MDL_MAPPED_TO_SYSTEM_VA;

	//����
	m_Mapped = MmMapLockedPages(m_MDL, KernelMode);



	
	//�����ؼ�HOOK
	HOOK_SYSCALL(ZwCreateSection,HookZwCreateSection,RealZwCreateSection);
	
	KdPrint(("�ڴ������������������.\n"));

	return STATUS_SUCCESS;
}







VOID OnUnload(IN PDRIVER_OBJECT DriverObject)
{
	//ж��ʱ�����
	UNICODE_STRING DeviceLinkString;
	PDEVICE_OBJECT DeviceObjectTemp1=NULL;
	PDEVICE_OBJECT DeviceObjectTemp2=NULL;
	PVOID Oldfun = NULL;

	RtlInitUnicodeString(&DeviceLinkString,DOS_DEVICE_NAME);
	IoDeleteSymbolicLink(&DeviceLinkString);
	if(DriverObject)
	{
		DeviceObjectTemp1=DriverObject->DeviceObject;
		while(DeviceObjectTemp1)
		{
			DeviceObjectTemp2=DeviceObjectTemp1;
			DeviceObjectTemp1=DeviceObjectTemp1->NextDevice;
			IoDeleteDevice(DeviceObjectTemp2);
		}
	}  

	
	//��ԭSSDT
	HOOK_SYSCALL(ZwCreateSection,RealZwCreateSection,Oldfun);
	
	
	if(m_MDL){
		MmUnmapLockedPages(m_Mapped,m_MDL);
		IoFreeMdl(m_MDL);
	}

	KdPrint(("�ڴ��������ж�����.\n"));

}


NTSTATUS DispatchCreate(IN PDEVICE_OBJECT DeviceObject,IN PIRP	 Irp)
{
  DbgPrint("IRP_MJ_Create\n");

  Irp->IoStatus.Information=0;
  Irp->IoStatus.Status=STATUS_SUCCESS;
  IoCompleteRequest(Irp,IO_NO_INCREMENT);
  return STATUS_SUCCESS;
}


//�����豸�������
NTSTATUS DispatchClose(IN PDEVICE_OBJECT DeviceObject,IN PIRP	 Irp)
{
  DbgPrint("IRP_MJ_Close\n");
  
  Irp->IoStatus.Information=0;
  Irp->IoStatus.Status=STATUS_SUCCESS;
  IoCompleteRequest(Irp,IO_NO_INCREMENT);
  return STATUS_SUCCESS;
}




//IO CTL/IOͨ��֮��
NTSTATUS DispatchDeviceControl(IN PDEVICE_OBJECT  DeviceObject,IN PIRP  Irp)
{

	PDEVICE_EXTENSION  deviceExtension;
	ULONG IoControlCode,InBufLength,outBufLength; 
	PCALLBACK_INFO pCallbackInfo;
	UNICODE_STRING uEventName;  
	PKEVENT pEvent;  
	HANDLE hEvent,hThread;  
	NTSTATUS status=STATUS_INVALID_DEVICE_REQUEST;
	PIO_STACK_LOCATION  irpStack = IoGetCurrentIrpStackLocation(Irp);
	deviceExtension=(PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
	IoControlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;
	pCallbackInfo=(PCALLBACK_INFO)Irp->AssociatedIrp.SystemBuffer;
	InBufLength  =irpStack->Parameters.DeviceIoControl.InputBufferLength;
	outBufLength =irpStack->Parameters.DeviceIoControl.OutputBufferLength;

	switch(IoControlCode)
	{
	

	case IOCTL_GET_MEMINFO:
	   {
			KdPrint((" ��ȡ�ڴ������Ϣ"));
		   if(outBufLength>=sizeof(CALLBACK_INFO))
		   {
				strcpy(pCallbackInfo->ProcName , deviceExtension->ProcName);
				strcpy(pCallbackInfo->ProcFullPath  , deviceExtension->ProcFullPath);
				strcpy(pCallbackInfo->Info1  , deviceExtension->Info1);
				strcpy(pCallbackInfo->Info2  , deviceExtension->Info2);
				
				status=STATUS_SUCCESS;
			}
	   }
	   	break;
	case IOCTL_SET_APPEVENT_OK: 
		{
			KdPrint((" ����!Ӧ��֪ͨ�������¼�"));
			MemPass=1;//ȫ��ͨ��,�¼����ñ����ں���
			KeSetEvent(MemAppEvent,IO_NO_INCREMENT,FALSE);
			status=STATUS_SUCCESS;
		}
		break;
	case IOCTL_SET_APPEVENT_NO: 
		{
			KdPrint((" ��ֹ!Ӧ��֪ͨ�������¼�"));
			MemPass=0;//ȫ�ӽ�ֹ			
			KeSetEvent(MemAppEvent,IO_NO_INCREMENT,FALSE);
			status=STATUS_SUCCESS;
		}
		break;
		case IOCTL_TURNON_MEMMON:
		{
			KdPrint((" �����ڴ���ؼ��"));
			TurnOnMemMon=1;	//����
			//KeSetEvent(MemAppEvent,IO_NO_INCREMENT,FALSE);
			status=STATUS_SUCCESS;
		}
		break;
	case IOCTL_TURNOFF_MEMMON:
		{
			KdPrint((" �ر��ڴ���ؼ��"));
			TurnOnMemMon=0;	//�ر�,֪ͨ�¼����ڹر�
			//KeSetEvent(MemAppEvent,IO_NO_INCREMENT,FALSE);
			status=STATUS_SUCCESS;
		}
	 break;
	}

	if(status==STATUS_SUCCESS)
		Irp->IoStatus.Information=outBufLength; 
	else
		Irp->IoStatus.Information=0;

	Irp->IoStatus.Status=status;
	IoCompleteRequest(Irp,IO_NO_INCREMENT);
	return status;

}





NTSTATUS HookZwCreateSection(
OUT PHANDLE             SectionHandle,
IN ULONG                DesiredAccess,
IN POBJECT_ATTRIBUTES   ObjectAttributes OPTIONAL,
IN PLARGE_INTEGER       MaximumSize OPTIONAL,
IN ULONG                PageAttributess,
IN ULONG                SectionAttributes,
IN HANDLE               FileHandle OPTIONAL )
{ 
    HANDLE hHandle;
    PFILE_OBJECT pFileObject;
	PDEVICE_EXTENSION deviceExtension;
    OBJECT_HANDLE_INFORMATION HandleInformationObject;
    ANSI_STRING asFileName;
    char aPathName[MAXPATHLEN];
    char aProcessName[PROCNAMELEN];
   UNICODE_STRING   dosName, fullUniName; 
    char* findpointer=NULL;

    dosName.Buffer=(PWSTR)ExAllocatePool(PagedPool, 16);
    dosName.MaximumLength=8;
    
    fullUniName.Buffer = (PWSTR)ExAllocatePool(PagedPool, MAXPATHLEN*2);//1024*2   
    fullUniName.MaximumLength   =   MAXPATHLEN*2; 

	deviceExtension=(PDEVICE_EXTENSION)g_pDriverObject->DeviceExtension;
    hHandle=(HANDLE)FileHandle;
    ObReferenceObjectByHandle(hHandle,0,0,KernelMode,&pFileObject,&HandleInformationObject);
    if(pFileObject != NULL)
    {
		   RtlVolumeDeviceToDosName(pFileObject->DeviceObject, &dosName); 
		// DbgPrint("   %ws\n",dosName.Buffer);
		   //����̷�
		   RtlCopyUnicodeString(&fullUniName, &dosName);                      
		  
		   //���̷������ַ���һ�𣬵õ���·��
		   RtlAppendUnicodeStringToString(&fullUniName,&((PFILE_OBJECT)pFileObject)->FileName);
			RtlUnicodeStringToAnsiString(&asFileName,&(fullUniName),TRUE); 
		if  (_stricmp(LastFile,asFileName.Buffer)!=0)//��ֹ�ظ�
		{
				strcpy(LastFile,asFileName.Buffer);
			   //DbgPrint(" %s %ws\n",asFileName,dosName.Buffer);
				if((findpointer=strstr(asFileName.Buffer, "."))==NULL)
				 return ((ZWCREATESECTION)(RealZwCreateSection))(SectionHandle, DesiredAccess, ObjectAttributes, MaximumSize, PageAttributess, SectionAttributes, FileHandle);
			if (

				(_stricmp(findpointer, ".exe") == 0) || (_stricmp(findpointer, ".dll") == 0) || (_stricmp(findpointer, ".sys") == 0)

				)   // ��
			   {
					//д���豸��չ
			   		strcpy(deviceExtension->ProcFullPath,(PCHAR )asFileName.Buffer);				
					//���￪ʼ֪ͨӦ�ò�
					KeSetEvent(deviceExtension->MemEvent,0,FALSE);
					KeClearEvent(deviceExtension->MemEvent);

					DbgPrint("   %s\n",asFileName.Buffer);

				}
				
		}
	}
  
 
return ((ZWCREATESECTION)(RealZwCreateSection))(SectionHandle, 
                 DesiredAccess, 
                ObjectAttributes, 
                MaximumSize, 
                PageAttributess, 
                SectionAttributes, 
                FileHandle);

}

