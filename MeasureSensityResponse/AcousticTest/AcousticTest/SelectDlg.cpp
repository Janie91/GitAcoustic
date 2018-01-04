// SelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AcousticTest.h"
#include "SelectDlg.h"
#include "afxdialogex.h"

//...My code...
#include "MyFunction.h"

//...end...
// CSelectDlg dialog

IMPLEMENT_DYNAMIC(CSelectDlg, CDialog)

CSelectDlg::CSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectDlg::IDD, pParent)
{

	m_Item = 0;
	mdlg=NULL;
}

CSelectDlg::~CSelectDlg()
{
}

void CSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_Item, m_Item);
}


BEGIN_MESSAGE_MAP(CSelectDlg, CDialog)
	ON_BN_CLICKED(IDC_select, &CSelectDlg::OnBnClickedselect)
	ON_BN_CLICKED(IDC_selectquit, &CSelectDlg::OnBnClickedselectquit)
END_MESSAGE_MAP()


// CSelectDlg message handlers

//...My code...
BOOL CSelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// TODO:  在此添加额外的初始化
	AfxGetMainWnd()->ShowWindow(SW_HIDE);//把选择测量项目的前一个进入系统的主对话框隐藏
	ModifyStyleEx(WS_EX_TOOLWINDOW, WS_EX_APPWINDOW);//运行程序的时候可以显示图标
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CSelectDlg::OnBnClickedselect()
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	switch(m_Item)
	{
	case 0:
		ChooseItem=0;break;//比较法测灵敏度
	case 1:
		ChooseItem=1;break;//发射电压响应
	case 2:
		ChooseItem=2;break;//单频点接收指向性
	case 3:
		ChooseItem=3;break;//单频点发射指向性
	case 4:
		ChooseItem=4;break;//多频点接收指向性
	case 5:
		ChooseItem=5;break;//互易法测量
	}
	mdlg=new CMeasure(this);
	mdlg->Create(IDD_Measure);
	mdlg->CenterWindow();//显示在中间
	mdlg->ShowWindow(SW_SHOW);
	//this->ShowWindow(SW_HIDE);//将本对话框隐藏,就不知道怎么再打开了

}


void CSelectDlg::OnBnClickedselectquit()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();//模态对话框，直接调用基类的函数	
	AfxGetMainWnd()->SendMessage(WM_CLOSE);//关闭主对话框
}



