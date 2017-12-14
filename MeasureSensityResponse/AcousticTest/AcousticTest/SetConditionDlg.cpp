
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
		if(filedata!="��׼ˮ�����ļ�") 
		{
			AfxMessageBox("�ļ�ͷ��һ�в���ȷ��������ѡ��");
			return;
		}
		file.ReadString(filedata);
		if(filedata.FindOneOf("��׼ˮ������ţ�")==-1)//�ҵ��������ַ����������ַ����ж�Ӧ�������ţ�û���ҵ��ͷ���-1�� 
		{
			AfxMessageBox("�ļ�ͷ�ڶ��в���ȷ��������ѡ��");
			return;
		}
		file.ReadString(filedata);
		if(filedata!="Ƶ��(Hz),������(dB)") 
		{
			AfxMessageBox("�ļ�ͷ�����в���ȷ��������ѡ��");
			return;
		}
		while(file.ReadString(filedata))
		{
			if(filedata=="")
			{
				AfxMessageBox("�ļ����ݸ�ʽ����ȷ�����飡");
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



