// TurnTable.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AcousticTest.h"
#include "TurnTable.h"
#include "afxdialogex.h"
#include "MyFunction.h"

// CTurnTable �Ի���

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
	//ֱ���ڴ��ڿؼ���������������������Щ����
	//m_mscom.put_CommPort(2);    //ѡ�񴮿�
	//m_mscom.put_InBufferSize(1024);
	//m_mscom.put_OutBufferSize(512);
	//m_mscom.put_InputLen(0);         //���õ�ǰ���������ݳ���Ϊ0,��ʾȫ����ȡ
 //   m_mscom.put_InputMode(0);           //�������뷽ʽΪ�ı���ʽ
 //   m_mscom.put_RTSEnable(1);           //����RT����
	//m_mscom.put_Settings("9600,e,7,2");         //comb2ѡ��Ĳ����ʣ�żУ�飬7����λ��2��ֹͣλ
    if(!m_mscom.get_PortOpen())
	{
		m_mscom.put_PortOpen(true);         //�򿪴���
	}
	else
	{
		m_mscom.put_OutBufferCount(0);
		MessageBox("����2��ʧ��");
	}
	SetManual();
	SetTimer(1,200,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

// CTurnTable ��Ϣ�������
void CTurnTable::SetManual()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	str=retVal.bstrVal;//��Ϊ���ڷ��ص����ַ������͵ģ�retVal��vt��VT_BSTR
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("�����ֶ�ģʽ��������!");
	}
}
void CTurnTable::SetSpeed(int speed)
{
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	char strspeed[9];
	databuf.SetSize(21);//���ٶ�Ϊspeed
	databuf.SetAt(0,64);
	databuf.SetAt(1,48);
	databuf.SetAt(2,49);
	databuf.SetAt(3,87);
	databuf.SetAt(4,68);
	databuf.SetAt(5,48);
	databuf.SetAt(6,50);
	databuf.SetAt(7,49);
	databuf.SetAt(8,48);
	sprintf_s(strspeed,"%08X",speed);//��߲��㣬���а�λ
	databuf.SetAt(9,strspeed[4]);//��ת�����������ȵͺ��
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
		AfxMessageBox("�����ٶȲ�������!");
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
	//angleΪ���ջ����ĵ�8λ��ʼ��4λ
	VARIANT retVal=m_mscom.get_Input();
	CString str;
	str=retVal.bstrVal;//��Ϊ���ڷ��ص����ַ������͵ģ�retVal��vt��VT_BSTR
	//if(str.Mid(5,2)!="00")
	//{
	//	AfxMessageBox("��ȡ�ǶȲ�������!");
	//}
	int temp;
	CString anglestring;
	anglestring.Append(str.Mid(7,1));
	anglestring.Append(str.Mid(7,1));
	anglestring.Append(str.Mid(7,1));
	anglestring.Append(str.Mid(7,1));
	anglestring+=str.Mid(7,4);
	sscanf_s(anglestring,"%X",&temp);
	//���ַ�����Ҫ��Ϊ��ֵ��Ȼ�����10���ǽǶ�
	float angle=temp/10.0f;
	m_CurrentAngle.Format("%.1f��",angle);
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
	databuf.SetSize(19);//˳ʱ��ת
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
		AfxMessageBox("˳ʱ��ת����������!");
	}
}
void CTurnTable::StopRotateRight()
{
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	databuf.SetSize(19);//˳ʱ��ͣ
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
		AfxMessageBox("˳ʱ��ͣ��������!");
	}
}
void CTurnTable::RotateLeft()
{
	SetSpeed(m_Speed);
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	databuf.SetSize(19);//��ʱ��ת
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
		AfxMessageBox("��ʱ��ת����������!");
	}
}
void CTurnTable::StopRotateLeft()
{
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	databuf.SetSize(19);//��ʱ��ͣ
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
		AfxMessageBox("��ʱ��ͣ��������!");
	}
}
void CTurnTable::StopRotate()
{
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	databuf.SetSize(19);//ֹͣת��
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
		AfxMessageBox("ֹͣת����������!");
	}
}
void CTurnTable::CancelCurrentZero()
{
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	databuf.SetSize(19);//ȡ����ǰ���
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
		AfxMessageBox("ȡ����ǰ����������!");
	}
}
void CTurnTable::SetCurrentPosZero()
{
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	databuf.SetSize(19);//�赱ǰλ��Ϊ���
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
		AfxMessageBox("���õ�ǰλ��Ϊ����������!");
	}
}
void CTurnTable::StopRotateBacktoZero()
{
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	databuf.SetSize(19);//ֹͣ�����
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
		AfxMessageBox("ֹͣ������������!");
	}
}
void CTurnTable::RotateBacktoZero()
{
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	databuf.SetSize(19);//�ص��ο����
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
		AfxMessageBox("�ص��ο�����������!");
	}
}
void CTurnTable::SetAuto()
{
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	databuf.SetSize(19);//��Ϊ�Զ�
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
		AfxMessageBox("��Ϊ�Զ���������!");
	}
}
void CTurnTable::SetTargetAngle(int targetangle)
{
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	char strangle[9];
	databuf.SetSize(21);//����Ŀ��Ƕ�
	databuf.SetAt(0,64);
	databuf.SetAt(1,48);
	databuf.SetAt(2,49);
	databuf.SetAt(3,87);
	databuf.SetAt(4,68);
	databuf.SetAt(5,48);
	databuf.SetAt(6,50);
	databuf.SetAt(7,55);
	databuf.SetAt(8,52);
	sprintf_s(strangle,"%08X",targetangle*10);//ת���Ƕ�ʱ�и�10����ϵ
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
		AfxMessageBox("����Ŀ��ǶȲ�������!");
	}
}
void CTurnTable::PositionStart()
{
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	databuf.SetSize(19);//��λ����
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
		AfxMessageBox("��λ������������!");
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
	if(str=="˳ʱ��ת")
	{
		RotateRight();
		GetDlgItem(IDC_RotateRight)->SetWindowTextA("˳ʱ��ͣ");
	}
	else if(str=="˳ʱ��ͣ")
	{
		StopRotateRight();
		GetDlgItem(IDC_RotateRight)->SetWindowTextA("˳ʱ��ת");
	}
}


void CTurnTable::OnBnClickedRotateleft()
{
	// TODO: Add your control notification handler code here
	CString str;
	SetManual();
	m_Speed=GetDlgItemInt(IDC_Speed);
	GetDlgItemTextA(IDC_RotateLeft,str);
	if(str=="��ʱ��ת")
	{
		RotateLeft();
		GetDlgItem(IDC_RotateLeft)->SetWindowTextA("��ʱ��ͣ");
	}
	else if(str=="��ʱ��ͣ")
	{
		StopRotateLeft();
		GetDlgItem(IDC_RotateLeft)->SetWindowTextA("��ʱ��ת");
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
	stemp.Format("����ʱ��Ϊ %.4f s",(double)(end-start)/CLOCKS_PER_SEC);
	MessageBox(stemp);

}

void CTurnTable::OnPaint()//��turntable���ػ溯�����������ʾ�Ƕȵĺ�������֪��������ʱ���ܲ���һֱ�����仯
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
