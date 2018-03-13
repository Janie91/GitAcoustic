// SetSigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AcousticTest.h"
#include "SetSigDlg.h"
#include "afxdialogex.h"

//...My code...
#include "MyFunction.h"
// CSetSigDlg dialog

IMPLEMENT_DYNAMIC(CSetSigDlg, CDialogEx)

CSetSigDlg::CSetSigDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetSigDlg::IDD, pParent)
{
	m_volt = v;
	m_pulseWid = Bwid;
	m_pulseRe = Brep;
		
}

CSetSigDlg::~CSetSigDlg()
{
}

void CSetSigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Text(pDX, IDC_DeltaF, m_deltaF);
	DDX_Text(pDX, IDC_EndF, m_endF);
	DDX_Text(pDX, IDC_DeltaF, m_deltaF);
	DDX_Text(pDX, IDC_StartF, m_startF);
	DDX_Text(pDX, IDC_Volt, m_volt);
	DDX_Text(pDX, IDC_PulseRep, m_pulseRe);
	DDX_Text(pDX, IDC_PulseWid, m_pulseWid);
	DDX_Control(pDX, IDC_OneThirdFreq, m_OneThirdFreq);
	DDX_Control(pDX, IDC_fUnitS, m_StartfUnit);
	//  DDX_Control(pDX, IDC_fUnitE, m_EndfUnit);
	//  DDX_Control(pDX, IDC_fUnitD, m_DeltafUnit);
}


BEGIN_MESSAGE_MAP(CSetSigDlg, CDialogEx)
	ON_BN_CLICKED(IDC_SigOk, &CSetSigDlg::OnBnClickedSigok)
	ON_BN_CLICKED(IDCANCEL, &CSetSigDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CSetSigDlg message handlers


void CSetSigDlg::OnBnClickedSigok()
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	if(m_startF>m_endF)
	{
		AfxMessageBox("起始频率大于终止频率！");
		return;
	}
	if(m_volt<10||m_volt>5000)
	{
		AfxMessageBox("信号源幅度范围为10mV~5V！");
		return;
	}
	if(m_pulseRe<0.1||m_pulseRe>2)
	{
		AfxMessageBox("重复周期范围为0.1s~2s！");
		return;
	}
	Unit=m_StartfUnit.GetCurSel();
	if(Unit==-1)
	{
		AfxMessageBox("请选择频率的单位！");
		return;
	}
	else if(Unit==0)
	{
		startf=m_startF*1000;
		endf=m_endF*1000;
		deltaf=m_deltaF*1000;
	}
	else
	{
		startf=m_startF;
		endf=m_endF;
		deltaf=m_deltaF;
	}
	f=startf;
	OneThird_f=(m_OneThirdFreq.GetCheck()==BST_CHECKED)? true:false;
	if(OneThird_f) 
	{
		for(unsigned int i=0;i<34;i++)
		{
			if(abs(OneThirdFreq[i]-f)<0.001) 
			{
				OTFreq=i;
				break;
			}
		}
		if(OTFreq==0) 
		{
			AfxMessageBox("起始频率不是三分之一倍频程！");
			return ;
		}
	}
	
	v=m_volt;
	Bwid=m_pulseWid;
	Brep=m_pulseRe;
	CDialogEx::OnOK();
}
void CSetSigDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	if(m_startF>m_endF)
	{
		AfxMessageBox("起始频率大于终止频率！");
		return;
	}
	if(m_volt<10||m_volt>5000)
	{
		AfxMessageBox("信号源幅度范围为10mV~5V！");
		return;
	}
	if(m_pulseRe<0.1||m_pulseRe>2)
	{
		AfxMessageBox("重复周期范围为0.1s~2s！");
		return;
	}
	CDialogEx::OnCancel();
}


BOOL CSetSigDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	if(OneThird_f) m_OneThirdFreq.SetCheck(1);
	if(Unit==-1) m_StartfUnit.SetCurSel(0);
	else m_StartfUnit.SetCurSel(Unit);
	if(m_StartfUnit.GetCurSel()==0)
	{
		m_startF = startf/1000;
		m_endF = endf/1000;
		m_deltaF = deltaf/1000;
	}
	else
	{
		m_startF = startf;
		m_endF = endf;
		m_deltaF = deltaf;
	}
	UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
