// MyPage0.cpp : ʵ���ļ�
// Download by http://www.51xue8xue8.com

#include "stdafx.h"
#include "MyPage0.h"
#include "HttpClient.h"

// CMyPage0 �Ի���

IMPLEMENT_DYNAMIC(CMyPage0, CPropertyPage)

CMyPage0::CMyPage0()
	: CPropertyPage(CMyPage0::IDD)
{

}

CMyPage0::~CMyPage0()
{
}

void CMyPage0::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMyPage0, CPropertyPage)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON1, &CMyPage0::OnBnClickedButton1)
END_MESSAGE_MAP()


// CMyPage0 ��Ϣ�������


void CMyPage0::OnPaint()
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
		///��ʾ��   
		//��������ʾ��С603*380
//		dc.BitBlt(0,0,rect.Width(),rect.Height(),&MemDC,0,0,SRCCOPY);   
		dc.BitBlt(0,0,603,380,&MemDC,0,0,SRCCOPY);   
		MemDC.SelectObject(OldBitmap);
	
		CDialog::OnPaint();
	
}

void CMyPage0::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
}
