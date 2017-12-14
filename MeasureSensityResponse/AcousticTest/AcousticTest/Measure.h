#pragma once

#include <vector>
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
	void renew();
	void huatu_sensity();
	void Capture(vector<int> cha,int count);
	void MeasureSensity();
	void MeasureResponse();
	void huatu_response();
	
};
