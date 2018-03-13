#pragma once
#include "afxwin.h"


// CSetSigDlg dialog

class CSetSigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetSigDlg)

public:
	CSetSigDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetSigDlg();

// Dialog Data
	enum { IDD = IDD_SetSig };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
//	double m_deltaF;
	float m_endF;
	float m_deltaF;
	float m_startF;
	float m_volt;
	float m_pulseRe;
	float m_pulseWid;
//	CComboBox m_unitF1;
//	CComboBox m_unitF2;
//	CComboBox m_unitF3;
//	CComboBox m_unitS1;
//	CComboBox m_unitS2;
//	CComboBox m_unitV;
	afx_msg void OnBnClickedSigok();
	afx_msg void OnBnClickedCancel();
	CButton m_OneThirdFreq;
	virtual BOOL OnInitDialog();
	CComboBox m_StartfUnit;
//	CComboBox m_EndfUnit;
//	CComboBox m_DeltafUnit;
};
