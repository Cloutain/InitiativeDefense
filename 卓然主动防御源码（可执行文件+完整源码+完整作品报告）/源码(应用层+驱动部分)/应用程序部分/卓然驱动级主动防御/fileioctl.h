//-------------------Filemon����-------------------
#define IOCTL_GET_FILEINFO CTL_CODE(FILE_DEVICE_UNKNOWN,0x0840, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_SET_APPEVENT_OK CTL_CODE(FILE_DEVICE_UNKNOWN,0x0841, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_SET_APPEVENT_NO CTL_CODE(FILE_DEVICE_UNKNOWN,0x0842, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_TURNON_FILEMON CTL_CODE(FILE_DEVICE_UNKNOWN,0x0843, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_TURNOFF_FILEMON CTL_CODE(FILE_DEVICE_UNKNOWN,0x0844, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
int TurnOnFileMon=1;//Ĭ�Ͽ���
int FilePass=1;//Ĭ��ͨ��
//-------------------Filemon���ƽ���-------------------


#define FILE_EVENT_NAME L"FileEvent"  //������֪ͨ�¼�
#define APP_EVENT_NAME L"FileAppEvent" //Ӧ�ò��Ƿ��¼�

#define NT_DEVICE_NAME      L"\\Device\\FileMon"
#define DOS_DEVICE_NAME     L"\\DosDevices\\FileMon"

typedef struct _FileCallbackInfo
{
	CHAR    ProcName[256];	//������
	CHAR    ProcFullPath[256];          //����·��
	CHAR    FilePre[256];					//��Ϣ1
	CHAR    FilePath[256];					//��Ϣ2
 } FILE_CALLBACK_INFO, *PFILE_CALLBACK_INFO;


