// Measure.cpp : implementation file
//

#include "stdafx.h"
#include "AcousticTest.h"
#include "Measure.h"
#include "afxdialogex.h"

//...My code...
#include "SetSigDlg.h"
#include "SetScopDlg.h"
#include "SetConditionDlg.h"
#include "SelectDlg.h"
#include "MyFunction.h"
#include <vector>
using namespace std;
//excel需要
#include "CApplication.h"
#include "CRange.h"
#include "CWorkbook.h"
#include "CWorkbooks.h"
#include "CWorksheet.h"
#include "CWorksheets.h"

vector<vector<float>>Result(4,vector<float>(0));
float zoomPosition[4]={-1,-1,-1,-1},zoomRange[4]={-1,-1,-1,-1};
//...end...

// CMeasure dialog

IMPLEMENT_DYNAMIC(CMeasure, CDialogEx)

CMeasure::CMeasure(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMeasure::IDD, pParent)
{

}

CMeasure::~CMeasure()
{
}

void CMeasure::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
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
END_MESSAGE_MAP()


// CMeasure message handlers

//...My code...
void renew();
void huatu_sensity();
void Capture(vector<int> cha,int count);
void MeasureSensity();
void MeasureResponse();
void huatu_response();
BOOL CMeasure::OnInitDialog()//加载对话框时的初始化函数
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	GetParent()->ShowWindow(SW_HIDE);//把测量的前一个选择测量项目对话框关闭
	//GetDlgItem(IDC_StartMea)->EnableWindow(false);
	CRect rect;  
	GetDlgItem(IDC_picture)->GetClientRect(&rect);
	GetDlgItem(IDC_picture)->MoveWindow(50,50,800,600,true); 
	//固定Picture Control控件的位置和大小 

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
	this->SendMessage(WM_CLOSE);
	GetParent()->ShowWindow(SW_SHOW);//显示选择测量选项的对话框	
}

void CMeasure::OnBnClickedView()
{
	// TODO: Add your control notification handler code here
	int delay=(int)(Brep*1000);//延迟delay，单位ms
	renew();
	//clock_t t_start=clock();
	//自动调整，合适的显示波形
	int chCount=0;
	float vRange=-1,vTemp=-1;
	//vector<int> chview;
	for(int i=0;i<4;i++)
	{
		if(isChaChoose[i]) {
			chCount++;
			/*chview.push_back(i+1);*/
			viPrintf(vip,":channel%d:display on\n",i+1);//打开通道
			viPrintf(vip,":channel%d:offset 0\n",i+1);			
		}
		else viPrintf(vip,":channel%d:display off\n",i+1);
	}
	if(chCount==0)//如果没有选择使用的通道，提示并返回
	{
		AfxMessageBox("请在示波器参数设置中选择测量通道！");
		viPrintf(vip,"*rst\n");
		return;
	}
	viPrintf(vip,"timebase:range %f\n",(Bwid/1000.0)*2);//设置时间轴代表的时间长度
	CreateBurst(f*1000,v/1000,Bwid/1000,Brep);
	Sleep(100);
	ScopeTrigger();
	//clock_t t_end=clock();

	//Sleep((Brep-(t_end-t_start)/CLOCKS_PER_SEC)*1000);
	
	//Capture(chview,chCount);
	//Sleep(delay+10);//要等待数据采集完成
	int chflag=0,flag=0;
	for(int i=0;i<4;i++)
	{
		if(isChaChoose[i])
		{
			chflag++;//标记第几个通道，用于调整在示波器屏幕上显示的位置
			viPrintf(vip,":measure:source channel%d\n",i+1);
			viQueryf(vip,":channel%d:range?\n","%f\n",i+1,&vRange);//获得纵向的电压范围
			viQueryf(vip,":measure:vpp?\n","%f\n",&vTemp);//测量峰峰值
			
			while(vTemp<-1e8||vRange<-1e8||vTemp==-1||vRange==-1)
			{
				if(flag>10) break;
				flag++;
				viQueryf(vip,":channel%d:range?\n","%f\n",i+1,&vRange);
				viQueryf(vip,":measure:vpp?\n","%f\n",&vTemp);
			}
			flag=0;
			while(vTemp>9e+37)//波形显示超出了屏幕外
			{
				if(flag>10||2*vRange>40) 
				{
					viPrintf(vip,":channel%d:range 40\n",i+1);
					viQueryf(vip,":channel%d:range?\n","%f\n",i+1,&vRange);
					viQueryf(vip,":measure:vpp?\n","%f\n",&vTemp);
					break;
				}
				flag++;
				viPrintf(vip,":channel%d:range %f\n",i+1,2*vRange);//重新扩大电压范围
				viQueryf(vip,":channel%d:range?\n","%f\n",i+1,&vRange);
				viQueryf(vip,":measure:vpp?\n","%f\n",&vTemp);
			}
			flag=0;
			
			//波形显示大于四格
			while(vTemp>vRange/8.0*4) 
			{
				if(2*vRange>40) 
				{
					viPrintf(vip,":channel%d:range 40\n",i+1);
					viQueryf(vip,":channel%d:range?\n","%f\n",i+1,&vRange);
					viQueryf(vip,":measure:vpp?\n","%f\n",&vTemp);
					break;
				}
				viPrintf(vip,":channel%d:range %f\n",i+1,2*vRange);
				viQueryf(vip,":channel%d:range?\n","%f\n",i+1,&vRange);
				viQueryf(vip,":measure:vpp?\n","%f\n",&vTemp);
			}
			//波形显示小于两格
			while(vTemp<vRange/8.0*2)
			{
				if(vRange/2<0.016) 
				{
					viPrintf(vip,":channel%d:range 0.016\n",i+1);
					viQueryf(vip,":channel%d:range?\n","%f\n",i+1,&vRange);
					viQueryf(vip,":measure:vpp?\n","%f\n",&vTemp);
					break;
				}
				viPrintf(vip,":channel%d:range %f\n",i+1,vRange/2);
				viQueryf(vip,":channel%d:range?\n","%f\n",i+1,&vRange);
				viQueryf(vip,":measure:vpp?\n","%f\n",&vTemp);
			}
			//viQueryf(vip,":channel%d:range?\n","%f\n",i+1,&vRange);
			switch(chCount)
			{
			case 1:
				viPrintf(vip,":channel%d:offset 0\n",i+1);//只有一个通道就在中间显示
				break;
			case 2:
				if(chflag%2==1) viPrintf(vip,":channel%d:offset %f\n",i+1,vRange/2-vRange/4);
				else viPrintf(vip,":channel%d:offset %f\n",i+1,vRange/2-3*vRange/4);
				break;
			case 3:
				if(chflag%3==1) viPrintf(vip,":channel%d:offset %f\n",i+1,vRange/2-vRange/6);
				else if(chflag%3==2) viPrintf(vip,":channel%d:offset %f\n",i+1,vRange/2-3*vRange/6);
				else viPrintf(vip,":channel%d:offset %f\n",i+1,vRange/2-5*vRange/6);
				break;
			case 4:
				if(chflag%4==1) viPrintf(vip,":channel%d:offset %f\n",i+1,vRange/2-vRange/8);
				else if(chflag%4==2) viPrintf(vip,":channel%d:offset %f\n",i+1,vRange/2-3*vRange/8);
				else if(chflag%4==3)viPrintf(vip,":channel%d:offset %f\n",i+1,vRange/2-5*vRange/8);
				else viPrintf(vip,":channel%d:offset %f\n",i+1,vRange/2-7*vRange/8);
				break;
			}
		}
	}
	/*viPrintf(vip,":run\n");*/
}

void CMeasure::OnBnClickedStartmea()
{
	// TODO: Add your control notification handler code here
	renew();
	//vector<int> ch;
	
	int delay=(int)(Brep*1000);
	if(standMp.empty()) 
	{
		AfxMessageBox("请选择标准水听器文件！");
		return;
	}
	int chcount=0;
	for(int i=0;i<4;i++)
	{
		if(isChaChoose[i])
		{
			chcount++;
			//ch.push_back(i+1);
		}
	}
	if(0==chcount)
	{
		AfxMessageBox("请选择示波器的测量通道！");
		return;
	}
	switch(ChooseItem)
	{
	case 0: //测量灵敏度
		MeasureSensity();
		SetTimer(1,1000,NULL);
		break;
	case 1://测量发射电压响应
		MeasureResponse();
		SetTimer(2,1000,NULL);
		break;
	}
	
	
}

void CMeasure::OnBnClickedStopmea()
{
	// TODO: Add your control notification handler code here
	KillTimer(1);
	huatu_sensity();
}

void CMeasure::OnBnClickedquitsys()
{
	// TODO: Add your control notification handler code here
	KillTimer(1);
	KillTimer(2);
	viClose(vip);
	viClose(vig);
	viClose(vidp);
	viClose(vidg);
	CDialog::OnCancel();
	GetParent()->SendMessage(WM_CLOSE);
	AfxGetMainWnd()->SendMessage(WM_CLOSE);//退出系统要把主对话框关闭
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
			_variant_t("频率（kHz)"));
		range.put_Item(_variant_t((long)1),_variant_t((long)2),
				_variant_t("灵敏度级(dB)"));
		break;
	case 1:
		range.put_Item(_variant_t((long)1),_variant_t((long)1),
			_variant_t("频率（kHz)"));
		range.put_Item(_variant_t((long)1),_variant_t((long)2),
				_variant_t("发射电压响应级(dB)"));
		break;

	}
	
	///debug
	//float Result[10]={-210.7,-210.6,-218.1,-217.6,-215.8,
	//				-214.9,-215.3,-215.4,-215.9,-217.0};
	//for(int i=0;i<10;i++)
	//{
	//	range.put_Item(_variant_t((long)(i+2)),_variant_t((long)1),
	//		_variant_t(i+1));
	//	//设置i+2排的第2列数据
	//	range.put_Item(_variant_t((long)(i+2)),_variant_t((long)2),
	//		_variant_t(Result[i]));
	//}
	///debug
	int col=1;
	for(int i=0;i<4;i++)
	{
		if(isChaChoose[i])
		{
			if(Result[i].size()==0)continue;
			col++;
			for(unsigned int j=0;j<Result[i].size();j++)
			{
				//设置j+2排的第1列数据
				range.put_Item(_variant_t((long)(j+2)),_variant_t((long)1),
					_variant_t(startf+deltaf*j));
				//设置j+2排的第2列数据或第3列……
				range.put_Item(_variant_t((long)(j+2)),_variant_t((long)col),
					_variant_t(Result[i][j]));
		
			}
		}
	}
	CString strPath;
	char szPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,szPath);
	strPath=szPath;
	strPath+="\\测试数据.xls";
	//保存excel文件
	sheet.SaveAs(strPath,vtMissing,vtMissing,vtMissing,vtMissing,
		vtMissing,vtMissing,vtMissing,vtMissing,vtMissing);
	
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
	//renew();
	switch(nIDEvent)
	{
	case 1:
		huatu_sensity();break;
	case 2:
		huatu_response();break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CMeasure::renew()
{
	f=startf;
	for(int i=0;i<4;i++)
	{
		u[i]=-1.0;
		zoomPosition[i]=-1;
		zoomRange[i]=-1;
		Result[i].clear();
	}
	//KillTimer(1);
	//KillTimer(2);
	CWnd *pWnd=GetDlgItem(IDC_picture);
	pWnd->UpdateWindow();
	CDC *pDC=pWnd->GetDC();
	CBrush rebrush;
	rebrush.CreateSolidBrush (RGB(255,255,255));
	CBrush *pOldBrush=pDC->SelectObject (&rebrush);
	CPen newPen;
	newPen.CreatePen (PS_SOLID,1,RGB(0,0,0));
	CPen *poldPen=pDC->SelectObject(&newPen);
	CRect rect;
	pWnd->GetClientRect(rect);
	pDC->Rectangle (rect);
	pDC->SelectObject (pOldBrush);
	pDC->SelectObject(poldPen);
}
void CMeasure::huatu_sensity()
{
	int x=0,y=0;
	CWnd *pWnd=GetDlgItem(IDC_picture);
	CRect rect;
	pWnd->GetClientRect(rect);
	CDC* pDC=pWnd->GetDC();
	CPen* pNewPen=new CPen;
	pNewPen->CreatePen(PS_SOLID,1,RGB(0,0,0));
	CPen* pOldPen=pDC->SelectObject(pNewPen);
	int deltaX=rect.Width()/50;
	int deltaY=rect.Height()/100;//100是随机取的，代表纵轴分为100份
	pDC->SetViewportOrg(rect.left,rect.bottom);//测量灵敏度时都是负值,原点设为最左下角的点
	//画网格线
	CString str;
	int temp=0;
	for(x=0;x<=50;x+=2)
	{
		pDC->MoveTo((int)(x*deltaX),0);
		pDC->LineTo((int)(x*deltaX),-rect.Height());
		if(endf==startf)
		{
			str.Format("%d",(int)(startf+x));
			pDC->TextOutA((int)(x*deltaX),rect.top+5,str);
		}
		else
		{
			if((int)(startf+(endf-startf)/50*x)==temp) continue;
			temp=(int)(startf+(endf-startf)/50*x);
			str.Format("%d",temp);
			pDC->TextOutA((int)(x*deltaX),rect.top+5,str);
		}
	}
	for(y=0;y<=100;y+=10)
	{
		pDC->MoveTo(rect.left,-y*deltaY);
		pDC->LineTo(rect.right,-y*deltaY);
		str.Format("%d",-150-y);//纵轴表示-150~-250共100个点，而纵轴总共划分出了100格，所以每一格代表y
		pDC->TextOutA(rect.left-30,-y*deltaY,str);
	}
	pDC->SelectObject(pOldPen);
	delete pNewPen;
	CPen* pPen=new CPen;
	pPen->CreatePen(PS_SOLID,2,RGB(255,0,0));
	pOldPen=pDC->SelectObject(pPen);

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
			if(Result[ch].size()==0) continue;//注意要用continue，如果用break直接就跳出循环了，不会画其他通道的图形
			pDC->MoveTo(0,(int)((Result[ch][0]+150)*deltaY));//将画笔移到起点
			if(endf==startf) continue;
			for(unsigned int i=1;i<Result[ch].size();i++)
			{
				x=(int)(i*deltaf*deltaX*50/(endf-startf));
				y=(int)((Result[ch][i]+150)*deltaY);
				pDC->LineTo(x,y);//连接两个点
			}
		}
	}

	pDC->SelectObject(pOldPen);
	delete pPen;

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
void CMeasure::MeasureSensity()
{
	float Mp=-1;
	CreateBurst(f*1000,v/1000,Bwid/1000,Brep);
	MessageBox("请根据提示选择各个通道的测量区域！");

	/*Capture(ch,chcount);
	Sleep(delay+10);*/
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
	if(MessageBox("参数设置完成?是否开始测量？","提示",MB_OKCANCEL)==IDCANCEL) return;
	f=startf;
	while(f<=endf)
	{
		viPrintf(vip,":run\n");
		CreateBurst(f*1000,v/1000,Bwid/1000,Brep);
		//Capture(ch,chcount);
		//Sleep(delay+10);
		viPrintf(vip,":timebase:mode window\n");
		for(int i=0;i<4;i++)
		{
			if(isChaChoose[i])
			{
				float vrange,vtemp;
				bool isok=true;
				int flag=0;
				viPrintf(vip,":timebase:window:position %f\n",zoomPosition[i]);
				viPrintf(vip,":timebase:window:range %f\n",zoomRange[i]);
				viPrintf(vip,":measure:source channel%d\n",i+1);
				viQueryf(vip,":measure:vpp?\n","%f\n",&u[i]);
				viQueryf(vip,":channel%d:range?\n","%f\n",i+1,&vrange);
				vtemp=u[i];
				while(vtemp<-1e8||vrange<-1e8)
				{
					if(flag>10)
					{
						isok=false;
						break;
					}
					flag++;
					viQueryf(vip,":channel%d:range?\n","%f\n",i+1,&vrange);
					viQueryf(vip,":measure:vpp?\n","%f\n",&vtemp);
				}
				while(vtemp>vrange/8.0*4) 
				{
					viPrintf(vip,":channel%d:range %f\n",i+1,2*vrange);
					viQueryf(vip,":channel%d:range?\n","%f\n",i+1,&vrange);
					viQueryf(vip,":measure:vpp?\n","%f\n",&vtemp);
					isok=false;
				}
				//波形显示小于两格
				while(vtemp<vrange/8.0*2)
				{
					viPrintf(vip,":channel%d:range %f\n",i+1,vrange/2);
					viQueryf(vip,":channel%d:range?\n","%f\n",i+1,&vrange);
					viQueryf(vip,":measure:vpp?\n","%f\n",&vtemp);
					isok=false;
				}
				if(isok==false)
				{
					u[i]=0;
					i=i-1;//此通道重新测量一遍
					
				}
			}
		}
		for(int i=0;i<4;i++)
		{
			if(!isChaChoose[i]||i==chaRefer-1) continue;
			
			map<float,float>::iterator it=standMp.find(f*1000);
			if(it==standMp.end())
			{
				Mp=(standMp.lower_bound(f*1000)->second+standMp.upper_bound(f*1000)->second)/2;
			}
			else
				Mp=it->second;
			Result[i].push_back(CalSensity(Mp,u[i]/Gain,u[chaRefer-1]/Gain,d[i],d[chaRefer-1]));
			//测出的电压值要除以放大倍数
		}
		huatu_sensity();
		f+=deltaf;
		//viPrintf(vip,":run\n");
	}

	viPrintf(vip,":timebase:mode main\n");
}
void CMeasure::MeasureResponse()
{
	float Mp=-1;
	CreateBurst(f*1000,v/1000,Bwid/1000,Brep);
	MessageBox("请根据提示选择各个通道的测量区域！");

	/*Capture(ch,chcount);
	Sleep(delay+10);*/
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
	if(MessageBox("参数设置完成?是否开始测量？","提示",MB_OKCANCEL)==IDCANCEL) return;
	f=startf;
	while(f<=endf)
	{
		viPrintf(vip,":run\n");
		CreateBurst(f*1000,v/1000,Bwid/1000,Brep);
		//Capture(ch,chcount);
		//Sleep(delay+10);
		viPrintf(vip,":timebase:mode window\n");
		for(int i=0;i<4;i++)
		{
			if(isChaChoose[i])
			{
				float vrange,vtemp;
				bool isok=true;
				int flag=0;
				viPrintf(vip,":timebase:window:position %f\n",zoomPosition[i]);
				viPrintf(vip,":timebase:window:range %f\n",zoomRange[i]);
				viPrintf(vip,":measure:source channel%d\n",i+1);
				viQueryf(vip,":measure:vpp?\n","%f\n",&u[i]);
				viQueryf(vip,":channel%d:range?\n","%f\n",i+1,&vrange);
				vtemp=u[i];
				while(vtemp<-1e8||vrange<-1e8)
				{
					if(flag>10)
					{
						isok=false;
						break;
					}
					flag++;
					viQueryf(vip,":channel%d:range?\n","%f\n",i+1,&vrange);
					viQueryf(vip,":measure:vpp?\n","%f\n",&vtemp);
				}
				while(vtemp>vrange/8.0*4) 
				{
					viPrintf(vip,":channel%d:range %f\n",i+1,2*vrange);
					viQueryf(vip,":channel%d:range?\n","%f\n",i+1,&vrange);
					viQueryf(vip,":measure:vpp?\n","%f\n",&vtemp);
					isok=false;
				}
				//波形显示小于两格
				while(vtemp<vrange/8.0*2)
				{
					viPrintf(vip,":channel%d:range %f\n",i+1,vrange/2);
					viQueryf(vip,":channel%d:range?\n","%f\n",i+1,&vrange);
					viQueryf(vip,":measure:vpp?\n","%f\n",&vtemp);
					isok=false;
				}
				if(isok==false)
				{
					u[i]=0;
					i=i-1;//此通道重新测量一遍
					
				}
			}
		}
		for(int i=0;i<4;i++)
		{
			if(!isChaChoose[i]||i==chaRefer-1) continue;
			
			map<float,float>::iterator it=standMp.find(f*1000);
			if(it==standMp.end())
			{
				Mp=(standMp.lower_bound(f*1000)->second+standMp.upper_bound(f*1000)->second)/2;
			}
			else
				Mp=it->second;
			Result[i].push_back(CalResponse(Mp,u[i]*Ratio,u[chaRefer-1]/Gain,d[chaRefer-1]));
			//测出的电压值要除以放大倍数
		}
		huatu_response();
		f+=deltaf;
	}

	viPrintf(vip,":timebase:mode main\n");
}
void CMeasure::huatu_response()
{
	int x=0,y=0;
	CWnd *pWnd=GetDlgItem(IDC_picture);
	CRect rect;
	pWnd->GetClientRect(rect);
	CDC* pDC=pWnd->GetDC();
	CPen pNewPen;
	pNewPen.CreatePen(PS_SOLID,1,RGB(0,0,0));
	CPen* pOldPen=pDC->SelectObject(&pNewPen);
	int deltaX=rect.Width()/50;
	int deltaY=rect.Height()/100;//100是随机取的，代表纵轴分为100份
	pDC->SetViewportOrg(rect.left,rect.bottom);//原点设为最左下角的点
	//画网格线
	CString str;
	int temp=0;
	for(x=0;x<=50;x+=2)
	{
		pDC->MoveTo((int)(x*deltaX),0);
		pDC->LineTo((int)(x*deltaX),-rect.Height());
		if(endf==startf)
		{
			str.Format("%d",(int)(startf+x));
			pDC->TextOutA((int)(x*deltaX),rect.top+5,str);
		}
		else
		{
			if((int)(startf+(endf-startf)/50*x)==temp) continue;
			temp=(int)(startf+(endf-startf)/50*x);
			str.Format("%d",temp);
			pDC->TextOutA((int)(x*deltaX),rect.top+5,str);
		}
	}
	for(y=0;y<=100;y+=10)
	{
		pDC->MoveTo(rect.left,-y*deltaY);
		pDC->LineTo(rect.right,-y*deltaY);
		str.Format("%d",50+2*y);//纵轴表示50~250共200个点，而纵轴总共划分出了100格，所以每一格代表2y
		pDC->TextOutA(rect.left-30,-y*deltaY,str);
	}
	pDC->SelectObject(pOldPen);
	CPen pPen[4];
	pPen[0].CreatePen(PS_SOLID,2,RGB(255,165,0));//橙色画笔
	pPen[1].CreatePen(PS_SOLID,2,RGB(0,255,0));//绿色画笔
	pPen[2].CreatePen(PS_SOLID,2,RGB(0,0,255));//蓝色画笔
	pPen[3].CreatePen(PS_SOLID,2,RGB(255,0,0));//红色画笔
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
			for(unsigned int i=1;i<Result[ch].size();i++)
			{
				x=(int)(i*deltaf*deltaX*50/(endf-startf));
				y=-(int)((Result[ch][i]-50)/2*deltaY);
				pDC->LineTo(x,y);//连接两个点
			}
		}
	}

	pDC->SelectObject(pOldPen);
}