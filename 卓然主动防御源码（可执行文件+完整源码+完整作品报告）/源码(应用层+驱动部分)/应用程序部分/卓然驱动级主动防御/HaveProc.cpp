// HaveProc.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "main.h"
#include "HaveProc.h"
#include "ioctls.h"
extern DWORD pid;//����ȫ�ֱ���PID
// HaveProc �Ի���
#include "HttpClient.h"

extern CListBox ProcListBox;

IMPLEMENT_DYNAMIC(HaveProc, CDialog)

HaveProc::HaveProc(CWnd* pParent /*=NULL*/)
	: CDialog(HaveProc::IDD, pParent)
	, ParentPid(_T(""))
	, ParentPath(_T(""))
	, NewProcessPid(_T(""))
	, NewProcessPath(_T(""))
	, procdescribe(_T(""))
{

}

HaveProc::~HaveProc()
{
}

void HaveProc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, ParentPid);
	DDX_Text(pDX, IDC_EDIT2, ParentPath);
	DDX_Text(pDX, IDC_EDIT3, NewProcessPid);
	DDX_Text(pDX, IDC_EDIT4, NewProcessPath);
	DDX_Text(pDX, IDC_EDIT5, procdescribe);
}


BEGIN_MESSAGE_MAP(HaveProc, CDialog)
	ON_EN_CHANGE(IDC_EDIT4, &HaveProc::OnEnChangeEdit4)
	ON_EN_CHANGE(IDC_EDIT1, &HaveProc::OnEnChangeEdit1)
	ON_BN_CLICKED(IDOK, &HaveProc::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &HaveProc::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &HaveProc::OnBnClickedButton1)
	ON_EN_CHANGE(IDC_EDIT3, &HaveProc::OnEnChangeEdit3)

	ON_BN_CLICKED(IDOK2, &HaveProc::OnBnClickedOk2)
	ON_BN_CLICKED(IDCANCEL2, &HaveProc::OnBnClickedCancel2)
END_MESSAGE_MAP()






// HaveProc ��Ϣ�������

void HaveProc::OnEnChangeEdit4()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}

void HaveProc::OnEnChangeEdit1()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}

void HaveProc::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ResumePro(pid);//�ָ�
	ProcListBox.AddString(L"�û�ȷ��,һֱ����ý��̿���");
	OnOK();
}

void HaveProc::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	KillPro(pid);//ɱ��
	ProcListBox.AddString(L"�û�ȷ��,һֱ��ֹ�ý��̿���");
	OnCancel();
}

void HaveProc::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strtemp;
	strtemp.Format(L"����ID:%d",pid);
	this->MessageBox(strtemp);
	ResumePro(pid);

}

void HaveProc::OnEnChangeEdit3()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}

void HaveProc::OnBnClickedOk2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ResumePro(pid);//�ָ�
	ProcListBox.AddString(L"�û�ȷ��,����ý��̿���");
	OnOK();
}

void HaveProc::OnBnClickedCancel2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	KillPro(pid);//ɱ��
	ProcListBox.AddString(L"�û�ȷ��,��ֹ�ý��̿���");
	OnCancel();
}

BOOL HaveProc::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

//	this->MessageBox(b);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
