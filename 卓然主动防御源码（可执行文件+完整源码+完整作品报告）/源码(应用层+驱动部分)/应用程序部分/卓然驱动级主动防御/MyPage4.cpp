// MyPage4.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "main.h"
#include "MyPage4.h"

#include "memioctl.h"
#include "winioctl.h"
#define SYMBOL_LINK     L"\\\\.\\MemMon"
#define NT_DEVICE_NAME      L"\\Device\\MemMon"
#define DOS_DEVICE_NAME     L"\\DosDevices\\MemMon"
#define MEM_EVENT_NAME L"MemEvent"  //������֪ͨ�¼�
#define APP_EVENT_NAME L"MenAppEvent" //Ӧ�ò��Ƿ��¼�


int memlp=0;
CListBox MemList;
MEM_CALLBACK_INFO meminfo={0}, memtemp = {0};



int MemMon(void)
{
	// �������豸����
	HANDLE hDriver = ::CreateFile(
							SYMBOL_LINK,
							GENERIC_READ | GENERIC_WRITE,
							0,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL,
							NULL);
	if (hDriver == INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL,L"������ʧ��,�밲װ����������\n",L"����",MB_OK);
		return -1;
	}

	HANDLE hProcessEvent = ::OpenEventW(SYNCHRONIZE, FALSE, MEM_EVENT_NAME);
	if (hProcessEvent == NULL)
	{
		MessageBox(NULL,L"���¼�ʧ�ܣ�\n",L"����",MB_OK);
		return -1;
	}
	//�������
	MemList.AddString(L"�ڴ���ؼ�ؿ����ɹ�");
	while (::WaitForSingleObject(hProcessEvent, INFINITE)==WAIT_OBJECT_0)
	{
		DWORD	dwRet;
		BOOL	bRet;

		bRet = ::DeviceIoControl(
							hDriver,
							IOCTL_GET_MEMINFO,
							NULL,
							0,
							&meminfo,
							sizeof(meminfo)+64,
							&dwRet,
							NULL);

		if (bRet)
		{
			if (strcmp(meminfo.ProcFullPath,memtemp.ProcFullPath)!=0)
			{
//				CString strtemp;
				DWORD num;
				WCHAR *wProcFullPath;//����һ��UNICODE��ָ��

				//����ת��WCHAR	   	wParentProcFullPath  to  wProcFullPath			
				num= MultiByteToWideChar(CP_ACP,0,meminfo.ProcFullPath,-1,NULL,0); //�������GB2312ʵ���м��������
				wProcFullPath= (WCHAR*)calloc(num,sizeof(WCHAR));//��̬������ռ����
				if(wProcFullPath == NULL) free(wProcFullPath);
				memset(wProcFullPath, 0, num*sizeof(WCHAR));//��ʼ������
				MultiByteToWideChar(CP_ACP,0, meminfo.ProcFullPath, -1, wProcFullPath, num);


//				strtemp.Format (L"%S",wProcFullPath);
				MemList.AddString(wProcFullPath);


	//			SetListBoxHScroll();
				memtemp = meminfo;
			}

		} 
		else
		{
			break;
		}
		
	}

	return 0;
}


UINT ThreadProcMemMon1(LPVOID lpParam)
{

	int ret=MemMon();
	return 0;
}

// CMyPage4 �Ի���

IMPLEMENT_DYNAMIC(CMyPage4, CPropertyPage)

CMyPage4::CMyPage4()
	: CPropertyPage(CMyPage4::IDD)
	, data1(0)
	, data2(0)
	, data3(0)
{

}

CMyPage4::~CMyPage4()
{
}

void CMyPage4::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, MemList);

}


BEGIN_MESSAGE_MAP(CMyPage4, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON1, &CMyPage4::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMyPage4::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CMyPage4::OnBnClickedButton3)
END_MESSAGE_MAP()


// CMyPage4 ��Ϣ�������

void CMyPage4::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	MemList.AddString(L"�ڴ���ؼ�ؿ����ɹ�");
//	HANDLE hProc=AfxBeginThread(ThreadProcMemMon1,&memlp);

}


void CMyPage4::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}



BOOL CMyPage4::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	HANDLE hProc=AfxBeginThread(ThreadProcMemMon1,&memlp);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CMyPage4::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	MemList.ResetContent();
}
