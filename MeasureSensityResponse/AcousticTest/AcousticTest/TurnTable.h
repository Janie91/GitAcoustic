#pragma once
#include "mscomm.h"


// CTurnTable 对话框

class CTurnTable : public CDialog
{
	DECLARE_DYNAMIC(CTurnTable)

public:
	CTurnTable(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTurnTable();

// 对话框数据
	enum { IDD = IDD_turntable };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CMscomm m_mscom;
	CString m_CurrentAngle;
	int m_Speed;
	int m_TargetAngle;
	void SetManual();
	float ReadCurrentAngle();
	void SetSpeed(int speed);
	void StopRotateRight();
	void StopRotateLeft();
	void StopRotate();
	void CancelCurrentZero();
	void SetCurrentPosZero();
	void StopRotateBacktoZero();
	void RotateBacktoZero();
	void SetAuto();
	void SetTargetAngle(int targetangle);
	void PositionStart();

	void RotateRight();
	void RotateLeft();
	void ReturnZero();	
	void SetZero();
	void RotateTargetAngle(int targetangle);

	afx_msg void OnBnClickedquit();
	afx_msg void OnBnClickedRotateright();
	afx_msg void OnBnClickedRotateleft();
	afx_msg void OnBnClickedSetzero();
	afx_msg void OnBnClickedReturnzero();
	afx_msg void OnBnClickedRotatesetangle();
	afx_msg void OnBnClickedStoprotate();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	int m_StartAngle;
	int m_EndAngle;
	afx_msg void OnPaint();
};
