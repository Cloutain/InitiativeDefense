// Dialog3.cpp : ʵ���ļ�
// Download by http://www.51xue8xue8.com

#include "stdafx.h"
#include "Dialog3.h"


// CDialog3 �Ի���

IMPLEMENT_DYNAMIC(CDialog3, CDialog)

CDialog3::CDialog3(CWnd* pParent /*=NULL*/)
	: CDialog(CDialog3::IDD, pParent)
{

}

CDialog3::~CDialog3()
{
}

void CDialog3::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialog3, CDialog)
END_MESSAGE_MAP()


// CDialog3 ��Ϣ�������
