#pragma once
#include "TurnTable.h"
#include <vector>
#include "mscomm.h"
using namespace std;
// CMeasure dialog

class CMeasure : public CDialogEx
{
	DECLARE_DYNAMIC(CMeasure)

public:
	CMeasure(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMeasure();

// Dialog Data
	enum { IDD = IDD_Measure };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSetsig();
	afx_msg void OnSetscop();
	afx_msg void OnSetcondition();
	afx_msg void OnBnClickedView();
	afx_msg void OnBackselect();
	afx_msg void OnBnClickedStartmea();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedStopmea();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedquitsys();
	afx_msg void Onsave();
	afx_msg void Onturntable();

	void renew();
	void huatu_sensity();
	void Capture(vector<int> cha,int count);
	int MeasureSensity();
	int MeasureResponse();
	void huatu_response();
	int MeasureDir();
	void huatu_recidir();
	int MeaMulDir();
	void huatu_muldir();
	int MeaHuyi();
	void huatu_huyi();
	void huatu_dB();
	
private:
	CTurnTable *pturntable;
	virtual void PostNcDestroy();
	virtual void OnCancel();

public:
	afx_msg void OnBnClickedChangesignal();
	CMscomm m_com;
//	CString m_CurrentAngle;
	void MeaSetManual();
	float MeaReadCurrentAngle();
	

	void MeaRotateRight(int speed);
	void MeaRotateLeft(int speed);
	void MeaRotateTargetAngle(int speed,int targetangle);
	void MeaStopRotateRight();
	void MeaStopRotateLeft();
	void MeaStopRotate();
//	CString m_Angle;
	int m_DirPic;
	afx_msg void OnBnClickeddirp();
	CString m_nowAngle;
	CString m_nowangle;
};
