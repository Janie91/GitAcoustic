
// AcousticTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AcousticTest.h"
#include "AcousticTestDlg.h"
#include "afxdialogex.h"
//...My code...
#include "MyFunction.h"
#include "SelectDlg.h"
//...end...
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAcousticTestDlg dialog




CAcousticTestDlg::CAcousticTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAcousticTestDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAcousticTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAcousticTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CAcousticTestDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CAcousticTestDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CAcousticTestDlg message handlers

BOOL CAcousticTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	//...My code...
	SetDlgItemText(IDC_SigName,"TCPIP0::223.3.43.206::inst0::INSTR");
	SetDlgItemText(IDC_ScopName,"TCPIP0::223.3.45.94::inst0::INSTR");
	//...end...
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAcousticTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAcousticTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAcousticTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//...My code...

void CAcousticTestDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	
	char SigName[50],ScopName[50];
	GetDlgItemText(IDC_SigName,SigName,50);
	GetDlgItemText(IDC_ScopName,ScopName,50);
	viOpenDefaultRM(&vidg);
	status=viOpen(vidg,SigName,VI_NULL,VI_NULL,&vig);
	if(status!=0)
	{
		AfxMessageBox("函数发生器连接不成功，请IP地址或本地连接再重新连接！");
		viClose(vig);
		viClose(vidg);
		return;
	}
	else
	{
		viClear(vig);
		viPrintf(vig,"*rst\n");
		viOpenDefaultRM(&vidp);
		status=viOpen(vidp,ScopName,VI_NULL,VI_NULL,&vip);
		if(status!=0)
		{
			AfxMessageBox("示波器连接不成功，请IP地址或本地连接再重新连接！");
			viClose(vip);
			viClose(vidp);
			viClose(vig);
			viClose(vidg);
			return;
		}
		else
		{//信号源和示波器都成功连接
			viClear(vip);			
			viPrintf(vip,"*rst\n");
			CSelectDlg selectdlg;//进入选择测量项目的对话框
			selectdlg.DoModal();
		}
	}
	////debug
	//CSelectDlg selectdlg;//进入选择测量项目的对话框
	//selectdlg.DoModal();
	////debug
}


void CAcousticTestDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	viClose(vip);
	viClose(vidp);
	viClose(vig);
	viClose(vidg);
	CDialogEx::OnCancel();
}




