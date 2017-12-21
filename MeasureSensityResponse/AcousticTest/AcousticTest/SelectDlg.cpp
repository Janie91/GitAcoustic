// SelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AcousticTest.h"
#include "SelectDlg.h"
#include "afxdialogex.h"

//...My code...
#include "MyFunction.h"
#include "Measure.h"
//...end...
// CSelectDlg dialog

IMPLEMENT_DYNAMIC(CSelectDlg, CDialog)

CSelectDlg::CSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectDlg::IDD, pParent)
{

	m_Item = 0;
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
	
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	//AfxGetMainWnd()->SendMessage(WM_CLOSE);//��ѡ�������Ŀ��ǰһ������ϵͳ�����Ի���ر�
	//AfxGetMainWnd()->ShowWindow(SW_HIDE);//��ѡ�������Ŀ��ǰһ������ϵͳ�����Ի�������
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
void CSelectDlg::OnBnClickedselect()
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	switch(m_Item)
	{
	case 0:
		ChooseItem=0;break;//�ȽϷ���������
	case 1:
		ChooseItem=1;break;//�����ѹ��Ӧ
	case 2:
		ChooseItem=2;break;//��Ƶ�����ָ����
	case 3:
		ChooseItem=3;break;//��Ƶ�㷢��ָ����
	case 4:
		ChooseItem=4;break;//��Ƶ�����ָ����
	case 5:
		ChooseItem=5;break;//���׷�����
	}
	CMeasure mdlg;
	mdlg.DoModal();

}


void CSelectDlg::OnBnClickedselectquit()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
	AfxGetMainWnd()->SendMessage(WM_CLOSE);
}



