#pragma once
#include "resource.h"
// Download by http://www.codefans.net
// AddRegRull �Ի���

class AddRegRull : public CDialog
{
	DECLARE_DYNAMIC(AddRegRull)

public:
	AddRegRull(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~AddRegRull();

// �Ի�������
	enum { IDD = IDD_DIALOG4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString pass;
	CString procname;
	CString procpath;
	CString regpath;
};
