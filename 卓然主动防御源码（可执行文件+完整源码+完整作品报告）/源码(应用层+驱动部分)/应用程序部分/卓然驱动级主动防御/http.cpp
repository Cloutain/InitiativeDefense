// http.cpp : ����Ӧ�ó��������Ϊ��
// Download by http://www.51xue8xue8.com

#include "stdafx.h"
#include "http.h"
#include "MainFrm.h"

#include "httpDoc.h"
#include "httpView.h"
#include "HttpClient.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ChttpApp

BEGIN_MESSAGE_MAP(ChttpApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &ChttpApp::OnAppAbout)
	// �����ļ��ı�׼�ĵ�����
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// ��׼��ӡ��������
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// ChttpApp ����

ChttpApp::ChttpApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� ChttpApp ����

ChttpApp theApp;


// ChttpApp ��ʼ��

BOOL ChttpApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// ��ʼ�� OLE ��
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));
	LoadStdProfileSettings(4);  // ���ر�׼ INI �ļ�ѡ��(���� MRU)
	// ע��Ӧ�ó�����ĵ�ģ�塣�ĵ�ģ��
	// �������ĵ�����ܴ��ں���ͼ֮�������
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(ChttpDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ��ܴ���
		RUNTIME_CLASS(ChttpView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);



	// ������׼������DDE�����ļ�������������
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// ��������������ָ����������
	// �� /RegServer��/Register��/Unregserver �� /Unregister ����Ӧ�ó����򷵻� FALSE��
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Ψһ��һ�������ѳ�ʼ���������ʾ����������и���
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// �������к�׺ʱ�ŵ��� DragAcceptFiles
	//  �� SDI Ӧ�ó����У���Ӧ�� ProcessShellCommand ֮����
	return TRUE;
}



// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CAboutDlg::OnBnClickedButton1)
END_MESSAGE_MAP()

// �������жԻ����Ӧ�ó�������
void ChttpApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// ChttpApp ��Ϣ�������


void CAboutDlg::OnBnClickedButton1()
{

/*
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CHttpClient   conn;   
  CString   value=L"http://127.0.0.1:801/f.php";   
  CStringA f;
  CString b;
  f=conn.doGet(value);
//  b.Format("%s",f);
  ::MessageBoxA(NULL,f,"��",0);
//					this->MessageBox(b);
*/


	//�ÿؼ��Ͷ�Ӧ�ı���֮��������ݽ��������ڽ��ؼ����ݴ�����Ӧ����


//����ҳ���ݵ�Դ��������༭��

CHttpClient   conn;   
CString   value=L"http://127.0.0.1:801/f.php";   
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
this->MessageBox(b);

return ;





//myHttpFile->Close ;
//mySession.Close ;

}
