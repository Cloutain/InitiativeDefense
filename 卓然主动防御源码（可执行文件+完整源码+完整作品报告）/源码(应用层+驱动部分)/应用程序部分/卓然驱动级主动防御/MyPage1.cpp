// MyPage1.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "main.h"
#include "MyPage1.h"
#include <winsvc.h>  
#include "ioctls.h"

//#include "getmd5.cpp"



#include "winioctl.h"
#define SYMBOL_LINK L"\\\\.\\ProcMon"
#define PROC_DRIVER_NAME L"ProcMon"
#define PROC_DRIVER_PATH L"procmon.sys"

#define REG_DRIVER_NAME L"RegMon"
#define REG_DRIVER_PATH L"regmon.sys"

#define MEM_DRIVER_NAME L"MemMon"
#define MEM_DRIVER_PATH L"memmon.sys"

#define FILE_DRIVER_NAME L"FileMon"
#define FILE_DRIVER_PATH L"filemon.sys"
// CMyPage1 �Ի���
HaveProc ProcWnd;//ȫ���Ƿ�Ի���
static CALLBACK_INFO cbkinfo={0}, cbktemp = {0};
DWORD pid;

CListBox MainList;

IMPLEMENT_DYNAMIC(CMyPage1, CPropertyPage)

CMyPage1::CMyPage1()
	: CPropertyPage(CMyPage1::IDD)
{


}

CMyPage1::~CMyPage1()
{
}

void CMyPage1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, Stat);
	DDX_Control(pDX, IDC_LIST1,MainList);
}


BEGIN_MESSAGE_MAP(CMyPage1, CPropertyPage)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON2, &CMyPage1::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CMyPage1::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON1, &CMyPage1::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON4, &CMyPage1::OnBnClickedButton4)
	ON_EN_CHANGE(IDC_EDIT1, &CMyPage1::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON7, &CMyPage1::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON6, &CMyPage1::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON8, &CMyPage1::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CMyPage1::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &CMyPage1::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON12, &CMyPage1::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON11, &CMyPage1::OnBnClickedButton11)
END_MESSAGE_MAP()


// CMyPage1 ��Ϣ�������

void CMyPage1::OnPaint()
{

		// ȡ�ô��ڿͻ������С
		CRect WndRect ;
		this->GetWindowRect ( &WndRect ) ;
		this->ScreenToClient ( &WndRect ) ;
	

		CPaintDC   dc(this);   
		CBitmap   bitmap;   
		bitmap.LoadBitmap(IDB_BITMAP1);   
		HBITMAP   OldBitmap;   
		CDC   MemDC;CRect   rect;   
		MemDC.CreateCompatibleDC(&dc);   
		GetClientRect(rect);   
		OldBitmap=(HBITMAP)MemDC.SelectObject(&bitmap);   
		dc.BitBlt(0,0,603,380,&MemDC,0,0,SRCCOPY);   
		MemDC.SelectObject(OldBitmap);
	
		CDialog::OnPaint();
	
}

bool CMyPage1::LoadNTDriver(LPCWSTR lpszDriverName, LPCWSTR lpszDriverPath)
{
	WCHAR szDriverImagePath[256];
	//�õ�����������·��
	GetFullPathName(lpszDriverPath, 256, szDriverImagePath, NULL);

	bool bRet = FALSE;

	SC_HANDLE hServiceMgr=NULL;//SCM�������ľ��
	SC_HANDLE hServiceDDK=NULL;//NT��������ķ�����

	//�򿪷�����ƹ�����
	hServiceMgr = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );

	if( hServiceMgr == NULL )  
	{
		//OpenSCManagerʧ��
		//printf( "OpenSCManager() Faild %d ! \n", GetLastError() );
		bRet = FALSE;
		goto BeforeLeave;
	}
	
	//������������Ӧ�ķ���
	hServiceDDK = CreateService( hServiceMgr,
		lpszDriverName, //�����������ע����е�����  
		lpszDriverName, // ע������������ DisplayName ֵ  
		SERVICE_ALL_ACCESS, // ������������ķ���Ȩ��  
		SERVICE_KERNEL_DRIVER,// ��ʾ���صķ�������������  
		SERVICE_DEMAND_START, // ע������������ Start ֵ  
		SERVICE_ERROR_IGNORE, // ע������������ ErrorControl ֵ  
		szDriverImagePath, // ע������������ ImagePath ֵ  
		NULL,  
		NULL,  
		NULL,  
		NULL,  
		NULL);  

	DWORD dwRtn;
	//�жϷ����Ƿ�ʧ��
	if( hServiceDDK == NULL )  
	{  
		dwRtn = GetLastError();
		if( dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_EXISTS )  
		{  
			//��������ԭ�򴴽�����ʧ��
			MainList.AddString(L"��������ԭ�򴴽�����ʧ��! \n");  
			bRet = FALSE;
			goto BeforeLeave;
		}  
		else  
		{
			//���񴴽�ʧ�ܣ������ڷ����Ѿ�������
			MainList.AddString(L"���񴴽�ʧ�ܣ������ڷ����Ѿ�������! \n");  
		}

		// ���������Ѿ����أ�ֻ��Ҫ��  
		hServiceDDK = OpenService( hServiceMgr, lpszDriverName, SERVICE_ALL_ACCESS );  
		if( hServiceDDK == NULL )  
		{
			//����򿪷���Ҳʧ�ܣ�����ζ����
			dwRtn = GetLastError();  
			this->MessageBox(L"�򿪷���ʧ��! \n");  
			bRet = FALSE;
			goto BeforeLeave;
		}  
		else 
		{

		}
	}  
	else  
	{

	}

	bRet = TRUE;
//�뿪ǰ�رվ��
BeforeLeave:
	if(hServiceDDK)
	{
		CloseServiceHandle(hServiceDDK);
	}
	if(hServiceMgr)
	{
		CloseServiceHandle(hServiceMgr);
	}
	return bRet;
}



bool CMyPage1::Startservice(LPCWSTR lpszDriverName, LPCWSTR lpszDriverPath)
{
	
	WCHAR szDriverImagePath[256];
	//�õ�����������·��
	GetFullPathName(lpszDriverPath, 256, szDriverImagePath, NULL);

	BOOL bRet = FALSE;

	SC_HANDLE hServiceMgr=NULL;//SCM�������ľ��
	SC_HANDLE hServiceDDK=NULL;//NT��������ķ�����

	//�򿪷�����ƹ�����
	hServiceMgr = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );

	if( hServiceMgr == NULL )  
	{
		//OpenSCManagerʧ��
		this->MessageBox(L"��SCManager()ʧ��! \n");
		bRet = FALSE;
		goto BeforeLeave;
	}

	hServiceDDK = OpenService( hServiceMgr, lpszDriverName, SERVICE_ALL_ACCESS );  
	if( hServiceDDK == NULL )  
	{
		//����򿪷���Ҳʧ�ܣ�����ζ����
		DWORD dwRtn = GetLastError();  
		this->MessageBox(L"�򿪷���ʧ��! \n");  
		bRet = FALSE;
		goto BeforeLeave;
	}  
	//��������
	bRet= StartService( hServiceDDK, NULL, NULL );  
	if( !bRet )  
	{  
		DWORD dwRtn = GetLastError();  
		if( dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_ALREADY_RUNNING )  
		{  
			this->MessageBox(L"��������ʧ��! \n");  
			bRet = FALSE;
			goto BeforeLeave;
		}  
		else  
		{  
			if( dwRtn == ERROR_IO_PENDING )  
			{  
				//�豸����ס
				this->MessageBox(L"StartService() Faild ERROR_IO_PENDING ! \n");
				bRet = FALSE;
				goto BeforeLeave;
			}  
			else  
			{  
				//�����Ѿ�����
				MainList.AddString(L"�����Ѿ�����! \n");
				bRet = TRUE;
				goto BeforeLeave;
			}  
		}  
	}
	bRet = TRUE;

BeforeLeave:
	if(hServiceDDK)
	{
		CloseServiceHandle(hServiceDDK);
	}
	if(hServiceMgr)
	{
		CloseServiceHandle(hServiceMgr);
	}
	return bRet;
}








bool CMyPage1::UnloadNTDriver(PCWSTR szSvrName)
{
	bool bRet = FALSE;
	SC_HANDLE hServiceMgr=NULL;//SCM�������ľ��
	SC_HANDLE hServiceDDK=NULL;//NT��������ķ�����
	SERVICE_STATUS SvrSta;
	//��SCM������
	hServiceMgr = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );  
	if( hServiceMgr == NULL )  
	{
		//����SCM������ʧ��
		this->MessageBox( L"��SCM������ʧ��! \n");  
		bRet = FALSE;
		goto BeforeLeave;
	}  

	//����������Ӧ�ķ���
	hServiceDDK = OpenService( hServiceMgr, szSvrName, SERVICE_ALL_ACCESS );  

	if( hServiceDDK == NULL )  
	{
		//����������Ӧ�ķ���ʧ��
		this->MessageBox( L"����������Ӧ�ķ���ʧ��,��Ϊ����û�а�װ! \n");  
		bRet = FALSE;
		goto BeforeLeave;
	}  

	//ֹͣ�����������ֹͣʧ�ܣ�ֻ�������������ܣ��ٶ�̬���ء�  
	if( !ControlService( hServiceDDK, SERVICE_CONTROL_STOP , &SvrSta ) )  
	{  
//		this->MessageBox( L"ֹͣ��������ʧ��!\n");  
	}  
	else  
	{
		//����������Ӧ��ʧ��
	//	this->MessageBox( L"����������Ӧ��ʧ��\n" );  
	}  
	//��̬ж����������  
	if( !DeleteService( hServiceDDK ) )  
	{
		//ж��ʧ��
		this->MessageBox( L"ж��ʧ��\n");  
		goto BeforeLeave;
	}  

	bRet = TRUE;
BeforeLeave:
//�뿪ǰ�رմ򿪵ľ��
	if(hServiceDDK)
	{
		CloseServiceHandle(hServiceDDK);
	}
	if(hServiceMgr)
	{
		CloseServiceHandle(hServiceMgr);
	}
	return bRet;	
}




bool CMyPage1::Stopservice(PCWSTR szSvrName)
{
	bool bRet = FALSE;
	SC_HANDLE hServiceMgr=NULL;//SCM�������ľ��
	SC_HANDLE hServiceDDK=NULL;//NT��������ķ�����
	SERVICE_STATUS SvrSta;
	//��SCM������
	hServiceMgr = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );  
	if( hServiceMgr == NULL )  
	{
		//����SCM������ʧ��
		this->MessageBox( L"��SCM������ʧ��! \n");  
		bRet = FALSE;
		goto BeforeLeave;
	}  
	//����������Ӧ�ķ���
	hServiceDDK = OpenService( hServiceMgr, szSvrName, SERVICE_ALL_ACCESS );  

	if( hServiceDDK == NULL )  
	{
		//����������Ӧ�ķ���ʧ��
		this->MessageBox( L"����������Ӧ�ķ���ʧ��,��Ϊ����û�а�װ! \n");  
		bRet = FALSE;
		goto BeforeLeave;
	}  
	//ֹͣ�����������ֹͣʧ�ܣ�ֻ�������������ܣ��ٶ�̬���ء�  
	if( !ControlService( hServiceDDK, SERVICE_CONTROL_STOP , &SvrSta ) )  
	{  
		this->MessageBox( L"ֹͣ��������ʧ��!\n");  
	} 

	bRet = TRUE;
BeforeLeave:
//�뿪ǰ�رմ򿪵ľ��
	if(hServiceDDK)
	{
		CloseServiceHandle(hServiceDDK);
	}
	if(hServiceMgr)
	{
		CloseServiceHandle(hServiceMgr);
	}
	return bRet;	

}





void CMyPage1::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	bool Ret=Startservice(PROC_DRIVER_NAME,PROC_DRIVER_PATH);
	if (Ret) {
		MainList.AddString(L"���̼����������ɹ�");
	}
	else 
	{
		this->MessageBox(L"�������̼�ط���ʧ��,��Ϊ����û�а�װ");
	}
}



void CMyPage1::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	bool Ret=Stopservice(PROC_DRIVER_NAME);
	if (Ret) {
		MainList.AddString(L"���̼�ط���ֹͣ�ɹ�");
	}
}



void CMyPage1::OnBnClickedButton3()
{

	bool Ret=LoadNTDriver(PROC_DRIVER_NAME,PROC_DRIVER_PATH);
	if (Ret) {
		MainList.AddString(L"���̼�ط���װ�ɹ�");
	}
	Ret=LoadNTDriver(REG_DRIVER_NAME,REG_DRIVER_PATH);
	if (Ret) {
		MainList.AddString(L"ע����ط���װ�ɹ�");
	}
	Ret=LoadNTDriver(MEM_DRIVER_NAME,MEM_DRIVER_PATH);
	if (Ret) {
		MainList.AddString(L"�ڴ��ط���װ�ɹ�");
	}
	Ret=LoadNTDriver(FILE_DRIVER_NAME,FILE_DRIVER_PATH);
	if (Ret) {
		MainList.AddString(L"�ļ���ط���װ�ɹ�");
	}
}


void CMyPage1::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	bool bRet=UnloadNTDriver(PROC_DRIVER_NAME);
	if (bRet){
		MainList.AddString(L"���̷���ж�سɹ�");
	}
	bRet=UnloadNTDriver(REG_DRIVER_NAME);
	if (bRet){
		MainList.AddString(L"ע������ж�سɹ�");
	}
	bRet=UnloadNTDriver(MEM_DRIVER_NAME);
	if (bRet){
		MainList.AddString(L"�ڴ����ж�سɹ�");
	}
	bRet=UnloadNTDriver(FILE_DRIVER_NAME);
	if (bRet){
		MainList.AddString(L"�ļ�����ж�سɹ�");
	}
}



void CMyPage1::OnEnChangeEdit1()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CPropertyPage::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


#include "haveproc.h"


void CMyPage1::OnBnClickedButton7()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//����ȡMD5
	//char *str="c:\\ch.php";
	//md5main(1,&str);

	//blg3.DoModal();
	//HaveProc pr;
	//pr.DoModal();

}

void CMyPage1::OnBnClickedButton6()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL Ret=Startservice(REG_DRIVER_NAME,REG_DRIVER_PATH);
	if (Ret) {
		MainList.AddString(L"ע�������������ɹ�");

	}
	else 
	{
		this->MessageBox(L"����ע����ط���ʧ��,��Ϊ����û�а�װ");
	}
}

void CMyPage1::OnBnClickedButton8()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL Ret=Stopservice(REG_DRIVER_NAME);
	if (Ret) {
		MainList.AddString(L"ע����ط���ֹͣ�ɹ�");
	}
}

void CMyPage1::OnBnClickedButton9()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL Ret=Startservice(MEM_DRIVER_NAME,MEM_DRIVER_PATH);
	if (Ret) {
		MainList.AddString(L"�ڴ���ؼ����������ɹ�");
	}
	else 
	{
		this->MessageBox(L"�ڴ���ؼ�ط�������ʧ��,��Ϊ����û�а�װ");
	}
}

void CMyPage1::OnBnClickedButton10()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL Ret=Stopservice(MEM_DRIVER_NAME);
	if (Ret) {
		MainList.AddString(L"�ڴ��ط���ֹͣ�ɹ�");
	}
}

void CMyPage1::OnBnClickedButton12()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL Ret=Stopservice(MEM_DRIVER_NAME);
	if (Ret) {
		MainList.AddString(L"�ļ���ط���ֹͣ�ɹ�");
	}
}

void CMyPage1::OnBnClickedButton11()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL Ret=Startservice(FILE_DRIVER_NAME,FILE_DRIVER_PATH);
	if (Ret) {
		MainList.AddString(L"�ļ������������ɹ�");
		UpdateData(false);

	}
	else 
	{
		this->MessageBox(L"�ļ����ؼ�ط�������ʧ��,��Ϊ����û�а�װ");
	}
}


