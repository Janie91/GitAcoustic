#include "stdafx.h"
#include "MyFunction.h"


ViSession vidg,vidp,vig,vip;
long status=-1;
int ChooseItem=0;
float f=1.0,v=500.0,Bwid=5.0,Brep=1.0;
float startf=1.0,endf=1.0,deltaf=1.0;
bool isChaChoose[4]={false,false,false,false};
int chaRefer=1;
float d[4]={1.0,1.0,1.0,1.0};
float u[4]={-1.0,-1.0,-1.0,-1.0};
map<float,float> standMp;
CString strDirFile="";
int Ratio=100,Gain=1;
int StartAngle=-180,EndAngle=180;

void CreateBurst(float f,float v,float wid,float perio)
{
	float ncyd=f*wid;
	int ncyi=(int) ncyd;
	if(ncyd-ncyi>=0.5) ncyi+=1;
	//viPrintf(vig,"function sin\n");
	viPrintf(vig,"frequency %lf\n",f);
	viPrintf(vig,"voltage %lf\n",v);
	viPrintf(vig,"burst:mode triggered\n");
	viPrintf(vig,"burst:ncycles %d\n",ncyi);
	viPrintf(vig,"burst:internal:period %lf\n",perio);
	viPrintf(vig,"burst:phase 0\n");
	viPrintf(vig,"trigger:source immediate\n");
	viPrintf(vig,"burst:state on\n");
	viPrintf(vig,"output:sync on\n");
	viPrintf(vig,"output on\n");
}
void ScopeTrigger()
{
	viPrintf(vip,":autoscale\n");
	viPrintf(vip,":timebase:reference left\n");
	viPrintf(vip,":trigger:edge:source external\n");
	viPrintf(vip,":trigger:mode edge\n");//���ش���
	viPrintf(vip,":trigger:sweep normal\n");//����ģʽΪ��׼
	viPrintf(vip,":trigger:edge:coupling dc\n");//��Ϸ�ʽΪDC
	viPrintf(vip,":trigger:edge:level 1\n");//������ƽΪ1V
	viPrintf(vip,":trigger:edge:slope positive\n");//������
	viPrintf(vip, ":acquire:type hresolution\n");
	viPrintf(vip,":acquire:complete 100\n");
	
}
float CalSensity(float mp,float ux,float up,float dx,float dp)
{
	float sensity;
	sensity=mp+20*log10(ux)-20*log10(up)+20*log10(dx)-20*log10(dp);
	return sensity;
}
float CalResponse(float mp,float ux,float up,float d)
{
	float res;
	res=20*log10(up)-20*log10(ux)+20*log10(d)-mp;
	return res;
}
void CreateMulFrePulse(float f1,float v,float delf)
{
	char SCPIcmd[100000];
	float fs=100000;//������
	float t0=0.005f;//������
	float deltaT=0.03f;//��Ƶ��������
	int points=deltaT*fs;//ÿ��Ƶ�ʶεĵ���
	int point=t0*fs;//����������ĵ���
	viPrintf(vig,"*rst\n");
	viPrintf(vig,"*cls\n");
	strcpy_s(SCPIcmd,"data volatile");
	for(int i=0;i<4;i++)
	{
		for(int j=0;j<point;j++)
			sprintf_s(SCPIcmd,"%s,%4.2f",SCPIcmd,sin(2*PI*(f1*1000+i*delf*1000)*j/fs));//ʹ��ASCII��ķ�ʽ��������
		for(int k=0;k<points-point;k++)
			strcat_s(SCPIcmd,",0");
	}
	strcat_s(SCPIcmd,"\n");
	viPrintf(vig,SCPIcmd);
	viPrintf(vig,"data:copy MulFrePulse, volatile\n");
	viPrintf(vig,"function:user MulFrePulse\n");
	viPrintf(vig,"function:shape user\n");
	viPrintf(vig,"output:load 50\n");
	viPrintf(vig,"frequency 100;voltage %f\n",v);
	viPrintf(vig,"output:sync on\n");
	viPrintf(vig,"output on\n");
	Sleep(1000);
}

