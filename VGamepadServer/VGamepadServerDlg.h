
// VGamepadServerDlg.h : header file
//

#pragma once
#include "Controller.h"


// CVGamepadServerDlg dialog
class CVGamepadServerDlg : public CDialogEx
{
// Construction
public:
	CVGamepadServerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VGAMEPADSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	int m_port;
	CController controller;
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	
};
