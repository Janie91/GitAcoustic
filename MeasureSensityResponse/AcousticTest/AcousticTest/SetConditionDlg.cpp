
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
	m_StandFile=strDirFile;
	m_Ratio=Ratio;
	m_Gain=Gain;
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
	DDX_Text(pDX, IDC_standFile, m_StandFile);
	DDX_Text(pDX, IDC_ratio, m_Ratio);
	DDX_Text(pDX, IDC_Gain, m_Gain);
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
	Gain=m_Gain;
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
	UpdateData(true);
	d[0]=m_d1;
	d[1]=m_d2;
	d[2]=m_d3;
	d[3]=m_d4;
	Ratio=m_Ratio;
	Gain=m_Gain;

	CString strfile;	
	CString filedata;
	
	float freq;
	float val;
	strfile="Txt Files(*.txt)|*.txt||";
	CFileDialog dlg(true,NULL,NULL,OFN_EXPLORER|OFN_HIDEREADONLY|OFN_ENABLESIZING|OFN_FILEMUSTEXIST,strfile);
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
	m_StandFile=strDirFile;
	UpdateData(false);
}



