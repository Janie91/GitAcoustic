
// CSetConditionDlg message handlers
// SetConditionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AcousticTest.h"
#include "SetConditionDlg.h"
#include "afxdialogex.h"

//...My code...
#include "MyFunction.h"
#include <map>
using namespace std;
//...end...
// CSetConditionDlg dialog


IMPLEMENT_DYNAMIC(CSetConditionDlg, CDialog)

CSetConditionDlg::CSetConditionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetConditionDlg::IDD, pParent)
{

	m_d1 = d[0];
	m_d2 = d[1];
	m_d3 = d[2];
	m_d4 = d[3];
	m_Ratio=Ratio;
	m_gain1 = Gain[0];
	m_gain2 = Gain[1];
	m_gain3 = Gain[2];
	m_gain4 = Gain[3];
}

CSetConditionDlg::~CSetConditionDlg()
{
}

void CSetConditionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_d1, m_d1);
	DDX_Text(pDX, IDC_d2, m_d2);
	DDX_Text(pDX, IDC_d3, m_d3);
	DDX_Text(pDX, IDC_d4, m_d4);
	DDX_Text(pDX, IDC_ratio, m_Ratio);
	DDX_Text(pDX, IDC_Gain1, m_gain1);
	DDX_Text(pDX, IDC_Gain2, m_gain2);
	DDX_Text(pDX, IDC_Gain3, m_gain3);
	DDX_Text(pDX, IDC_Gain4, m_gain4);
	DDX_Control(pDX, IDC_MeaCount, m_MeaCount);
}


BEGIN_MESSAGE_MAP(CSetConditionDlg, CDialog)
	ON_BN_CLICKED(IDC_ConditionOK, &CSetConditionDlg::OnBnClickedConditionok)
	ON_BN_CLICKED(IDCANCEL, &CSetConditionDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_openFile, &CSetConditionDlg::OnBnClickedopenfile)
	
END_MESSAGE_MAP()


// CSetConditionDlg message handlers

//...My code...
void CSetConditionDlg::OnBnClickedConditionok()
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	d[0]=m_d1;
	d[1]=m_d2;
	d[2]=m_d3;
	d[3]=m_d4;
	Ratio=m_Ratio;
	Gain[0]=m_gain1;
	Gain[1]=m_gain2;
	Gain[2]=m_gain3;
	Gain[3]=m_gain4;
	if(ChooseItem==0)
	{
		CString temp;
		m_MeaCount.GetLBText(m_MeaCount.GetCurSel(),temp);	
		MeaCount=atoi(temp);
	}
	CDialog::OnOK();
}


void CSetConditionDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}


void CSetConditionDlg::OnBnClickedopenfile()
{
	// TODO: Add your control notification handler code here
	CString strfile;	
	CString filedata;
	
	float freq;
	float val;
	strfile="Txt Files(*.txt)|*.txt||";
	//CFileDialog dlg(true,NULL,NULL,OFN_EXPLORER|OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_FILEMUSTEXIST,strfile);
	CFileDialog  dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,strfile);
	dlg.m_ofn.lStructSize=sizeof(OPENFILENAME);
	if(dlg.DoModal()==IDOK)
	{
		strDirFile=dlg.GetPathName();
	}
	//UpdateData(false);
	CStdioFile file;
	if(file.Open(strDirFile,CFile::modeRead))
	{
		file.ReadString(filedata);
		if(filedata!="标准水听器文件") 
		{
			AfxMessageBox("文件头第一行不正确！请重新选择！");
			return;
		}
		file.ReadString(filedata);
		if(filedata.FindOneOf("标准水听器编号：")==-1)//找到给定的字符返回它在字符串中对应的索引号；没有找到就返回-1。 
		{
			AfxMessageBox("文件头第二行不正确！请重新选择！");
			return;
		}
		file.ReadString(filedata);
		if(filedata!="频率(Hz),灵敏度(dB)") 
		{
			AfxMessageBox("文件头第三行不正确！请重新选择！");
			return;
		}
		while(file.ReadString(filedata))
		{
			if(filedata=="")
			{
				AfxMessageBox("文件内容格式不正确，请检查！");
				standMp.clear();
				return;
			}
			sscanf_s(filedata,"%f,%f",&freq,&val);
			standMp.insert(make_pair(freq,val));
		}
	}
	if(file!=NULL) file.Close();
	SetDlgItemText(IDC_standFile,strDirFile);	
}





BOOL CSetConditionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	SetDlgItemText(IDC_standFile,strDirFile);
	m_MeaCount.SetCurSel(MeaCount-1);
	if(ChooseItem==0)
	{
		GetDlgItem(IDC_SmeaCount)->EnableWindow(TRUE);
		GetDlgItem(IDC_MeaCount)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_SmeaCount)->EnableWindow(FALSE);
		GetDlgItem(IDC_MeaCount)->EnableWindow(FALSE);
	}
	if(ChooseItem==5)
	{
		SetDlgItemText(IDC_ch1Dis,"FJ距离");
		SetDlgItemText(IDC_ch2Dis,"FH距离");
		SetDlgItemText(IDC_ch3Dis,"HJ距离");
		GetDlgItem(IDC_ch4Dis)->EnableWindow(FALSE);
		GetDlgItem(IDC_d4)->EnableWindow(FALSE);
		GetDlgItem(IDC_S4)->EnableWindow(FALSE);
		GetDlgItem(IDC_SD)->EnableWindow(FALSE);
		GetDlgItem(IDC_Gain4)->EnableWindow(FALSE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
