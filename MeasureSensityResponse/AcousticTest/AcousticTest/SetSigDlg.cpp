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
	m_startF = startf;
	m_endF = endf;
	m_deltaF = deltaf;
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
		AfxMessageBox("��ʼƵ�ʴ�����ֹƵ�ʣ�");
		return;
	}
	if(m_volt<10||m_volt>5000)
	{
		AfxMessageBox("�ź�Դ���ȷ�ΧΪ10mV~5V��");
		return;
	}
	if(m_pulseRe<0.1||m_pulseRe>2)
	{
		AfxMessageBox("�ظ����ڷ�ΧΪ0.1s~2s��");
		return;
	}
	f=m_startF;
	startf=m_startF;
	endf=m_endF;
	deltaf=m_deltaF;
	OneThird_f=(m_OneThirdFreq.GetCheck()==BST_CHECKED)? true:false;
	if(OneThird_f) 
	{
		for(unsigned int i=0;i<31;i++)
		{
			if(abs(OneThirdFreq[i]-f)<0.001) 
			{
				OTFreq=i;
				break;
			}
		}
		if(OTFreq==0) 
		{
			AfxMessageBox("��ʼƵ�ʲ�������֮һ��Ƶ�̣�");
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
		AfxMessageBox("��ʼƵ�ʴ�����ֹƵ�ʣ�");
		return;
	}
	if(m_volt<10||m_volt>5000)
	{
		AfxMessageBox("�ź�Դ���ȷ�ΧΪ10mV~5V��");
		return;
	}
	if(m_pulseRe<0.1||m_pulseRe>2)
	{
		AfxMessageBox("�ظ����ڷ�ΧΪ0.1s~2s��");
		return;
	}
	CDialogEx::OnCancel();
}


BOOL CSetSigDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	if(OneThird_f) m_OneThirdFreq.SetCheck(1);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
