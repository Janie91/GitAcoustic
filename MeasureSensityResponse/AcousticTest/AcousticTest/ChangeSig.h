#pragma once


// CChangeSig dialog

class CChangeSig : public CDialogEx
{
	DECLARE_DYNAMIC(CChangeSig)

public:
	CChangeSig(CWnd* pParent = NULL);   // standard constructor
	virtual ~CChangeSig();

// Dialog Data
	enum { IDD = IDD_ChangeSig };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	float m_f;
	float m_p;
	float m_v;
	float m_w;
	afx_msg void OnBnClickedOk();
	virtual void OnCancel();
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedCancel();
};
