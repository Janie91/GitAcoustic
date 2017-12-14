#pragma once


// CSelectDlg dialog

class CSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectDlg)

public:
	CSelectDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectDlg();

// Dialog Data
	enum { IDD = IDD_SelectItem };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnBnClickedselect();
	int m_Item;
	afx_msg void OnBnClickedselectquit();
	virtual BOOL OnInitDialog();
};
