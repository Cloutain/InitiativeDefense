#pragma once
#include "resource.h"

// AddProcRull �Ի���

class AddProcRull : public CDialog
{
	DECLARE_DYNAMIC(AddProcRull)

public:
	AddProcRull(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~AddProcRull();

// �Ի�������
	enum { IDD = IDD_DIALOG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString pass;
	CString parentprocname;
	CString procpath;
	CString parentprocpath;
};
