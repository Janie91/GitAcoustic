// TurnTable.cpp : 实现文件
//

#include "stdafx.h"
#include "AcousticTest.h"
#include "TurnTable.h"
#include "afxdialogex.h"
#include "MyFunction.h"

// CTurnTable 对话框

IMPLEMENT_DYNAMIC(CTurnTable, CDialog)

CTurnTable::CTurnTable(CWnd* pParent /*=NULL*/)
	: CDialog(CTurnTable::IDD, pParent)
	
{

	m_CurrentAngle = _T("");
	m_Speed = Speed;
	m_TargetAngle=0;
	m_StartAngle=StartAngle;
	m_EndAngle=EndAngle;
}

CTurnTable::~CTurnTable()
{
}

void CTurnTable::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSCOMM, m_mscom);
	DDX_Text(pDX, IDC_CurrentAngle, m_CurrentAngle);
	DDX_Text(pDX, IDC_Speed, m_Speed);
	DDX_Text(pDX, IDC_TargetAngle, m_TargetAngle);
	DDX_Text(pDX, IDC_StartAngle, m_StartAngle);
	DDX_Text(pDX, IDC_EndAngle, m_EndAngle);
}


BEGIN_MESSAGE_MAP(CTurnTable, CDialog)
	ON_BN_CLICKED(IDC_quit, &CTurnTable::OnBnClickedquit)
	ON_BN_CLICKED(IDC_RotateRight, &CTurnTable::OnBnClickedRotateright)
	ON_BN_CLICKED(IDC_RotateLeft, &CTurnTable::OnBnClickedRotateleft)
	ON_BN_CLICKED(IDC_SetZero, &CTurnTable::OnBnClickedSetzero)
	ON_BN_CLICKED(IDC_ReturnZero, &CTurnTable::OnBnClickedReturnzero)
	ON_BN_CLICKED(IDC_RotateSetAngle, &CTurnTable::OnBnClickedRotatesetangle)
	ON_BN_CLICKED(IDC_StopRotate, &CTurnTable::OnBnClickedStoprotate)
	ON_WM_TIMER()
	ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL CTurnTable::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	SetWindowPos(NULL,50,80,0,0,SWP_NOZORDER | SWP_NOSIZE);
	if(m_mscom.get_PortOpen())
		m_mscom.put_PortOpen(false); 
	//直接在串口控件的属性中设置了以下这些参数
	//m_mscom.put_CommPort(2);    //选择串口
	//m_mscom.put_InBufferSize(1024);
	//m_mscom.put_OutBufferSize(512);
	//m_mscom.put_InputLen(0);         //设置当前接收区数据长度为0,表示全部读取
 //   m_mscom.put_InputMode(0);           //设置输入方式为文本方式
 //   m_mscom.put_RTSEnable(1);           //设置RT允许
	//m_mscom.put_Settings("9600,e,7,2");         //comb2选择的波特率，偶校验，7数据位，2个停止位
    if(!m_mscom.get_PortOpen())
	{
		m_mscom.put_PortOpen(true);         //打开串口
	}
	else
	{
		m_mscom.put_OutBufferCount(0);
		MessageBox("串口2打开失败");
	}
	SetManual();
	SetTimer(1,200,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// CTurnTable 消息处理程序
void CTurnTable::SetManual()
{
	// TODO: 在此添加控件通知处理程序代码
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	databuf.SetSize(19);	
	databuf.SetAt(0,64);
	databuf.SetAt(1,48);
	databuf.SetAt(2,49);
	databuf.SetAt(3,75);
	databuf.SetAt(4,82);
	databuf.SetAt(5,67);
	databuf.SetAt(6,73);
	databuf.SetAt(7,79);
	databuf.SetAt(8,32);
	databuf.SetAt(9,48);
	databuf.SetAt(10,48);
	databuf.SetAt(11,53);
	databuf.SetAt(12,48);
	databuf.SetAt(13,49);
	databuf.SetAt(14,48);
	commanddata=databuf[0];
	for(int i=1;i<=14;i++)
	{
		commanddata^=databuf[i];
	}
	sprintf_s(crc,"%02X",commanddata);
	databuf.SetAt(15,crc[0]);
	databuf.SetAt(16,crc[1]);
	databuf.SetAt(17,42);
	databuf.SetAt(18,13);
	m_mscom.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_mscom.get_Input();
	CString str;
	str=retVal.bstrVal;//因为串口返回的是字符串类型的，retVal的vt是VT_BSTR
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("设置手动模式操作出错!");
	}
}
void CTurnTable::SetSpeed(int speed)
{
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	char strspeed[9];
	databuf.SetSize(21);//设速度为speed
	databuf.SetAt(0,64);
	databuf.SetAt(1,48);
	databuf.SetAt(2,49);
	databuf.SetAt(3,87);
	databuf.SetAt(4,68);
	databuf.SetAt(5,48);
	databuf.SetAt(6,50);
	databuf.SetAt(7,49);
	databuf.SetAt(8,48);
	sprintf_s(strspeed,"%08X",speed);//左边补零，共有八位
	databuf.SetAt(9,strspeed[4]);//回转命令数据是先低后高
	databuf.SetAt(10,strspeed[5]);
	databuf.SetAt(11,strspeed[6]);
	databuf.SetAt(12,strspeed[7]);
	databuf.SetAt(13,strspeed[0]);
	databuf.SetAt(14,strspeed[1]);
	databuf.SetAt(15,strspeed[2]);
	databuf.SetAt(16,strspeed[3]);	
	commanddata=databuf[0];
	for(int i=1;i<=16;i++)
	{
		commanddata^=databuf[i];
	}
	sprintf_s(crc,"%02X",commanddata);
	databuf.SetAt(17,crc[0]);
	databuf.SetAt(18,crc[1]);
	databuf.SetAt(19,42);
	databuf.SetAt(20,13);
	m_mscom.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_mscom.get_Input();
	CString str;
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("设置速度操作出错!");
	}
}
float CTurnTable::ReadCurrentAngle()
{
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	databuf.SetSize(17);	
	databuf.SetAt(0,64);
	databuf.SetAt(1,48);
	databuf.SetAt(2,49);
	databuf.SetAt(3,82);
	databuf.SetAt(4,68);
	databuf.SetAt(5,48);
	databuf.SetAt(6,50);
	databuf.SetAt(7,55);
	databuf.SetAt(8,54);
	databuf.SetAt(9,48);
	databuf.SetAt(10,48);
	databuf.SetAt(11,48);
	databuf.SetAt(12,50);
	commanddata=databuf[0];
	for(int i=1;i<=12;i++)
	{
		commanddata^=databuf[i];
	}
	sprintf_s(crc,"%02X",commanddata);
	databuf.SetAt(13,crc[0]);
	databuf.SetAt(14,crc[1]);
	databuf.SetAt(15,42);
	databuf.SetAt(16,13);
	m_mscom.put_Output(COleVariant(databuf));
	Sleep(200);
	//angle为接收回来的第8位开始的4位
	VARIANT retVal=m_mscom.get_Input();
	CString str;
	str=retVal.bstrVal;//因为串口返回的是字符串类型的，retVal的vt是VT_BSTR
	//if(str.Mid(5,2)!="00")
	//{
	//	AfxMessageBox("读取角度操作出错!");
	//}
	int temp;
	CString anglestring;
	anglestring.Append(str.Mid(7,1));
	anglestring.Append(str.Mid(7,1));
	anglestring.Append(str.Mid(7,1));
	anglestring.Append(str.Mid(7,1));
	anglestring+=str.Mid(7,4);
	sscanf_s(anglestring,"%X",&temp);
	//是字符串，要化为数值，然后除以10才是角度
	float angle=temp/10.0f;
	m_CurrentAngle.Format("%.1f°",angle);
	SetDlgItemText(IDC_CurrentAngle,m_CurrentAngle);
	Sleep(200);
	return angle;
}


void CTurnTable::RotateRight()
{
	SetSpeed(m_Speed);
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	databuf.SetSize(19);//顺时针转
	databuf.SetAt(0,64);
	databuf.SetAt(1,48);
	databuf.SetAt(2,49);
	databuf.SetAt(3,75);
	databuf.SetAt(4,83);
	databuf.SetAt(5,67);
	databuf.SetAt(6,73);
	databuf.SetAt(7,79);
	databuf.SetAt(8,32);
	databuf.SetAt(9,48);
	databuf.SetAt(10,48);
	databuf.SetAt(11,53);
	databuf.SetAt(12,48);
	databuf.SetAt(13,48);
	databuf.SetAt(14,55);
	commanddata=databuf[0];
	for(int i=1;i<=14;i++)
	{
		commanddata^=databuf[i];
	}
	sprintf_s(crc,"%02X",commanddata);
	databuf.SetAt(15,crc[0]);
	databuf.SetAt(16,crc[1]);
	databuf.SetAt(17,42);
	databuf.SetAt(18,13);
	m_mscom.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_mscom.get_Input();
	CString str;
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("顺时针转动操作出错!");
	}
}
void CTurnTable::StopRotateRight()
{
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	databuf.SetSize(19);//顺时针停
	databuf.SetAt(0,64);
	databuf.SetAt(1,48);
	databuf.SetAt(2,49);
	databuf.SetAt(3,75);
	databuf.SetAt(4,82);
	databuf.SetAt(5,67);
	databuf.SetAt(6,73);
	databuf.SetAt(7,79);
	databuf.SetAt(8,32);
	databuf.SetAt(9,48);
	databuf.SetAt(10,48);
	databuf.SetAt(11,53);
	databuf.SetAt(12,48);
	databuf.SetAt(13,48);
	databuf.SetAt(14,55);
	commanddata=databuf[0];
	for(int i=1;i<=14;i++)
	{
		commanddata^=databuf[i];
	}
	sprintf_s(crc,"%02X",commanddata);
	databuf.SetAt(15,crc[0]);
	databuf.SetAt(16,crc[1]);
	databuf.SetAt(17,42);
	databuf.SetAt(18,13);
	m_mscom.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_mscom.get_Input();
	CString str;
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("顺时针停操作出错!");
	}
}
void CTurnTable::RotateLeft()
{
	SetSpeed(m_Speed);
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	databuf.SetSize(19);//逆时针转
	databuf.SetAt(0,64);
	databuf.SetAt(1,48);
	databuf.SetAt(2,49);
	databuf.SetAt(3,75);
	databuf.SetAt(4,83);
	databuf.SetAt(5,67);
	databuf.SetAt(6,73);
	databuf.SetAt(7,79);
	databuf.SetAt(8,32);
	databuf.SetAt(9,48);
	databuf.SetAt(10,48);
	databuf.SetAt(11,53);
	databuf.SetAt(12,48);
	databuf.SetAt(13,48);
	databuf.SetAt(14,56);
	commanddata=databuf[0];
	for(int i=1;i<=14;i++)
	{
		commanddata^=databuf[i];
	}
	sprintf_s(crc,"%02X",commanddata);
	databuf.SetAt(15,crc[0]);
	databuf.SetAt(16,crc[1]);
	databuf.SetAt(17,42);
	databuf.SetAt(18,13);
	m_mscom.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_mscom.get_Input();
	CString str;
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("逆时针转动操作出错!");
	}
}
void CTurnTable::StopRotateLeft()
{
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	databuf.SetSize(19);//逆时针停
	databuf.SetAt(0,64);
	databuf.SetAt(1,48);
	databuf.SetAt(2,49);
	databuf.SetAt(3,75);
	databuf.SetAt(4,82);
	databuf.SetAt(5,67);
	databuf.SetAt(6,73);
	databuf.SetAt(7,79);
	databuf.SetAt(8,32);
	databuf.SetAt(9,48);
	databuf.SetAt(10,48);
	databuf.SetAt(11,53);
	databuf.SetAt(12,48);
	databuf.SetAt(13,48);
	databuf.SetAt(14,56);
	commanddata=databuf[0];
	for(int i=1;i<=14;i++)
	{
		commanddata^=databuf[i];
	}
	sprintf_s(crc,"%02X",commanddata);
	databuf.SetAt(15,crc[0]);
	databuf.SetAt(16,crc[1]);
	databuf.SetAt(17,42);
	databuf.SetAt(18,13);
	m_mscom.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_mscom.get_Input();
	CString str;
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("逆时针停操作出错!");
	}
}
void CTurnTable::StopRotate()
{
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	databuf.SetSize(19);//停止转动
	databuf.SetAt(0,64);
	databuf.SetAt(1,48);
	databuf.SetAt(2,49);
	databuf.SetAt(3,75);
	databuf.SetAt(4,82);
	databuf.SetAt(5,67);
	databuf.SetAt(6,73);
	databuf.SetAt(7,79);
	databuf.SetAt(8,32);
	databuf.SetAt(9,48);
	databuf.SetAt(10,48);
	databuf.SetAt(11,53);
	databuf.SetAt(12,48);
	databuf.SetAt(13,48);
	databuf.SetAt(14,57);
	commanddata=databuf[0];
	for(int i=1;i<=14;i++)
	{
		commanddata^=databuf[i];
	}
	sprintf_s(crc,"%02X",commanddata);
	databuf.SetAt(15,crc[0]);
	databuf.SetAt(16,crc[1]);
	databuf.SetAt(17,42);
	databuf.SetAt(18,13);
	m_mscom.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_mscom.get_Input();
	CString str;
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("停止转动操作出错!");
	}
}
void CTurnTable::CancelCurrentZero()
{
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	databuf.SetSize(19);//取消当前零点
	databuf.SetAt(0,64);
	databuf.SetAt(1,48);
	databuf.SetAt(2,49);
	databuf.SetAt(3,75);
	databuf.SetAt(4,82);
	databuf.SetAt(5,67);
	databuf.SetAt(6,73);
	databuf.SetAt(7,79);
	databuf.SetAt(8,32);
	databuf.SetAt(9,48);
	databuf.SetAt(10,48);
	databuf.SetAt(11,53);
	databuf.SetAt(12,48);
	databuf.SetAt(13,48);
	databuf.SetAt(14,51);
	commanddata=databuf[0];
	for(int i=1;i<=14;i++)
	{
		commanddata^=databuf[i];
	}
	sprintf_s(crc,"%02X",commanddata);
	databuf.SetAt(15,crc[0]);
	databuf.SetAt(16,crc[1]);
	databuf.SetAt(17,42);
	databuf.SetAt(18,13);
	m_mscom.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_mscom.get_Input();
	CString str;
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("取消当前零点操作出错!");
	}
}
void CTurnTable::SetCurrentPosZero()
{
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	databuf.SetSize(19);//设当前位置为零点
	databuf.SetAt(0,64);
	databuf.SetAt(1,48);
	databuf.SetAt(2,49);
	databuf.SetAt(3,75);
	databuf.SetAt(4,83);
	databuf.SetAt(5,67);
	databuf.SetAt(6,73);
	databuf.SetAt(7,79);
	databuf.SetAt(8,32);
	databuf.SetAt(9,48);
	databuf.SetAt(10,48);
	databuf.SetAt(11,53);
	databuf.SetAt(12,48);
	databuf.SetAt(13,48);
	databuf.SetAt(14,51);
	commanddata=databuf[0];
	for(int i=1;i<=14;i++)
	{
		commanddata^=databuf[i];
	}
	sprintf_s(crc,"%02X",commanddata);
	databuf.SetAt(15,crc[0]);
	databuf.SetAt(16,crc[1]);
	databuf.SetAt(17,42);
	databuf.SetAt(18,13);
	m_mscom.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_mscom.get_Input();
	CString str;
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("设置当前位置为零点操作出错!");
	}
}
void CTurnTable::StopRotateBacktoZero()
{
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	databuf.SetSize(19);//停止回零点
	databuf.SetAt(0,64);
	databuf.SetAt(1,48);
	databuf.SetAt(2,49);
	databuf.SetAt(3,75);
	databuf.SetAt(4,82);
	databuf.SetAt(5,67);
	databuf.SetAt(6,73);
	databuf.SetAt(7,79);
	databuf.SetAt(8,32);
	databuf.SetAt(9,48);
	databuf.SetAt(10,48);
	databuf.SetAt(11,53);
	databuf.SetAt(12,48);
	databuf.SetAt(13,48);
	databuf.SetAt(14,54);
	commanddata=databuf[0];
	for(int i=1;i<=14;i++)
	{
		commanddata^=databuf[i];
	}
	sprintf_s(crc,"%02X",commanddata);
	databuf.SetAt(15,crc[0]);
	databuf.SetAt(16,crc[1]);
	databuf.SetAt(17,42);
	databuf.SetAt(18,13);
	m_mscom.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_mscom.get_Input();
	CString str;
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("停止回零点操作出错!");
	}
}
void CTurnTable::RotateBacktoZero()
{
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	databuf.SetSize(19);//回到参考零点
	databuf.SetAt(0,64);
	databuf.SetAt(1,48);
	databuf.SetAt(2,49);
	databuf.SetAt(3,75);
	databuf.SetAt(4,83);
	databuf.SetAt(5,67);
	databuf.SetAt(6,73);
	databuf.SetAt(7,79);
	databuf.SetAt(8,32);
	databuf.SetAt(9,48);
	databuf.SetAt(10,48);
	databuf.SetAt(11,53);
	databuf.SetAt(12,48);
	databuf.SetAt(13,48);
	databuf.SetAt(14,54);
	commanddata=databuf[0];
	for(int i=1;i<=14;i++)
	{
		commanddata^=databuf[i];
	}
	sprintf_s(crc,"%02X",commanddata);
	databuf.SetAt(15,crc[0]);
	databuf.SetAt(16,crc[1]);
	databuf.SetAt(17,42);
	databuf.SetAt(18,13);
	m_mscom.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_mscom.get_Input();
	CString str;
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("回到参考零点操作出错!");
	}
}
void CTurnTable::SetAuto()
{
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	databuf.SetSize(19);//设为自动
	databuf.SetAt(0,64);
	databuf.SetAt(1,48);
	databuf.SetAt(2,49);
	databuf.SetAt(3,75);
	databuf.SetAt(4,83);
	databuf.SetAt(5,67);
	databuf.SetAt(6,73);
	databuf.SetAt(7,79);
	databuf.SetAt(8,32);
	databuf.SetAt(9,48);
	databuf.SetAt(10,48);
	databuf.SetAt(11,53);
	databuf.SetAt(12,48);
	databuf.SetAt(13,49);
	databuf.SetAt(14,48);
	commanddata=databuf[0];
	for(int i=1;i<=14;i++)
	{
		commanddata^=databuf[i];
	}
	sprintf_s(crc,"%02X",commanddata);
	databuf.SetAt(15,crc[0]);
	databuf.SetAt(16,crc[1]);
	databuf.SetAt(17,42);
	databuf.SetAt(18,13);
	m_mscom.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_mscom.get_Input();
	CString str;
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("设为自动操作出错!");
	}
}
void CTurnTable::SetTargetAngle(int targetangle)
{
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	char strangle[9];
	databuf.SetSize(21);//设置目标角度
	databuf.SetAt(0,64);
	databuf.SetAt(1,48);
	databuf.SetAt(2,49);
	databuf.SetAt(3,87);
	databuf.SetAt(4,68);
	databuf.SetAt(5,48);
	databuf.SetAt(6,50);
	databuf.SetAt(7,55);
	databuf.SetAt(8,52);
	sprintf_s(strangle,"%08X",targetangle*10);//转换角度时有个10倍关系
	databuf.SetAt(9,strangle[4]);
	databuf.SetAt(10,strangle[5]);
	databuf.SetAt(11,strangle[6]);
	databuf.SetAt(12,strangle[7]);
	databuf.SetAt(13,strangle[0]);
	databuf.SetAt(14,strangle[1]);
	databuf.SetAt(15,strangle[2]);
	databuf.SetAt(16,strangle[3]);
	commanddata=databuf[0];
	for(int i=1;i<=16;i++)
	{
		commanddata^=databuf[i];
	}
	sprintf_s(crc,"%02X",commanddata);
	databuf.SetAt(17,crc[0]);
	databuf.SetAt(18,crc[1]);
	databuf.SetAt(19,42);
	databuf.SetAt(20,13);
	m_mscom.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_mscom.get_Input();
	CString str;
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("设置目标角度操作出错!");
	}
}
void CTurnTable::PositionStart()
{
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	databuf.SetSize(19);//定位启动
	databuf.SetAt(0,64);
	databuf.SetAt(1,48);
	databuf.SetAt(2,49);
	databuf.SetAt(3,75);
	databuf.SetAt(4,83);
	databuf.SetAt(5,67);
	databuf.SetAt(6,73);
	databuf.SetAt(7,79);
	databuf.SetAt(8,32);
	databuf.SetAt(9,48);
	databuf.SetAt(10,48);
	databuf.SetAt(11,53);
	databuf.SetAt(12,48);
	databuf.SetAt(13,48);
	databuf.SetAt(14,57);
	commanddata=databuf[0];
	for(int i=1;i<=14;i++)
	{
		commanddata^=databuf[i];
	}
	sprintf_s(crc,"%02X",commanddata);
	databuf.SetAt(15,crc[0]);
	databuf.SetAt(16,crc[1]);
	databuf.SetAt(17,42);
	databuf.SetAt(18,13);
	m_mscom.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_mscom.get_Input();
	CString str;
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("定位启动操作出错!");
	}
}
void CTurnTable::ReturnZero()
{
	StopRotateBacktoZero();
	RotateBacktoZero();
}
void CTurnTable::SetZero()
{
	CancelCurrentZero();
	SetCurrentPosZero();
}
void CTurnTable::RotateTargetAngle(int targetangle)
{
	SetAuto();
	Sleep(200);
	StopRotate();
	Sleep(100);
	m_Speed=GetDlgItemInt(IDC_Speed);
	SetSpeed(m_Speed);
	SetTargetAngle(targetangle);
	PositionStart();
}
void CTurnTable::OnBnClickedRotateright()
{
	// TODO: Add your control notification handler code here
	CString str;
	SetManual();
	m_Speed=GetDlgItemInt(IDC_Speed);
	GetDlgItemTextA(IDC_RotateRight,str);
	if(str=="顺时针转")
	{
		RotateRight();
		GetDlgItem(IDC_RotateRight)->SetWindowTextA("顺时针停");
	}
	else if(str=="顺时针停")
	{
		StopRotateRight();
		GetDlgItem(IDC_RotateRight)->SetWindowTextA("顺时针转");
	}
}


void CTurnTable::OnBnClickedRotateleft()
{
	// TODO: Add your control notification handler code here
	CString str;
	SetManual();
	m_Speed=GetDlgItemInt(IDC_Speed);
	GetDlgItemTextA(IDC_RotateLeft,str);
	if(str=="逆时针转")
	{
		RotateLeft();
		GetDlgItem(IDC_RotateLeft)->SetWindowTextA("逆时针停");
	}
	else if(str=="逆时针停")
	{
		StopRotateLeft();
		GetDlgItem(IDC_RotateLeft)->SetWindowTextA("逆时针转");
	}
}


void CTurnTable::OnBnClickedSetzero()
{
	// TODO: Add your control notification handler code here
	SetZero();
}


void CTurnTable::OnBnClickedReturnzero()
{
	// TODO: Add your control notification handler code here
	ReturnZero();
}


void CTurnTable::OnBnClickedRotatesetangle()
{
	// TODO: Add your control notification handler code here
	m_TargetAngle=GetDlgItemInt(IDC_TargetAngle);
	RotateTargetAngle(m_TargetAngle);
}


void CTurnTable::OnBnClickedStoprotate()
{
	// TODO: Add your control notification handler code here
	StopRotate();
}


void CTurnTable::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	ReadCurrentAngle();
	CDialog::OnTimer(nIDEvent);
}
void CTurnTable::OnBnClickedquit()
{
	// TODO: Add your control notification handler code here
	Speed=GetDlgItemInt(IDC_Speed);
	StartAngle=GetDlgItemInt(IDC_StartAngle);
	EndAngle=GetDlgItemInt(IDC_EndAngle);
	clock_t start=clock();
	ReadCurrentAngle();
	clock_t end=clock();
	CString stemp;
	stemp.Format("测量时间为 %.4f s",(double)(end-start)/CLOCKS_PER_SEC);
	MessageBox(stemp);

}

void CTurnTable::OnPaint()//在turntable的重绘函数中添加了显示角度的函数，不知道测量的时候能不能一直看到变化
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages
	SetDlgItemText(IDC_CurrentAngle,m_CurrentAngle);
}


void CTurnTable::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	KillTimer(1);
	if(m_mscom.get_PortOpen())
		m_mscom.put_PortOpen(false);
	DestroyWindow();
}


void CTurnTable::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
	
	CDialog::PostNcDestroy();
	//delete this;
}
