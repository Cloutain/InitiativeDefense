#pragma once


// CSetReg �Ի���

class CSetReg : public CDialog
{
	DECLARE_DYNAMIC(CSetReg)

public:
	CSetReg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSetReg();

// �Ի�������
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString processname;
	CString processpath;
	CString regname;
	CString regvalue;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	int TurnOffRegMon(void);
	afx_msg void OnBnClickedOk2();
	afx_msg void OnBnClickedCancel2();
	CString regdescribe;
};
