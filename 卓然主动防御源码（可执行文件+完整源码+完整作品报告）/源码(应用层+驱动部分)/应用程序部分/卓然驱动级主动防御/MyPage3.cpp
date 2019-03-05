// MyPage3.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "main.h"
#include "MyPage3.h"

#include "SetReg.h"
#include "regioctl.h"
#include "winioctl.h"

#include "HttpClient.h"
#define SYMBOL_LINK     L"\\\\.\\RegMon"
#define REG_EVENT_NAME L"RegEvent"
#define APP_EVENT_NAME L"RegAppEvent" //Ӧ�ò��Ƿ��¼�

CSetReg RegWnd;//��������
int reglp=0;
CListBox reglistbox;



REG_CALLBACK_INFO reginfo={0}, regtemp = {0};
// CMyPage3 �Ի���


//����-------------------------------
#include <list>
#include <string>
using   namespace   std; 

typedef struct _REG_INFO{
    WCHAR lpFilePath[4][256];//4��256�����ִ�
}REG_INFO, *PREG_INFO;

typedef list<REG_INFO> REGRullList;
typedef REGRullList::iterator REGITERATOR;

REGRullList RegRullList;
REG_INFO ri;
REGITERATOR regiter = RegRullList.begin();
//���н���-------------------------------


//���ݿ�---------------------------------



#include "AddRegRull.h"


int RegMon(void)
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
		MessageBox(NULL,L"�������豸����ʧ�ܣ�\n",L"����",MB_OK);
		return -1;
	}

	HANDLE hProcessEvent = ::OpenEventW(SYNCHRONIZE, FALSE, REG_EVENT_NAME);
	if (hProcessEvent == NULL)
	{
		MessageBox(NULL,L"���¼�ʧ�ܣ�\n",L"����",MB_OK);
		return -1;
	}

	while (::WaitForSingleObject(hProcessEvent, INFINITE)==WAIT_OBJECT_0)
	{
		DWORD	dwRet;
		BOOL	bRet;

		bRet = ::DeviceIoControl(
							hDriver,
							IOCTL_NTPROCDRV_GET_REGINFO,
							NULL,
							0,
							&reginfo,
							sizeof(reginfo)+64,
							&dwRet,
							NULL);

		if (bRet)
		{

						
			DWORD num;
			WCHAR *wProcName;//����һ��UNICODE��ָ��
			WCHAR *wProcFullPath;//����һ��UNICODE��ָ��
			WCHAR *wInfo1;//����һ��UNICODE��ָ��
			WCHAR *wInfo2;//����һ��UNICODE��ָ��


			//����ת��WCHAR	   	cbkinfo.ProcFullPath  to  	wword			
			num= MultiByteToWideChar(CP_ACP,0,reginfo.ProcName,-1,NULL,0); //�������GB2312ʵ���м��������
			wProcName= (WCHAR*)calloc(num,sizeof(WCHAR));//��̬������ռ����
			if(wProcName == NULL) free(wProcName);
			memset(wProcName, 0, num*sizeof(WCHAR));//��ʼ������
			MultiByteToWideChar(CP_ACP,0, reginfo.ProcName, -1, wProcName, num); 
			

			//����ת��WCHAR	   	wParentProcFullPath  to  wProcFullPath			
			num= MultiByteToWideChar(CP_ACP,0,reginfo.ProcFullPath,-1,NULL,0); //�������GB2312ʵ���м��������
			wProcFullPath= (WCHAR*)calloc(num,sizeof(WCHAR));//��̬������ռ����
			if(wProcFullPath == NULL) free(wProcFullPath);
			memset(wProcFullPath, 0, num*sizeof(WCHAR));//��ʼ������
			MultiByteToWideChar(CP_ACP,0, reginfo.ProcFullPath, -1, wProcFullPath, num); 

			//����ת��WCHAR	   	wParentProcFullPath  to  wProcFullPath			
			num= MultiByteToWideChar(CP_ACP,0,reginfo.Info1,-1,NULL,0); //�������GB2312ʵ���м��������
			wInfo1= (WCHAR*)calloc(num,sizeof(WCHAR));//��̬������ռ����
			if(wInfo1 == NULL) free(wInfo1);
			memset(wInfo1, 0, num*sizeof(WCHAR));//��ʼ������
			MultiByteToWideChar(CP_ACP,0, reginfo.Info1, -1, wInfo1, num); 

			//����ת��WCHAR	   	wParentProcFullPath  to  wProcFullPath			
			num= MultiByteToWideChar(CP_ACP,0,reginfo.Info2,-1,NULL,0); //�������GB2312ʵ���м��������
			wInfo2= (WCHAR*)calloc(num,sizeof(WCHAR));//��̬������ռ����
			if(wInfo2 == NULL) free(wInfo2);
			memset(wInfo2, 0, num*sizeof(WCHAR));//��ʼ������
			MultiByteToWideChar(CP_ACP,0, reginfo.Info2, -1, wInfo2, num); 


			RegWnd.processname.Format(L"%s",wProcName);
			RegWnd.processpath.Format(L"%s",wProcFullPath);
			RegWnd.regname.Format(L"%s",wInfo1);
			RegWnd.regvalue.Format(L"%s",wInfo2);
			CString strtemp;


			strtemp.Format (L"����%s(%s)",wProcName,wProcFullPath);
			reglistbox.AddString(strtemp);
  
			reglistbox.AddString(L"  �޸�ע���:");

			strtemp.Format(L"    ·��%s",wInfo2);
			reglistbox.AddString(strtemp);

			strtemp.Format(L"    ��ֵ%s",wInfo1);
			reglistbox.AddString(strtemp);


			regtemp = reginfo;




			//C:\Program Files\Thunder Network\Thunder\Program\Thunder5.exe
			int isfind=0;//��ʼδ�ҵ�
			for (regiter=RegRullList.begin();regiter!=RegRullList.end();regiter++   )  
			{
				if (wcsstr((*regiter).lpFilePath[2],wProcFullPath)!=NULL)
				{
					isfind=1;
					break;
				}
			}

			if (isfind==1) //����м�¼
			{ 
				//0Ϊ��ͨ��,1Ϊһֱ������
				if (wcscmp((*regiter).lpFilePath[0],L"1")==0)//���Ϊ1
				{	
					REG_CALLBACK_INFO reginfo1={0}, regtemp1 = {0};
					DWORD	dwRet;
					BOOL	bRet;
					HANDLE hDriver = ::CreateFile(SYMBOL_LINK,GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
					if (hDriver == INVALID_HANDLE_VALUE)
					{
						::MessageBox(NULL,L"�������豸����ʧ�ܣ�\n",L"����",MB_OK);
					}

					HANDLE hProcessEvent = ::OpenEventW(SYNCHRONIZE, FALSE, APP_EVENT_NAME);
					if (hProcessEvent == NULL)
					{
						::MessageBox(NULL,L"���¼�ʧ�ܣ�\n",L"����",MB_OK);
					}
					//IOCTL_NTPROCDRV_SET_APPEVENT_OK
					bRet = ::DeviceIoControl(hDriver,IOCTL_NTPROCDRV_SET_APPEVENT_OK,NULL,0,&reginfo1,sizeof(reginfo1)+64,&dwRet,NULL);
					if (bRet)
					{
						reglistbox.AddString(L"    �û�ȷ��,��ͬ��һֱ����!");
					} 
					::CloseHandle(hDriver);
				}
				else if (wcscmp((*regiter).lpFilePath[0],L"0")==0)//���Ϊ0
				{	
					REG_CALLBACK_INFO reginfo1={0}, regtemp1 = {0};
					DWORD	dwRet;
					BOOL	bRet;
					HANDLE hDriver = ::CreateFile(SYMBOL_LINK,GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
					if (hDriver == INVALID_HANDLE_VALUE)
					{
						::MessageBox(NULL,L"�������豸����ʧ�ܣ�\n",L"����",MB_OK);
					}
				
					HANDLE hProcessEvent = ::OpenEventW(SYNCHRONIZE, FALSE, APP_EVENT_NAME);
					if (hProcessEvent == NULL)
					{
						::MessageBox(NULL,L"���¼�ʧ�ܣ�\n",L"����",MB_OK);
					}
					//IOCTL_NTPROCDRV_SET_APPEVENT_NO
					bRet = ::DeviceIoControl(hDriver,IOCTL_NTPROCDRV_SET_APPEVENT_NO,NULL,0,&reginfo1,sizeof(reginfo1)+64,&dwRet,NULL);
					if (bRet)
					{
						reglistbox.AddString(L"    �û�ȷ��,һֱ�ܾ�!");
					} 
					::CloseHandle(hDriver);
				}
				else 
					
				{		
/*
					CHttpClient   conn;   
					CString   value=L"http://127.0.0.1:801/f.php?procpath=";
					value=value+wProcName;
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
					RegWnd.regdescribe.Format(L"%s",wword);
//					::MessageBox(NULL,RegWnd.regdescribe,L"��",0);
*/
					RegWnd.regdescribe.Format(L"������������԰棬��֧��������ȡ������Ϣ��������ѡ����У�");
					RegWnd.DoModal();
				}

} 
else //���û�м�¼,�򵯴�
{ 
	/*
					CHttpClient   conn;   
					CString   value=L"http://127.0.0.1:801/f.php?procpath=";
					value=value+wProcName;
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
					RegWnd.regdescribe.Format(L"%s",wword);
					::MessageBox(NULL,RegWnd.regdescribe,L"��",0);
		*/
				RegWnd.regdescribe.Format(L"������������԰棬��֧��������ȡ������Ϣ��������ѡ����У�");
				RegWnd.DoModal();
} 					
isfind=0;//�ָ�ȫ�ֱ���





		} 
		else
		{
			break;
		}
		
	}

	return 0;
}




IMPLEMENT_DYNAMIC(CMyPage3, CPropertyPage)

CMyPage3::CMyPage3()
	: CPropertyPage(CMyPage3::IDD)
{

}

CMyPage3::~CMyPage3()
{
}

void CMyPage3::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, reglistbox);
	DDX_Control(pDX, IDC_LIST3, RegRull);
}


BEGIN_MESSAGE_MAP(CMyPage3, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON1, &CMyPage3::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMyPage3::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CMyPage3::OnBnClickedButton3)
	ON_LBN_SELCHANGE(IDC_LIST1, &CMyPage3::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_BUTTON5, &CMyPage3::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CMyPage3::OnBnClickedButton6)
END_MESSAGE_MAP()


// CMyPage3 ��Ϣ�������

UINT ThreadProcRegMon1(LPVOID lpParam)
{

	int ret=RegMon();
	return 0;
}




void CMyPage3::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//���߳�
//	HANDLE hProc=AfxBeginThread(ThreadProcRegMon1,&reglp);


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
		::MessageBox(NULL,L"�������豸����ʧ�ܣ�\n",L"����",MB_OK);
		return;
	}
	DWORD	dwRet;
	BOOL	bRet;
	bRet = ::DeviceIoControl(
							hDriver,
							IOCTL_NTPROCDRV_TURNON_REGMON,
							NULL,
							0,
							&reginfo,
							sizeof(reginfo)+64,
							&dwRet,
							NULL);
	if (bRet)
	{
		reglistbox.AddString(L"ע����سɹ�����!");
	} 
	else
	{
		::MessageBox(NULL,L"��ؿ���ʧ��",L"ע���",0);
		return;
	}
	return;
}



void CMyPage3::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//ֹͣ���
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
		::MessageBox(NULL,L"�������豸����ʧ�ܣ�\n",L"����",MB_OK);
		return;
	}
	DWORD	dwRet;
	BOOL	bRet;
	bRet = ::DeviceIoControl(
							hDriver,
							IOCTL_NTPROCDRV_TURNOFF_REGMON,
							NULL,
							0,
							&reginfo,
							sizeof(reginfo)+64,
							&dwRet,
							NULL);
	if (bRet)
	{
		reglistbox.AddString(L"ע�����ֹͣ�ɹ�!");
	} 
	else
	{
		::MessageBox(NULL,L"���ֹͣʧ��",L"ע���",0);
		return;
	}
	return;
}

void CMyPage3::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	reglistbox.ResetContent();
}

void CMyPage3::OnLbnSelchangeList1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}





void CMyPage3::SetListBoxHScroll(void)
{

CDC* dc = GetDC();
SIZE s;
int i;
TCHAR str[100];
HDC hdc = dc->m_hDC; // �������豸���
for(i = 0; i < reglistbox.GetCount(); i++)
{
     reglistbox.GetText(i, str);
     GetTextExtentPoint32(hdc, str, lstrlen(str), &s); // ��ȡ�ַ��������ش�С
     // ����µ��ַ�����ȴ�����ǰ��ˮƽ��������ȣ����������ù��������
     // IDC_LISTBOXΪm_reglistboxBox����ԴID
     if (s.cx > (LONG)SendDlgItemMessage(IDC_LIST1, LB_GETHORIZONTALEXTENT, 0, 0))
      SendDlgItemMessage(IDC_LIST1, LB_SETHORIZONTALEXTENT, (WPARAM)s.cx, 0);
}
ReleaseDC(dc);
}

BOOL CMyPage3::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	RegRull.InsertColumn ( 0, L"ID",LVCFMT_LEFT, 30 ) ;
	RegRull.InsertColumn ( 1, L"�Ƿ�ͨ��",LVCFMT_CENTER, 80 ) ;
	RegRull.InsertColumn ( 2, L"������",LVCFMT_CENTER, 100 ) ;
	RegRull.InsertColumn ( 3, L"����·��",LVCFMT_LEFT, 500 ) ;
	RegRull.InsertColumn ( 4, L"ע���·��",LVCFMT_LEFT, 500 ) ;
	RegRull.SetExtendedStyle(     LVS_EX_FULLROWSELECT   |   LVS_EX_GRIDLINES   |LVS_EX_HEADERDRAGDROP   |   LVS_EX_ONECLICKACTIVATE     );   

	this->db.Open ( L"rulls.mdb", FALSE, FALSE, NULL ) ;
	this->OnBnClickedFlush () ;

//����
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
		return 0;
	}
	DWORD	dwRet;
	BOOL	bRet;
	bRet = ::DeviceIoControl(
							hDriver,
							IOCTL_NTPROCDRV_TURNON_REGMON,
							NULL,
							0,
							&reginfo,
							sizeof(reginfo)+64,
							&dwRet,
							NULL);
	if (bRet)
	{
		reglistbox.AddString(L"ע����سɹ�����!");
	} 
	else
	{
		::MessageBox(NULL,L"��ؿ���ʧ��",L"ע���",0);
		return 0;
	}

	HANDLE hProc=AfxBeginThread(ThreadProcRegMon1,&reglp);


	return TRUE;

	// �쳣: OCX ����ҳӦ���� FALSE
}



void CMyPage3::OnBnClickedFlush(void)
{
	// ������ݿ��Ƿ�������
	if ( this->db.IsOpen () == FALSE )
	{
		this->MessageBox ( L"δ�������ݿ⣡" ) ;
		return ;
	}

	// ɾ���б�ؼ����м�¼
	this->RegRull.DeleteAllItems () ;
	RegRullList.clear();
	// �����¼�����󣬲��򿪼�¼��
	CDaoRecordset	Record ( &this->db ) ;
	Record.Open ( dbOpenDynaset, L"SELECT * FROM regrull" ) ;

	UINT		nIndex = 0 ;
	CString		TempStr ;
	COleVariant	OleVariant ;

	// �ƶ�����һ����¼
	Record.MoveFirst () ;
	while( !Record.IsEOF() )//ȫ���г���
	{
		// ���б�ؼ���Ӽ�¼
		TempStr.Format ( L"%d", nIndex+1 ) ;
		this->RegRull.InsertItem ( nIndex, TempStr ) ;

		for ( int i = 1; i <= 4; i++ )
		{//һ������4��
			Record.GetFieldValue ( i, OleVariant ) ;
			this->RegRull.SetItemText ( nIndex, i, OleVariant.bstrVal ) ;
			wcscpy(ri.lpFilePath[i-1],OleVariant.bstrVal);
		}
		//�������,�ɹ�
		RegRullList.push_back(ri);

		// �Ƶ���һ����¼
		Record.MoveNext () ;
		nIndex++ ;
	}
	// �رռ�¼��
	Record.Close () ;

}





void CMyPage3::OnBnClickedButton5()
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
	Record.Open ( dbOpenDynaset, L"SELECT * FROM regrull" ) ;

	// ����Ի�����󣬲�����Ϊ����ӡ�ģʽ
	AddRegRull AddBookDlg ;


	if ( AddBookDlg.DoModal() == IDOK )
	{
		// ʹ��CDaoRecordset::AddNew��������Ҫ��ʼ����¼�¼
		Record.AddNew () ;
		// �����¼�¼
		Record.SetFieldValue ( 1, AddBookDlg.pass.GetBuffer() ) ;
		Record.SetFieldValue ( 2, AddBookDlg.procname.GetBuffer() ) ;
		Record.SetFieldValue ( 3, AddBookDlg.procpath.GetBuffer() ) ;
		Record.SetFieldValue ( 4, AddBookDlg.regpath.GetBuffer() ) ;
		// ֻ����ִ��CDaoRecordset::Update������Ż�д�뵽���ݿ�
		Record.Update () ;
		// �رռ�¼��
		Record.Close () ;
		// ˢ�½����б�ؼ�������ʾ
		this->OnBnClickedFlush () ;

	}


}

void CMyPage3::OnBnClickedButton6()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if ( this->db.IsOpen () == FALSE )
	{
		this->MessageBox ( L"δ�������ݿ⣡" ) ;
		return ;
	}

	// ����Ƿ���ѡ����Ŀ
	int nCurSel = this->RegRull.GetNextItem ( -1,LVNI_SELECTED) ;
	if ( nCurSel == -1 )
	{
		this->MessageBox ( L"û��ѡ����Ŀ��" ) ;
		return ;
	}

	// �����¼�����󣬲��ƶ�ָ�뵽ָ��λ��
	CDaoRecordset Record ( &this->db ) ;
	Record.Open ( dbOpenDynaset, L"SELECT * FROM regrull" ) ;
	Record.Move ( nCurSel ) ;
	// ɾ����ǰ��¼
	Record.Delete () ;
	// �رռ�¼��
	Record.Close () ;
	// ˢ�½����б�ؼ�������ʾ
	this->OnBnClickedFlush () ;
}
