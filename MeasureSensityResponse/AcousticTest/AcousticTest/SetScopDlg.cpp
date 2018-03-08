// SetScopDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AcousticTest.h"
#include "SetScopDlg.h"
#include "afxdialogex.h"


// CSetScopDlg dialog
//...My code...
#include "MyFunction.h"
//...end...

IMPLEMENT_DYNAMIC(CSetScopDlg, CDialog)

CSetScopDlg::CSetScopDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetScopDlg::IDD, pParent)
{
}

CSetScopDlg::~CSetScopDlg()
{
}

void CSetScopDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ch1, m_ch1);
	DDX_Control(pDX, IDC_ch2, m_ch2);
	DDX_Control(pDX, IDC_ch3, m_ch3);
	DDX_Control(pDX, IDC_ch4, m_ch4);
	DDX_Control(pDX, IDC_ChRefer, m_chRefer);
}


BEGIN_MESSAGE_MAP(CSetScopDlg, CDialog)
	ON_BN_CLICKED(IDC_ScopOK, &CSetScopDlg::OnBnClickedScopok)
	ON_BN_CLICKED(IDCANCEL, &CSetScopDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CSetScopDlg message handlers

//...My code...
BOOL CSetScopDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_chRefer.SetCurSel(chaRefer-1);
	for(int i=0;i<4;i++)
	{
		if(isChaChoose[i]) 
		{
			switch(i)
			{
			case 0:  m_ch1.SetCheck(1);break;
			case 1:  m_ch2.SetCheck(1);break;
			case 2:  m_ch3.SetCheck(1);break;
			case 3:  m_ch4.SetCheck(1);break;
			}
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
void CSetScopDlg::OnBnClickedScopok()
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	isChaChoose[0]=(m_ch1.GetCheck()==BST_CHECKED)? true:false;
	isChaChoose[1]=(m_ch2.GetCheck()==BST_CHECKED)? true:false;
	isChaChoose[2]=(m_ch3.GetCheck()==BST_CHECKED)? true:false;
	isChaChoose[3]=(m_ch4.GetCheck()==BST_CHECKED)? true:false;

	//if(m_ch4.GetCheck()==BST_CHECKED) isChaChoose[3]=true;
	CString temp;
	m_chRefer.GetLBText(m_chRefer.GetCurSel(),temp);	
	if(isChaChoose[atoi(temp)-1]==false)
	{
		AfxMessageBox("参考通道不是以上选择的通道，请确认！");
	}
	else chaRefer=atoi(temp);
	CDialog::OnOK();
}




void CSetScopDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialog::OnCancel();
}
