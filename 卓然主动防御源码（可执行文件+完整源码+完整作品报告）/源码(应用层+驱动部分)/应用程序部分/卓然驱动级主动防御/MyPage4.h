#pragma once


// CMyPage4 �Ի���

class CMyPage4 : public CPropertyPage
{
	DECLARE_DYNAMIC(CMyPage4)

public:
	CMyPage4();
	virtual ~CMyPage4();

// �Ի�������
	enum { IDD = IDD_PROPPAGE_4 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	long data1;
	long data2;
	long data3;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton3();
};
