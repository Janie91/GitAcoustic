// ChangeSig.cpp : implementation file
//

#include "stdafx.h"
#include "AcousticTest.h"
#include "ChangeSig.h"
#include "afxdialogex.h"
#include "MyFunction.h"

// CChangeSig dialog

IMPLEMENT_DYNAMIC(CChangeSig, CDialogEx)

CChangeSig::CChangeSig(CWnd* pParent /*=NULL*/)
	: CDialogEx(CChangeSig::IDD, pParent)
{
	m_f = f;
	m_v = v;
	m_w = Bwid;
	m_p = Brep;
	isChange=true;
}

CChangeSig::~CChangeSig()
{
}

void CChangeSig::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_f, m_f);
	DDX_Text(pDX, IDC_p, m_p);
	DDX_Text(pDX, IDC_v, m_v);
	DDX_Text(pDX, IDC_w, m_w);
}


BEGIN_MESSAGE_MAP(CChangeSig, CDialogEx)
	ON_BN_CLICKED(IDOK, &CChangeSig::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CChangeSig::OnBnClickedCancel)
END_MESSAGE_MAP()


// CChangeSig message handlers


void CChangeSig::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	f=m_f;
	v=m_v;
	Bwid=m_w;
	Brep=m_p;
	if(ChooseItem==4) 
		CreateMulFrePulse(100000,f*1000,deltaf*1000,Bwid/1000,v/1000,Brep);
	else CreateBurst(f*1000,v/1000,Bwid/1000,Brep);
	
	//OnCancel();
}

void CChangeSig::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
void CChangeSig::OnCancel()//这个对话框是非模态创建的，所以重载了OnCancel()函数
{
	// TODO: Add your specialized code here and/or call the base class

	DestroyWindow();
}


void CChangeSig::PostNcDestroy()//重载了这个函数，在其中删除非模态对话框对象
{
	// TODO: Add your specialized code here and/or call the base class

	CDialogEx::PostNcDestroy();
	delete this;
	isChange=false;
}



