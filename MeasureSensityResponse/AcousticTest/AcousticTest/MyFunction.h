#ifndef MYFUNCTION
#define MYFUNCTION
#include "stdafx.h"
#include "visa.h"
#include <math.h>
#include <map>
#include<vector>
using namespace std;
const float PI=3.1415926f;
const int Fs=100000;
extern ViSession vidg,vidp,vig,vip;
extern long status;
extern int ChooseItem;
extern float f,v,Bwid,Brep;
extern float startf,endf,deltaf;
extern bool isChaChoose[4];
extern int chaRefer;
extern float d[4];
extern float u[4];
extern map<float,float> standMp;
extern CString strDirFile;
extern int Ratio,Gain[4],MeaCount;
extern float Cv;
extern int Speed,StartAngle,EndAngle;
extern bool isChange;
extern int PulseCount;
extern float OneThirdFreq[34];
extern bool OneThird_f;
extern unsigned int OTFreq;
extern int Unit;

extern void CreateBurst(float f,float v,float wid,float perio);
extern float CalSensity(float mp,float ux,float up,float dx,float dp);
extern float CalResponse(float mp,float ux,float up,float d);
extern void ScopeTrigger();
extern void CreateMulFrePulse(int fs,float f1,float deltaf,float wid,float v,float repeat);
extern float LineP(float x0,float y0,float x1,float y1,float x);
#endif

