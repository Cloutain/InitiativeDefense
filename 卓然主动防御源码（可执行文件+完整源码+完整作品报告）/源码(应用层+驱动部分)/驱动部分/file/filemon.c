#include "filemon.h"
// Download by http://www.51xue8xue8.com

//�����ļ��䶯
PCWSTR  PreCreateFileName=L"love";
PCWSTR  PreOpenFileName=L"love";
PCWSTR  PreWriteFileName=L"love";



//-------------------Filemon����-------------------
#define IOCTL_GET_FILEINFO CTL_CODE(FILE_DEVICE_UNKNOWN,0x0840, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_SET_APPEVENT_OK CTL_CODE(FILE_DEVICE_UNKNOWN,0x0841, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_SET_APPEVENT_NO CTL_CODE(FILE_DEVICE_UNKNOWN,0x0842, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_TURNON_FILEMON CTL_CODE(FILE_DEVICE_UNKNOWN,0x0843, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_TURNOFF_FILEMON CTL_CODE(FILE_DEVICE_UNKNOWN,0x0844, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
int TurnOnFileMon=1;//Ĭ�Ͽ���
int FilePass=1;//Ĭ��ͨ��
//-------------------Filemon���ƽ���-------------------





#define NT_DEVICE_NAME      L"\\Device\\FileMon"
#define DOS_DEVICE_NAME     L"\\DosDevices\\FileMon"

HANDLE             FileAppHandle;        // �¼�������
PKEVENT            FileAppEvent;          // �û����ں�ͨ�ŵ��¼�����ָ��


PDEVICE_OBJECT  g_pDriverObject;//����ȫ������
NTSTATUS DispatchCreate(IN PDEVICE_OBJECT DeviceObject,IN PIRP	 Irp);
NTSTATUS DispatchClose(IN PDEVICE_OBJECT DeviceObject,IN PIRP	 Irp);
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject,IN PUNICODE_STRING theRegistryPath)
{
	//������ں���
	NTSTATUS        ntStatus = STATUS_SUCCESS;
	UNICODE_STRING  ntDeviceName,linkString;
	UNICODE_STRING  DeviceLinkString;
	UNICODE_STRING	FileEventString;
	UNICODE_STRING	FileAppEventString;
	PDEVICE_OBJECT  deviceObject = NULL;
    PDEVICE_EXTENSION	deviceExtension={0};
	RtlInitUnicodeString( &ntDeviceName, NT_DEVICE_NAME );
	GetProcessNameOffset();
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

    RtlInitUnicodeString(&linkString,L"\\DosDevices\\FileMon");

    ntStatus = IoCreateSymbolicLink (&linkString, &ntDeviceName);

	if(!NT_SUCCESS(ntStatus))
	{
		return ntStatus;
	}

     deviceExtension=(PDEVICE_EXTENSION)deviceObject->DeviceExtension;   
	g_pDriverObject=deviceObject;	 
	 
	//��ʼ���¼�
	RtlInitUnicodeString(&FileEventString,EVENT_NAME);  //������
	RtlInitUnicodeString(&FileAppEventString,APP_EVENT_NAME);  //Ӧ�ò�
	deviceExtension->FileEvent=IoCreateNotificationEvent(&FileEventString,&deviceExtension->FileHandle);	
	FileAppEvent=IoCreateNotificationEvent(&FileAppEventString,&FileAppHandle);
	//����Ϊ������״̬
	KeClearEvent(deviceExtension->FileEvent);	
	KeClearEvent(FileAppEvent);	
	
	
	
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
	HOOK_SYSCALL(ZwWriteFile,HookZwWriteFile,RealZwWriteFile);
	
	KdPrint(("�ļ������������������.\n"));

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
	HOOK_SYSCALL(ZwWriteFile,RealZwWriteFile,Oldfun);

	
	
	if(m_MDL){
		MmUnmapLockedPages(m_Mapped,m_MDL);
		IoFreeMdl(m_MDL);
	}

	KdPrint(("�ļ��������ж�����.\n"));

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
	

	case IOCTL_GET_FILEINFO:
	   {
			KdPrint((" Ӧ�ò��ȡ�ļ���Ϣ"));
		   if(outBufLength>=sizeof(CALLBACK_INFO))
		   {
				strcpy(pCallbackInfo->ProcName , deviceExtension->ProcName);
				strcpy(pCallbackInfo->ProcFullPath  , deviceExtension->ProcFullPath);
				strcpy(pCallbackInfo->FilePre  , deviceExtension->FilePre);
				strcpy(pCallbackInfo->FilePath  , deviceExtension->FilePath);
		
				status=STATUS_SUCCESS;
			}
			else 
			{
				KdPrint(("�ռ䲻����"));			
			}
			KeClearEvent(deviceExtension->FileEvent);		
	   }
	   	break;
	case IOCTL_SET_APPEVENT_OK: 
		{
			KdPrint((" ����!Ӧ��֪ͨ�������¼�"));
			FilePass=1;//ȫ��ͨ��,�¼����ñ����ں���
			KeSetEvent(FileAppEvent,IO_NO_INCREMENT,FALSE);
			status=STATUS_SUCCESS;
		}
		break;
	case IOCTL_SET_APPEVENT_NO: 
		{
			KdPrint((" ��ֹ!Ӧ��֪ͨ�������¼�"));
			FilePass=0;//ȫ�ӽ�ֹ			
			KeSetEvent(FileAppEvent,IO_NO_INCREMENT,FALSE);
			status=STATUS_SUCCESS;
		}
		break;
		case IOCTL_TURNON_FILEMON:
		{
			KdPrint((" �����ļ����"));
			TurnOnFileMon=1;	//����
			//KeSetEvent(FileAppEvent,IO_NO_INCREMENT,FALSE);
			status=STATUS_SUCCESS;
		}
		break;
	case IOCTL_TURNOFF_FILEMON:
		{
			KdPrint((" �ر��ļ����"));
			TurnOnFileMon=0;	//�ر�,֪ͨ�¼����ڹر�
			//KeSetEvent(FileAppEvent,IO_NO_INCREMENT,FALSE);
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



	
//HookNtWriteFile
HookZwWriteFile(
IN HANDLE FileHandle, 
IN HANDLE Event OPTIONAL, 
IN PIO_APC_ROUTINE ApcRoutine OPTIONAL, 
IN PVOID ApcContext OPTIONAL, 
OUT PIO_STATUS_BLOCK IoStatusBlock, 
IN PVOID Buffer, 
IN ULONG Length, 
IN PLARGE_INTEGER ByteOffset OPTIONAL, 
IN PULONG Key OPTIONAL 
)
{
	if (TurnOnFileMon==1)//������ܿ����Ļ�
	{
		PDEVICE_EXTENSION	deviceExtension;//�豸��չ
		char aProcessName[PROCNAMELEN];
		UNICODE_STRING volumeDosName;
		WCHAR	DelFullName[1024];
		PCWSTR WriteFileName;
		PFILE_OBJECT fileobj;
		FILE_INFORMATION_CLASS FileInformationClass = FileNameInformation ;
		if ( FileHandle != NULL)
		{
				UNICODE_STRING uProcessPath={0};//����·��
				ANSI_STRING aProcessPath={0};//����·��
				
			   NTSTATUS nts = STATUS_UNSUCCESSFUL;
			   IO_STATUS_BLOCK iosb ={ 0,0 };
			   ANSI_STRING    ansiUndeleteFileName ;//
			   UNICODE_STRING usFileName ={ 0,0,0 }; //
			   
			   
			   PCWSTR ProcessName = GetCurrentProcessFileName();
				PCWSTR WDosName = NULL;
			   
			   PFILE_NAME_INFORMATION pFileInfo = NULL;
			   
			   RtlInitUnicodeString(&uProcessPath,ProcessName);
			   RtlUnicodeStringToAnsiString(&aProcessPath,&uProcessPath,TRUE);
			   RtlInitEmptyUnicodeString( &usFileName,'\0',0 );
			   pFileInfo = (PFILE_NAME_INFORMATION) ExAllocatePool( PagedPool, sizeof(FILE_NAME_INFORMATION) + MAXPATHLEN);
			   if ( NULL == pFileInfo )
			   {
					return STATUS_INSUFFICIENT_RESOURCES;
			   }//if ( NULL == pfni )
			   RtlZeroMemory( pFileInfo ,sizeof(FILE_NAME_INFORMATION) + MAXPATHLEN );
			   //���ļ��������ļ�����Ϣ
			   nts = ZwQueryInformationFile(   
					FileHandle,
					&iosb, 
					pFileInfo,
					sizeof(FILE_NAME_INFORMATION) + MAXPATHLEN, 
					FileNameInformation
				);
			   if ( !NT_SUCCESS(nts) )
			   {
					ExFreePool(pFileInfo);
					pFileInfo = NULL;
					return nts; 
			   } 
			   else
			   {
					ANSI_STRING ansiDesFileName; 
					PWSTR pwstr=NULL;
					ULONG len =0;

	////////////////////////////////		ת��·����		////////////////////////////////
					ANSI_STRING apath;
					UNICODE_STRING upath;
					
					ANSI_STRING apathpre;
					UNICODE_STRING upathpre;
	////////////////////////////////		ת��·���Ľ���		////////////////////////////////

					pwstr= wcsrchr( pFileInfo->FileName, L'\\' );
					RtlInitUnicodeString(&usFileName, pwstr+1); // uniFileName �����ͷ�
					RtlUnicodeStringToAnsiString(&ansiDesFileName, &usFileName, TRUE); // TRUE, �����ͷ�
					GetProcessName(aProcessName);
					ObReferenceObjectByHandle(FileHandle,GENERIC_READ,*IoFileObjectType,KernelMode,(PVOID *)&fileobj,NULL);	
					WriteFileName=fileobj->FileName.Buffer;
					
					//	�˴�ת��	·��	
					RtlInitUnicodeString(&upath,WriteFileName);
					RtlUnicodeStringToAnsiString(&apath,&upath,TRUE);	

					
					if (wcscmp(WriteFileName,PreWriteFileName)!=0){
						IoVolumeDeviceToDosName( ((PFILE_OBJECT)fileobj)->DeviceObject, &volumeDosName );
						if  (&volumeDosName!=NULL && 
						(volumeDosName.Length<=6)//Ӧ�������������
						)
						{
							//	�˴�ת��	�̷�	
							RtlInitUnicodeString(&upathpre,volumeDosName.Buffer);
							RtlUnicodeStringToAnsiString(&apathpre,&upathpre,TRUE);	

							KdPrint(("%s(%s)�޸��ļ�:%s%s \r\n",aProcessName,aProcessPath.Buffer,apathpre.Buffer,apath.Buffer));
							
							//��д�豸��չ
							deviceExtension=(PDEVICE_EXTENSION)g_pDriverObject->DeviceExtension;

							strcpy(deviceExtension->ProcName,aProcessName);
							strcpy(deviceExtension->ProcFullPath,aProcessPath.Buffer);
							strcpy(deviceExtension->FilePre,apathpre.Buffer);
							strcpy(deviceExtension->FilePath,apath.Buffer);
			
							//֪ͨӦ�ò������ȡ��
							KeSetEvent(deviceExtension->FileEvent,0,FALSE);
							KeClearEvent(deviceExtension->FileEvent);	

							PreWriteFileName=WriteFileName;
						}
					}
			   }
		}
	}
	
	return RealZwWriteFile(
	   FileHandle, 
	   Event OPTIONAL, 
	   ApcRoutine OPTIONAL, 
	   ApcContext OPTIONAL, 
	   IoStatusBlock, 
	   Buffer, 
	   Length, 
	   ByteOffset OPTIONAL, 
	   Key OPTIONAL 
	   );
}	
