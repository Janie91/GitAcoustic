#pragma once


// CSetScopDlg dialog

class CSetScopDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetScopDlg)

public:
	CSetScopDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetScopDlg();

// Dialog Data
	enum { IDD = IDD_SetScop };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_ch1;
	CButton m_ch2;
	CButton m_ch3;
	CButton m_ch4;
	CComboBox m_chRefer;
	afx_msg void OnBnClickedScopok();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
};
