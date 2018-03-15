// Measure.cpp : implementation file
//

#include "stdafx.h"
#include "AcousticTest.h"
#include "Measure.h"
#include "afxdialogex.h"
#include "SelectDlg.h"
//...My code...
#include "SetSigDlg.h"
#include "SetScopDlg.h"
#include "SetConditionDlg.h"
#include "MyFunction.h"
#include "ChangeSig.h"

//excel��Ҫ
#include "CApplication.h"
#include "CRange.h"
#include "CWorkbook.h"
#include "CWorkbooks.h"
#include "CWorksheet.h"
#include "CWorksheets.h"

#include <vector>
using namespace std;


vector<vector<float>>Result(4,vector<float>(0));
vector<vector<float>>MulSensity(4,vector<float>(0));
vector<float> MeaAngle;
float zoomPosition[4]={-1,-1,-1,-1},zoomRange[4]={-1,-1,-1,-1};
bool isTimer[5]={false,false,false,false,false};
bool isMeasure=true;
ViSession rm,U2751;

//...end...

// CMeasure dialog

IMPLEMENT_DYNAMIC(CMeasure, CDialogEx)

CMeasure::CMeasure(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMeasure::IDD, pParent)
	, m_DirPic(0)
{
	pturntable=NULL;
}

CMeasure::~CMeasure()
{
}

void CMeasure::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MSCOM, m_com);
	//  DDX_Text(pDX, IDC_Angle, m_Angle);
	DDX_Radio(pDX, IDC_dirPic, m_DirPic);
}


BEGIN_MESSAGE_MAP(CMeasure, CDialogEx)
	ON_COMMAND(ID_SetSig, &CMeasure::OnSetsig)
	ON_COMMAND(ID_SetScop, &CMeasure::OnSetscop)
	ON_COMMAND(ID_SetCondition, &CMeasure::OnSetcondition)
	ON_BN_CLICKED(IDC_View, &CMeasure::OnBnClickedView)
	ON_COMMAND(ID_BackSelect, &CMeasure::OnBackselect)
	ON_BN_CLICKED(IDC_StartMea, &CMeasure::OnBnClickedStartmea)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_StopMea, &CMeasure::OnBnClickedStopmea)
	ON_BN_CLICKED(IDC_quitSys, &CMeasure::OnBnClickedquitsys)
	ON_COMMAND(ID_save, &CMeasure::Onsave)
	ON_COMMAND(ID_turntable, &CMeasure::Onturntable)
	ON_BN_CLICKED(IDC_ChangeSignal, &CMeasure::OnBnClickedChangesignal)
END_MESSAGE_MAP()


// CMeasure message handlers

//...My code...
void renew();
void huatu_sensity();
void Capture(vector<int> cha,int count);
int MeasureSensity();
int MeasureResponse();
void huatu_response();
int MeasureDir();
void huatu_recidir();
int MeaMulDir();
int huatu_muldir();
int MeaHuyi();
void huatu_huyi();
float autoV(int chann);

void MeaSetManual();
float MeaReadCurrentAngle();
void MeaRotateRight(int speed);
void MeaRotateLeft(int speed);
void MeaRotateTargetAngle(int speed,int targetangle);
void MeaStopRotateRight();
void MeaStopRotateLeft();
void MeaStopRotate();
BOOL CMeasure::OnInitDialog()//���ضԻ���ʱ�ĳ�ʼ������
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	ModifyStyleEx(WS_EX_TOOLWINDOW, WS_EX_APPWINDOW);//���г����ʱ�������ʾͼ��
	//CRect rect;  
	//GetDlgItem(IDC_picture)->GetClientRect(&rect);
	//GetDlgItem(IDC_picture)->MoveWindow(50,50,800,600,true); 
	//�̶�Picture Control�ؼ���λ�úʹ�С 
	 if(ChooseItem==5)
	 {
		 viOpenDefaultRM(&rm);
		 status=viOpen(rm,"USB0::0x0957::0x3D18::MY51380004::0::INSTR",VI_NULL,VI_NULL,&U2751);
		 for(int i=0;i<3;i++)
			 isChaChoose[i]=true;
	 }
	 if(ChooseItem==2||ChooseItem==3)
	 {
		 GetDlgItem(IDC_group)->EnableWindow(TRUE);
		 GetDlgItem(IDC_dirPic)->EnableWindow(TRUE);
		 GetDlgItem(IDC_dirP)->EnableWindow(TRUE);
	 }
	 else
	 {
		 m_DirPic=1;
		 UpdateData(false);
		 GetDlgItem(IDC_group)->EnableWindow(FALSE);
		 GetDlgItem(IDC_dirPic)->EnableWindow(FALSE);
		 GetDlgItem(IDC_dirP)->EnableWindow(FALSE);
	 }
	 SetDlgItemTextA(IDC_showPara,"\r\n\r\n\r\n\r\n�ȴ�����......");
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
void CMeasure::OnSetsig()
{
	// TODO: Add your command handler code here
	CSetSigDlg setsig;
	setsig.DoModal();
}
void CMeasure::OnSetscop()
{
	// TODO: Add your command handler code here
	CSetScopDlg setscop;
	setscop.DoModal();
}
void CMeasure::OnSetcondition()
{
	// TODO: Add your command handler code here
	CSetConditionDlg setcon;
	setcon.DoModal();
}
void CMeasure::OnBackselect()
{
	// TODO: Add your command handler code here
	//this->SendMessage(WM_CLOSE);
	OnCancel();
}

void CMeasure::Onturntable()
{
	// TODO: �ڴ���������������
	if(pturntable!=NULL) delete pturntable;
	pturntable = new CTurnTable(); //��ָ������ڴ� 
	pturntable->Create(IDD_turntable); //����һ����ģ̬�Ի���  
	pturntable->ShowWindow(SW_SHOW); //��ʾ��ģ̬�Ի��� 
	

}
void autoScale(int chaflag,int cha,int chacount)
{
	float vRange=-1,vTemp=-1;
	int flag=0;
	
	viPrintf(vip,":measure:source channel%d\n",cha);
	viQueryf(vip,":channel%d:range?\n","%f\n",cha,&vRange);//�������ĵ�ѹ��Χ
	viQueryf(vip,":measure:vpp?\n","%f\n",&vTemp);//�������ֵ
			
	while(vTemp==-1||vRange==-1)
	{
		if(flag>2) 
		{
			AfxMessageBox("ʾ����û���źţ�");
			return;
		}
		flag++;
		viQueryf(vip,":channel%d:range?\n","%f\n",cha,&vRange);
		viQueryf(vip,":measure:vpp?\n","%f\n",&vTemp);
	}
	flag=0;
	while(vTemp>9e+37)//������ʾ��������Ļ��
	{
		if(flag>10||2*vRange>40) 
		{
			viPrintf(vip,":channel%d:range 40\n",cha);
			viQueryf(vip,":channel%d:range?\n","%f\n",cha,&vRange);
			viQueryf(vip,":measure:vpp?\n","%f\n",&vTemp);
			break;
		}
		flag++;
		viPrintf(vip,":channel%d:range %f\n",cha,2*vRange);//���������ѹ��Χ
		viQueryf(vip,":channel%d:range?\n","%f\n",cha,&vRange);
		viQueryf(vip,":measure:vpp?\n","%f\n",&vTemp);
	}
	flag=0;
			
	//������ʾ�����ĸ�
	while(vTemp>vRange/8.0*4) 
	{
		if(2*vRange>40) 
		{
			viPrintf(vip,":channel%d:range 40\n",cha);
			viQueryf(vip,":channel%d:range?\n","%f\n",cha,&vRange);
			viQueryf(vip,":measure:vpp?\n","%f\n",&vTemp);
			break;
		}
		viPrintf(vip,":channel%d:range %f\n",cha,2*vRange);
		viQueryf(vip,":channel%d:range?\n","%f\n",cha,&vRange);
		viQueryf(vip,":measure:vpp?\n","%f\n",&vTemp);
	}
	//������ʾС��һ��
	while(vTemp<vRange/8.0)
	{
		if(vRange/2<0.016) 
		{
			viPrintf(vip,":channel%d:range 0.016\n",cha);
			viQueryf(vip,":channel%d:range?\n","%f\n",cha,&vRange);
			viQueryf(vip,":measure:vpp?\n","%f\n",&vTemp);
			break;
		}
		viPrintf(vip,":channel%d:range %f\n",cha,vRange/2);
		viQueryf(vip,":channel%d:range?\n","%f\n",cha,&vRange);
		viQueryf(vip,":measure:vpp?\n","%f\n",&vTemp);
	}
	//viQueryf(vip,":channel%d:range?\n","%f\n",i+1,&vRange);
	switch(chacount)
	{
	case 1:
		viPrintf(vip,":channel%d:offset 0\n",cha);//ֻ��һ��ͨ�������м���ʾ
		break;
	case 2:
		if(chaflag%2==1) viPrintf(vip,":channel%d:offset %f\n",cha,vRange/2-vRange/4);
		else viPrintf(vip,":channel%d:offset %f\n",cha,vRange/2-3*vRange/4);
		break;
	case 3:
		if(chaflag%3==1) viPrintf(vip,":channel%d:offset %f\n",cha,vRange/2-vRange/6);
		else if(chaflag%3==2) viPrintf(vip,":channel%d:offset %f\n",cha,vRange/2-3*vRange/6);
		else viPrintf(vip,":channel%d:offset %f\n",cha,vRange/2-5*vRange/6);
		break;
	case 4:
		if(chaflag%4==1) viPrintf(vip,":channel%d:offset %f\n",cha,vRange/2-vRange/8);
		else if(chaflag%4==2) viPrintf(vip,":channel%d:offset %f\n",cha,vRange/2-3*vRange/8);
		else if(chaflag%4==3)viPrintf(vip,":channel%d:offset %f\n",cha,vRange/2-5*vRange/8);
		else viPrintf(vip,":channel%d:offset %f\n",cha,vRange/2-7*vRange/8);
		break;
	}
}
float autoV(int chann)
{
	float vrange=-1,vtemp=-1;
	int flag=0;
	
	//viQueryf(vip,":measure:vrms?\n","%f\n",&vtemp);
	viQueryf(vip,":measure:vpp?\n","%f\n",&vtemp);
	viQueryf(vip,":channel%d:range?\n","%f\n",chann,&vrange);
	
	while(vtemp==-1||vrange==-1||vtemp==0||vrange==0)
	{
		if(flag>2) break;
		flag++;
		viQueryf(vip,":channel%d:range?\n","%f\n",chann,&vrange);
		//viQueryf(vip,":measure:vrms?\n","%f\n",&vtemp);
		viQueryf(vip,":measure:vpp?\n","%f\n",&vtemp);
	}

	while(vtemp>9.0e+036) 
	{
		viPrintf(vip,":channel%d:range %f\n",chann,2*vrange);
		viQueryf(vip,":channel%d:range?\n","%f\n",chann,&vrange);
		//viQueryf(vip,":measure:vrms?\n","%f\n",&vtemp);
		viQueryf(vip,":measure:vpp?\n","%f\n",&vtemp);
	}
	while(vtemp>vrange/8.0*4) 
	{
		viPrintf(vip,":channel%d:range %f\n",chann,2*vrange);
		viQueryf(vip,":channel%d:range?\n","%f\n",chann,&vrange);
		//viQueryf(vip,":measure:vrms?\n","%f\n",&vtemp);
		viQueryf(vip,":measure:vpp?\n","%f\n",&vtemp);
	}
	while(vtemp>9.0e+036) 
	{
		viPrintf(vip,":channel%d:range %f\n",chann,vrange/2);
		viQueryf(vip,":channel%d:range?\n","%f\n",chann,&vrange);
		//viQueryf(vip,":measure:vrms?\n","%f\n",&vtemp);
		viQueryf(vip,":measure:vpp?\n","%f\n",&vtemp);
	}
	//������ʾС��һ��
	while(vtemp<vrange/16.0)
	{
		if(vrange/2<0.016) 
		{
			viPrintf(vip,":channel%d:range 0.016\n",chann);
			viQueryf(vip,":channel%d:range?\n","%f\n",chann,&vrange);
			//viQueryf(vip,":measure:vrms?\n","%f\n",&vtemp);
			viQueryf(vip,":measure:vpp?\n","%f\n",&vtemp);
			break;
		}
		viPrintf(vip,":channel%d:range %f\n",chann,vrange/2);
		viQueryf(vip,":channel%d:range?\n","%f\n",chann,&vrange);
		//viQueryf(vip,":measure:vrms?\n","%f\n",&vtemp);
		viQueryf(vip,":measure:vpp?\n","%f\n",&vtemp);
	}
	while(vtemp>9.0e+036) 
	{
		viPrintf(vip,":channel%d:range %f\n",chann,vrange*2);
		viQueryf(vip,":channel%d:range?\n","%f\n",chann,&vrange);
		//viQueryf(vip,":measure:vrms?\n","%f\n",&vtemp);
		viQueryf(vip,":measure:vpp?\n","%f\n",&vtemp);
	}
	//viQueryf(vip,":measure:vrms?\n","%f\n",&vtemp);//��Чֵ���ܼ�̲���Ӱ��
	return vtemp;
}
void CMeasure::OnBnClickedView()
{
	// TODO: Add your control notification handler code here
	SetDlgItemTextA(IDC_Show,"���ڵ����źţ����Ժ�......");
	renew();
	
	//�Զ����������ʵ���ʾ����
	int chflag=0,chCount=0;	
	
	for(int i=0;i<4;i++)
	{
		if(isChaChoose[i]) {
			chCount++;
			viPrintf(vip,":channel%d:display on\n",i+1);//��ͨ��
			viPrintf(vip,":channel%d:offset 0\n",i+1);			
		}
		else viPrintf(vip,":channel%d:display off\n",i+1);
	}
	if(chCount==0)//���û��ѡ��ʹ�õ�ͨ������ʾ������
	{
		AfxMessageBox("����ʾ��������������ѡ�����ͨ����");
		viPrintf(vip,"*rst\n");
		SetDlgItemTextA(IDC_Show,"");
		return;
	}
	if(ChooseItem==5)
	{
		if(status!=0)
		{
			AfxMessageBox("���ؾ�����ͨ����ȷ��");
			viClose(U2751);
			viClose(rm);
			SetDlgItemTextA(IDC_Show,"");
			return;
		}
		viClear(U2751);
		viPrintf(U2751,"*rst\n");//���ؾ���λ
		viPrintf(U2751,"*cls\n");//���ؾ���
		CreateBurst(f,v/1000,Bwid/1000,Brep);
		Sleep(100);
		viPrintf(U2751,"ROUTe:CLOSe (@402,107,304)\n");
		//��ͨ402��107��304,���任��������ˮ�����գ����׻�������
		ScopeTrigger();
		viPrintf(vip,"timebase:range %f\n",(Bwid/1000.0)*4);//����ʱ��������ʱ�䳤��
		autoScale(1,1,3);//������1����ʾ����1ͨ����һ����3��ͨ����ˮ�������յ�ͨ��
		autoScale(2,2,3);//���׻��������յ�ͨ��
		autoScale(3,3,3);//�����ƽ��յ�ͨ��
	}
	else
	{
		if(ChooseItem==4)
		{
			CreateMulFrePulse(Fs,f,deltaf,Bwid/1000,v/1000,Brep);
			Sleep(100);
			ScopeTrigger();
			viPrintf(vip,"timebase:range %f\n",(Bwid/1000.0)*6*PulseCount);//����ʱ��������ʱ�䳤��
		}
		else 
		{
			CreateBurst(f,v/1000,Bwid/1000,Brep);
			Sleep(100);
			ScopeTrigger();
			viPrintf(vip,"timebase:range %f\n",(Bwid/1000.0)*4);//����ʱ��������ʱ�䳤��
		}
		for(int i=0;i<4;i++)
		{
			if(isChaChoose[i])
			{
				chflag++;
				autoScale(chflag,i+1,chCount);
			}
		}
	}
	SetDlgItemTextA(IDC_Show,"����ʾ�����Ϲ۲��ź�......");
}
void CMeasure::OnBnClickedChangesignal()
{
	// TODO: Add your control notification handler code here
	if(!isChange)
	{
		CChangeSig *signal=new CChangeSig();
		signal->Create(IDD_ChangeSig); //����һ����ģ̬�Ի���  
		signal->ShowWindow(SW_SHOW); //��ʾ��ģ̬�Ի��� 
	}
}
void CMeasure::OnBnClickedStartmea()
{
	// TODO: Add your control notification handler code here
	renew();
	if(ChooseItem==0||ChooseItem==1) 
	{
		if(standMp.empty())
		{
			AfxMessageBox("��ѡ���׼ˮ�����ļ���");
			return;
		}
		if(startf<(standMp.begin()->first)||endf>(standMp.rbegin()->first))
		{
			AfxMessageBox("����Ƶ�ʷ�Χ������׼ˮ�����ķ�Χ��");
			return;
		}
	}
	int chcount=0;
	for(int i=0;i<4;i++)
	{
		if(isChaChoose[i])
		{
			chcount++;
		}
	}
	if(0==chcount||(ChooseItem==5&&chcount<3))
	{
		AfxMessageBox("��ѡ��ʾ�����Ĳ���ͨ����");
		return;
	}
	int meaStatus=0;
	CString strshow;
	switch(ChooseItem)
	{
	case 0: //����������
		meaStatus=MeasureSensity();
		if(meaStatus==-1) 
		{
			SetDlgItemTextA(IDC_Show,"��ֹ����������......");
			return;
		}
		SetTimer(1,1000,NULL);
		GetDlgItemTextA(IDC_showPara,strshow);
		if(MeaCount==1)
		{
			for(int ch=0;ch<4;ch++)
			{
				if(isChaChoose[ch]&&ch!=chaRefer-1)
				{
					CString s;
					s.Format("\r\n\r\n�����Ȳ��������\r\n����ˮ����ͨ�� %dͨ��\r\n",ch+1);
					strshow+=s;
					float maxf=startf,maxm=Result[ch][0],minf=startf,minm=Result[ch][0];
					for(unsigned int i=1;i<Result[ch].size();i++)
					{
						if(maxm<Result[ch][i]) {
							maxm=Result[ch][i];
							if(OneThird_f)
								maxf=OneThirdFreq[OTFreq+i];
							else maxf=startf+i*deltaf;
						}
						if(minm>Result[ch][i]) {
							minm=Result[ch][i];
							if(OneThird_f)
								minf=OneThirdFreq[OTFreq+i];
							else minf=startf+i*deltaf;
						}
					}
					s.Format("��������ȼ�\r\n  %.2fdB @ %.2fHz\r\n��С�����ȼ�\r\n  %.2fdB @ %.2fHz",maxm,maxf,minm,minf);
					strshow+=s;
				}				
			}
			SetDlgItemTextA(IDC_showPara,strshow);
		}		
		break;
	case 1://�������͵�ѹ��Ӧ
		meaStatus=MeasureResponse();
		if(meaStatus==-1) 
		{
			SetDlgItemTextA(IDC_Show,"��ֹ�������͵�ѹ��Ӧ......");
			return;
		}
		SetTimer(2,1000,NULL);
		GetDlgItemTextA(IDC_showPara,strshow);
		if(MeaCount==1)
		{
			for(int ch=0;ch<4;ch++)
			{
				if(isChaChoose[ch]&&ch!=chaRefer-1)
				{
					CString s;
					s.Format("\r\n���͵�ѹ��Ӧ���������\r\n���ⷢ�任����ͨ�� %dͨ��\r\n",ch+1);
					strshow+=s;
					float maxf=startf,maxm=Result[ch][0],minf=startf,minm=Result[ch][0];
					for(unsigned int i=1;i<Result[ch].size();i++)
					{
						if(maxm<Result[ch][i]) {
							maxm=Result[ch][i];
							maxf=startf+i*deltaf;
						}
						if(minm>Result[ch][i]) {
							minm=Result[ch][i];
							minf=startf+i*deltaf;
						}
					}
					s.Format("����͵�ѹ��Ӧ��\r\n  %.2fdB @ %.2fHz\r\n��С���͵�ѹ��Ӧ��\r\n  %.2fdB @ %.2fHz",maxm,maxf,minm,minf);
					strshow+=s;
				}				
			}
			SetDlgItemTextA(IDC_showPara,strshow);
		}
		break;
	case 2://��������ָ����
	case 3://��������ָ����
		m_DirPic=1;
		UpdateData(false);
		meaStatus=MeasureDir();
		if(meaStatus==-1) 
		{
			SetDlgItemTextA(IDC_Show,"��ֹ��Ƶ��ָ���Բ���......");
			return;
		}
		SetTimer(3,1000,NULL);
		GetDlgItemTextA(IDC_showPara,strshow);
		for(int ch=0;ch<4;ch++)
		{
			if(isChaChoose[ch])
			{
				CString s;
				s.Format("\r\n\r\nָ���Բ��������\r\n���⻻����ͨ�� %dͨ��\r\n",ch+1);
				strshow+=s;
				float maxa=MeaAngle[0],maxm=Result[ch][0],a[6]={0,0,0,0,0,0};
				unsigned int k=0;
				for(unsigned int i=1;i<Result[ch].size();i++)
				{
					if(maxm<Result[ch][i]) {
						maxm=Result[ch][i];
						maxa=MeaAngle[i];
						k=i;
					}
				}
				for(unsigned int j=k;j<Result[ch].size();j++)
				{
					int temp=(int)(20*log10(Result[ch][j]/maxm));
					if(temp==-2) a[0]=MeaAngle[j];
					if(temp==-3) 
					{
						a[1]=MeaAngle[j];break;//��ȷ���Ƿ������-3dB��ֵ
					}
					if(temp==-4) a[2]=MeaAngle[j];				
				}
				for(unsigned int j=0;j<k;j++)
				{
					int temp=(int)(20*log10(Result[ch][j]/maxm));
					if(temp==-2) a[5]=MeaAngle[j];
					if(temp==-3) 
					{
						a[4]=MeaAngle[j];break;
					}
					if(temp==-4) a[3]=MeaAngle[j];				
				}
				float da1,da2;
				if((a[1]-0.0>=0.001)&&(a[4]-0.0>=0.001)) {da1=a[1];da2=a[4];}
				else if((a[1]-0.0>0.001)&&(a[4]-0.0<0.001)) {da1=a[1];da2=(a[3]+a[5])/2;}
				else if((a[1]-0.0<0.001)&&(a[4]-0.0>0.001)) {da1=(a[0]+a[2])/2;da2=a[4];}
				else {da1=(a[0]+a[2])/2;da2=(a[3]+a[5])/2;}
				s.Format("����ĽǶ�λ�� %.1f��\r\n-3dB������� %.1f��",maxa,abs(da1-da2));
				strshow+=s;
			}				
		}
		SetDlgItemTextA(IDC_showPara,strshow);
		break;
	case 4://��Ƶ�����ָ����
		meaStatus=MeaMulDir();
		if(meaStatus==-1) 
		{
			SetDlgItemTextA(IDC_Show,"��ֹ��Ƶ�����ָ����......");
			return;
		}
		SetTimer(4,1000,NULL);
		break;
	case 5://���׷��Զ�����������
		meaStatus=MeaHuyi();
		if(meaStatus==-1) 
		{
			SetDlgItemTextA(IDC_Show,"��ֹ���׷��Զ�����������......");
			return;
		}
		SetTimer(5,1000,NULL);
		break;
	}
	SetDlgItemTextA(IDC_Show,"�������......");
}

void CMeasure::OnBnClickedStopmea()
{
	// TODO: Add your control notification handler code here
	isMeasure=false;
	//CreateMulFrePulse(Fs,5000,1000,Bwid/1000,0.5,1);
	//huatu_sensity();
	//huatu_response();
	//huatu_huyi();
	//huatu_recidir();
	//huatu_dB();
	//huatu_muldir();

}

void CMeasure::OnBnClickedquitsys()
{
	// TODO: Add your control notification handler code here
	for(int i=0;i<5;i++)
	{
		if(isTimer[i]) KillTimer(i+1);
	}
	viClose(vip);
	viClose(vig);
	viClose(vidp);
	viClose(vidg);
	if(ChooseItem==5&&status==0)
	{
		viPrintf(U2751,"*rst\n");//���ؾ���λ
		viClose(U2751);
		viClose(rm);
	}
	OnCancel();//��Ϊ���Ƿ�ģ̬�Ի����Լ��������������	
}
void CMeasure::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
	CDialog::PostNcDestroy();
	delete pturntable;
	delete this;
}
void CMeasure::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	viClose(U2751);
	viClose(rm);
	CWnd::DestroyWindow();
}
void CMeasure::Onsave()
{
	// TODO: Add your command handler code here
	CApplication app;  
    CWorkbooks books;  
    CWorkbook book;  
    CWorksheets sheets;  
    CWorksheet sheet;  
    CRange range;
	if(!app.CreateDispatch("Excel.Application"))  
    {
		AfxMessageBox("�޷�����Excel������!");  
		return;  
    } 
	app.put_Visible(true);
	books.AttachDispatch(app.get_Workbooks());
	//�õ�Workbook  
    book.AttachDispatch(books.Add(vtMissing));  
    //�õ�Worksheets  
    sheets.AttachDispatch(book.get_Worksheets());
	sheet.AttachDispatch(sheets.get_Item(variant_t("sheet1")));
	CString strPage="��1ҳ";
	sheet.put_Name(strPage);
	for(int i=0;i<sheets.get_Count()-1;i++)
	{
		sheet=sheet.get_Next();
		strPage.Format("��%dҳ",i+2);
		//Ϊÿ��ҳ����������
		sheet.put_Name(strPage);
	}
	sheet.AttachDispatch(sheets.get_Item(_variant_t("��1ҳ")));
	range.AttachDispatch(sheet.get_Cells());
	switch(ChooseItem)
	{
	case 0:
		range.put_Item(_variant_t((long)1),_variant_t((long)1),
			_variant_t("�ȽϷ����������ȼ�"));
		range.put_Item(_variant_t((long)2),_variant_t((long)1),
			_variant_t("Ƶ�ʣ�Hz)"));
		range.put_Item(_variant_t((long)2),_variant_t((long)2),
				_variant_t("�����ȼ�(dB)"));
		break;
	case 1:
		range.put_Item(_variant_t((long)1),_variant_t((long)1),
			_variant_t("�ȽϷ����������ѹ��Ӧ��"));
		range.put_Item(_variant_t((long)2),_variant_t((long)1),
			_variant_t("Ƶ�ʣ�Hz)"));
		range.put_Item(_variant_t((long)2),_variant_t((long)2),
				_variant_t("�����ѹ��Ӧ��(dB)"));
		break;
	case 2:
		range.put_Item(_variant_t((long)1),_variant_t((long)1),
			_variant_t("��Ƶ����ջ�����ָ����"));
		range.put_Item(_variant_t((long)2),_variant_t((long)1),
			_variant_t("�Ƕ�(��)"));
		range.put_Item(_variant_t((long)2),_variant_t((long)2),
				_variant_t("��ѹ(V)"));
		break;
	case 3:
		range.put_Item(_variant_t((long)1),_variant_t((long)1),
			_variant_t("��Ƶ�㷢�任����ָ����"));
		range.put_Item(_variant_t((long)2),_variant_t((long)1),
			_variant_t("�Ƕ�(��)"));
		range.put_Item(_variant_t((long)2),_variant_t((long)2),
				_variant_t("��ѹ(V)"));
		break;
	case 4:
		range.put_Item(_variant_t((long)1),_variant_t((long)1),
			_variant_t("��Ƶ�㻻����ָ����"));
		range.put_Item(_variant_t((long)2),_variant_t((long)1),
			_variant_t("�Ƕ�(��)"));
		range.put_Item(_variant_t((long)2),_variant_t((long)2),
				_variant_t("��ѹ(V)"));
		break;
	case 5:
		range.put_Item(_variant_t((long)1),_variant_t((long)1),
			_variant_t("���׷����������ȼ�"));
		range.put_Item(_variant_t((long)2),_variant_t((long)1),
			_variant_t("Ƶ�ʣ�Hz)"));
		range.put_Item(_variant_t((long)2),_variant_t((long)2),
				_variant_t("�����ȼ�(dB)"));
		break;

	}

	int col=1;//�ж��е�ʱ��
	if(ChooseItem==5)
	{
		if(OneThird_f)
		{
			for(unsigned int j=0;j<Result[0].size();j++)
			{
				range.put_Item(_variant_t((long)(j+3)),_variant_t((long)1),
					_variant_t(OneThirdFreq[OTFreq+j]));
				//����j+3�ŵĵ�2�����ݻ��3�С���
				range.put_Item(_variant_t((long)(j+3)),_variant_t((long)2),
					_variant_t(Result[0][j]));
			}
		}
		else
		{
			for(unsigned int j=0;j<Result[0].size();j++)
			{
				range.put_Item(_variant_t((long)(j+3)),_variant_t((long)1),
					_variant_t(startf+deltaf*j));
				//����j+3�ŵĵ�2�����ݻ��3�С���
				range.put_Item(_variant_t((long)(j+3)),_variant_t((long)2),
					_variant_t(Result[0][j]));
			}
		}
	}
	else
	{
		for(int i=0;i<4;i++)
		{
			if(isChaChoose[i])
			{
				if(ChooseItem==4)//��Ƶ���ָ����
				{
					for(int j=0;j<PulseCount;j++)
					{
						if(Result[j].size()==0)continue;
						for(unsigned int k=0;k<Result[j].size();k++)
						{
							range.put_Item(_variant_t((long)(k+3)),_variant_t((long)1),
							_variant_t(MeaAngle[k]));
							//����k+3�ŵĵ�2�����ݻ��3�С���
							range.put_Item(_variant_t((long)(k+3)),_variant_t((long)(2*j+2)),
							_variant_t(Result[j][k]));
						}
					}
				}
				else if((ChooseItem==0||ChooseItem==1)&&MeaCount>1)//�����Ⱥͷ�����Ӧ��β���
				{
					if(MulSensity[i].size()==0) continue;
					unsigned int sz=MulSensity[i].size()/MeaCount;
					for(int c=0;c<MeaCount;c++)
					{
						col++;
						for(unsigned int j=0;j<sz;j++)
						{
							if(col==2)
							{
								if(OneThird_f)
								range.put_Item(_variant_t((long)(j+3)),_variant_t((long)1),
									_variant_t(OneThirdFreq[OTFreq+j]));
								else range.put_Item(_variant_t((long)(j+3)),_variant_t((long)1),
								_variant_t(startf+deltaf*j));
							}
							//����j+3�ŵĵ�2�����ݻ��3�С���
							range.put_Item(_variant_t((long)(j+3)),_variant_t((long)col),
							_variant_t(MulSensity[i][j+c*sz]));
						}
					}
				}
				else
				{
					if(Result[i].size()==0)continue;
					col++;
					for(unsigned int j=0;j<Result[i].size();j++)
					{	
						//����j+3�ŵĵ�1������
						switch(ChooseItem)
						{
						case 0:
						case 1:
							if(OneThird_f)
								range.put_Item(_variant_t((long)(j+3)),_variant_t((long)1),
									_variant_t(OneThirdFreq[OTFreq+j]));
							else range.put_Item(_variant_t((long)(j+3)),_variant_t((long)1),
							_variant_t(startf+deltaf*j));
							//����j+3�ŵĵ�2�����ݻ��3�С���
							range.put_Item(_variant_t((long)(j+3)),_variant_t((long)col),
							_variant_t(Result[i][j]));
							break;
						case 2:
						case 3:
							range.put_Item(_variant_t((long)(j+3)),_variant_t((long)1),
							_variant_t(MeaAngle[j]));
							//����j+3�ŵĵ�2�����ݻ��3�С���
							range.put_Item(_variant_t((long)(j+3)),_variant_t((long)col),
							_variant_t(Result[i][j]));
							break;
						}			
		
					}
				}
			}
		}
	}
	///*CString strPath;*/
	//char szPath[MAX_PATH];
	//GetCurrentDirectory(MAX_PATH,szPath);
	////strPath=szPath;
	////strPath+="\\��������.xls";
	////����excel�ļ�
	//sheet.SaveAs(szPath,vtMissing,vtMissing,vtMissing,vtMissing,
	//	vtMissing,vtMissing,vtMissing,vtMissing,vtMissing);
	//
	//�ͷŶ���    
    range.ReleaseDispatch();  
    sheet.ReleaseDispatch();  
    sheets.ReleaseDispatch();  
    book.ReleaseDispatch();  
    books.ReleaseDispatch();  
    app.ReleaseDispatch(); 
}

void CMeasure::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	UpdateData(true);
	switch(nIDEvent)
	{
	case 1:
		huatu_sensity();isTimer[0]=true;break;
	case 2:
		huatu_response();isTimer[1]=true;break;
	case 3:
		if(m_DirPic==0)huatu_recidir();
		else huatu_dB();
		isTimer[2]=true;break;
	case 4:
		huatu_muldir();isTimer[3]=true;break;
	case 5:
		huatu_huyi();isTimer[4]=true;break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CMeasure::renew()
{
	f=startf;
	isMeasure=true;
	MeaAngle.clear();
	for(int i=0;i<4;i++)
	{
		u[i]=-1.0;
		zoomPosition[i]=-1.0;
		zoomRange[i]=-1.0;
		Result[i].clear();
		MulSensity[i].clear();
	}
	CWnd *pWnd=GetDlgItem(IDC_picture);
	pWnd->UpdateWindow();
	CDC *pDC=pWnd->GetDC();
	CBrush rebrush;
	rebrush.CreateSolidBrush (RGB(255,255,255));
	CBrush *pOldBrush=pDC->SelectObject (&rebrush);
	CRect rect;
	pWnd->GetClientRect(rect);
	pDC->Rectangle (rect);
	pDC->SelectObject (pOldBrush);
}
void CMeasure::Capture(vector<int> cha,int count)
{
	//viPrintf(vip,":acquire:type normal\n");
	/*viPrintf(vip, ":acquire:type average\n");
	viPrintf(vip, ":acquire:count 1\n");*/
	viPrintf(vip, ":acquire:type hresolution\n");
	viPrintf(vip,":acquire:complete 100\n");
	//viPrintf(vip,":digitize channel1\n");

	switch(count)
	{
	case 1:
		viPrintf(vip,":digitize channel%d\n",cha[0]);break;
	case 2:
		viPrintf(vip,":digitize channel%d,channel%d\n",cha[0],cha[1]);break;
	case 3:
		viPrintf(vip,":digitize channel%d,channel%d,channel%d\n",cha[0],cha[1],cha[2]);break;
	case 4:
		viPrintf(vip,":digitize channel%d,channel%d,channel%d,channel%d\n",cha[0],cha[1],cha[2],cha[3]);break;
	}
}
int CMeasure::MeasureSensity()
{
	SetDlgItemTextA(IDC_Show,"׼�����������Ժ�......");
	float Mp=-1;
	CreateBurst(f,v/1000,Bwid/1000,Brep);
	MessageBox("�������ʾѡ�����ͨ���Ĳ�������");
	viPrintf(vip,":timebase:mode window\n");
	for(int i=0;i<4;i++)
	{
		if(isChaChoose[i])
		{
			CString s;
			s.Format("���ѡ��ͨ��%d�Ĳ����������ȷ��",i+1);
			MessageBox(s);
			viQueryf(vip,":timebase:window:position?\n","%f\n",&zoomPosition[i]);
			viQueryf(vip,":timebase:window:range?\n","%f\n",&zoomRange[i]);
		}
	}
	viPrintf(vip,":timebase:mode main\n");
	viPrintf(vip,":run\n");
	CString stemp;
	stemp.Format("Ƶ�ʷ�Χ %.1fkHz~%.1fkHz\r\n�ź�Դ���� %.1fmVpp\r\n������ %.1fms\r\n�ظ����� %.1fs\r\n�������� %d��\r\n��׼ˮ����ͨ�� %d",
		startf/1000,endf/1000,v,Bwid,Brep,MeaCount,chaRefer);
	if(MessageBox(stemp,"��ʾ",MB_OKCANCEL)==IDCANCEL) return -1;
	SetDlgItemTextA(IDC_showPara,stemp);
	isMeasure=true;
	for(int c=0;isMeasure&&c<MeaCount;c++)
	{
		f=startf;
		for(int i=0;i<4;i++)
		{
			if(isChaChoose[i]) Result[i].clear();
			u[i]=-1;
		}
		if(MeaCount>1)
		{
			stemp.Format("���ڽ��е� %d �β��������Ժ�......",c+1);
			SetDlgItemTextA(IDC_Show,stemp);
		}
		else SetDlgItemTextA(IDC_Show,"���ڲ��������ȣ����Ժ�......");
		unsigned int count_freq=0;
		while(isMeasure&&f<=endf)
		{
			//�������ա�ֹͣ��������ť���µ���Ϣ
			MSG  msg;
			if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))          
			{          
				if (msg.message == MAKEWPARAM(IDC_StopMea, BN_CLICKED)) 
				{
					isMeasure=false;
					break ; 
				}
				TranslateMessage (&msg) ;          
				DispatchMessage (&msg) ;          
			} 

			viPrintf(vip,":run\n");
			CreateBurst(f,v/1000,Bwid/1000,Brep);
			viPrintf(vip,":timebase:mode window\n");
			for(int i=0;i<4;i++)
			{
				if(isChaChoose[i])
				{
					viPrintf(vip,":timebase:window:position %f\n",zoomPosition[i]);
					viPrintf(vip,":timebase:window:range %f\n",zoomRange[i]);
					viPrintf(vip,":measure:source channel%d\n",i+1);
					u[i]=autoV(i+1)/Gain[i];
				}
			}
			for(int i=0;i<4;i++)
			{
				if(!isChaChoose[i]||i==chaRefer-1) continue;
			
				map<float,float>::iterator it=standMp.find(f);
				if(it==standMp.end())
				{
					//Mp=(standMp.lower_bound(f)->second+standMp.upper_bound(f)->second)/2;
					//ʹ�����Բ�ֵ�����Ƶ�ʵ��������ֵ
					Mp=LineP(standMp.lower_bound(f)->first,standMp.lower_bound(f)->second,
						standMp.upper_bound(f)->first,standMp.upper_bound(f)->second,f);
				}
				else
					Mp=it->second;
				Result[i].push_back(CalSensity(Mp,u[i],u[chaRefer-1],d[i],d[chaRefer-1]));
				//����ĵ�ѹֵҪ���ԷŴ���
			}
			huatu_sensity();
			if(OneThird_f)
			{
				count_freq++;
				f=OneThirdFreq[OTFreq+count_freq];
			}
			else f+=deltaf;
		}
		for(int i=0;i<4;i++)
		{
			if(isChaChoose[i]&&i!=chaRefer-1)
				MulSensity[i].insert(MulSensity[i].end(),Result[i].begin(),Result[i].end());
		}
	}

	viPrintf(vip,":timebase:mode main\n");
	return 0;
}
void CMeasure::huatu_sensity()
{
	int x=0,y=0;
	CWnd *pWnd=GetDlgItem(IDC_picture);
	CRect rect;
	pWnd->GetClientRect(rect);
	CDC* pDC=pWnd->GetDC();
	if(MeaCount>1);
	else 
	{
		CBrush rebrush;
		rebrush.CreateSolidBrush (RGB(255,255,255));//��ɫˢ��
		CBrush *pOldBrush=pDC->SelectObject (&rebrush);
		pDC->Rectangle (rect);//���picture�еĻ滭
		pDC->SelectObject (pOldBrush);
	}
	CPen pNewPen;
	pNewPen.CreatePen(PS_SOLID,1,RGB(0,0,0));
	CPen* pOldPen=pDC->SelectObject(&pNewPen);
	int deltaX=rect.Width()/50;
	int deltaY=rect.Height()/100;//100�����ȡ�ģ����������Ϊ100��
	pDC->SetViewportOrg(rect.left,rect.top);//����������ʱ���Ǹ�ֵ,ԭ����Ϊ�����Ͻǵĵ�
	//��������
	CString str;
	float temp=0.0;
	for(x=0;x<=50;x+=1)
	{
		pDC->MoveTo((int)(x*deltaX),0);
		pDC->LineTo((int)(x*deltaX),rect.Height());
	}
	for(x=0;x<=50;x+=5)
	{
		if(endf==startf)
		{
			str.Format("%.1f",(startf+x)/1000);
			pDC->TextOutA((int)(x*deltaX-10),rect.top-15,str);
		}
		else
		{
			temp=startf+(endf-startf)/50*x;
			str.Format("%.1f",temp/1000);
			pDC->TextOutA((int)(x*deltaX-10),rect.top-15,str);
		}
	}
	for(y=0;y<=100;y+=10)
	{
		pDC->MoveTo(rect.left,y*deltaY);
		pDC->LineTo(rect.right,y*deltaY);
		str.Format("%d",-150-y);//�����ʾ-150~-250��100���㣬�������ܹ����ֳ���100������ÿһ�����y
		pDC->TextOutA(rect.left-40,y*deltaY-10,str);
	}
	str.Format("Ƶ��(kHz)");
	pDC->TextOutA(100,-50,str);
	str.Format("��");
	pDC->TextOutA(-80,200,str);
	str.Format("��");
	pDC->TextOutA(-80,220,str);
	str.Format("��");
	pDC->TextOutA(-80,240,str);
	str.Format("(dB)");
	pDC->TextOutA(-85,260,str);
	str.Format("dB re 1/uPa");
	pDC->TextOutA(rect.left+10,rect.bottom+20,str);
	pDC->SelectObject(pOldPen);
	CPen pPen[4];
	pPen[0].CreatePen(PS_SOLID,3,RGB(255,165,0));//��ɫ����
	pPen[1].CreatePen(PS_SOLID,3,RGB(0,255,0));//��ɫ����
	pPen[2].CreatePen(PS_SOLID,3,RGB(0,0,255));//��ɫ����
	pPen[3].CreatePen(PS_SOLID,3,RGB(255,0,0));//��ɫ����

	////debug
	//float Result[10]={-210.7f,-210.6f,-218.1f,-217.6f,-215.8f,-214.9f,-215.3f,-215.4f,-215.9f,-217.0f};
	//pDC->MoveTo(0,(int)((Result[0]+150)*deltaY));
	//for(unsigned int i=1;i<10;i++)
	//{
	//	x=i*deltaX;
	//	y=(int)((Result[i]+150)*deltaY);
	//	pDC->LineTo(x,y);//����������
	//}
	////debug
	for(int ch=0;ch<4;ch++)
	{
		if(isChaChoose[ch])
		{
			pOldPen=pDC->SelectObject(&pPen[ch]);
			if(Result[ch].size()==0) continue;//ע��Ҫ��continue�������breakֱ�Ӿ�����ѭ���ˣ����ử����ͨ����ͼ��
			pDC->MoveTo(0,-(int)((Result[ch][0]+150)*deltaY));//�������Ƶ���㣬������ֵ�Ǹ��ģ���ͼ����������
			if(endf==startf) 
			{
				pDC->SetPixel(0,-(int)((Result[0][0]+150)*deltaY),RGB(255,0,0));
				continue;
			}
			if(OneThird_f)
			{
				for(unsigned int i=1;i<Result[ch].size();i++)
				{

					x=(int)((OneThirdFreq[OTFreq+i]-startf)*50/(endf-startf)*deltaX);
					y=-(int)((Result[ch][i]+150)*deltaY);
					pDC->LineTo(x,y);//����������
				}
			}
			else
			{
				for(unsigned int i=1;i<Result[ch].size();i++)
				{
					x=(int)(i*deltaf*deltaX*50/(endf-startf));
					y=-(int)((Result[ch][i]+150)*deltaY);
					pDC->LineTo(x,y);//����������
				}
			}
		}
	}

	pDC->SelectObject(pOldPen);

}
int CMeasure::MeasureResponse()
{
	SetDlgItemTextA(IDC_Show,"׼�����������Ժ�......");
	float Mp=-1;
	CreateBurst(f,v/1000,Bwid/1000,Brep);
	MessageBox("�������ʾѡ�����ͨ���Ĳ�������");
	viPrintf(vip,":timebase:mode window\n");
	for(int i=0;i<4;i++)
	{
		if(isChaChoose[i])
		{
			CString s;
			s.Format("���ѡ��ͨ��%d�Ĳ����������ȷ��",i+1);
			MessageBox(s);
			viQueryf(vip,":timebase:window:position?\n","%f\n",&zoomPosition[i]);
			viQueryf(vip,":timebase:window:range?\n","%f\n",&zoomRange[i]);
		}
	}
	viPrintf(vip,":timebase:mode main\n");
	viPrintf(vip,":run\n");
	CString stemp;
	stemp.Format("������Ƶ�ʷ�Χ %.1fkHz~%.1fkHz\r\n�ź�Դ���� %.1fmVpp\r\n������ %.1fms\r\n�ظ����� %.1fs\r\n%d�β���\r\n��׼ˮ����ͨ�� %d",
		startf/1000,endf/1000,v,Bwid,Brep,MeaCount,chaRefer);
	if(MessageBox(stemp,"��ʾ",MB_OKCANCEL)==IDCANCEL) return -1;
	SetDlgItemTextA(IDC_showPara,stemp);
	f=startf;
	isMeasure=true;
	for(int c=0;isMeasure&&c<MeaCount;c++)
	{
		f=startf;
		for(int i=0;i<4;i++)
		{
			if(isChaChoose[i]) Result[i].clear();
			u[i]=-1;
		}
		if(MeaCount>1)
		{
			stemp.Format("���ڽ��е� %d �β��������Ժ�......",c+1);
			SetDlgItemTextA(IDC_Show,stemp);
		}
		else SetDlgItemTextA(IDC_Show,"���ڲ������͵�ѹ��Ӧ�����Ժ�......");
		unsigned int count_freq=0;
		while(isMeasure&&f<=endf)
		{
			//�������ա�ֹͣ��������ť���µ���Ϣ
			MSG  msg;
			if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))          
			{          
				if (msg.message == MAKEWPARAM(IDC_StopMea, BN_CLICKED))  
				{
					isMeasure=false;
					break ;
				}
				TranslateMessage (&msg) ;          
				DispatchMessage (&msg) ;          
			} 
			viPrintf(vip,":run\n");
			CreateBurst(f,v/1000,Bwid/1000,Brep);
			viPrintf(vip,":timebase:mode window\n");
			for(int i=0;i<4;i++)
			{
				if(isChaChoose[i])
				{
					viPrintf(vip,":timebase:window:position %f\n",zoomPosition[i]);
					viPrintf(vip,":timebase:window:range %f\n",zoomRange[i]);
					viPrintf(vip,":measure:source channel%d\n",i+1);
					u[i]=autoV(i+1);
				}
			}
			for(int i=0;i<4;i++)
			{
				if(!isChaChoose[i]||i==chaRefer-1) continue;
			
				map<float,float>::iterator it=standMp.find(f);
				if(it==standMp.end())
				{
					//Mp=(standMp.lower_bound(f)->second+standMp.upper_bound(f)->second)/2;
					//ʹ�����Բ�ֵ�����Ƶ�ʵ��������ֵ
					float x0=0,y0=0,x1=0,y1=0;
					for(it=standMp.begin();it!=standMp.end();it++)
					{
						if(it->first<f) 
						{
							x0=it->first;
							y0=it->second;
						}
						if(it->first>f)
						{
							x1=it->first;
							y1=it->second;
							break;
						}
					}
					
					Mp=LineP(x0,y0,x1,y1,f);
				}
				else
					Mp=it->second;
				Result[i].push_back(CalResponse(Mp,u[i]*1000/Ratio,u[chaRefer-1]/Gain[chaRefer-1],d[chaRefer-1]));
			
			}
			huatu_response();
			if(OneThird_f)
			{
				count_freq++;
				f=OneThirdFreq[OTFreq+count_freq];
			}
			else f+=deltaf;
		}
		for(int i=0;i<4;i++)
		{
			if(isChaChoose[i]&&i!=chaRefer-1)
				MulSensity[i].insert(MulSensity[i].end(),Result[i].begin(),Result[i].end());
		}
	}
	viPrintf(vip,":timebase:mode main\n");
	return 0;
}
void CMeasure::huatu_response()
{
	int x=0,y=0;
	CWnd *pWnd=GetDlgItem(IDC_picture);
	CRect rect;
	pWnd->GetClientRect(rect);
	CDC* pDC=pWnd->GetDC();
	if(MeaCount>1);
	else 
	{
		CBrush rebrush;
		rebrush.CreateSolidBrush (RGB(255,255,255));//��ɫˢ��
		CBrush *pOldBrush=pDC->SelectObject (&rebrush);
		pDC->Rectangle (rect);//���picture�еĻ滭
		pDC->SelectObject (pOldBrush);
	}
	CPen pNewPen;
	pNewPen.CreatePen(PS_SOLID,1,RGB(0,0,0));
	CPen* pOldPen=pDC->SelectObject(&pNewPen);
	int deltaX=rect.Width()/50;
	int deltaY=rect.Height()/100;//100�����ȡ�ģ����������Ϊ100��
	pDC->SetViewportOrg(rect.left,rect.bottom);//ԭ����Ϊ�����½ǵĵ�
	//��������
	CString str;
	float temp=0.0;
	for(x=0;x<=50;x+=1)
	{
		pDC->MoveTo((int)(x*deltaX),0);
		pDC->LineTo((int)(x*deltaX),-rect.Height());
	}
	for(x=0;x<=50;x+=5)
	{
		if(endf==startf)
		{
			str.Format("%.1f",(startf+x)/1000);
			pDC->TextOutA((int)(x*deltaX)-10,rect.top+5,str);
		}
		else
		{
			temp=startf+(endf-startf)/50*x;
			str.Format("%.1f",temp/1000);
			pDC->TextOutA((int)(x*deltaX)-10,rect.top+5,str);
		}
	}
	for(y=0;y<=100;y+=10)
	{
		pDC->MoveTo(rect.left,-y*deltaY);
		pDC->LineTo(rect.right,-y*deltaY);
		str.Format("%d",50+2*y);//�����ʾ50~250��200���㣬�������ܹ����ֳ���100������ÿһ�����2y
		pDC->TextOutA(rect.left-30,-y*deltaY-10,str);
	}
	str.Format("Ƶ��(kHz)");
	pDC->TextOutA(400,30,str);
	str.Format("��");
	pDC->TextOutA(-80,-280,str);
	str.Format("��");
	pDC->TextOutA(-80,-260,str);
	str.Format("��");
	pDC->TextOutA(-80,-240,str);
	str.Format("Ӧ");
	pDC->TextOutA(-80,-220,str);
	str.Format("(dB)");
	pDC->TextOutA(-85,-200,str);
	str.Format("dB re 1/uPa");
	pDC->TextOutA(rect.left+10,-rect.bottom-30,str);
	pDC->SelectObject(pOldPen);
	CPen pPen[4];
	pPen[0].CreatePen(PS_SOLID,3,RGB(255,165,0));//��ɫ����
	pPen[1].CreatePen(PS_SOLID,3,RGB(0,255,0));//��ɫ����
	pPen[2].CreatePen(PS_SOLID,3,RGB(0,0,255));//��ɫ����
	pPen[3].CreatePen(PS_SOLID,3,RGB(255,0,0));//��ɫ����
	//pOldPen=pDC->SelectObject(&pPen);

	////debug
	//float Result[10]={-210.7f,-210.6f,-218.1f,-217.6f,-215.8f,-214.9f,-215.3f,-215.4f,-215.9f,-217.0f};
	//pDC->MoveTo(0,(int)((Result[0]+150)*deltaY));
	//for(unsigned int i=1;i<10;i++)
	//{
	//	x=i*deltaX;
	//	y=(int)((Result[i]+150)*deltaY);
	//	pDC->LineTo(x,y);//����������
	//}
	////debug
	for(int ch=0;ch<4;ch++)
	{
		if(isChaChoose[ch])
		{
			pOldPen=pDC->SelectObject(&pPen[ch]);
			if(Result[ch].size()==0) continue;//ע��Ҫ��continue�������breakֱ�Ӿ�����ѭ���ˣ����ử����ͨ����ͼ��
			pDC->MoveTo(0,-(int)((Result[ch][0]-50)/2*deltaY));//�������Ƶ����
			if(endf==startf) continue;
			if(OneThird_f)
			{
				for(unsigned int i=1;i<Result[ch].size();i++)
				{

					x=(int)((OneThirdFreq[OTFreq+i]-startf)*50/(endf-startf)*deltaX);
					y=-(int)((Result[ch][i]-50)/2*deltaY);
					pDC->LineTo(x,y);//����������
				}
			}
			else 
			{
				for(unsigned int i=1;i<Result[ch].size();i++)
				{
					x=(int)(i*deltaf*deltaX*50/(endf-startf));
					y=-(int)((Result[ch][i]-50)/2*deltaY);
					pDC->LineTo(x,y);//����������
				}
			}
		}
	}

	pDC->SelectObject(pOldPen);
}
float angle=0.0;
int CMeasure::MeasureDir()
{
	if(m_com.get_PortOpen())
	{
		AfxMessageBox("��������ʹ�ã����ȹرտ��ƻ�תϵͳ��");
		m_com.put_PortOpen(false);
		return -1;
	}
	if(!m_com.get_PortOpen())
	{
		m_com.put_PortOpen(true);         //�򿪴���
	}
	else
	{
		m_com.put_OutBufferCount(0);
		MessageBox("����2��ʧ��");
		m_com.put_PortOpen(false);
		return -1;
	}
	MeaSetManual();//��Ϊ�ֶ�
	CString stemp;
	stemp.Format("��ǰƵ��Ϊ %.1fHz\r\n�����ĽǶȷ�Χ %d��~%d��\r\n��ת�ٶ�Ϊ %d��/Ȧ\r\n�ظ����� %.1fs",f,StartAngle,EndAngle,Speed,Brep);
	if(MessageBox(stemp,"�Ƿ������",MB_OKCANCEL)==IDCANCEL) 
	{
		m_com.put_OutBufferCount(0);
		m_com.put_PortOpen(false);
		return -1;
	}
	SetDlgItemTextA(IDC_showPara,stemp);
	m_DirPic=0;
	f=startf;
	isMeasure=true;
	CreateBurst(f,v/1000,Bwid/1000,Brep);//�����ź�Դ
	MessageBox("�������ʾѡ�����ͨ���Ĳ�������");
	viPrintf(vip,":timebase:mode window\n");
	for(int i=0;i<4;i++)
	{
		if(isChaChoose[i])
		{
			CString s;
			s.Format("���ѡ��ͨ��%d�Ĳ����������ȷ��",i+1);
			MessageBox(s);
			viQueryf(vip,":timebase:window:position?\n","%f\n",&zoomPosition[i]);
			viQueryf(vip,":timebase:window:range?\n","%f\n",&zoomRange[i]);
		}
	}
	viPrintf(vip,":timebase:mode main\n");
	viPrintf(vip,":run\n");
	angle=MeaReadCurrentAngle();//������ǰ�ĽǶ�ֵ
	huatu_recidir(); 
	bool isRightDir=true;
	SetDlgItemTextA(IDC_Show,"����ת��ָ���Ƕȣ����Ժ�......");
	if(angle<=StartAngle)
	{
		MeaRotateTargetAngle(Speed,StartAngle);//�����ǰλ������ʼ�Ƕȿ�������ת����ʼ�Ƕ�
		isRightDir=true;
	}
	else if(abs(angle-EndAngle)<=abs(angle-StartAngle))
	{
		if(angle<=EndAngle)
		{
			MeaRotateTargetAngle(Speed,EndAngle);
			isRightDir=false;
		}
		else
		{
			int t=(int)((angle-EndAngle)*Speed/360+2);
			MeaRotateLeft(Speed);
			Sleep(t*1000);
			MeaStopRotateLeft();
			MeaRotateTargetAngle(Speed,EndAngle);
			isRightDir=false;
		}		
	}
	else 
	{
		int t=(int)((angle-StartAngle)*Speed/360+2);
		MeaRotateLeft(Speed);
		Sleep(t*1000);
		MeaStopRotateLeft();
		MeaRotateTargetAngle(Speed,StartAngle);//�����ǰλ������ʼ�Ƕȿ�������ת����ʼ�Ƕ�
		isRightDir=true;
	}
	angle=MeaReadCurrentAngle();
	while((isRightDir&&abs(angle-StartAngle)>0.1)||(!isRightDir&&abs(angle-EndAngle)>0.1))//֮ǰ���Ե�ʱ����д��1���������1���е�����Ը�Ϊ0.1����
	{
		angle=MeaReadCurrentAngle();
	}//�ȴ�ת��ָ���Ƕ����
	 
	viPrintf(vip,":run\n");
	SetDlgItemTextA(IDC_Show,"���ڽ��е�Ƶ��ָ���Բ��������Ժ�......");
	clock_t t_start=clock();
	viPrintf(vip,":timebase:mode window\n");
	
	for(int i=0;i<4;i++)
	{
		if(!isChaChoose[i]) continue;
		viPrintf(vip,":timebase:window:position %f\n",zoomPosition[i]);
		viPrintf(vip,":timebase:window:range %f\n",zoomRange[i]);
		viPrintf(vip,":measure:source channel%d\n",i+1);
		u[i]=autoV(i+1);
		//viQueryf(vip,":measure:vpp?\n","%f\n",&u[i]);
		Result[i].push_back(u[i]);
	}//��ȡ��һ���Ƕȵĵ�ѹֵ
	angle=MeaReadCurrentAngle();
	MeaAngle.push_back(angle);
	if(isRightDir) MeaRotateRight(Speed);//˳ʱ��ת������
	else MeaRotateLeft(Speed);//��ʱ��ת��
	angle=MeaReadCurrentAngle();	
	while((isRightDir&&angle<EndAngle)||(!isRightDir&&angle>StartAngle))
	{
		//�������ա�ֹͣ��������ť���µ���Ϣ
		MSG  msg;
		if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))          
		{          
			if (msg.message == MAKEWPARAM(IDC_StopMea, BN_CLICKED))
			{
				isMeasure=false;
				break ;
			}
			TranslateMessage (&msg) ;          
			DispatchMessage (&msg) ;          
		}
		for(int i=0;i<4;i++)
		{
			if(isChaChoose[i])
			{
				viPrintf(vip,":timebase:window:position %f\n",zoomPosition[i]);
				viPrintf(vip,":timebase:window:range %f\n",zoomRange[i]);
				viPrintf(vip,":measure:source channel%d\n",i+1);
				u[i]=autoV(i+1);
			}
		}
		angle=MeaReadCurrentAngle();
		MeaAngle.push_back(angle);
		 
		//����ǶȺ͵�ѹֵ
		for(int i=0;i<4;i++)
		{
			if(!isChaChoose[i]) continue;
			Result[i].push_back(u[i]);
		}
		
		//���Ƽ�����ͼ
		huatu_recidir();
		angle=MeaReadCurrentAngle();
		if(isRightDir&&angle>=EndAngle)
		{
			MeaStopRotateRight();
			break;
		}
		else if(!isRightDir&&angle<=StartAngle)
		{
			MeaStopRotateLeft();
			break;
		}		
	}
	if(isRightDir) MeaStopRotateRight();
	else MeaStopRotateLeft();
	
	for(int i=0;i<4;i++)
	{
		if(!isChaChoose[i]) continue;
		viPrintf(vip,":timebase:window:position %f\n",zoomPosition[i]);
		viPrintf(vip,":timebase:window:range %f\n",zoomRange[i]);
		viPrintf(vip,":measure:source channel%d\n",i+1);
		u[i]=autoV(i+1);
		Result[i].push_back(u[i]);
	}//���һ���Ƕȵĵ�ѹֵ
	angle=MeaReadCurrentAngle();
	MeaAngle.push_back(angle);
	 
	huatu_recidir();
	viPrintf(vip,":timebase:mode main\n");
	clock_t t_end=clock();
	stemp.Format("����ʱ��Ϊ %.4f s",(double)(t_end-t_start)/CLOCKS_PER_SEC);
	MessageBox(stemp);
	if(m_com.get_PortOpen())
		m_com.put_PortOpen(false);
	return 0;
}
void CMeasure::huatu_recidir()
{
	int cycle_num=10,redius_num=36;//cycle_num��ͬ��Բ�ĸ�����redius_num��ֱ��������
	CWnd *pWnd = GetDlgItem(IDC_picture);
	CRect rect;
	pWnd->GetClientRect(rect);
	CDC *pDC = pWnd->GetDC();
	CBrush rebrush;
	rebrush.CreateSolidBrush (RGB(255,255,255));//��ɫˢ��
	CBrush *pOldBrush=pDC->SelectObject (&rebrush);
	pDC->Rectangle (rect);//���picture�еĻ滭
	pDC->SelectObject (pOldBrush);
	CPen newPen;
	newPen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	CPen* pOldPen = (CPen*)pDC->SelectObject(&newPen);
	pDC->SelectStockObject(NULL_BRUSH);//û�л�ˢ�ͱ���Բ�����ǣ����Ŀ���Բ
	int w=rect.Width(),h=rect.Height ();
	pDC->SetViewportOrg(w/2,h/2);//����ԭ��
	int R=(h-60)/2;//�뾶
	float deltaR=(float)R/cycle_num;
	for(int i=1;i<=cycle_num;i++)
	{
		if(i==cycle_num-3) continue;
		pDC->Ellipse(-(int)(i*deltaR),-(int)(i*deltaR),(int)(i*deltaR),(int)(i*deltaR));//��Բ����ʵ���ҵ�һ�����������ε����ϽǶ�������½Ƕ���
	}
	pDC->Ellipse(-(int)(cycle_num*deltaR),-(int)(cycle_num*deltaR),(int)(cycle_num*deltaR),(int)(cycle_num*deltaR));//�������Բ
	float deltaA=2*PI/redius_num;
	int x,y;
	for(int i=0;i<=redius_num/2;i++)
	{
		pDC->MoveTo(0,0);
		x=(int)(R*sin(i*deltaA));
		y=(int)(R*cos(i*deltaA));
		pDC->LineTo(x,y);//����һ�������޵İ뾶
		pDC->MoveTo(0,0);
		x=(int)(R*sin(-i*deltaA));
		y=(int)(R*cos(-i*deltaA));
		pDC->LineTo(x,y);//���˶������޵İ뾶
	}
	CPen p;
	p.CreatePen(PS_DASH, 1, RGB(255,0,0));
	pOldPen=pDC->SelectObject(&p);
	pDC->Ellipse(-(int)((cycle_num-3)*deltaR),-(int)((cycle_num-3)*deltaR),
		(int)((cycle_num-3)*deltaR),(int)((cycle_num-3)*deltaR));//-3dB��Բ
	CString str;
	str.Format("%d��",0);
	pDC->TextOutA(0,-R-20,str);
	str.Format("%d��",30);
	x=(int)(R*sin(PI/6));
	y=-(int)(R*cos(PI/6));
	pDC->TextOutA(x+10,y-20,str);
	str.Format("%d��",60);
	x=(int)(R*sin(PI/3));
	y=-(int)(R*cos(PI/3));
	pDC->TextOutA(x+10,y-20,str);
	str.Format("%d��",90);
	pDC->TextOutA(R+10,0,str);
	str.Format("%d��",120);
	x=(int)(R*sin(2*PI/3));
	y=-(int)(R*cos(2*PI/3));
	pDC->TextOutA(x+10,y+20,str);
	str.Format("%d��",150);
	x=(int)(R*sin(5*PI/6));
	y=-(int)(R*cos(5*PI/6));
	pDC->TextOutA(x+10,y+20,str);
	str.Format("%d��",-30);
	x=(int)(R*sin(-PI/6));
	y=-(int)(R*cos(-PI/6));
	pDC->TextOutA(x-30,y-20,str);
	str.Format("%d��",-60);
	x=(int)(R*sin(-PI/3));
	y=-(int)(R*cos(-PI/3));
	pDC->TextOutA(x-30,y-20,str);
	str.Format("%d��",-90);
	pDC->TextOutA(-R-35,0,str);
	str.Format("%d��",-120);
	x=(int)(R*sin(-2*PI/3));
	y=-(int)(R*cos(-2*PI/3));
	pDC->TextOutA(x-30,y+20,str);
	str.Format("%d��",-150);
	x=(int)(R*sin(-5*PI/6));
	y=-(int)(R*cos(-5*PI/6));
	pDC->TextOutA(x-30,y+20,str);
	str.Format("-180��(180��)");
	pDC->TextOutA(-20,R+10,str);
	str.Format("��ǰ�Ƕ�Ϊ��%.1f��",angle);
	pDC->TextOutA(-w/2+20,-h/2+20,str);
	////debug
	//CPen ppen;
	//ppen.CreatePen(PS_SOLID, 1, RGB(255,0,0));
	//pOldPen=pDC->SelectObject(&ppen);
	//const int N=9;
	//float Result[N]={100,90,80,70,60,50,40,30,20};
	//float MeaAngle[N]={-180,-145,-90,-60,0,30,90,120,180};
	//pDC->MoveTo(0,0);
	//float max=Result[0];
	//for(unsigned int i=1;i<N;i++)
	//			if(max<Result[i]) max=Result[i];
	//for(unsigned int i=0;i<N;i++)
	//{
	//	deltaA=PI*MeaAngle[i]/180;
	//	x=(int)(Result[i]/max*R*sin(deltaA));
	//	y=-(int)(Result[i]/max*R*cos(deltaA));
	//	pDC->LineTo(x,y);//����������
	//}

	//////debug
	CPen pPen[4];
	pPen[0].CreatePen(PS_SOLID,3,RGB(255,165,0));//��ɫ����
	pPen[1].CreatePen(PS_SOLID,3,RGB(0,255,0));//��ɫ����
	pPen[2].CreatePen(PS_SOLID,3,RGB(0,0,255));//��ɫ����
	pPen[3].CreatePen(PS_SOLID,3,RGB(255,0,0));//��ɫ����
	
	for(int ch=0;ch<4;ch++)
	{
		if(isChaChoose[ch])
		{
			pOldPen=pDC->SelectObject(&pPen[ch]);
			if(Result[ch].size()==0) continue;//ע��Ҫ��continue�������breakֱ�Ӿ�����ѭ���ˣ����ử����ͨ����ͼ��
			
			float max=Result[ch][0];
			for(unsigned int i=1;i<Result[ch].size();i++)
				if(max<Result[ch][i]) max=Result[ch][i];
			deltaA=PI*MeaAngle[0]/180;
			pDC->MoveTo((int)(Result[ch][0]/max*R*sin(deltaA)),-(int)(Result[ch][0]/max*R*cos(deltaA)));//�������Ƶ�Բ��
			for(unsigned int i=1;i<Result[ch].size();i++)
			{
				deltaA=PI*MeaAngle[i]/180;
				x=(int)(Result[ch][i]/max*R*sin(deltaA));
				y=-(int)(Result[ch][i]/max*R*cos(deltaA));
				pDC->LineTo(x,y);//����������
			}
		}
	}

	pDC->SelectObject(pOldPen);
}
void CMeasure::huatu_dB()
{
	int cycle_num=10,redius_num=36;//cycle_num��ͬ��Բ�ĸ�����redius_num��ֱ��������
	float mindB=-45.0f,perdb=-mindB/(cycle_num-1);
	CWnd *pWnd = GetDlgItem(IDC_picture);
	CRect rect;
	pWnd->GetClientRect(rect);
	CDC *pDC = pWnd->GetDC();
	CBrush rebrush;
	rebrush.CreateSolidBrush (RGB(255,255,255));//��ɫˢ��
	CBrush *pOldBrush=pDC->SelectObject (&rebrush);
	pDC->Rectangle (rect);//���picture�еĻ滭
	pDC->SelectObject (pOldBrush);
	CPen newPen;
	newPen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	CPen* pOldPen = (CPen*)pDC->SelectObject(&newPen);
	pDC->SelectStockObject(NULL_BRUSH);//û�л�ˢ�ͱ���Բ�����ǣ����Ŀ���Բ
	int w=rect.Width(),h=rect.Height ();
	pDC->SetViewportOrg(w/2,h/2);//����ԭ��
	int R=(h-60)/2;//�뾶
	float deltaR=(float)R/cycle_num;
	for(int i=1;i<=cycle_num;i++)
	{
		pDC->Ellipse(-(int)(i*deltaR),-(int)(i*deltaR),(int)(i*deltaR),(int)(i*deltaR));//��Բ����ʵ���ҵ�һ�����������ε����ϽǶ�������½Ƕ���
	}
	pDC->Ellipse(-(int)(cycle_num*deltaR),-(int)(cycle_num*deltaR),(int)(cycle_num*deltaR),(int)(cycle_num*deltaR));//�������Բ
	float deltaA=2*PI/redius_num;
	int x,y;
	for(int i=0;i<=redius_num/2;i++)
	{
		pDC->MoveTo((int)(deltaR*sin(i*deltaA)),(int)(deltaR*cos(i*deltaA)));
		x=(int)(R*sin(i*deltaA));
		y=(int)(R*cos(i*deltaA));
		pDC->LineTo(x,y);//����һ�������޵İ뾶
		pDC->MoveTo((int)(deltaR*sin(-i*deltaA)),(int)(deltaR*cos(-i*deltaA)));
		x=(int)(R*sin(-i*deltaA));
		y=(int)(R*cos(-i*deltaA));
		pDC->LineTo(x,y);//���˶������޵İ뾶
	}
	CPen p;
	p.CreatePen(PS_DASH, 1, RGB(255,0,0));
	pOldPen=pDC->SelectObject(&p);
	pDC->Ellipse(-(int)(((-3-mindB)/perdb+1)*deltaR),-(int)(((-3-mindB)/perdb+1)*deltaR),
		(int)(((-3-mindB)/perdb+1)*deltaR),(int)(((-3-mindB)/perdb+1)*deltaR));//-3dB��Բ
	CString str;
	str.Format("%d��",0);
	pDC->TextOutA(0,-R-20,str);
	str.Format("%d��",30);
	x=(int)(R*sin(PI/6));
	y=-(int)(R*cos(PI/6));
	pDC->TextOutA(x+10,y-20,str);
	str.Format("%d��",60);
	x=(int)(R*sin(PI/3));
	y=-(int)(R*cos(PI/3));
	pDC->TextOutA(x+10,y-20,str);
	str.Format("%d��",90);
	pDC->TextOutA(R+10,0,str);
	str.Format("%d��",120);
	x=(int)(R*sin(2*PI/3));
	y=-(int)(R*cos(2*PI/3));
	pDC->TextOutA(x+10,y+20,str);
	str.Format("%d��",150);
	x=(int)(R*sin(5*PI/6));
	y=-(int)(R*cos(5*PI/6));
	pDC->TextOutA(x+10,y+20,str);
	str.Format("%d��",-30);
	x=(int)(R*sin(-PI/6));
	y=-(int)(R*cos(-PI/6));
	pDC->TextOutA(x-30,y-20,str);
	str.Format("%d��",-60);
	x=(int)(R*sin(-PI/3));
	y=-(int)(R*cos(-PI/3));
	pDC->TextOutA(x-30,y-20,str);
	str.Format("%d��",-90);
	pDC->TextOutA(-R-35,0,str);
	str.Format("%d��",-120);
	x=(int)(R*sin(-2*PI/3));
	y=-(int)(R*cos(-2*PI/3));
	pDC->TextOutA(x-30,y+20,str);
	str.Format("%d��",-150);
	x=(int)(R*sin(-5*PI/6));
	y=-(int)(R*cos(-5*PI/6));
	pDC->TextOutA(x-30,y+20,str);
	str.Format("-180��(180��)");
	pDC->TextOutA(-20,R+10,str);
	str.Format("��ǰ�Ƕ�Ϊ��%.1f��",angle);
	pDC->TextOutA(-w/2+20,-h/2+20,str);
	str.Format("5dB/div");
	pDC->TextOutA(-w/2+20,h/2-20,str);
	CPen pPen[4];
	pPen[0].CreatePen(PS_SOLID,3,RGB(255,165,0));//��ɫ����
	pPen[1].CreatePen(PS_SOLID,3,RGB(0,255,0));//��ɫ����
	pPen[2].CreatePen(PS_SOLID,3,RGB(0,0,255));//��ɫ����
	pPen[3].CreatePen(PS_SOLID,3,RGB(255,0,0));//��ɫ����
	
	for(int ch=0;ch<4;ch++)
	{
		if(isChaChoose[ch])
		{
			pOldPen=pDC->SelectObject(&pPen[ch]);
			if(Result[ch].size()==0) continue;//ע��Ҫ��continue�������breakֱ�Ӿ�����ѭ���ˣ����ử����ͨ����ͼ��
			
			float max=Result[ch][0];
			for(unsigned int i=1;i<Result[ch].size();i++)
				if(max<Result[ch][i]) max=Result[ch][i];
			deltaA=PI*MeaAngle[0]/180;
			float db=20*log10(Result[ch][0]/max);
			if(db<=mindB)//�ֱ�����СΪ-45dB��0.0056������С�ľͲ�������
				pDC->MoveTo((int)(deltaR*sin(deltaA)),-(int)(deltaR*cos(deltaA)));//�������Ƶ�Բ��
			else
				pDC->MoveTo((int)(((db-mindB)/perdb+1)*deltaR*sin(deltaA)),-(int)(((db-mindB)/perdb+1)*deltaR*cos(deltaA)));//�������Ƶ�Բ��
			for(unsigned int i=1;i<Result[ch].size();i++)
			{
				deltaA=PI*MeaAngle[i]/180;
				db=20*log10(Result[ch][i]/max);
				if(db<mindB)
				{
					x=(int)(deltaR*sin(deltaA));
					y=-(int)(deltaR*cos(deltaA));
				}
				else
				{
					x=(int)(((db-mindB)/perdb+1)*deltaR*sin(deltaA));
					y=-(int)(((db-mindB)/perdb+1)*deltaR*cos(deltaA));
				}
				pDC->LineTo(x,y);//����������
			}
		}
	}

	pDC->SelectObject(pOldPen);

}
int CMeasure::MeaMulDir()//ֻ����һ��ͨ�������������Ǿ������ĸ�ͨ������ѡ��
{
	if(m_com.get_PortOpen())
	{
		AfxMessageBox("��������ʹ�ã����ȹرտ��ƻ�תϵͳ��");
		m_com.put_PortOpen(false);
		return -1;
	}
	if(!m_com.get_PortOpen())
	{
		m_com.put_PortOpen(true);         //�򿪴���
	}
	else
	{
		m_com.put_OutBufferCount(0);
		MessageBox("����2��ʧ��");
		m_com.put_PortOpen(false);
		return -1;
	}
	MeaSetManual();//��Ϊ�ֶ�
	f=startf;
	isMeasure=true;
	CreateMulFrePulse(Fs,f,deltaf,Bwid/1000,v/1000,Brep);//��Ƶ���ź�
	MessageBox("�������ʾѡ�����ͨ������Ƶ�ʵĲ�������");
	viPrintf(vip,":timebase:mode window\n");
	for(int i=0;i<4;i++)
	{
		if(isChaChoose[i])//ֻ��һ����true
		{
			CString s;
			s.Format("���ѡ��ͨ��%d�ĵ�1��Ƶ�ʵĲ����������ȷ��",i+1);
			MessageBox(s);
			viQueryf(vip,":timebase:window:position?\n","%f\n",&zoomPosition[0]);
			viQueryf(vip,":timebase:window:range?\n","%f\n",&zoomRange[0]);
		
		}
	}
	viPrintf(vip,":timebase:mode main\n");
	float angle=MeaReadCurrentAngle();//������ǰ�ĽǶ�ֵ
	bool isRightDir=true;
	SetDlgItemTextA(IDC_showPara,"����ת��ָ���Ƕȣ����Ժ�......");
	if(abs(angle-StartAngle)<=abs(angle-EndAngle))
	{
		MeaRotateTargetAngle(Speed,StartAngle);//�����ǰλ������ʼ�Ƕȿ�������ת����ʼ�Ƕ�
		isRightDir=true;
	}
	else 
	{
		MeaRotateTargetAngle(Speed,EndAngle);
		isRightDir=false;
	}
	angle=MeaReadCurrentAngle();
	while((isRightDir&&abs(angle-StartAngle)>0.1)||(!isRightDir&&abs(angle-EndAngle)>0.1))//֮ǰ���Ե�ʱ����д��1���������1���е�����Ը�Ϊ0.1����
	{
		angle=MeaReadCurrentAngle();
	}//�ȴ�ת��ָ���Ƕ����
	CString stemp;
	stemp.Format("��ʼƵ��Ϊ %.1fHz\r\nƵ�ʵ��� %d\r\n�����ĽǶȷ�Χ %d��~%d��\r\n��ת�ٶ�Ϊ %d��/Ȧ\r\n�ظ����� %.1fs",f,PulseCount,StartAngle,EndAngle,Speed,Brep);
	if(MessageBox(stemp,"�Ƿ������",MB_OKCANCEL)==IDCANCEL) 
	{
		m_com.put_OutBufferCount(0);
		m_com.put_PortOpen(false);
		return -1;
	}
	SetDlgItemTextA(IDC_showPara,stemp);
	viPrintf(vip,":run\n");
	SetDlgItemTextA(IDC_Show,"���ڽ��ж�Ƶ��ָ���Բ��������Ժ�......");
	viPrintf(vip,":timebase:mode window\n");
	clock_t t_start=clock();
	angle=MeaReadCurrentAngle();	
	for(int i=0;i<4;i++)
	{
		if(!isChaChoose[i]) continue;
		else
		{
			viPrintf(vip,":digitize channel%d\n",i+1);
			for(int j=0;j<PulseCount;j++)
			{
				viPrintf(vip,":timebase:window:position %f\n",zoomPosition[0]+Bwid/1000*5*j);
				//��zoom���ƶ�һ�����
				viPrintf(vip,":timebase:window:range %f\n",zoomRange[0]);
				viPrintf(vip,":measure:source channel%d\n",i+1);
				u[j]=autoV(i+1);
				Result[j].push_back(u[j]);
				MeaAngle.push_back(angle);//��һ�������еĽǶ�һ��
			}
			viPrintf(vip,":run\n");
		}
	}//��ȡ��һ���Ƕȵĵ�ѹֵ
	if(isRightDir) MeaRotateRight(Speed);//˳ʱ��ת������
	else MeaRotateLeft(Speed);//��ʱ��ת��
	angle=MeaReadCurrentAngle();	
	while(isMeasure&&((isRightDir&&angle<EndAngle)||(!isRightDir&&angle>StartAngle)))
	{
		for(int i=0;i<4;i++)
		{
			if(isChaChoose[i])
			{
				viPrintf(vip,":digitize channel%d\n",i+1);
				angle=MeaReadCurrentAngle();//���Ƕȵ�ʱ��
				for(int j=0;j<PulseCount;j++)
				{
					viPrintf(vip,":timebase:window:position %f\n",zoomPosition[0]+Bwid/1000*5*j);
					viPrintf(vip,":timebase:window:range %f\n",zoomRange[0]);
					viPrintf(vip,":measure:source channel%d\n",i+1);
					u[j]=autoV(i+1);	
					Result[j].push_back(u[j]);
					MeaAngle.push_back(angle);
				}
				viPrintf(vip,":run\n");
			}
		}
		//���Ƽ�����ͼ
		huatu_muldir();
		angle=MeaReadCurrentAngle();
		if(isRightDir&&angle>=EndAngle)
		{
			MeaStopRotateRight();
			isMeasure=false;
			break;
		}
		else if(!isRightDir&&angle<=StartAngle)
		{
			MeaStopRotateLeft();
			isMeasure=false;
			break;
		}				
	}
	if(isRightDir) MeaStopRotateRight();
	else MeaStopRotateLeft();
	angle=MeaReadCurrentAngle();
	for(int i=0;i<4;i++)
	{
		if(!isChaChoose[i]) continue;
		viPrintf(vip,":digitize channel%d\n",i+1);
		for(int j=0;j<PulseCount;j++)
		{
			viPrintf(vip,":timebase:window:position %f\n",zoomPosition[0]+Bwid/1000*5*j);
			viPrintf(vip,":timebase:window:range %f\n",zoomRange[0]);
			viPrintf(vip,":measure:source channel%d\n",i+1);
			u[j]=autoV(i+1);
			Result[j].push_back(u[j]);
			MeaAngle.push_back(angle);
		}
		viPrintf(vip,":run\n");
	}//���һ���Ƕȵĵ�ѹֵ
	huatu_muldir();
	viPrintf(vip,":timebase:mode main\n");
	clock_t t_end=clock();
	stemp.Format("����ʱ��Ϊ %.4f s",(double)(t_end-t_start)/CLOCKS_PER_SEC);
	MessageBox(stemp);
	if(m_com.get_PortOpen())
		m_com.put_PortOpen(false);
	return 0;
}
void CMeasure::huatu_muldir()
{
	int cycle_num=10,redius_num=36;//cycle_num��ͬ��Բ�ĸ�����redius_num��ֱ��������
	float mindB=-45.0f,perdb=-mindB/(cycle_num-1);
	CWnd *pWnd = GetDlgItem(IDC_picture);
	CRect rect;
	pWnd->GetClientRect(rect);
	CDC *pDC = pWnd->GetDC();
	CBrush rebrush;
	rebrush.CreateSolidBrush (RGB(255,255,255));//��ɫˢ��
	CBrush *pOldBrush=pDC->SelectObject (&rebrush);
	pDC->Rectangle (rect);//���picture�еĻ滭
	pDC->SelectObject (pOldBrush);
	CPen newPen;
	newPen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	CPen* pOldPen = (CPen*)pDC->SelectObject(&newPen);
	pDC->SelectStockObject(NULL_BRUSH);//û�л�ˢ�ͱ���Բ�����ǣ����Ŀ���Բ
	int w=rect.Width(),h=rect.Height ();
	pDC->SetViewportOrg(w/2,h/2);//����ԭ��
	int R=(h-60)/2;//�뾶
	float deltaR=(float)R/cycle_num;
	for(int i=1;i<=cycle_num;i++)
	{
		pDC->Ellipse(-(int)(i*deltaR),-(int)(i*deltaR),(int)(i*deltaR),(int)(i*deltaR));//��Բ����ʵ���ҵ�һ�����������ε����ϽǶ�������½Ƕ���
	}
	pDC->Ellipse(-(int)(cycle_num*deltaR),-(int)(cycle_num*deltaR),(int)(cycle_num*deltaR),(int)(cycle_num*deltaR));//�������Բ
	float dAngle=2*PI/redius_num;
	int x,y;
	for(int i=0;i<=redius_num/2;i++)
	{
		pDC->MoveTo((int)(deltaR*sin(i*dAngle)),(int)(deltaR*cos(i*dAngle)));
		x=(int)(R*sin(i*dAngle));
		y=(int)(R*cos(i*dAngle));
		pDC->LineTo(x,y);//����һ�������޵İ뾶
		pDC->MoveTo((int)(deltaR*sin(-i*dAngle)),(int)(deltaR*cos(-i*dAngle)));
		x=(int)(R*sin(-i*dAngle));
		y=(int)(R*cos(-i*dAngle));
		pDC->LineTo(x,y);//���˶������޵İ뾶
	}
	CPen p;
	p.CreatePen(PS_DASH, 1, RGB(255,0,0));
	pOldPen=pDC->SelectObject(&p);
	pDC->Ellipse(-(int)(((-3-mindB)/perdb+1)*deltaR),-(int)(((-3-mindB)/perdb+1)*deltaR),
		(int)(((-3-mindB)/perdb+1)*deltaR),(int)(((-3-mindB)/perdb+1)*deltaR));//-3dB��Բ
	CString str;
	str.Format("%d��",0);
	pDC->TextOutA(0,-R-20,str);
	str.Format("%d��",30);
	x=(int)(R*sin(PI/6));
	y=-(int)(R*cos(PI/6));
	pDC->TextOutA(x+10,y-20,str);
	str.Format("%d��",60);
	x=(int)(R*sin(PI/3));
	y=-(int)(R*cos(PI/3));
	pDC->TextOutA(x+10,y-20,str);
	str.Format("%d��",90);
	pDC->TextOutA(R+10,0,str);
	str.Format("%d��",120);
	x=(int)(R*sin(2*PI/3));
	y=-(int)(R*cos(2*PI/3));
	pDC->TextOutA(x+10,y+20,str);
	str.Format("%d��",150);
	x=(int)(R*sin(5*PI/6));
	y=-(int)(R*cos(5*PI/6));
	pDC->TextOutA(x+10,y+20,str);
	str.Format("%d��",-30);
	x=(int)(R*sin(-PI/6));
	y=-(int)(R*cos(-PI/6));
	pDC->TextOutA(x-30,y-20,str);
	str.Format("%d��",-60);
	x=(int)(R*sin(-PI/3));
	y=-(int)(R*cos(-PI/3));
	pDC->TextOutA(x-30,y-20,str);
	str.Format("%d��",-90);
	pDC->TextOutA(-R-35,0,str);
	str.Format("%d��",-120);
	x=(int)(R*sin(-2*PI/3));
	y=-(int)(R*cos(-2*PI/3));
	pDC->TextOutA(x-30,y+20,str);
	str.Format("%d��",-150);
	x=(int)(R*sin(-5*PI/6));
	y=-(int)(R*cos(-5*PI/6));
	pDC->TextOutA(x-30,y+20,str);
	str.Format("-180��(180��)");
	pDC->TextOutA(-20,R+10,str);
	str.Format("��ǰ�Ƕ�Ϊ��%.1f��",angle);
	pDC->TextOutA(-w/2+20,-h/2+20,str);
	str.Format("5dB/div");
	pDC->TextOutA(-w/2+20,h/2-20,str);

	str.Format("��ɫ����%.1fHzָ����ͼ",f);
	pDC->TextOutA(-w/2+10,h/2-100,str);
	str.Format("��ɫ����%.1fHzָ����ͼ",f+deltaf);
	pDC->TextOutA(-w/2+10,h/2-80,str);
	str.Format("��ɫ����%.1fHzָ����ͼ",f+2*deltaf);
	pDC->TextOutA(-w/2+10,h/2-60,str);
	str.Format("��ɫ����%.1fHzָ����ͼ",f+3*deltaf);
	pDC->TextOutA(-w/2+10,h/2-40,str);

	CPen pPen[4];
	pPen[0].CreatePen(PS_SOLID,3,RGB(255,165,0));//��ɫ����
	pPen[1].CreatePen(PS_SOLID,3,RGB(0,255,0));//��ɫ����
	pPen[2].CreatePen(PS_SOLID,3,RGB(0,0,255));//��ɫ����
	pPen[3].CreatePen(PS_SOLID,3,RGB(255,0,0));//��ɫ����
	for(int ch=0;ch<4;ch++)
	{
		if(isChaChoose[ch])//ֻ��һ��ͨ��
		{
			for(int j=0;j<PulseCount;j++)
			{
				pOldPen=pDC->SelectObject(&pPen[j]);
				if(Result[j].size()==0) continue;
				//ע��Ҫ��continue�������breakֱ�Ӿ�����ѭ���ˣ����ử����ͨ����ͼ��
			
				float max=Result[j][0];
				for(unsigned int k=1;k<Result[j].size();k++)
					if(max<Result[j][k]) max=Result[j][k];
				dAngle=PI*MeaAngle[0]/180;
				float db=20*log10(Result[ch][0]/max);
			if(db<=mindB)//�ֱ�����СΪ-45dB��0.0056������С�ľͲ�������
				pDC->MoveTo((int)(deltaR*sin(dAngle)),-(int)(deltaR*cos(dAngle)));//�������Ƶ�Բ��
			else
				pDC->MoveTo((int)(((db-mindB)/perdb+1)*deltaR*sin(dAngle)),-(int)(((db-mindB)/perdb+1)*deltaR*cos(dAngle)));//�������Ƶ�Բ��
				
				for(unsigned int k=1;k<Result[j].size();k++)
				{
					dAngle=PI*MeaAngle[k]/180;
					db=20*log10(Result[j][k]/max);
					if(db<mindB)
					{
						x=(int)(deltaR*sin(dAngle));
						y=-(int)(deltaR*cos(dAngle));
					}
					else
					{
						x=(int)(((db-mindB)/perdb+1)*deltaR*sin(dAngle));
						y=-(int)(((db-mindB)/perdb+1)*deltaR*cos(dAngle));
					}
					pDC->LineTo(x,y);//����������
				}
			}
		}
	}

	pDC->SelectObject(pOldPen);
}

int CMeasure::MeaHuyi()
{
	SetDlgItemTextA(IDC_Show,"׼�����������Ժ�......");
	float UI[3]={0,0,0};
	if(status!=0)
	{
		AfxMessageBox("���ؾ�����ͨ����ȷ��");
		viClose(U2751);
		viClose(rm);
		return -1;
	}
	viClear(U2751);
	viPrintf(U2751,"*RST\n");//���ؾ���λ
	viPrintf(U2751,"*CLS\n");//���ؾ���
	viPrintf(U2751,"ROUTe:CLOSe (@402,107,304)\n");//���䷢�����׺�ˮ������
	float Mp=-1;
	CreateBurst(f,v/1000,Bwid/1000,Brep);
	MessageBox("�������ʾѡ�����ͨ���Ĳ�������");
	//��������R4����������C2����������C4��ʾ����CH2����R3����
	//ˮ��������C7��ʾ����CH1����R1����ʾ����CH3�ӵ��ǵ�����
	viPrintf(vip,":timebase:mode window\n");
	
	CString s;
	//���䷢��ˮ����ͨ��
	s.Format("���ѡ��ͨ��1(��-��)�Ĳ����������ȷ��");
	MessageBox(s);
	viQueryf(vip,":timebase:window:position?\n","%f\n",&zoomPosition[0]);
	viQueryf(vip,":timebase:window:range?\n","%f\n",&zoomRange[0]);
	//���䷢�����׻�����ͨ��
	s.Format("���ѡ��ͨ��2(��-��)�Ĳ����������ȷ��");
	MessageBox(s);
	viQueryf(vip,":timebase:window:position?\n","%f\n",&zoomPosition[1]);
	viQueryf(vip,":timebase:window:range?\n","%f\n",&zoomRange[1]);
	//���䷢������ͨ�������軥�׷�Ҳ���������
	s.Format("���ѡ��ͨ��3(������)�Ĳ����������ȷ��");
	MessageBox(s);
	viQueryf(vip,":timebase:window:position?\n","%f\n",&zoomPosition[2]);
	viQueryf(vip,":timebase:window:range?\n","%f\n",&zoomRange[2]);
	//���׷���ˮ����ͨ��
	viPrintf(U2751,"ROUTe:OPEN (@402,304)\n");//�ص�����ͻ�����
	viPrintf(U2751,"ROUTe:CLOSe (@404)\n");//���׷�
	s.Format("���ѡ��ͨ��1(��-��)�Ĳ����������ȷ��");
	MessageBox(s);
	viQueryf(vip,":timebase:window:position?\n","%f\n",&zoomPosition[3]);
	viQueryf(vip,":timebase:window:range?\n","%f\n",&zoomRange[3]);

	viPrintf(vip,":timebase:mode main\n");
	viPrintf(vip,":run\n");
	CString stemp;
	stemp.Format("\r\n\r\n���׷��Զ����������ȣ�\r\nƵ�ʷ�Χ %.1fkHz~%.1fkHz\r\n�ź�Դ���� %.1fmVpp\r\n",
	startf/1000,endf/1000,v,Bwid,Brep,MeaCount,chaRefer);
	SetDlgItemTextA(IDC_showPara,stemp);
	if(MessageBox(stemp,"��ʾ",MB_OKCANCEL)==IDCANCEL) return -1;	
	f=startf;
	isMeasure=true;
	
	SetDlgItemTextA(IDC_Show,"���ڽ��л��׷��Զ����������ȣ����Ժ�......");
	int count_OTFreq=0;
	while(isMeasure&&f<=endf)
	{
		//�������ա�ֹͣ��������ť���µ���Ϣ
		MSG  msg;
		if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))          
		{          
			if (msg.message == MAKEWPARAM(IDC_StopMea, BN_CLICKED))          
					break ;          
			TranslateMessage (&msg) ;          
			DispatchMessage (&msg) ;          
		} 
		viPrintf(vip,":run\n");
		viPrintf(U2751,"*RST\n");//���ؾ���λ
		viPrintf(U2751,"*CLS\n");//���ؾ���
		viPrintf(U2751,"ROUTe:CLOSe (@402,107,304)\n");//���䷢��ˮ�����գ�������
		CreateBurst(f,v/1000,Bwid/1000,Brep);
		viPrintf(vip,":timebase:mode window\n");
		viPrintf(vip,":timebase:window:position %f\n",zoomPosition[0]);
		viPrintf(vip,":timebase:window:range %f\n",zoomRange[0]);
		viPrintf(vip,":measure:source channel%d\n",1);
		u[0]=autoV(1)/Gain[0];//uFJ
		viPrintf(vip,":timebase:window:position %f\n",zoomPosition[2]);
		viPrintf(vip,":timebase:window:range %f\n",zoomRange[2]);
		viPrintf(vip,":measure:source channel%d\n",3);
		UI[0]=autoV(3)/Cv;//iFJ
		viPrintf(vip,":timebase:window:position %f\n",zoomPosition[1]);
		viPrintf(vip,":timebase:window:range %f\n",zoomRange[1]);
		viPrintf(vip,":measure:source channel%d\n",2);
		u[1]=autoV(2)/Gain[1];//uFH
		viPrintf(vip,":timebase:window:position %f\n",zoomPosition[2]);
		viPrintf(vip,":timebase:window:range %f\n",zoomRange[2]);
		viPrintf(vip,":measure:source channel%d\n",3);
		UI[1]=autoV(3)/Cv;//iFH
		viPrintf(U2751,"ROUTe:OPEN (@402,304)\n");
		viPrintf(U2751,"ROUTe:CLOSe (@404)\n");//���׷���ˮ������
		viPrintf(vip,":timebase:window:position %f\n",zoomPosition[3]);
		viPrintf(vip,":timebase:window:range %f\n",zoomRange[3]);
		viPrintf(vip,":measure:source channel%d\n",1);
		u[2]=autoV(1)/Gain[0];//uHJ
		viPrintf(vip,":timebase:window:position %f\n",zoomPosition[2]);
		viPrintf(vip,":timebase:window:range %f\n",zoomRange[2]);
		viPrintf(vip,":measure:source channel%d\n",3);
		UI[2]=autoV(3)/Cv;//iHJ
	
		float M_j=20*log10(sqrt((u[0]/UI[0])*(u[2]/UI[2])/(u[1]/UI[1])*(d[0]*d[2]/d[1])*(2/(1000*f))*exp(0.0005f*(d[0]+d[2]-d[1]))))-120;
		//��ʽ�е�ruo=1000kg/m3,alpha=0.0005
		Result[0].push_back(M_j);//ˮ��������ʾ����1ͨ����
		huatu_huyi();
		if(OneThird_f)
		{
			count_OTFreq++;
			f=OneThirdFreq[OTFreq+count_OTFreq];
		}
		else f+=deltaf;
	}

	viPrintf(vip,":timebase:mode main\n");
	return 0;

}
void CMeasure::huatu_huyi()
{
	int x=0,y=0;
	CWnd *pWnd=GetDlgItem(IDC_picture);
	CRect rect;
	pWnd->GetClientRect(rect);
	CDC* pDC=pWnd->GetDC();
	CBrush rebrush;
	rebrush.CreateSolidBrush (RGB(255,255,255));//��ɫˢ��
	CBrush *pOldBrush=pDC->SelectObject (&rebrush);
	pDC->Rectangle (rect);//���picture�еĻ滭
	pDC->SelectObject (pOldBrush);
	CPen pNewPen;
	pNewPen.CreatePen(PS_SOLID,1,RGB(0,0,0));
	CPen* pOldPen=pDC->SelectObject(&pNewPen);
	int deltaX=rect.Width()/50;
	int deltaY=rect.Height()/100;//100�����ȡ�ģ����������Ϊ100��
	pDC->SetViewportOrg(rect.left,rect.top);//����������ʱ���Ǹ�ֵ,ԭ����Ϊ�����Ͻǵĵ�
	//��������
	CString str;
	float temp=0.0;
	for(x=0;x<=50;x+=1)
	{
		pDC->MoveTo((int)(x*deltaX),0);
		pDC->LineTo((int)(x*deltaX),rect.Height());
	}
	for(x=0;x<=50;x+=5)
	{
		if(endf==startf)
		{
			str.Format("%.1f",(startf+x)/1000);
			pDC->TextOutA((int)(x*deltaX-10),rect.top-15,str);
		}
		else
		{
			temp=startf+(endf-startf)/50*x;
			str.Format("%.1f",temp/1000);
			pDC->TextOutA((int)(x*deltaX-10),rect.top-15,str);
		}
	}
	for(y=0;y<=100;y+=10)
	{
		pDC->MoveTo(rect.left,y*deltaY);
		pDC->LineTo(rect.right,y*deltaY);
		str.Format("%d",-150-y);//�����ʾ-150~-250��100���㣬�������ܹ����ֳ���100������ÿһ�����y
		pDC->TextOutA(rect.left-40,y*deltaY-10,str);
	}
	str.Format("Ƶ��(kHz)");
	pDC->TextOutA(100,-50,str);
	str.Format("��");
	pDC->TextOutA(-80,200,str);
	str.Format("��");
	pDC->TextOutA(-80,220,str);
	str.Format("��");
	pDC->TextOutA(-80,240,str);
	str.Format("(dB)");
	pDC->TextOutA(-85,260,str);
	str.Format("dB re 1/uPa");
	pDC->TextOutA(rect.left+10,rect.bottom+20,str);
	pDC->SelectObject(pOldPen);
	CPen pPen;
	pPen.CreatePen(PS_SOLID,3,RGB(255,0,0));
	pOldPen=pDC->SelectObject(&pPen);
	if(Result[0].size()!=0)
	{
		if(endf!=startf)
		{
			if(Result[0].size()==1)
			{
				pDC->SetPixel(0,-(int)((Result[0][0]+150)*deltaY),RGB(255,0,0));
			}
			else
			{
				pDC->MoveTo(0,-(int)((Result[0][0]+150)*deltaY));//�������Ƶ���㣬������ֵ�Ǹ��ģ���ͼ����������
				if(OneThird_f)
				{
					for(unsigned int i=1;i<Result[0].size();i++)
					{

						x=(int)((OneThirdFreq[OTFreq+i]-startf)*50/(endf-startf)*deltaX);
						y=-(int)((Result[0][i]+150)*deltaY);
						pDC->LineTo(x,y);//����������
					}
				}
				else
				{
					for(unsigned int i=1;i<Result[0].size();i++)
					{
						x=(int)(i*deltaf*deltaX*50/(endf-startf));
						y=-(int)((Result[0][i]+150)*deltaY);
						pDC->LineTo(x,y);//����������
					}
				}
			}
		}
		else
		{
			pDC->SetPixel(0,-(int)((Result[0][0]+150)*deltaY),RGB(255,0,0));
		}

	}
	pDC->SelectObject(pOldPen);	
}
void CMeasure::MeaSetManual()
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
	m_com.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_com.get_Input();
	CString str;
	str=retVal.bstrVal;//��Ϊ���ڷ��ص����ַ������͵ģ�retVal��vt��VT_BSTR
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("�����ֶ�ģʽ��������!");
	}
}
float CMeasure::MeaReadCurrentAngle()
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
	m_com.put_Output(COleVariant(databuf));
	Sleep(200);
	//angleΪ���ջ����ĵ�8λ��ʼ��4λ
	VARIANT retVal=m_com.get_Input();
	CString str;
	str=retVal.bstrVal;//��Ϊ���ڷ��ص����ַ������͵ģ�retVal��vt��VT_BSTR
	int temp;
	CString anglestring;
	anglestring.Append(str.Mid(7,1));
	anglestring.Append(str.Mid(7,1));
	anglestring.Append(str.Mid(7,1));
	anglestring.Append(str.Mid(7,1));
	anglestring+=str.Mid(7,4);
	sscanf_s(anglestring,"%X",&temp);
	//���ַ�����Ҫ��Ϊ��ֵ��Ȼ�����10���ǽǶ�
	return temp/10.0f;
}
void CMeasure::MeaRotateRight(int speed)
{
	//�������ٶ�
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
	m_com.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_com.get_Input();
	CString str;
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("�����ٶȲ�������!");
	}
	//˳ʱ��ת��
	databuf.SetSize(19);
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
	m_com.put_Output(COleVariant(databuf));
	Sleep(200);
	retVal=m_com.get_Input();
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("˳ʱ��ת����������!");
	}
}
void CMeasure::MeaRotateLeft(int speed)
{
	//�������ٶ�
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
	m_com.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_com.get_Input();
	CString str;
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("�����ٶȲ�������!");
	}
	//��ʱ��ת
	databuf.SetSize(19);
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
	m_com.put_Output(COleVariant(databuf));
	Sleep(200);
	retVal=m_com.get_Input();
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("��ʱ��ת����������!");
	}
}
void CMeasure::MeaRotateTargetAngle(int speed,int targetangle)
{
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	//��Ϊ�Զ�
	databuf.SetSize(19);
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
	m_com.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_com.get_Input();
	CString str;
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("��Ϊ�Զ���������!");
	}
	//ֹͣת��
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
	m_com.put_Output(COleVariant(databuf));
	Sleep(200);
	retVal=m_com.get_Input();
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("ֹͣת����������!");
	}
	//�������ٶ�
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
	m_com.put_Output(COleVariant(databuf));
	Sleep(200);
	retVal=m_com.get_Input();
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("�����ٶȲ�������!");
	}
	//����Ŀ��Ƕ�
	char strangle[9];
	databuf.SetSize(21);
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
	m_com.put_Output(COleVariant(databuf));
	Sleep(200);
	retVal=m_com.get_Input();
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("����Ŀ��ǶȲ�������!");
	}
	//��λ����
	databuf.SetSize(19);
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
	m_com.put_Output(COleVariant(databuf));
	Sleep(200);
	retVal=m_com.get_Input();
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("��λ������������!");
	}

}
void CMeasure::MeaStopRotate()
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
	m_com.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_com.get_Input();
	CString str;
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("ֹͣת����������!");
	}
}
void CMeasure::MeaStopRotateRight()
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
	m_com.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_com.get_Input();
	CString str;
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("˳ʱ��ͣ��������!");
	}
}
void CMeasure::MeaStopRotateLeft()
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
	m_com.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_com.get_Input();
	CString str;
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("��ʱ��ͣ��������!");
	}
}

