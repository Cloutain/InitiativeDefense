#pragma once
#include "MyPage0.h"
#include "MyPage1.h"
#include "MyPage2.h"
#include "MyPage3.h"
#include "MyPage4.h"
#include "MyPage5.h"

// CMySheet

class CMySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CMySheet)


	//������Ϊ���������С����
	BOOL   OnInitDialog()   
	  {   
	  CPropertySheet::OnInitDialog();   
	
	  //���������ȷ��ȡ�����߰�����ť
	GetDlgItem(   IDOK   )->ShowWindow(   SW_HIDE   );   
	GetDlgItem(   IDCANCEL   )->ShowWindow(   SW_HIDE   );   
	GetDlgItem(   ID_APPLY_NOW   )->ShowWindow(   SW_HIDE   );   
		//SetWindowTextW(L"�˳�");
	//ShowWindow(   SW_HIDE   );   
	GetDlgItem(   IDHELP   )->ShowWindow(   SW_HIDE   );   
	
	ModifyStyle(0,WS_MINIMIZEBOX,0);   
	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		pSysMenu->InsertMenu(0,MF_BYPOSITION | MF_STRING, SC_MINIMIZE, _T("��С��(&N)"));
		pSysMenu->InsertMenu(0,MF_BYPOSITION | MF_STRING, SC_RESTORE, _T("��ԭ(&R)"));
	}
	  return 0;
	}



	void CMySheet::OnSysCommand(UINT nID, LPARAM lParam) 
	{
	switch(nID)
	{
	case SC_MINIMIZE:
	ShowWindow(SW_SHOWMINIMIZED);
	return;
	}
	CPropertySheet::OnSysCommand(nID, lParam);
	}
  


public:
	CMySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CMySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	virtual ~CMySheet();

protected:

	DECLARE_MESSAGE_MAP()

private:
	CMyPage0 page0 ;
	CMyPage1 page1 ;
	CMyPage2 page2 ;
	CMyPage3 page3 ;
	CMyPage4 page4 ;
	CMyPage5 page5 ;

};




