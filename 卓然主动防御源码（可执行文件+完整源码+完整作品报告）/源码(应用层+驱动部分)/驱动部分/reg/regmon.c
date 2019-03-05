#include "regmon.h"

// Download by http://www.51xue8xue8.com
//-------------------regmon����-------------------
int TurnOnRegMon=1;//Ĭ�Ͽ���
int RegPass=1;//Ĭ��ͨ��
#define NT_DEVICE_NAME      L"\\Device\\RegMon"
#define DOS_DEVICE_NAME     L"\\DosDevices\\RegMon"
#define EVENT_NAME L"\\BaseNamedObjects\\RegEvent"  //������֪ͨ�¼�
#define APP_EVENT_NAME L"\\BaseNamedObjects\\RegAppEvent" //Ӧ�ò��Ƿ��¼�
#define IOCTL_NTPROCDRV_GET_REGINFO CTL_CODE(FILE_DEVICE_UNKNOWN,0x0910, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_NTPROCDRV_SET_APPEVENT_OK CTL_CODE(FILE_DEVICE_UNKNOWN,0x0911, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_NTPROCDRV_SET_APPEVENT_NO CTL_CODE(FILE_DEVICE_UNKNOWN,0x0912, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_NTPROCDRV_TURNON_REGMON CTL_CODE(FILE_DEVICE_UNKNOWN,0x0913, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_NTPROCDRV_TURNOFF_REGMON CTL_CODE(FILE_DEVICE_UNKNOWN,0x0914, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
//-------------------regmon���ƽ���-------------------




//-------------------ȫ�ֱ���-------------------
HANDLE             RegAppHandle;        // �¼�������
PKEVENT            RegAppEvent;          // �û����ں�ͨ�ŵ��¼�����ָ��
PDEVICE_OBJECT  g_pDriverObject;//����ȫ������
//-------------------------------------------------



NTSTATUS DispatchCreate(IN PDEVICE_OBJECT DeviceObject,IN PIRP	 Irp);
NTSTATUS DispatchClose(IN PDEVICE_OBJECT DeviceObject,IN PIRP	 Irp);
NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject,IN PUNICODE_STRING theRegistryPath)
{
	//������ں���
	NTSTATUS        ntStatus = STATUS_SUCCESS;
	UNICODE_STRING  ntDeviceName,linkString;
	UNICODE_STRING  DeviceLinkString;
	UNICODE_STRING	RegEventString;
	UNICODE_STRING	RegAppEventString;
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

    RtlInitUnicodeString(&linkString,L"\\DosDevices\\RegMon");

    ntStatus = IoCreateSymbolicLink (&linkString, &ntDeviceName);

	if(!NT_SUCCESS(ntStatus))
	{
		return ntStatus;
	}

     deviceExtension=(PDEVICE_EXTENSION)deviceObject->DeviceExtension;   
	g_pDriverObject=deviceObject;	 
	 
	//��ʼ���¼�
	RtlInitUnicodeString(&RegEventString,EVENT_NAME);  //������
	RtlInitUnicodeString(&RegAppEventString,APP_EVENT_NAME);  //Ӧ�ò�
	deviceExtension->RegEvent=IoCreateNotificationEvent(&RegEventString,&deviceExtension->RegHandle);	
	RegAppEvent=IoCreateNotificationEvent(&RegAppEventString,&RegAppHandle);
	//����Ϊ������״̬
	KeClearEvent(deviceExtension->RegEvent);	
	KeClearEvent(RegAppEvent);	
	
	
	
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
	HOOK_SYSCALL(ZwSetValueKey,HookZwSetValueKey,RealZwSetValueKey);
	KdPrint(("[*]ע�����������������.\n"));

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
	HOOK_SYSCALL(ZwSetValueKey,RealZwSetValueKey,Oldfun);

	if(m_MDL){
		MmUnmapLockedPages(m_Mapped,m_MDL);
		IoFreeMdl(m_MDL);
	}

	KdPrint(("[*]ע�������ж�����.\n"));

}


NTSTATUS DispatchCreate(IN PDEVICE_OBJECT DeviceObject,IN PIRP	 Irp)
{
  //DbgPrint("IRP_MJ_Create\n");

  Irp->IoStatus.Information=0;
  Irp->IoStatus.Status=STATUS_SUCCESS;
  IoCompleteRequest(Irp,IO_NO_INCREMENT);
  return STATUS_SUCCESS;
}


//�����豸�������
NTSTATUS DispatchClose(IN PDEVICE_OBJECT DeviceObject,IN PIRP	 Irp)
{
  //DbgPrint("IRP_MJ_Close\n");
  
  Irp->IoStatus.Information=0;
  Irp->IoStatus.Status=STATUS_SUCCESS;
  IoCompleteRequest(Irp,IO_NO_INCREMENT);
  return STATUS_SUCCESS;
}



//HookZwSetValueKey 
NTSTATUS HookZwSetValueKey(
IN HANDLE KeyHandle,
IN PUNICODE_STRING ValueName,
IN ULONG TitleIndex OPTIONAL,
IN ULONG Type,
IN PVOID Data,
IN ULONG DataSize)
{
    NTSTATUS rc;
    UNICODE_STRING *pUniName,uProcessPath; //����õ��޸�ע����UNI·��
    ULONG actualLen;
    ANSI_STRING keyname, akeyname, m_keyname,aProcessPath,aProcessName1;       //����õ��޸�ע����UNI·��
    PVOID pKey;
	char aProcessName[PROCNAMELEN];//������
	
	PCSTR APN;
	PCSTR regkeyname;
	PCSTR regkeyvalue;
	
	PDEVICE_EXTENSION	deviceExtension;
	PCWSTR ProcessName = GetCurrentProcessFileName();//����·��
	RtlInitUnicodeString(&uProcessPath,ProcessName);
	RtlUnicodeStringToAnsiString( &aProcessPath, &uProcessPath, TRUE);
    RtlUnicodeStringToAnsiString( &akeyname, ValueName, TRUE);
	GetProcessName(aProcessName);
	RtlInitAnsiString(&aProcessName1,aProcessName);
	APN=aProcessName;
    // �õ��ļ������ָ��
	
	
	if (TurnOnRegMon==0)  //���ע����ع��ܲ�����
	{
		RealZwSetValueKey(KeyHandle,ValueName,TitleIndex,Type,Data,DataSize);
	}
	
	if (TurnOnRegMon==1)//���ע����ع��ܿ���
	{
		if( pKey = GetPointer( KeyHandle))
		{
			RegPass=1;//Ӧ�ò�Ĭ��ͨ��
			pUniName = ExAllocatePool( NonPagedPool, 512*2+2*sizeof(ULONG));
			pUniName->MaximumLength = 512*2;
			memset(pUniName,0,pUniName->MaximumLength); 
		   
			if( NT_SUCCESS( ObQueryNameString( pKey, pUniName, 512*2, &actualLen) ) )
			{
				RtlUnicodeStringToAnsiString( &keyname, pUniName, TRUE);
				
				//����������
				regkeyname=keyname.Buffer;
				regkeyvalue=akeyname.Buffer;
				
				regkeyname=_strupr(keyname.Buffer);
				regkeyvalue=_strupr(akeyname.Buffer);
				
				//�˴��ж�,�ɶ�����������
				if (strstr(regkeyname,"\\REGISTRY\\MACHINE\\SOFTWARE\\MICROSOFT\\WINDOWS\\CURRENTVERSION\\RUN"))
				{

					DbgPrint("����:%s(%ws)\r\n�޸�ע���:%s\r\n%s\r\n",aProcessName,ProcessName,akeyname.Buffer,keyname.Buffer); 

					deviceExtension=(PDEVICE_EXTENSION)g_pDriverObject->DeviceExtension;
					//��Ϣ�����豸��չ
					strcpy(deviceExtension->ProcName,APN);//����������
					strcpy(deviceExtension->ProcFullPath,(PCHAR )aProcessPath.Buffer);
					strcpy(deviceExtension->Info1,akeyname.Buffer);//����
					strcpy(deviceExtension->Info2,keyname.Buffer);//·��
					
					KdPrint(("deviceExtension->ProcName:%s",deviceExtension->ProcName));
					KdPrint(("deviceExtension->ProcFullPath:%s",deviceExtension->ProcFullPath));
					KdPrint(("deviceExtension->Info1:%s",(PCSTR)deviceExtension->Info1));
					KdPrint(("deviceExtension->Info2:%s",(PCSTR)deviceExtension->Info2));

					//�����¼�,Ӧ�ò�ȡ��Ϣ
					KeClearEvent(RegAppEvent);	//���ⲻ����
					KeSetEvent(deviceExtension->RegEvent,0,FALSE);
					KeClearEvent(deviceExtension->RegEvent);
					
					KdPrint(("�ȴ�Ӧ�ò�ȷ��"));	
					KeWaitForSingleObject(RegAppEvent,Executive,KernelMode,0,NULL);
					KeClearEvent(RegAppEvent);	
					KdPrint(("Ӧ�ò�ȷ�Ͻ���:%d",RegPass));		
					
					if (RegPass==1)//Ӧ�ò�ȷ��ͨ��
					{
						//ȱ��Ϣ���,ȱ�ͷ���Դ
						KdPrint(("     ����"));	
						return RealZwSetValueKey(KeyHandle,ValueName,TitleIndex,Type,Data,DataSize);

					} 
					if (RegPass==0)//Ӧ�ò�ȷ��ͨ��
					{
						KdPrint(("    �ܾ�"));	
						return STATUS_ACCESS_DENIED;
					} 
				}
			}
		}
	}			

		

    // �ͷ��ڴ�
    return STATUS_ACCESS_DENIED;
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
	

	case IOCTL_NTPROCDRV_GET_REGINFO:
	   {
			KdPrint(("����޸�ע������Ϣ"));
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
	case IOCTL_NTPROCDRV_SET_APPEVENT_OK: 
		{
			KdPrint(("����!Ӧ��֪ͨ�������¼�"));
			RegPass=1;//ȫ��ͨ��,�¼����ñ����ں���
			KeSetEvent(RegAppEvent,IO_NO_INCREMENT,FALSE);
			status=STATUS_SUCCESS;
		}
		break;
	case IOCTL_NTPROCDRV_SET_APPEVENT_NO: 
		{
			KdPrint(("��ֹ!Ӧ��֪ͨ�������¼�"));
			RegPass=0;//ȫ�ӽ�ֹ			
			KeSetEvent(RegAppEvent,IO_NO_INCREMENT,FALSE);
			status=STATUS_SUCCESS;
		}
		break;
		case IOCTL_NTPROCDRV_TURNON_REGMON:
		{
			KdPrint(("����ע�����"));
			TurnOnRegMon=1;	//����
			//KeSetEvent(RegAppEvent,IO_NO_INCREMENT,FALSE);
			status=STATUS_SUCCESS;
		}
		break;
	case IOCTL_NTPROCDRV_TURNOFF_REGMON:
		{
			KdPrint(("�ر�ע�����"));
			TurnOnRegMon=0;	//�ر�,֪ͨ�¼����ڹر�
			//KeSetEvent(RegAppEvent,IO_NO_INCREMENT,FALSE);
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











	
