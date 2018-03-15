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

//excel需要
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
BOOL CMeasure::OnInitDialog()//加载对话框时的初始化函数
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	ModifyStyleEx(WS_EX_TOOLWINDOW, WS_EX_APPWINDOW);//运行程序的时候可以显示图标
	//CRect rect;  
	//GetDlgItem(IDC_picture)->GetClientRect(&rect);
	//GetDlgItem(IDC_picture)->MoveWindow(50,50,800,600,true); 
	//固定Picture Control控件的位置和大小 
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
	 SetDlgItemTextA(IDC_showPara,"\r\n\r\n\r\n\r\n等待测量......");
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
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
	// TODO: 在此添加命令处理程序代码
	if(pturntable!=NULL) delete pturntable;
	pturntable = new CTurnTable(); //给指针分配内存 
	pturntable->Create(IDD_turntable); //创建一个非模态对话框  
	pturntable->ShowWindow(SW_SHOW); //显示非模态对话框 
	

}
void autoScale(int chaflag,int cha,int chacount)
{
	float vRange=-1,vTemp=-1;
	int flag=0;
	
	viPrintf(vip,":measure:source channel%d\n",cha);
	viQueryf(vip,":channel%d:range?\n","%f\n",cha,&vRange);//获得纵向的电压范围
	viQueryf(vip,":measure:vpp?\n","%f\n",&vTemp);//测量峰峰值
			
	while(vTemp==-1||vRange==-1)
	{
		if(flag>2) 
		{
			AfxMessageBox("示波器没有信号！");
			return;
		}
		flag++;
		viQueryf(vip,":channel%d:range?\n","%f\n",cha,&vRange);
		viQueryf(vip,":measure:vpp?\n","%f\n",&vTemp);
	}
	flag=0;
	while(vTemp>9e+37)//波形显示超出了屏幕外
	{
		if(flag>10||2*vRange>40) 
		{
			viPrintf(vip,":channel%d:range 40\n",cha);
			viQueryf(vip,":channel%d:range?\n","%f\n",cha,&vRange);
			viQueryf(vip,":measure:vpp?\n","%f\n",&vTemp);
			break;
		}
		flag++;
		viPrintf(vip,":channel%d:range %f\n",cha,2*vRange);//重新扩大电压范围
		viQueryf(vip,":channel%d:range?\n","%f\n",cha,&vRange);
		viQueryf(vip,":measure:vpp?\n","%f\n",&vTemp);
	}
	flag=0;
			
	//波形显示大于四格
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
	//波形显示小于一格
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
		viPrintf(vip,":channel%d:offset 0\n",cha);//只有一个通道就在中间显示
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
	//波形显示小于一格
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
	//viQueryf(vip,":measure:vrms?\n","%f\n",&vtemp);//有效值不受尖刺波的影响
	return vtemp;
}
void CMeasure::OnBnClickedView()
{
	// TODO: Add your control notification handler code here
	SetDlgItemTextA(IDC_Show,"正在调整信号，请稍候......");
	renew();
	
	//自动调整，合适的显示波形
	int chflag=0,chCount=0;	
	
	for(int i=0;i<4;i++)
	{
		if(isChaChoose[i]) {
			chCount++;
			viPrintf(vip,":channel%d:display on\n",i+1);//打开通道
			viPrintf(vip,":channel%d:offset 0\n",i+1);			
		}
		else viPrintf(vip,":channel%d:display off\n",i+1);
	}
	if(chCount==0)//如果没有选择使用的通道，提示并返回
	{
		AfxMessageBox("请在示波器参数设置中选择测量通道！");
		viPrintf(vip,"*rst\n");
		SetDlgItemTextA(IDC_Show,"");
		return;
	}
	if(ChooseItem==5)
	{
		if(status!=0)
		{
			AfxMessageBox("开关矩阵连通不正确！");
			viClose(U2751);
			viClose(rm);
			SetDlgItemTextA(IDC_Show,"");
			return;
		}
		viClear(U2751);
		viPrintf(U2751,"*rst\n");//开关矩阵复位
		viPrintf(U2751,"*cls\n");//开关矩阵
		CreateBurst(f,v/1000,Bwid/1000,Brep);
		Sleep(100);
		viPrintf(U2751,"ROUTe:CLOSe (@402,107,304)\n");
		//连通402、107和304,发射换能器发，水听器收，互易换能器收
		ScopeTrigger();
		viPrintf(vip,"timebase:range %f\n",(Bwid/1000.0)*4);//设置时间轴代表的时间长度
		autoScale(1,1,3);//调整第1个显示的是1通道，一共有3个通道，水听器接收的通道
		autoScale(2,2,3);//互易换能器接收的通道
		autoScale(3,3,3);//电流计接收的通道
	}
	else
	{
		if(ChooseItem==4)
		{
			CreateMulFrePulse(Fs,f,deltaf,Bwid/1000,v/1000,Brep);
			Sleep(100);
			ScopeTrigger();
			viPrintf(vip,"timebase:range %f\n",(Bwid/1000.0)*6*PulseCount);//设置时间轴代表的时间长度
		}
		else 
		{
			CreateBurst(f,v/1000,Bwid/1000,Brep);
			Sleep(100);
			ScopeTrigger();
			viPrintf(vip,"timebase:range %f\n",(Bwid/1000.0)*4);//设置时间轴代表的时间长度
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
	SetDlgItemTextA(IDC_Show,"请在示波器上观察信号......");
}
void CMeasure::OnBnClickedChangesignal()
{
	// TODO: Add your control notification handler code here
	if(!isChange)
	{
		CChangeSig *signal=new CChangeSig();
		signal->Create(IDD_ChangeSig); //创建一个非模态对话框  
		signal->ShowWindow(SW_SHOW); //显示非模态对话框 
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
			AfxMessageBox("请选择标准水听器文件！");
			return;
		}
		if(startf<(standMp.begin()->first)||endf>(standMp.rbegin()->first))
		{
			AfxMessageBox("测量频率范围超过标准水听器的范围！");
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
		AfxMessageBox("请选择示波器的测量通道！");
		return;
	}
	int meaStatus=0;
	CString strshow;
	switch(ChooseItem)
	{
	case 0: //测量灵敏度
		meaStatus=MeasureSensity();
		if(meaStatus==-1) 
		{
			SetDlgItemTextA(IDC_Show,"中止测量灵敏度......");
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
					s.Format("\r\n\r\n灵敏度测量结果：\r\n待测水听器通道 %d通道\r\n",ch+1);
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
					s.Format("最大灵敏度级\r\n  %.2fdB @ %.2fHz\r\n最小灵敏度级\r\n  %.2fdB @ %.2fHz",maxm,maxf,minm,minf);
					strshow+=s;
				}				
			}
			SetDlgItemTextA(IDC_showPara,strshow);
		}		
		break;
	case 1://测量发送电压响应
		meaStatus=MeasureResponse();
		if(meaStatus==-1) 
		{
			SetDlgItemTextA(IDC_Show,"中止测量发送电压响应......");
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
					s.Format("\r\n发送电压响应测量结果：\r\n待测发射换能器通道 %d通道\r\n",ch+1);
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
					s.Format("最大发送电压响应级\r\n  %.2fdB @ %.2fHz\r\n最小发送电压响应级\r\n  %.2fdB @ %.2fHz",maxm,maxf,minm,minf);
					strshow+=s;
				}				
			}
			SetDlgItemTextA(IDC_showPara,strshow);
		}
		break;
	case 2://测量接收指向性
	case 3://测量发射指向性
		m_DirPic=1;
		UpdateData(false);
		meaStatus=MeasureDir();
		if(meaStatus==-1) 
		{
			SetDlgItemTextA(IDC_Show,"中止单频点指向性测量......");
			return;
		}
		SetTimer(3,1000,NULL);
		GetDlgItemTextA(IDC_showPara,strshow);
		for(int ch=0;ch<4;ch++)
		{
			if(isChaChoose[ch])
			{
				CString s;
				s.Format("\r\n\r\n指向性测量结果：\r\n待测换能器通道 %d通道\r\n",ch+1);
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
						a[1]=MeaAngle[j];break;//不确定是否真的有-3dB的值
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
				s.Format("主瓣的角度位置 %.1f°\r\n-3dB波束宽度 %.1f°",maxa,abs(da1-da2));
				strshow+=s;
			}				
		}
		SetDlgItemTextA(IDC_showPara,strshow);
		break;
	case 4://多频点测量指向性
		meaStatus=MeaMulDir();
		if(meaStatus==-1) 
		{
			SetDlgItemTextA(IDC_Show,"中止多频点测量指向性......");
			return;
		}
		SetTimer(4,1000,NULL);
		break;
	case 5://互易法自动测量灵敏度
		meaStatus=MeaHuyi();
		if(meaStatus==-1) 
		{
			SetDlgItemTextA(IDC_Show,"中止互易法自动测量灵敏度......");
			return;
		}
		SetTimer(5,1000,NULL);
		break;
	}
	SetDlgItemTextA(IDC_Show,"测量完成......");
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
		viPrintf(U2751,"*rst\n");//开关矩阵复位
		viClose(U2751);
		viClose(rm);
	}
	OnCancel();//因为它是非模态对话框，自己重载了这个函数	
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
		AfxMessageBox("无法启动Excel服务器!");  
		return;  
    } 
	app.put_Visible(true);
	books.AttachDispatch(app.get_Workbooks());
	//得到Workbook  
    book.AttachDispatch(books.Add(vtMissing));  
    //得到Worksheets  
    sheets.AttachDispatch(book.get_Worksheets());
	sheet.AttachDispatch(sheets.get_Item(variant_t("sheet1")));
	CString strPage="第1页";
	sheet.put_Name(strPage);
	for(int i=0;i<sheets.get_Count()-1;i++)
	{
		sheet=sheet.get_Next();
		strPage.Format("第%d页",i+2);
		//为每个页面设置名字
		sheet.put_Name(strPage);
	}
	sheet.AttachDispatch(sheets.get_Item(_variant_t("第1页")));
	range.AttachDispatch(sheet.get_Cells());
	switch(ChooseItem)
	{
	case 0:
		range.put_Item(_variant_t((long)1),_variant_t((long)1),
			_variant_t("比较法测量灵敏度级"));
		range.put_Item(_variant_t((long)2),_variant_t((long)1),
			_variant_t("频率（Hz)"));
		range.put_Item(_variant_t((long)2),_variant_t((long)2),
				_variant_t("灵敏度级(dB)"));
		break;
	case 1:
		range.put_Item(_variant_t((long)1),_variant_t((long)1),
			_variant_t("比较法测量发射电压响应级"));
		range.put_Item(_variant_t((long)2),_variant_t((long)1),
			_variant_t("频率（Hz)"));
		range.put_Item(_variant_t((long)2),_variant_t((long)2),
				_variant_t("发射电压响应级(dB)"));
		break;
	case 2:
		range.put_Item(_variant_t((long)1),_variant_t((long)1),
			_variant_t("单频点接收换能器指向性"));
		range.put_Item(_variant_t((long)2),_variant_t((long)1),
			_variant_t("角度(°)"));
		range.put_Item(_variant_t((long)2),_variant_t((long)2),
				_variant_t("电压(V)"));
		break;
	case 3:
		range.put_Item(_variant_t((long)1),_variant_t((long)1),
			_variant_t("单频点发射换能器指向性"));
		range.put_Item(_variant_t((long)2),_variant_t((long)1),
			_variant_t("角度(°)"));
		range.put_Item(_variant_t((long)2),_variant_t((long)2),
				_variant_t("电压(V)"));
		break;
	case 4:
		range.put_Item(_variant_t((long)1),_variant_t((long)1),
			_variant_t("多频点换能器指向性"));
		range.put_Item(_variant_t((long)2),_variant_t((long)1),
			_variant_t("角度(°)"));
		range.put_Item(_variant_t((long)2),_variant_t((long)2),
				_variant_t("电压(V)"));
		break;
	case 5:
		range.put_Item(_variant_t((long)1),_variant_t((long)1),
			_variant_t("互易法测量灵敏度级"));
		range.put_Item(_variant_t((long)2),_variant_t((long)1),
			_variant_t("频率（Hz)"));
		range.put_Item(_variant_t((long)2),_variant_t((long)2),
				_variant_t("灵敏度级(dB)"));
		break;

	}

	int col=1;//有多列的时候
	if(ChooseItem==5)
	{
		if(OneThird_f)
		{
			for(unsigned int j=0;j<Result[0].size();j++)
			{
				range.put_Item(_variant_t((long)(j+3)),_variant_t((long)1),
					_variant_t(OneThirdFreq[OTFreq+j]));
				//设置j+3排的第2列数据或第3列……
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
				//设置j+3排的第2列数据或第3列……
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
				if(ChooseItem==4)//多频点测指向性
				{
					for(int j=0;j<PulseCount;j++)
					{
						if(Result[j].size()==0)continue;
						for(unsigned int k=0;k<Result[j].size();k++)
						{
							range.put_Item(_variant_t((long)(k+3)),_variant_t((long)1),
							_variant_t(MeaAngle[k]));
							//设置k+3排的第2列数据或第3列……
							range.put_Item(_variant_t((long)(k+3)),_variant_t((long)(2*j+2)),
							_variant_t(Result[j][k]));
						}
					}
				}
				else if((ChooseItem==0||ChooseItem==1)&&MeaCount>1)//灵敏度和发送响应多次测量
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
							//设置j+3排的第2列数据或第3列……
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
						//设置j+3排的第1列数据
						switch(ChooseItem)
						{
						case 0:
						case 1:
							if(OneThird_f)
								range.put_Item(_variant_t((long)(j+3)),_variant_t((long)1),
									_variant_t(OneThirdFreq[OTFreq+j]));
							else range.put_Item(_variant_t((long)(j+3)),_variant_t((long)1),
							_variant_t(startf+deltaf*j));
							//设置j+3排的第2列数据或第3列……
							range.put_Item(_variant_t((long)(j+3)),_variant_t((long)col),
							_variant_t(Result[i][j]));
							break;
						case 2:
						case 3:
							range.put_Item(_variant_t((long)(j+3)),_variant_t((long)1),
							_variant_t(MeaAngle[j]));
							//设置j+3排的第2列数据或第3列……
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
	////strPath+="\\测试数据.xls";
	////保存excel文件
	//sheet.SaveAs(szPath,vtMissing,vtMissing,vtMissing,vtMissing,
	//	vtMissing,vtMissing,vtMissing,vtMissing,vtMissing);
	//
	//释放对象    
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
	SetDlgItemTextA(IDC_Show,"准备测量，请稍候......");
	float Mp=-1;
	CreateBurst(f,v/1000,Bwid/1000,Brep);
	MessageBox("请根据提示选择各个通道的测量区域！");
	viPrintf(vip,":timebase:mode window\n");
	for(int i=0;i<4;i++)
	{
		if(isChaChoose[i])
		{
			CString s;
			s.Format("完成选择通道%d的测量区域后点击确定",i+1);
			MessageBox(s);
			viQueryf(vip,":timebase:window:position?\n","%f\n",&zoomPosition[i]);
			viQueryf(vip,":timebase:window:range?\n","%f\n",&zoomRange[i]);
		}
	}
	viPrintf(vip,":timebase:mode main\n");
	viPrintf(vip,":run\n");
	CString stemp;
	stemp.Format("频率范围 %.1fkHz~%.1fkHz\r\n信号源幅度 %.1fmVpp\r\n脉冲宽度 %.1fms\r\n重复周期 %.1fs\r\n测量次数 %d次\r\n标准水听器通道 %d",
		startf/1000,endf/1000,v,Bwid,Brep,MeaCount,chaRefer);
	if(MessageBox(stemp,"提示",MB_OKCANCEL)==IDCANCEL) return -1;
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
			stemp.Format("正在进行第 %d 次测量，请稍候......",c+1);
			SetDlgItemTextA(IDC_Show,stemp);
		}
		else SetDlgItemTextA(IDC_Show,"正在测量灵敏度，请稍候......");
		unsigned int count_freq=0;
		while(isMeasure&&f<=endf)
		{
			//用来接收“停止测量”按钮按下的消息
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
					//使用线性插值计算该频率点的灵敏度值
					Mp=LineP(standMp.lower_bound(f)->first,standMp.lower_bound(f)->second,
						standMp.upper_bound(f)->first,standMp.upper_bound(f)->second,f);
				}
				else
					Mp=it->second;
				Result[i].push_back(CalSensity(Mp,u[i],u[chaRefer-1],d[i],d[chaRefer-1]));
				//测出的电压值要除以放大倍数
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
		rebrush.CreateSolidBrush (RGB(255,255,255));//白色刷子
		CBrush *pOldBrush=pDC->SelectObject (&rebrush);
		pDC->Rectangle (rect);//清空picture中的绘画
		pDC->SelectObject (pOldBrush);
	}
	CPen pNewPen;
	pNewPen.CreatePen(PS_SOLID,1,RGB(0,0,0));
	CPen* pOldPen=pDC->SelectObject(&pNewPen);
	int deltaX=rect.Width()/50;
	int deltaY=rect.Height()/100;//100是随机取的，代表纵轴分为100份
	pDC->SetViewportOrg(rect.left,rect.top);//测量灵敏度时都是负值,原点设为最左上角的点
	//画网格线
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
		str.Format("%d",-150-y);//纵轴表示-150~-250共100个点，而纵轴总共划分出了100格，所以每一格代表y
		pDC->TextOutA(rect.left-40,y*deltaY-10,str);
	}
	str.Format("频率(kHz)");
	pDC->TextOutA(100,-50,str);
	str.Format("灵");
	pDC->TextOutA(-80,200,str);
	str.Format("敏");
	pDC->TextOutA(-80,220,str);
	str.Format("度");
	pDC->TextOutA(-80,240,str);
	str.Format("(dB)");
	pDC->TextOutA(-85,260,str);
	str.Format("dB re 1/uPa");
	pDC->TextOutA(rect.left+10,rect.bottom+20,str);
	pDC->SelectObject(pOldPen);
	CPen pPen[4];
	pPen[0].CreatePen(PS_SOLID,3,RGB(255,165,0));//橙色画笔
	pPen[1].CreatePen(PS_SOLID,3,RGB(0,255,0));//绿色画笔
	pPen[2].CreatePen(PS_SOLID,3,RGB(0,0,255));//蓝色画笔
	pPen[3].CreatePen(PS_SOLID,3,RGB(255,0,0));//红色画笔

	////debug
	//float Result[10]={-210.7f,-210.6f,-218.1f,-217.6f,-215.8f,-214.9f,-215.3f,-215.4f,-215.9f,-217.0f};
	//pDC->MoveTo(0,(int)((Result[0]+150)*deltaY));
	//for(unsigned int i=1;i<10;i++)
	//{
	//	x=i*deltaX;
	//	y=(int)((Result[i]+150)*deltaY);
	//	pDC->LineTo(x,y);//连接两个点
	//}
	////debug
	for(int ch=0;ch<4;ch++)
	{
		if(isChaChoose[ch])
		{
			pOldPen=pDC->SelectObject(&pPen[ch]);
			if(Result[ch].size()==0) continue;//注意要用continue，如果用break直接就跳出循环了，不会画其他通道的图形
			pDC->MoveTo(0,-(int)((Result[ch][0]+150)*deltaY));//将画笔移到起点，灵敏度值是负的，画图向下是正的
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
					pDC->LineTo(x,y);//连接两个点
				}
			}
			else
			{
				for(unsigned int i=1;i<Result[ch].size();i++)
				{
					x=(int)(i*deltaf*deltaX*50/(endf-startf));
					y=-(int)((Result[ch][i]+150)*deltaY);
					pDC->LineTo(x,y);//连接两个点
				}
			}
		}
	}

	pDC->SelectObject(pOldPen);

}
int CMeasure::MeasureResponse()
{
	SetDlgItemTextA(IDC_Show,"准备测量，请稍候......");
	float Mp=-1;
	CreateBurst(f,v/1000,Bwid/1000,Brep);
	MessageBox("请根据提示选择各个通道的测量区域！");
	viPrintf(vip,":timebase:mode window\n");
	for(int i=0;i<4;i++)
	{
		if(isChaChoose[i])
		{
			CString s;
			s.Format("完成选择通道%d的测量区域后点击确定",i+1);
			MessageBox(s);
			viQueryf(vip,":timebase:window:position?\n","%f\n",&zoomPosition[i]);
			viQueryf(vip,":timebase:window:range?\n","%f\n",&zoomRange[i]);
		}
	}
	viPrintf(vip,":timebase:mode main\n");
	viPrintf(vip,":run\n");
	CString stemp;
	stemp.Format("测量的频率范围 %.1fkHz~%.1fkHz\r\n信号源幅度 %.1fmVpp\r\n脉冲宽度 %.1fms\r\n重复周期 %.1fs\r\n%d次测量\r\n标准水听器通道 %d",
		startf/1000,endf/1000,v,Bwid,Brep,MeaCount,chaRefer);
	if(MessageBox(stemp,"提示",MB_OKCANCEL)==IDCANCEL) return -1;
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
			stemp.Format("正在进行第 %d 次测量，请稍候......",c+1);
			SetDlgItemTextA(IDC_Show,stemp);
		}
		else SetDlgItemTextA(IDC_Show,"正在测量发送电压响应，请稍候......");
		unsigned int count_freq=0;
		while(isMeasure&&f<=endf)
		{
			//用来接收“停止测量”按钮按下的消息
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
					//使用线性插值计算该频率点的灵敏度值
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
		rebrush.CreateSolidBrush (RGB(255,255,255));//白色刷子
		CBrush *pOldBrush=pDC->SelectObject (&rebrush);
		pDC->Rectangle (rect);//清空picture中的绘画
		pDC->SelectObject (pOldBrush);
	}
	CPen pNewPen;
	pNewPen.CreatePen(PS_SOLID,1,RGB(0,0,0));
	CPen* pOldPen=pDC->SelectObject(&pNewPen);
	int deltaX=rect.Width()/50;
	int deltaY=rect.Height()/100;//100是随机取的，代表纵轴分为100份
	pDC->SetViewportOrg(rect.left,rect.bottom);//原点设为最左下角的点
	//画网格线
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
		str.Format("%d",50+2*y);//纵轴表示50~250共200个点，而纵轴总共划分出了100格，所以每一格代表2y
		pDC->TextOutA(rect.left-30,-y*deltaY-10,str);
	}
	str.Format("频率(kHz)");
	pDC->TextOutA(400,30,str);
	str.Format("发");
	pDC->TextOutA(-80,-280,str);
	str.Format("送");
	pDC->TextOutA(-80,-260,str);
	str.Format("响");
	pDC->TextOutA(-80,-240,str);
	str.Format("应");
	pDC->TextOutA(-80,-220,str);
	str.Format("(dB)");
	pDC->TextOutA(-85,-200,str);
	str.Format("dB re 1/uPa");
	pDC->TextOutA(rect.left+10,-rect.bottom-30,str);
	pDC->SelectObject(pOldPen);
	CPen pPen[4];
	pPen[0].CreatePen(PS_SOLID,3,RGB(255,165,0));//橙色画笔
	pPen[1].CreatePen(PS_SOLID,3,RGB(0,255,0));//绿色画笔
	pPen[2].CreatePen(PS_SOLID,3,RGB(0,0,255));//蓝色画笔
	pPen[3].CreatePen(PS_SOLID,3,RGB(255,0,0));//红色画笔
	//pOldPen=pDC->SelectObject(&pPen);

	////debug
	//float Result[10]={-210.7f,-210.6f,-218.1f,-217.6f,-215.8f,-214.9f,-215.3f,-215.4f,-215.9f,-217.0f};
	//pDC->MoveTo(0,(int)((Result[0]+150)*deltaY));
	//for(unsigned int i=1;i<10;i++)
	//{
	//	x=i*deltaX;
	//	y=(int)((Result[i]+150)*deltaY);
	//	pDC->LineTo(x,y);//连接两个点
	//}
	////debug
	for(int ch=0;ch<4;ch++)
	{
		if(isChaChoose[ch])
		{
			pOldPen=pDC->SelectObject(&pPen[ch]);
			if(Result[ch].size()==0) continue;//注意要用continue，如果用break直接就跳出循环了，不会画其他通道的图形
			pDC->MoveTo(0,-(int)((Result[ch][0]-50)/2*deltaY));//将画笔移到起点
			if(endf==startf) continue;
			if(OneThird_f)
			{
				for(unsigned int i=1;i<Result[ch].size();i++)
				{

					x=(int)((OneThirdFreq[OTFreq+i]-startf)*50/(endf-startf)*deltaX);
					y=-(int)((Result[ch][i]-50)/2*deltaY);
					pDC->LineTo(x,y);//连接两个点
				}
			}
			else 
			{
				for(unsigned int i=1;i<Result[ch].size();i++)
				{
					x=(int)(i*deltaf*deltaX*50/(endf-startf));
					y=-(int)((Result[ch][i]-50)/2*deltaY);
					pDC->LineTo(x,y);//连接两个点
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
		AfxMessageBox("串口正在使用，请先关闭控制回转系统！");
		m_com.put_PortOpen(false);
		return -1;
	}
	if(!m_com.get_PortOpen())
	{
		m_com.put_PortOpen(true);         //打开串口
	}
	else
	{
		m_com.put_OutBufferCount(0);
		MessageBox("串口2打开失败");
		m_com.put_PortOpen(false);
		return -1;
	}
	MeaSetManual();//设为手动
	CString stemp;
	stemp.Format("当前频率为 %.1fHz\r\n测量的角度范围 %d°~%d°\r\n回转速度为 %d秒/圈\r\n重复周期 %.1fs",f,StartAngle,EndAngle,Speed,Brep);
	if(MessageBox(stemp,"是否测量？",MB_OKCANCEL)==IDCANCEL) 
	{
		m_com.put_OutBufferCount(0);
		m_com.put_PortOpen(false);
		return -1;
	}
	SetDlgItemTextA(IDC_showPara,stemp);
	m_DirPic=0;
	f=startf;
	isMeasure=true;
	CreateBurst(f,v/1000,Bwid/1000,Brep);//触发信号源
	MessageBox("请根据提示选择各个通道的测量区域！");
	viPrintf(vip,":timebase:mode window\n");
	for(int i=0;i<4;i++)
	{
		if(isChaChoose[i])
		{
			CString s;
			s.Format("完成选择通道%d的测量区域后点击确定",i+1);
			MessageBox(s);
			viQueryf(vip,":timebase:window:position?\n","%f\n",&zoomPosition[i]);
			viQueryf(vip,":timebase:window:range?\n","%f\n",&zoomRange[i]);
		}
	}
	viPrintf(vip,":timebase:mode main\n");
	viPrintf(vip,":run\n");
	angle=MeaReadCurrentAngle();//读出当前的角度值
	huatu_recidir(); 
	bool isRightDir=true;
	SetDlgItemTextA(IDC_Show,"正在转到指定角度，请稍候......");
	if(angle<=StartAngle)
	{
		MeaRotateTargetAngle(Speed,StartAngle);//如果当前位置与起始角度靠近，就转到起始角度
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
		MeaRotateTargetAngle(Speed,StartAngle);//如果当前位置与起始角度靠近，就转到起始角度
		isRightDir=true;
	}
	angle=MeaReadCurrentAngle();
	while((isRightDir&&abs(angle-StartAngle)>0.1)||(!isRightDir&&abs(angle-EndAngle)>0.1))//之前调试的时候用写的1，但是相差1度有点大，所以改为0.1试试
	{
		angle=MeaReadCurrentAngle();
	}//等待转到指定角度完成
	 
	viPrintf(vip,":run\n");
	SetDlgItemTextA(IDC_Show,"正在进行单频点指向性测量，请稍候......");
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
	}//读取第一个角度的电压值
	angle=MeaReadCurrentAngle();
	MeaAngle.push_back(angle);
	if(isRightDir) MeaRotateRight(Speed);//顺时针转动起来
	else MeaRotateLeft(Speed);//逆时针转动
	angle=MeaReadCurrentAngle();	
	while((isRightDir&&angle<EndAngle)||(!isRightDir&&angle>StartAngle))
	{
		//用来接收“停止测量”按钮按下的消息
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
		 
		//保存角度和电压值
		for(int i=0;i<4;i++)
		{
			if(!isChaChoose[i]) continue;
			Result[i].push_back(u[i]);
		}
		
		//绘制极坐标图
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
	}//最后一个角度的电压值
	angle=MeaReadCurrentAngle();
	MeaAngle.push_back(angle);
	 
	huatu_recidir();
	viPrintf(vip,":timebase:mode main\n");
	clock_t t_end=clock();
	stemp.Format("测量时间为 %.4f s",(double)(t_end-t_start)/CLOCKS_PER_SEC);
	MessageBox(stemp);
	if(m_com.get_PortOpen())
		m_com.put_PortOpen(false);
	return 0;
}
void CMeasure::huatu_recidir()
{
	int cycle_num=10,redius_num=36;//cycle_num是同心圆的个数，redius_num是直径的条数
	CWnd *pWnd = GetDlgItem(IDC_picture);
	CRect rect;
	pWnd->GetClientRect(rect);
	CDC *pDC = pWnd->GetDC();
	CBrush rebrush;
	rebrush.CreateSolidBrush (RGB(255,255,255));//白色刷子
	CBrush *pOldBrush=pDC->SelectObject (&rebrush);
	pDC->Rectangle (rect);//清空picture中的绘画
	pDC->SelectObject (pOldBrush);
	CPen newPen;
	newPen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	CPen* pOldPen = (CPen*)pDC->SelectObject(&newPen);
	pDC->SelectStockObject(NULL_BRUSH);//没有画刷就避免圆被覆盖，画的空心圆
	int w=rect.Width(),h=rect.Height ();
	pDC->SetViewportOrg(w/2,h/2);//设置原点
	int R=(h-60)/2;//半径
	float deltaR=(float)R/cycle_num;
	for(int i=1;i<=cycle_num;i++)
	{
		if(i==cycle_num-3) continue;
		pDC->Ellipse(-(int)(i*deltaR),-(int)(i*deltaR),(int)(i*deltaR),(int)(i*deltaR));//画圆，其实是找到一个外切正方形的左上角顶点和右下角顶点
	}
	pDC->Ellipse(-(int)(cycle_num*deltaR),-(int)(cycle_num*deltaR),(int)(cycle_num*deltaR),(int)(cycle_num*deltaR));//最外面的圆
	float deltaA=2*PI/redius_num;
	int x,y;
	for(int i=0;i<=redius_num/2;i++)
	{
		pDC->MoveTo(0,0);
		x=(int)(R*sin(i*deltaA));
		y=(int)(R*cos(i*deltaA));
		pDC->LineTo(x,y);//画了一、四象限的半径
		pDC->MoveTo(0,0);
		x=(int)(R*sin(-i*deltaA));
		y=(int)(R*cos(-i*deltaA));
		pDC->LineTo(x,y);//画了二三象限的半径
	}
	CPen p;
	p.CreatePen(PS_DASH, 1, RGB(255,0,0));
	pOldPen=pDC->SelectObject(&p);
	pDC->Ellipse(-(int)((cycle_num-3)*deltaR),-(int)((cycle_num-3)*deltaR),
		(int)((cycle_num-3)*deltaR),(int)((cycle_num-3)*deltaR));//-3dB的圆
	CString str;
	str.Format("%d°",0);
	pDC->TextOutA(0,-R-20,str);
	str.Format("%d°",30);
	x=(int)(R*sin(PI/6));
	y=-(int)(R*cos(PI/6));
	pDC->TextOutA(x+10,y-20,str);
	str.Format("%d°",60);
	x=(int)(R*sin(PI/3));
	y=-(int)(R*cos(PI/3));
	pDC->TextOutA(x+10,y-20,str);
	str.Format("%d°",90);
	pDC->TextOutA(R+10,0,str);
	str.Format("%d°",120);
	x=(int)(R*sin(2*PI/3));
	y=-(int)(R*cos(2*PI/3));
	pDC->TextOutA(x+10,y+20,str);
	str.Format("%d°",150);
	x=(int)(R*sin(5*PI/6));
	y=-(int)(R*cos(5*PI/6));
	pDC->TextOutA(x+10,y+20,str);
	str.Format("%d°",-30);
	x=(int)(R*sin(-PI/6));
	y=-(int)(R*cos(-PI/6));
	pDC->TextOutA(x-30,y-20,str);
	str.Format("%d°",-60);
	x=(int)(R*sin(-PI/3));
	y=-(int)(R*cos(-PI/3));
	pDC->TextOutA(x-30,y-20,str);
	str.Format("%d°",-90);
	pDC->TextOutA(-R-35,0,str);
	str.Format("%d°",-120);
	x=(int)(R*sin(-2*PI/3));
	y=-(int)(R*cos(-2*PI/3));
	pDC->TextOutA(x-30,y+20,str);
	str.Format("%d°",-150);
	x=(int)(R*sin(-5*PI/6));
	y=-(int)(R*cos(-5*PI/6));
	pDC->TextOutA(x-30,y+20,str);
	str.Format("-180°(180°)");
	pDC->TextOutA(-20,R+10,str);
	str.Format("当前角度为：%.1f°",angle);
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
	//	pDC->LineTo(x,y);//连接两个点
	//}

	//////debug
	CPen pPen[4];
	pPen[0].CreatePen(PS_SOLID,3,RGB(255,165,0));//橙色画笔
	pPen[1].CreatePen(PS_SOLID,3,RGB(0,255,0));//绿色画笔
	pPen[2].CreatePen(PS_SOLID,3,RGB(0,0,255));//蓝色画笔
	pPen[3].CreatePen(PS_SOLID,3,RGB(255,0,0));//红色画笔
	
	for(int ch=0;ch<4;ch++)
	{
		if(isChaChoose[ch])
		{
			pOldPen=pDC->SelectObject(&pPen[ch]);
			if(Result[ch].size()==0) continue;//注意要用continue，如果用break直接就跳出循环了，不会画其他通道的图形
			
			float max=Result[ch][0];
			for(unsigned int i=1;i<Result[ch].size();i++)
				if(max<Result[ch][i]) max=Result[ch][i];
			deltaA=PI*MeaAngle[0]/180;
			pDC->MoveTo((int)(Result[ch][0]/max*R*sin(deltaA)),-(int)(Result[ch][0]/max*R*cos(deltaA)));//将画笔移到圆心
			for(unsigned int i=1;i<Result[ch].size();i++)
			{
				deltaA=PI*MeaAngle[i]/180;
				x=(int)(Result[ch][i]/max*R*sin(deltaA));
				y=-(int)(Result[ch][i]/max*R*cos(deltaA));
				pDC->LineTo(x,y);//连接两个点
			}
		}
	}

	pDC->SelectObject(pOldPen);
}
void CMeasure::huatu_dB()
{
	int cycle_num=10,redius_num=36;//cycle_num是同心圆的个数，redius_num是直径的条数
	float mindB=-45.0f,perdb=-mindB/(cycle_num-1);
	CWnd *pWnd = GetDlgItem(IDC_picture);
	CRect rect;
	pWnd->GetClientRect(rect);
	CDC *pDC = pWnd->GetDC();
	CBrush rebrush;
	rebrush.CreateSolidBrush (RGB(255,255,255));//白色刷子
	CBrush *pOldBrush=pDC->SelectObject (&rebrush);
	pDC->Rectangle (rect);//清空picture中的绘画
	pDC->SelectObject (pOldBrush);
	CPen newPen;
	newPen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	CPen* pOldPen = (CPen*)pDC->SelectObject(&newPen);
	pDC->SelectStockObject(NULL_BRUSH);//没有画刷就避免圆被覆盖，画的空心圆
	int w=rect.Width(),h=rect.Height ();
	pDC->SetViewportOrg(w/2,h/2);//设置原点
	int R=(h-60)/2;//半径
	float deltaR=(float)R/cycle_num;
	for(int i=1;i<=cycle_num;i++)
	{
		pDC->Ellipse(-(int)(i*deltaR),-(int)(i*deltaR),(int)(i*deltaR),(int)(i*deltaR));//画圆，其实是找到一个外切正方形的左上角顶点和右下角顶点
	}
	pDC->Ellipse(-(int)(cycle_num*deltaR),-(int)(cycle_num*deltaR),(int)(cycle_num*deltaR),(int)(cycle_num*deltaR));//最外面的圆
	float deltaA=2*PI/redius_num;
	int x,y;
	for(int i=0;i<=redius_num/2;i++)
	{
		pDC->MoveTo((int)(deltaR*sin(i*deltaA)),(int)(deltaR*cos(i*deltaA)));
		x=(int)(R*sin(i*deltaA));
		y=(int)(R*cos(i*deltaA));
		pDC->LineTo(x,y);//画了一、四象限的半径
		pDC->MoveTo((int)(deltaR*sin(-i*deltaA)),(int)(deltaR*cos(-i*deltaA)));
		x=(int)(R*sin(-i*deltaA));
		y=(int)(R*cos(-i*deltaA));
		pDC->LineTo(x,y);//画了二三象限的半径
	}
	CPen p;
	p.CreatePen(PS_DASH, 1, RGB(255,0,0));
	pOldPen=pDC->SelectObject(&p);
	pDC->Ellipse(-(int)(((-3-mindB)/perdb+1)*deltaR),-(int)(((-3-mindB)/perdb+1)*deltaR),
		(int)(((-3-mindB)/perdb+1)*deltaR),(int)(((-3-mindB)/perdb+1)*deltaR));//-3dB的圆
	CString str;
	str.Format("%d°",0);
	pDC->TextOutA(0,-R-20,str);
	str.Format("%d°",30);
	x=(int)(R*sin(PI/6));
	y=-(int)(R*cos(PI/6));
	pDC->TextOutA(x+10,y-20,str);
	str.Format("%d°",60);
	x=(int)(R*sin(PI/3));
	y=-(int)(R*cos(PI/3));
	pDC->TextOutA(x+10,y-20,str);
	str.Format("%d°",90);
	pDC->TextOutA(R+10,0,str);
	str.Format("%d°",120);
	x=(int)(R*sin(2*PI/3));
	y=-(int)(R*cos(2*PI/3));
	pDC->TextOutA(x+10,y+20,str);
	str.Format("%d°",150);
	x=(int)(R*sin(5*PI/6));
	y=-(int)(R*cos(5*PI/6));
	pDC->TextOutA(x+10,y+20,str);
	str.Format("%d°",-30);
	x=(int)(R*sin(-PI/6));
	y=-(int)(R*cos(-PI/6));
	pDC->TextOutA(x-30,y-20,str);
	str.Format("%d°",-60);
	x=(int)(R*sin(-PI/3));
	y=-(int)(R*cos(-PI/3));
	pDC->TextOutA(x-30,y-20,str);
	str.Format("%d°",-90);
	pDC->TextOutA(-R-35,0,str);
	str.Format("%d°",-120);
	x=(int)(R*sin(-2*PI/3));
	y=-(int)(R*cos(-2*PI/3));
	pDC->TextOutA(x-30,y+20,str);
	str.Format("%d°",-150);
	x=(int)(R*sin(-5*PI/6));
	y=-(int)(R*cos(-5*PI/6));
	pDC->TextOutA(x-30,y+20,str);
	str.Format("-180°(180°)");
	pDC->TextOutA(-20,R+10,str);
	str.Format("当前角度为：%.1f°",angle);
	pDC->TextOutA(-w/2+20,-h/2+20,str);
	str.Format("5dB/div");
	pDC->TextOutA(-w/2+20,h/2-20,str);
	CPen pPen[4];
	pPen[0].CreatePen(PS_SOLID,3,RGB(255,165,0));//橙色画笔
	pPen[1].CreatePen(PS_SOLID,3,RGB(0,255,0));//绿色画笔
	pPen[2].CreatePen(PS_SOLID,3,RGB(0,0,255));//蓝色画笔
	pPen[3].CreatePen(PS_SOLID,3,RGB(255,0,0));//红色画笔
	
	for(int ch=0;ch<4;ch++)
	{
		if(isChaChoose[ch])
		{
			pOldPen=pDC->SelectObject(&pPen[ch]);
			if(Result[ch].size()==0) continue;//注意要用continue，如果用break直接就跳出循环了，不会画其他通道的图形
			
			float max=Result[ch][0];
			for(unsigned int i=1;i<Result[ch].size();i++)
				if(max<Result[ch][i]) max=Result[ch][i];
			deltaA=PI*MeaAngle[0]/180;
			float db=20*log10(Result[ch][0]/max);
			if(db<=mindB)//分贝数最小为-45dB（0.0056），更小的就不计算了
				pDC->MoveTo((int)(deltaR*sin(deltaA)),-(int)(deltaR*cos(deltaA)));//将画笔移到圆心
			else
				pDC->MoveTo((int)(((db-mindB)/perdb+1)*deltaR*sin(deltaA)),-(int)(((db-mindB)/perdb+1)*deltaR*cos(deltaA)));//将画笔移到圆心
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
				pDC->LineTo(x,y);//连接两个点
			}
		}
	}

	pDC->SelectObject(pOldPen);

}
int CMeasure::MeaMulDir()//只能用一个通道来测量，但是具体是哪个通道可以选择
{
	if(m_com.get_PortOpen())
	{
		AfxMessageBox("串口正在使用，请先关闭控制回转系统！");
		m_com.put_PortOpen(false);
		return -1;
	}
	if(!m_com.get_PortOpen())
	{
		m_com.put_PortOpen(true);         //打开串口
	}
	else
	{
		m_com.put_OutBufferCount(0);
		MessageBox("串口2打开失败");
		m_com.put_PortOpen(false);
		return -1;
	}
	MeaSetManual();//设为手动
	f=startf;
	isMeasure=true;
	CreateMulFrePulse(Fs,f,deltaf,Bwid/1000,v/1000,Brep);//多频点信号
	MessageBox("请根据提示选择各个通道各个频率的测量区域！");
	viPrintf(vip,":timebase:mode window\n");
	for(int i=0;i<4;i++)
	{
		if(isChaChoose[i])//只有一个是true
		{
			CString s;
			s.Format("完成选择通道%d的第1个频率的测量区域后点击确定",i+1);
			MessageBox(s);
			viQueryf(vip,":timebase:window:position?\n","%f\n",&zoomPosition[0]);
			viQueryf(vip,":timebase:window:range?\n","%f\n",&zoomRange[0]);
		
		}
	}
	viPrintf(vip,":timebase:mode main\n");
	float angle=MeaReadCurrentAngle();//读出当前的角度值
	bool isRightDir=true;
	SetDlgItemTextA(IDC_showPara,"正在转到指定角度，请稍候......");
	if(abs(angle-StartAngle)<=abs(angle-EndAngle))
	{
		MeaRotateTargetAngle(Speed,StartAngle);//如果当前位置与起始角度靠近，就转到起始角度
		isRightDir=true;
	}
	else 
	{
		MeaRotateTargetAngle(Speed,EndAngle);
		isRightDir=false;
	}
	angle=MeaReadCurrentAngle();
	while((isRightDir&&abs(angle-StartAngle)>0.1)||(!isRightDir&&abs(angle-EndAngle)>0.1))//之前调试的时候用写的1，但是相差1度有点大，所以改为0.1试试
	{
		angle=MeaReadCurrentAngle();
	}//等待转到指定角度完成
	CString stemp;
	stemp.Format("起始频率为 %.1fHz\r\n频率点数 %d\r\n测量的角度范围 %d°~%d°\r\n回转速度为 %d秒/圈\r\n重复周期 %.1fs",f,PulseCount,StartAngle,EndAngle,Speed,Brep);
	if(MessageBox(stemp,"是否测量？",MB_OKCANCEL)==IDCANCEL) 
	{
		m_com.put_OutBufferCount(0);
		m_com.put_PortOpen(false);
		return -1;
	}
	SetDlgItemTextA(IDC_showPara,stemp);
	viPrintf(vip,":run\n");
	SetDlgItemTextA(IDC_Show,"正在进行多频点指向性测量，请稍候......");
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
				//将zoom框移动一个间距
				viPrintf(vip,":timebase:window:range %f\n",zoomRange[0]);
				viPrintf(vip,":measure:source channel%d\n",i+1);
				u[j]=autoV(i+1);
				Result[j].push_back(u[j]);
				MeaAngle.push_back(angle);//第一个点所有的角度一样
			}
			viPrintf(vip,":run\n");
		}
	}//读取第一个角度的电压值
	if(isRightDir) MeaRotateRight(Speed);//顺时针转动起来
	else MeaRotateLeft(Speed);//逆时针转动
	angle=MeaReadCurrentAngle();	
	while(isMeasure&&((isRightDir&&angle<EndAngle)||(!isRightDir&&angle>StartAngle)))
	{
		for(int i=0;i<4;i++)
		{
			if(isChaChoose[i])
			{
				viPrintf(vip,":digitize channel%d\n",i+1);
				angle=MeaReadCurrentAngle();//读角度的时间
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
		//绘制极坐标图
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
	}//最后一个角度的电压值
	huatu_muldir();
	viPrintf(vip,":timebase:mode main\n");
	clock_t t_end=clock();
	stemp.Format("测量时间为 %.4f s",(double)(t_end-t_start)/CLOCKS_PER_SEC);
	MessageBox(stemp);
	if(m_com.get_PortOpen())
		m_com.put_PortOpen(false);
	return 0;
}
void CMeasure::huatu_muldir()
{
	int cycle_num=10,redius_num=36;//cycle_num是同心圆的个数，redius_num是直径的条数
	float mindB=-45.0f,perdb=-mindB/(cycle_num-1);
	CWnd *pWnd = GetDlgItem(IDC_picture);
	CRect rect;
	pWnd->GetClientRect(rect);
	CDC *pDC = pWnd->GetDC();
	CBrush rebrush;
	rebrush.CreateSolidBrush (RGB(255,255,255));//白色刷子
	CBrush *pOldBrush=pDC->SelectObject (&rebrush);
	pDC->Rectangle (rect);//清空picture中的绘画
	pDC->SelectObject (pOldBrush);
	CPen newPen;
	newPen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	CPen* pOldPen = (CPen*)pDC->SelectObject(&newPen);
	pDC->SelectStockObject(NULL_BRUSH);//没有画刷就避免圆被覆盖，画的空心圆
	int w=rect.Width(),h=rect.Height ();
	pDC->SetViewportOrg(w/2,h/2);//设置原点
	int R=(h-60)/2;//半径
	float deltaR=(float)R/cycle_num;
	for(int i=1;i<=cycle_num;i++)
	{
		pDC->Ellipse(-(int)(i*deltaR),-(int)(i*deltaR),(int)(i*deltaR),(int)(i*deltaR));//画圆，其实是找到一个外切正方形的左上角顶点和右下角顶点
	}
	pDC->Ellipse(-(int)(cycle_num*deltaR),-(int)(cycle_num*deltaR),(int)(cycle_num*deltaR),(int)(cycle_num*deltaR));//最外面的圆
	float dAngle=2*PI/redius_num;
	int x,y;
	for(int i=0;i<=redius_num/2;i++)
	{
		pDC->MoveTo((int)(deltaR*sin(i*dAngle)),(int)(deltaR*cos(i*dAngle)));
		x=(int)(R*sin(i*dAngle));
		y=(int)(R*cos(i*dAngle));
		pDC->LineTo(x,y);//画了一、四象限的半径
		pDC->MoveTo((int)(deltaR*sin(-i*dAngle)),(int)(deltaR*cos(-i*dAngle)));
		x=(int)(R*sin(-i*dAngle));
		y=(int)(R*cos(-i*dAngle));
		pDC->LineTo(x,y);//画了二三象限的半径
	}
	CPen p;
	p.CreatePen(PS_DASH, 1, RGB(255,0,0));
	pOldPen=pDC->SelectObject(&p);
	pDC->Ellipse(-(int)(((-3-mindB)/perdb+1)*deltaR),-(int)(((-3-mindB)/perdb+1)*deltaR),
		(int)(((-3-mindB)/perdb+1)*deltaR),(int)(((-3-mindB)/perdb+1)*deltaR));//-3dB的圆
	CString str;
	str.Format("%d°",0);
	pDC->TextOutA(0,-R-20,str);
	str.Format("%d°",30);
	x=(int)(R*sin(PI/6));
	y=-(int)(R*cos(PI/6));
	pDC->TextOutA(x+10,y-20,str);
	str.Format("%d°",60);
	x=(int)(R*sin(PI/3));
	y=-(int)(R*cos(PI/3));
	pDC->TextOutA(x+10,y-20,str);
	str.Format("%d°",90);
	pDC->TextOutA(R+10,0,str);
	str.Format("%d°",120);
	x=(int)(R*sin(2*PI/3));
	y=-(int)(R*cos(2*PI/3));
	pDC->TextOutA(x+10,y+20,str);
	str.Format("%d°",150);
	x=(int)(R*sin(5*PI/6));
	y=-(int)(R*cos(5*PI/6));
	pDC->TextOutA(x+10,y+20,str);
	str.Format("%d°",-30);
	x=(int)(R*sin(-PI/6));
	y=-(int)(R*cos(-PI/6));
	pDC->TextOutA(x-30,y-20,str);
	str.Format("%d°",-60);
	x=(int)(R*sin(-PI/3));
	y=-(int)(R*cos(-PI/3));
	pDC->TextOutA(x-30,y-20,str);
	str.Format("%d°",-90);
	pDC->TextOutA(-R-35,0,str);
	str.Format("%d°",-120);
	x=(int)(R*sin(-2*PI/3));
	y=-(int)(R*cos(-2*PI/3));
	pDC->TextOutA(x-30,y+20,str);
	str.Format("%d°",-150);
	x=(int)(R*sin(-5*PI/6));
	y=-(int)(R*cos(-5*PI/6));
	pDC->TextOutA(x-30,y+20,str);
	str.Format("-180°(180°)");
	pDC->TextOutA(-20,R+10,str);
	str.Format("当前角度为：%.1f°",angle);
	pDC->TextOutA(-w/2+20,-h/2+20,str);
	str.Format("5dB/div");
	pDC->TextOutA(-w/2+20,h/2-20,str);

	str.Format("橙色——%.1fHz指向性图",f);
	pDC->TextOutA(-w/2+10,h/2-100,str);
	str.Format("绿色——%.1fHz指向性图",f+deltaf);
	pDC->TextOutA(-w/2+10,h/2-80,str);
	str.Format("蓝色——%.1fHz指向性图",f+2*deltaf);
	pDC->TextOutA(-w/2+10,h/2-60,str);
	str.Format("红色——%.1fHz指向性图",f+3*deltaf);
	pDC->TextOutA(-w/2+10,h/2-40,str);

	CPen pPen[4];
	pPen[0].CreatePen(PS_SOLID,3,RGB(255,165,0));//橙色画笔
	pPen[1].CreatePen(PS_SOLID,3,RGB(0,255,0));//绿色画笔
	pPen[2].CreatePen(PS_SOLID,3,RGB(0,0,255));//蓝色画笔
	pPen[3].CreatePen(PS_SOLID,3,RGB(255,0,0));//红色画笔
	for(int ch=0;ch<4;ch++)
	{
		if(isChaChoose[ch])//只有一个通道
		{
			for(int j=0;j<PulseCount;j++)
			{
				pOldPen=pDC->SelectObject(&pPen[j]);
				if(Result[j].size()==0) continue;
				//注意要用continue，如果用break直接就跳出循环了，不会画其他通道的图形
			
				float max=Result[j][0];
				for(unsigned int k=1;k<Result[j].size();k++)
					if(max<Result[j][k]) max=Result[j][k];
				dAngle=PI*MeaAngle[0]/180;
				float db=20*log10(Result[ch][0]/max);
			if(db<=mindB)//分贝数最小为-45dB（0.0056），更小的就不计算了
				pDC->MoveTo((int)(deltaR*sin(dAngle)),-(int)(deltaR*cos(dAngle)));//将画笔移到圆心
			else
				pDC->MoveTo((int)(((db-mindB)/perdb+1)*deltaR*sin(dAngle)),-(int)(((db-mindB)/perdb+1)*deltaR*cos(dAngle)));//将画笔移到圆心
				
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
					pDC->LineTo(x,y);//连接两个点
				}
			}
		}
	}

	pDC->SelectObject(pOldPen);
}

int CMeasure::MeaHuyi()
{
	SetDlgItemTextA(IDC_Show,"准备测量，请稍候......");
	float UI[3]={0,0,0};
	if(status!=0)
	{
		AfxMessageBox("开关矩阵连通不正确！");
		viClose(U2751);
		viClose(rm);
		return -1;
	}
	viClear(U2751);
	viPrintf(U2751,"*RST\n");//开关矩阵复位
	viPrintf(U2751,"*CLS\n");//开关矩阵
	viPrintf(U2751,"ROUTe:CLOSe (@402,107,304)\n");//发射发，互易和水听器收
	float Mp=-1;
	CreateBurst(f,v/1000,Bwid/1000,Brep);
	MessageBox("请根据提示选择各个通道的测量区域！");
	//功放连接R4，发射连接C2，互易连接C4（示波器CH2连接R3），
	//水听器连接C7（示波器CH1连接R1），示波器CH3接的是电流计
	viPrintf(vip,":timebase:mode window\n");
	
	CString s;
	//发射发，水听器通道
	s.Format("完成选择通道1(发-听)的测量区域后点击确定");
	MessageBox(s);
	viQueryf(vip,":timebase:window:position?\n","%f\n",&zoomPosition[0]);
	viQueryf(vip,":timebase:window:range?\n","%f\n",&zoomRange[0]);
	//发射发，互易换能器通道
	s.Format("完成选择通道2(发-互)的测量区域后点击确定");
	MessageBox(s);
	viQueryf(vip,":timebase:window:position?\n","%f\n",&zoomPosition[1]);
	viQueryf(vip,":timebase:window:range?\n","%f\n",&zoomRange[1]);
	//发射发，电流通道，假设互易发也用这个区间
	s.Format("完成选择通道3(电流计)的测量区域后点击确定");
	MessageBox(s);
	viQueryf(vip,":timebase:window:position?\n","%f\n",&zoomPosition[2]);
	viQueryf(vip,":timebase:window:range?\n","%f\n",&zoomRange[2]);
	//互易发，水听器通道
	viPrintf(U2751,"ROUTe:OPEN (@402,304)\n");//关掉发射和互易收
	viPrintf(U2751,"ROUTe:CLOSe (@404)\n");//互易发
	s.Format("完成选择通道1(互-听)的测量区域后点击确定");
	MessageBox(s);
	viQueryf(vip,":timebase:window:position?\n","%f\n",&zoomPosition[3]);
	viQueryf(vip,":timebase:window:range?\n","%f\n",&zoomRange[3]);

	viPrintf(vip,":timebase:mode main\n");
	viPrintf(vip,":run\n");
	CString stemp;
	stemp.Format("\r\n\r\n互易法自动测量灵敏度：\r\n频率范围 %.1fkHz~%.1fkHz\r\n信号源幅度 %.1fmVpp\r\n",
	startf/1000,endf/1000,v,Bwid,Brep,MeaCount,chaRefer);
	SetDlgItemTextA(IDC_showPara,stemp);
	if(MessageBox(stemp,"提示",MB_OKCANCEL)==IDCANCEL) return -1;	
	f=startf;
	isMeasure=true;
	
	SetDlgItemTextA(IDC_Show,"正在进行互易法自动测量灵敏度，请稍候......");
	int count_OTFreq=0;
	while(isMeasure&&f<=endf)
	{
		//用来接收“停止测量”按钮按下的消息
		MSG  msg;
		if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE))          
		{          
			if (msg.message == MAKEWPARAM(IDC_StopMea, BN_CLICKED))          
					break ;          
			TranslateMessage (&msg) ;          
			DispatchMessage (&msg) ;          
		} 
		viPrintf(vip,":run\n");
		viPrintf(U2751,"*RST\n");//开关矩阵复位
		viPrintf(U2751,"*CLS\n");//开关矩阵
		viPrintf(U2751,"ROUTe:CLOSe (@402,107,304)\n");//发射发，水听器收，互易收
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
		viPrintf(U2751,"ROUTe:CLOSe (@404)\n");//互易发，水听器收
		viPrintf(vip,":timebase:window:position %f\n",zoomPosition[3]);
		viPrintf(vip,":timebase:window:range %f\n",zoomRange[3]);
		viPrintf(vip,":measure:source channel%d\n",1);
		u[2]=autoV(1)/Gain[0];//uHJ
		viPrintf(vip,":timebase:window:position %f\n",zoomPosition[2]);
		viPrintf(vip,":timebase:window:range %f\n",zoomRange[2]);
		viPrintf(vip,":measure:source channel%d\n",3);
		UI[2]=autoV(3)/Cv;//iHJ
	
		float M_j=20*log10(sqrt((u[0]/UI[0])*(u[2]/UI[2])/(u[1]/UI[1])*(d[0]*d[2]/d[1])*(2/(1000*f))*exp(0.0005f*(d[0]+d[2]-d[1]))))-120;
		//公式中的ruo=1000kg/m3,alpha=0.0005
		Result[0].push_back(M_j);//水听器接在示波器1通道上
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
	rebrush.CreateSolidBrush (RGB(255,255,255));//白色刷子
	CBrush *pOldBrush=pDC->SelectObject (&rebrush);
	pDC->Rectangle (rect);//清空picture中的绘画
	pDC->SelectObject (pOldBrush);
	CPen pNewPen;
	pNewPen.CreatePen(PS_SOLID,1,RGB(0,0,0));
	CPen* pOldPen=pDC->SelectObject(&pNewPen);
	int deltaX=rect.Width()/50;
	int deltaY=rect.Height()/100;//100是随机取的，代表纵轴分为100份
	pDC->SetViewportOrg(rect.left,rect.top);//测量灵敏度时都是负值,原点设为最左上角的点
	//画网格线
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
		str.Format("%d",-150-y);//纵轴表示-150~-250共100个点，而纵轴总共划分出了100格，所以每一格代表y
		pDC->TextOutA(rect.left-40,y*deltaY-10,str);
	}
	str.Format("频率(kHz)");
	pDC->TextOutA(100,-50,str);
	str.Format("灵");
	pDC->TextOutA(-80,200,str);
	str.Format("敏");
	pDC->TextOutA(-80,220,str);
	str.Format("度");
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
				pDC->MoveTo(0,-(int)((Result[0][0]+150)*deltaY));//将画笔移到起点，灵敏度值是负的，画图向下是正的
				if(OneThird_f)
				{
					for(unsigned int i=1;i<Result[0].size();i++)
					{

						x=(int)((OneThirdFreq[OTFreq+i]-startf)*50/(endf-startf)*deltaX);
						y=-(int)((Result[0][i]+150)*deltaY);
						pDC->LineTo(x,y);//连接两个点
					}
				}
				else
				{
					for(unsigned int i=1;i<Result[0].size();i++)
					{
						x=(int)(i*deltaf*deltaX*50/(endf-startf));
						y=-(int)((Result[0][i]+150)*deltaY);
						pDC->LineTo(x,y);//连接两个点
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
	m_com.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_com.get_Input();
	CString str;
	str=retVal.bstrVal;//因为串口返回的是字符串类型的，retVal的vt是VT_BSTR
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("设置手动模式操作出错!");
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
	//angle为接收回来的第8位开始的4位
	VARIANT retVal=m_com.get_Input();
	CString str;
	str=retVal.bstrVal;//因为串口返回的是字符串类型的，retVal的vt是VT_BSTR
	int temp;
	CString anglestring;
	anglestring.Append(str.Mid(7,1));
	anglestring.Append(str.Mid(7,1));
	anglestring.Append(str.Mid(7,1));
	anglestring.Append(str.Mid(7,1));
	anglestring+=str.Mid(7,4);
	sscanf_s(anglestring,"%X",&temp);
	//是字符串，要化为数值，然后除以10才是角度
	return temp/10.0f;
}
void CMeasure::MeaRotateRight(int speed)
{
	//先设置速度
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
	m_com.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_com.get_Input();
	CString str;
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("设置速度操作出错!");
	}
	//顺时针转动
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
		AfxMessageBox("顺时针转动操作出错!");
	}
}
void CMeasure::MeaRotateLeft(int speed)
{
	//先设置速度
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
	m_com.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_com.get_Input();
	CString str;
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("设置速度操作出错!");
	}
	//逆时针转
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
		AfxMessageBox("逆时针转动操作出错!");
	}
}
void CMeasure::MeaRotateTargetAngle(int speed,int targetangle)
{
	BYTE commanddata;
	CByteArray databuf;
	char crc[3];
	//设为自动
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
		AfxMessageBox("设为自动操作出错!");
	}
	//停止转动
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
		AfxMessageBox("停止转动操作出错!");
	}
	//先设置速度
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
	m_com.put_Output(COleVariant(databuf));
	Sleep(200);
	retVal=m_com.get_Input();
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("设置速度操作出错!");
	}
	//设置目标角度
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
	m_com.put_Output(COleVariant(databuf));
	Sleep(200);
	retVal=m_com.get_Input();
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("设置目标角度操作出错!");
	}
	//定位启动
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
		AfxMessageBox("定位启动操作出错!");
	}

}
void CMeasure::MeaStopRotate()
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
	m_com.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_com.get_Input();
	CString str;
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("停止转动操作出错!");
	}
}
void CMeasure::MeaStopRotateRight()
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
	m_com.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_com.get_Input();
	CString str;
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("顺时针停操作出错!");
	}
}
void CMeasure::MeaStopRotateLeft()
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
	m_com.put_Output(COleVariant(databuf));
	Sleep(200);
	VARIANT retVal=m_com.get_Input();
	CString str;
	str=retVal.bstrVal;
	if(str.Mid(5,2)!="00")
	{
		AfxMessageBox("逆时针停操作出错!");
	}
}

