#pragma once


// CSetConditionDlg dialog

class CSetConditionDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetConditionDlg)

public:
	CSetConditionDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetConditionDlg();

// Dialog Data
	enum { IDD = IDD_SetCondition };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedConditionok();
	float m_d1;
	float m_d2;
	float m_d3;
	float m_d4;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedopenfile();
//	CString m_StandFile;
	int m_Ratio;
	int m_gain1;
	int m_gain2;
	int m_gain3;
	int m_gain4;
	virtual BOOL OnInitDialog();
};
