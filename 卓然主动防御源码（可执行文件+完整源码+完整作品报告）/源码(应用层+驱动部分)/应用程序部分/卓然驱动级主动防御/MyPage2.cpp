// MyPage2.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "main.h"
#include "MyPage2.h"
#include "HaveProc.h"
#include <algorithm> 
#include "winioctl.h"
#include "ioctls.h"
#include <string>
#define SYMBOL_LINK L"\\\\.\\ProcMon"
#define DRIVER_NAME L"ProcMon"
#define DRIVER_PATH L"WATCH.sys"

#define IOCTL_NTPROCDRV_TURNON_PROCMON CTL_CODE(FILE_DEVICE_UNKNOWN,0x0802, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_NTPROCDRV_TURNOFF_PROCMON CTL_CODE(FILE_DEVICE_UNKNOWN,0x0803, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)

int TurnOnProcMon=1;
extern HaveProc ProcWnd;//ȫ���Ƿ�Ի���
extern CALLBACK_INFO cbkinfo={0}, cbktemp = {0};
extern DWORD pid;
extern int lp2=1;
//extern CString procdescribe;
#include "AddProcRull.h"
#include "HttpClient.h"



//����-------------------------------
#include <list>
using   namespace   std;   
typedef struct _PROCESS_INFO{
    WCHAR lpFilePath[4][256];//4��256�����ִ�
}PROCESS_INFO, *PPROCESS_INFO;
typedef list<PROCESS_INFO> ProcessList;
typedef ProcessList::iterator ITERATOR;

ProcessList List;
PROCESS_INFO pi;
ITERATOR iter = List.begin();
//���н���-------------------------------




// CMyPage2 �Ի���


CListBox ProcListBox;

IMPLEMENT_DYNAMIC(CMyPage2, CPropertyPage)

CMyPage2::CMyPage2()
	: CPropertyPage(CMyPage2::IDD)
	, ProcList(_T(""))
{

}

CMyPage2::~CMyPage2()
{
}

void CMyPage2::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST1, ProcListBox);
	DDX_Control(pDX, IDC_LIST3, ProcRull);
}


BEGIN_MESSAGE_MAP(CMyPage2, CPropertyPage)
	ON_BN_CLICKED(IDC_CHECK1, &CMyPage2::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_BUTTON1, &CMyPage2::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON3, &CMyPage2::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &CMyPage2::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, &CMyPage2::OnBnClickedButton4)
	ON_LBN_SELCHANGE(IDC_LIST1, &CMyPage2::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_BUTTON5, &CMyPage2::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CMyPage2::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON8, &CMyPage2::OnBnClickedButton8)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CMyPage2 ��Ϣ�������

//�̺߳���

int ProcMon2(void)
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
		MessageBox(NULL,L"������ʧ��,�밲װ����������!\n",L"����",MB_OK);
		return -1;
	}

	HANDLE hProcessEvent = ::OpenEventW(SYNCHRONIZE, FALSE, EVENT_NAME);
	if (hProcessEvent == NULL)
	{
		MessageBox(NULL,L"���¼�ʧ�ܣ�\n",L"����",MB_OK);
		return -1;
	}
	//����
	TurnOnProcMon=1;
	ProcListBox.AddString(L"���̼�سɹ�����");
	//ѭ��
//	while (1) 
	{

		while (::WaitForSingleObject(hProcessEvent, INFINITE)==WAIT_OBJECT_0)
		{
			DWORD	dwRet;
			BOOL	bRet;

			bRet = ::DeviceIoControl(
								hDriver,
								IOCTL_NTPROCDRV_GET_PROCINFO,
								NULL,
								0,
								&cbkinfo,
								sizeof(cbkinfo)+64,
								&dwRet,
								NULL);
			if (bRet)
			{
				if (//��ֹ�ظ�����
					cbkinfo.hParentId != cbktemp.hParentId || \
					cbkinfo.hProcessId != cbktemp.hProcessId || \
					cbkinfo.bCreate != cbktemp.bCreate
					)
				{
					if (cbkinfo.bCreate)//����Ǵ���
					{

//						MessageBox(NULL,cbkinfo.ParentProcFullPath,L"����",MB_OK);

						
						DWORD num;


						//����ת��WCHAR	   	cbkinfo.ProcFullPath  to  	wword			
						num= MultiByteToWideChar(CP_ACP,0,cbkinfo.ProcFullPath,-1,NULL,0); //�������GB2312ʵ���м��������
						WCHAR *wProcFullPath;//����һ��UNICODE��ָ��
						wProcFullPath= (WCHAR*)calloc(num,sizeof(WCHAR));//��̬������ռ����
						if(wProcFullPath == NULL) free(wProcFullPath);
						memset(wProcFullPath, 0, num*sizeof(WCHAR));//��ʼ������
						MultiByteToWideChar(CP_ACP,0, cbkinfo.ProcFullPath, -1, wProcFullPath, num); 
						
						CString wprocname;
						wprocname.Format(L"%s",wProcFullPath);
						int n=wprocname.ReverseFind(L'\\');
						wprocname=wprocname.Right(wprocname.GetLength()-n-1);


						//����ת��WCHAR	   	wParentProcFullPath  to  wProcFullPath			
						num= MultiByteToWideChar(CP_ACP,0,cbkinfo.ParentProcFullPath,-1,NULL,0); //�������GB2312ʵ���м��������
						WCHAR *wParentProcFullPath;//����һ��UNICODE��ָ��
						wParentProcFullPath= (WCHAR*)calloc(num,sizeof(WCHAR));//��̬������ռ����
						if(wParentProcFullPath == NULL) free(wProcFullPath);
						memset(wParentProcFullPath, 0, num*sizeof(WCHAR));//��ʼ������
						MultiByteToWideChar(CP_ACP,0, cbkinfo.ParentProcFullPath, -1, wParentProcFullPath, num); 


						ProcWnd.ParentPid.Format(L"%d",cbkinfo.hParentId);
						ProcWnd.ParentPath.Format(L"%s",wParentProcFullPath);
						ProcWnd.NewProcessPid.Format(L"%d",cbkinfo.hProcessId);
						ProcWnd.NewProcessPath.Format(L"%s",wProcFullPath);
						pid = (DWORD)cbkinfo.hProcessId;

					
						if (TurnOnProcMon==1) //�����ؿ���
						{
							int isfind=0;//��ʼδ�ҵ�
							for (iter=List.begin();iter!=List.end();iter++   )  
							{
								if (wcsstr((*iter).lpFilePath[2],wProcFullPath)!=NULL)
								{
									isfind=1;
									break;
								}
							}

							if (isfind==1) //����м�¼
							{ 
								//0Ϊ��ͨ��,1Ϊһֱ������
								if (wcscmp((*iter).lpFilePath[0],L"0")==0)//���Ϊ0
								{	
									KillPro(pid);//�ɵ�
								}
								if (wcscmp((*iter).lpFilePath[0],L"1")==0)//���Ϊ1
								{	
									//ʲôҲ����
								}

							} 
							else //���û�м�¼,�򵯴�
							{ 
								SuspendPro(pid);//�������

										
	/*						
								CHttpClient   conn;   
								CString   value=L"http://127.0.0.1:801/f.php?procpath=";
								value=value+wprocname;
								CString f;
								CString b;
								f=conn.doGet(value);//ȡֵ
								char * p = (char *)f.GetBuffer();
								DWORD num= MultiByteToWideChar(CP_ACP,0,p,-1,NULL,0); 
								WCHAR *wword;//����һ��UNICODE��ָ��
								wword= (WCHAR*)calloc(num,sizeof(WCHAR));//��̬������ռ����
								memset(wword, 0, num*sizeof(WCHAR));       //��ʼ������
								MultiByteToWideChar(CP_ACP,0, p, -1, wword, num); 

								b.Format(L"%s",wword);
								ProcWnd.procdescribe.Format(L"%s",wword);
	*/
								//ProcWnd.UpdateData(true);
								ProcWnd.procdescribe.Format(L"������������԰棬��֧��������ȡ������Ϣ��������ѡ����У�");
								ProcWnd.DoModal();//����
							} 					
							isfind=0;//�ָ�ȫ�ֱ���
							
						}
						//��ӵ�listbox
						CString procstrtemp;
						procstrtemp.Format(L"����PID%d:(%S)",cbkinfo.hParentId,cbkinfo.ParentProcFullPath);
						ProcListBox.AddString(procstrtemp);
						procstrtemp.Format(L"  �½��̴���PID:%d:(%S)",cbkinfo.hProcessId,cbkinfo.ProcFullPath);
						ProcListBox.AddString(procstrtemp);
						ProcListBox.AddString(L"");
					} 
					else
					{
						CString procstrtemp;
						procstrtemp.Format(L"���̽���PID:%d:(%S)",cbkinfo.hProcessId,cbkinfo.ProcFullPath);
						ProcListBox.AddString(procstrtemp);
						ProcListBox.AddString(L"");
					}

					cbktemp = cbkinfo;
				}
			} 
			else
			{
				CString procstrtemp;
				procstrtemp.Format(L"��ȡ������Ϣʧ��");
				ProcListBox.AddString(procstrtemp);
				break;
			}
		}
	}
	::CloseHandle(hDriver);

	return 0;

}


UINT ThreadProc2(LPVOID lpParam)
{
	
//	MessageBox(NULL,L"�����߳�",L"�߳�1",MB_OK);
	int ret=ProcMon2();
	return 0;
}

void CMyPage2::OnBnClickedCheck1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CMyPage2::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TurnOnProcMon=1;
	ProcListBox.AddString(L"���̼�سɹ�����");
	//HANDLE hProc=AfxBeginThread(ThreadProc2,&lp2);
}

void CMyPage2::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ProcListBox.AddString(L"������Ӳ���");
}

void CMyPage2::OnBnClickedButton2()
{
	TurnOnProcMon=0;
	DWORD	dwRet;
	BOOL	bRet;
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
		::MessageBox(NULL,L"������ʧ��,�밲װ����������!\n",L"����",MB_OK);
		return ;
	}

	bRet = ::DeviceIoControl(
						hDriver,
						IOCTL_NTPROCDRV_TURNOFF_PROCMON,
						NULL,
						0,
						&cbkinfo,
						sizeof(cbkinfo)+64,
						&dwRet,
						NULL);
	if (bRet)
	{
		CString procstrtemp;
		procstrtemp.Format(L"�ɹ��رս��̼��");
		ProcListBox.AddString(procstrtemp);		
	} 
	else
	{
		CString procstrtemp;
		procstrtemp.Format(L"�رս��̼��ʧ��");
		ProcListBox.AddString(procstrtemp);
	}

	::CloseHandle(hDriver);

	return ;
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CMyPage2::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ProcListBox.ResetContent();
}

void CMyPage2::OnLbnSelchangeList1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

BOOL CMyPage2::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	ProcRull.InsertColumn ( 0, L"ID",LVCFMT_LEFT, 30 ) ;
	ProcRull.InsertColumn ( 1, L"�Ƿ�ͨ��",LVCFMT_CENTER, 80 ) ;
	ProcRull.InsertColumn ( 2, L"��������",LVCFMT_CENTER, 100 ) ;
	ProcRull.InsertColumn ( 3, L"����·��",LVCFMT_LEFT, 500 ) ;
	ProcRull.InsertColumn ( 4, L"������·��",LVCFMT_LEFT, 500 ) ;
	ProcRull.SetExtendedStyle(     LVS_EX_FULLROWSELECT   |   LVS_EX_GRIDLINES   |LVS_EX_HEADERDRAGDROP   |   LVS_EX_ONECLICKACTIVATE     );   



	this->ProcRull.SetExtendedStyle ( this->ProcRull.GetExtendedStyle()|LVS_EX_FULLROWSELECT) ;

	this->db.Open ( L"rulls.mdb", FALSE, FALSE, NULL ) ;
	this->OnBnClickedFlush () ;


	//�����߳�
	HANDLE hProc=AfxBeginThread(ThreadProc2,&lp2);
	return TRUE;  // return TRUE unless you set the focus to a control

	// �쳣: OCX ����ҳӦ���� FALSE
}

void CMyPage2::OnBnClickedFlush(void)
{
	// ������ݿ��Ƿ�������
	if ( this->db.IsOpen () == FALSE )
	{
		this->MessageBox ( L"δ�������ݿ⣡" ) ;
		return ;
	}

	// ɾ���б�ؼ����м�¼
	this->ProcRull.DeleteAllItems () ;
	List.clear();
	// �����¼�����󣬲��򿪼�¼��
	CDaoRecordset	Record ( &this->db ) ;
	Record.Open ( dbOpenDynaset, L"SELECT * FROM procrull" ) ;

	UINT		nIndex = 0 ;
	CString		TempStr ;
	COleVariant	OleVariant ;

	// �ƶ�����һ����¼
	Record.MoveFirst () ;
	while( !Record.IsEOF() )//ȫ���г���
	{
		// ���б�ؼ���Ӽ�¼
		TempStr.Format ( L"%d", nIndex+1 ) ;
		this->ProcRull.InsertItem ( nIndex, TempStr ) ;

		for ( int i = 1; i <= 4; i++ )
		{//һ������4��
			Record.GetFieldValue ( i, OleVariant ) ;
			this->ProcRull.SetItemText ( nIndex, i, OleVariant.bstrVal ) ;
			wcscpy(pi.lpFilePath[i-1],OleVariant.bstrVal);
		}
		//�������,�ɹ�
		List.push_back(pi);


//		Record.GetFieldValue ( 4, OleVariant ) ;
//		TempStr.Format ( L"%d", OleVariant.uintVal ) ;
//		this->ProcRull.SetItemText ( nIndex, 4, TempStr ) ;

		// �Ƶ���һ����¼
		Record.MoveNext () ;
		nIndex++ ;
	}
	// �رռ�¼��
	Record.Close () ;

}




void CMyPage2::OnBnClickedButton5()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// ������ݿ��Ƿ�������
	if ( this->db.IsOpen () == FALSE )
	{
		this->MessageBox ( L"δ�������ݿ⣡" ) ;
		return ;
	}

	// �����¼�����󣬴򿪼�¼��
	CDaoRecordset Record ( &this->db ) ;
	Record.Open ( dbOpenDynaset, L"SELECT * FROM procrull" ) ;

	// ����Ի�����󣬲�����Ϊ����ӡ�ģʽ
	AddProcRull AddBookDlg ;


	if ( AddBookDlg.DoModal() == IDOK )
	{
		// ʹ��CDaoRecordset::AddNew��������Ҫ��ʼ����¼�¼
		Record.AddNew () ;
		// �����¼�¼
		Record.SetFieldValue ( 1, AddBookDlg.pass.GetBuffer() ) ;
		Record.SetFieldValue ( 2, AddBookDlg.parentprocname.GetBuffer() ) ;
		Record.SetFieldValue ( 3, AddBookDlg.procpath.GetBuffer() ) ;
		Record.SetFieldValue ( 4, AddBookDlg.parentprocpath.GetBuffer() ) ;
		// ֻ����ִ��CDaoRecordset::Update������Ż�д�뵽���ݿ�
		Record.Update () ;
		// �رռ�¼��
		Record.Close () ;
		// ˢ�½����б�ؼ�������ʾ
		this->OnBnClickedFlush () ;
	}


}



void CMyPage2::OnBnClickedButton6()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if ( this->db.IsOpen () == FALSE )
	{
		this->MessageBox ( L"δ�������ݿ⣡" ) ;
		return ;
	}

	// ����Ƿ���ѡ��
	int nCurSel = this->ProcRull.GetNextItem ( -1,LVNI_SELECTED) ;
	if ( nCurSel == -1 )
	{
		this->MessageBox ( L"û��ѡ��" ) ;
		return ;
	}

	// �����¼�����󣬲��ƶ�ָ�뵽ָ��λ��
	CDaoRecordset Record ( &this->db ) ;
	Record.Open ( dbOpenDynaset, L"SELECT * FROM procrull" ) ;
	Record.Move ( nCurSel ) ;
	// ɾ����ǰ��¼
	Record.Delete () ;
	// �رռ�¼��
	Record.Close () ;
	// ˢ�½����б�ؼ�������ʾ
	this->OnBnClickedFlush () ;

}



void CMyPage2::OnBnClickedButton8()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//�˲������ݿ�
	CDaoRecordset Record ( &this->db ) ;

	Record.Open ( dbOpenDynaset, L"SELECT Author FROM procrull WHERE BookName='���������'" ) ;
	COleVariant	OleVariant ;

	Record.GetFieldValue ( 0, OleVariant ) ;
	this->MessageBox(OleVariant.bstrVal);

}

void CMyPage2::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CPropertyPage::OnPaint()
}
