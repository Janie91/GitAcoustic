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
//excel��Ҫ
#include "CApplication.h"
#include "CRange.h"
#include "CWorkbook.h"
#include "CWorkbooks.h"
#include "CWorksheet.h"
#include "CWorksheets.h"

vector<vector<float>>Result(4,vector<float>(0));
vector<float> MeaAngle;
float zoomPosition[4]={-1,-1,-1,-1},zoomRange[4]={-1,-1,-1,-1};
bool isTimer[3]={false,false,false};
bool isMeasure=true;

//...end...

// CMeasure dialog

IMPLEMENT_DYNAMIC(CMeasure, CDialogEx)

CMeasure::CMeasure(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMeasure::IDD, pParent)
{
	pturntable=NULL;
}

CMeasure::~CMeasure()
{
	if(pturntable!=NULL) delete pturntable;
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
	ON_COMMAND(ID_turntable, &CMeasure::Onturntable)
END_MESSAGE_MAP()


// CMeasure message handlers

//...My code...
void renew();
void huatu_sensity();
void Capture(vector<int> cha,int count);
void MeasureSensity();
void MeasureResponse();
void huatu_response();
void MeasureReciDir();
void huatu_recidir();
BOOL CMeasure::OnInitDialog()//���ضԻ���ʱ�ĳ�ʼ������
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	GetParent()->ShowWindow(SW_HIDE);//�Ѳ�����ǰһ��ѡ�������Ŀ�Ի���ر�
	//GetDlgItem(IDC_StartMea)->EnableWindow(false);
	CRect rect;  
	GetDlgItem(IDC_picture)->GetClientRect(&rect);
	GetDlgItem(IDC_picture)->MoveWindow(50,50,800,600,true); 
	//�̶�Picture Control�ؼ���λ�úʹ�С 
	 
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
	this->SendMessage(WM_CLOSE);
	GetParent()->ShowWindow(SW_SHOW);//��ʾѡ�����ѡ��ĶԻ���	
}

void CMeasure::Onturntable()
{
	// TODO: �ڴ���������������
	pturntable = new CTurnTable(); //��ָ������ڴ� 
	pturntable->Create(IDD_turntable); //����һ����ģ̬�Ի���  
	pturntable->ShowWindow(SW_SHOWNORMAL); //��ʾ��ģ̬�Ի���  
}

void CMeasure::OnBnClickedView()
{
	// TODO: Add your control notification handler code here
	int delay=(int)(Brep*1000);//�ӳ�delay����λms
	renew();
	//clock_t t_start=clock();
	//�Զ����������ʵ���ʾ����
	int chCount=0;
	float vRange=-1,vTemp=-1;
	//vector<int> chview;
	for(int i=0;i<4;i++)
	{
		if(isChaChoose[i]) {
			chCount++;
			/*chview.push_back(i+1);*/
			viPrintf(vip,":channel%d:display on\n",i+1);//��ͨ��
			viPrintf(vip,":channel%d:offset 0\n",i+1);			
		}
		else viPrintf(vip,":channel%d:display off\n",i+1);
	}
	if(chCount==0)//���û��ѡ��ʹ�õ�ͨ������ʾ������
	{
		AfxMessageBox("����ʾ��������������ѡ�����ͨ����");
		viPrintf(vip,"*rst\n");
		return;
	}
	viPrintf(vip,"timebase:range %f\n",(Bwid/1000.0)*2);//����ʱ��������ʱ�䳤��
	CreateBurst(f*1000,v/1000,Bwid/1000,Brep);
	Sleep(100);
	ScopeTrigger();
	//clock_t t_end=clock();

	//Sleep((Brep-(t_end-t_start)/CLOCKS_PER_SEC)*1000);
	
	//Capture(chview,chCount);
	//Sleep(delay+10);//Ҫ�ȴ����ݲɼ����
	int chflag=0,flag=0;
	for(int i=0;i<4;i++)
	{
		if(isChaChoose[i])
		{
			chflag++;//��ǵڼ���ͨ�������ڵ�����ʾ������Ļ����ʾ��λ��
			viPrintf(vip,":measure:source channel%d\n",i+1);
			viQueryf(vip,":channel%d:range?\n","%f\n",i+1,&vRange);//�������ĵ�ѹ��Χ
			viQueryf(vip,":measure:vpp?\n","%f\n",&vTemp);//�������ֵ
			
			while(vTemp<-1e8||vRange<-1e8||vTemp==-1||vRange==-1)
			{
				if(flag>10) break;
				flag++;
				viQueryf(vip,":channel%d:range?\n","%f\n",i+1,&vRange);
				viQueryf(vip,":measure:vpp?\n","%f\n",&vTemp);
			}
			flag=0;
			while(vTemp>9e+37)//������ʾ��������Ļ��
			{
				if(flag>10||2*vRange>40) 
				{
					viPrintf(vip,":channel%d:range 40\n",i+1);
					viQueryf(vip,":channel%d:range?\n","%f\n",i+1,&vRange);
					viQueryf(vip,":measure:vpp?\n","%f\n",&vTemp);
					break;
				}
				flag++;
				viPrintf(vip,":channel%d:range %f\n",i+1,2*vRange);//���������ѹ��Χ
				viQueryf(vip,":channel%d:range?\n","%f\n",i+1,&vRange);
				viQueryf(vip,":measure:vpp?\n","%f\n",&vTemp);
			}
			flag=0;
			
			//������ʾ�����ĸ�
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
			//������ʾС������
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
				viPrintf(vip,":channel%d:offset 0\n",i+1);//ֻ��һ��ͨ�������м���ʾ
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
		AfxMessageBox("��ѡ���׼ˮ�����ļ���");
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
		AfxMessageBox("��ѡ��ʾ�����Ĳ���ͨ����");
		return;
	}
	switch(ChooseItem)
	{
	case 0: //����������
		MeasureSensity();
		SetTimer(1,1000,NULL);
		break;
	case 1://���������ѹ��Ӧ
		MeasureResponse();
		SetTimer(2,1000,NULL);
		break;
	case 2:
		MeasureReciDir();
		SetTimer(3,1000,NULL);
	}
	
	
}

void CMeasure::OnBnClickedStopmea()
{
	// TODO: Add your control notification handler code here
	isMeasure=false;
	CreateMulFrePulse(1,0.5f,1);
}

void CMeasure::OnBnClickedquitsys()
{
	// TODO: Add your control notification handler code here
	for(int i=0;i<3;i++)
	{
		if(isTimer[i]) KillTimer(i+1);
	}
	viClose(vip);
	viClose(vig);
	viClose(vidp);
	viClose(vidg);
	CDialog::OnCancel();
	GetParent()->SendMessage(WM_CLOSE);
	AfxGetMainWnd()->SendMessage(WM_CLOSE);//�˳�ϵͳҪ�����Ի���ر�
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
			_variant_t("Ƶ�ʣ�kHz)"));
		range.put_Item(_variant_t((long)1),_variant_t((long)2),
				_variant_t("�����ȼ�(dB)"));
		break;
	case 1:
		range.put_Item(_variant_t((long)1),_variant_t((long)1),
			_variant_t("Ƶ�ʣ�kHz)"));
		range.put_Item(_variant_t((long)1),_variant_t((long)2),
				_variant_t("�����ѹ��Ӧ��(dB)"));
		break;
	case 2:
		range.put_Item(_variant_t((long)1),_variant_t((long)1),
			_variant_t("�Ƕ�(��)"));
		range.put_Item(_variant_t((long)1),_variant_t((long)2),
				_variant_t("��ѹ(V)"));
		break;

	}
	
	///debug
	//float Result[10]={-210.7,-210.6,-218.1,-217.6,-215.8,
	//				-214.9,-215.3,-215.4,-215.9,-217.0};
	//for(int i=0;i<10;i++)
	//{
	//	range.put_Item(_variant_t((long)(i+2)),_variant_t((long)1),
	//		_variant_t(i+1));
	//	//����i+2�ŵĵ�2������
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
				//����j+2�ŵĵ�1������
				if(ChooseItem==2) range.put_Item(_variant_t((long)(j+2)),_variant_t((long)1),
					_variant_t(MeaAngle[j]));
				else range.put_Item(_variant_t((long)(j+2)),_variant_t((long)1),
					_variant_t(startf+deltaf*j));
				//����j+2�ŵĵ�2�����ݻ��3�С���
				range.put_Item(_variant_t((long)(j+2)),_variant_t((long)col),
					_variant_t(Result[i][j]));
		
			}
		}
	}
	CString strPath;
	char szPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH,szPath);
	strPath=szPath;
	strPath+="\\��������.xls";
	//����excel�ļ�
	sheet.SaveAs(strPath,vtMissing,vtMissing,vtMissing,vtMissing,
		vtMissing,vtMissing,vtMissing,vtMissing,vtMissing);
	
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
	//renew();
	switch(nIDEvent)
	{
	case 1:
		huatu_sensity();isTimer[0]=true;break;
	case 2:
		huatu_response();isTimer[1]=true;break;
	case 3:
		huatu_recidir();isTimer[2]=true;break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CMeasure::renew()
{
	f=startf;
	isMeasure=true;
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
	int deltaY=rect.Height()/100;//100�����ȡ�ģ����������Ϊ100��
	pDC->SetViewportOrg(rect.left,rect.bottom);//����������ʱ���Ǹ�ֵ,ԭ����Ϊ�����½ǵĵ�
	//��������
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
		str.Format("%d",-150-y);//�����ʾ-150~-250��100���㣬�������ܹ����ֳ���100������ÿһ�����y
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
	//	pDC->LineTo(x,y);//����������
	//}
	////debug
	for(int ch=0;ch<4;ch++)
	{
		if(isChaChoose[ch])
		{
			if(Result[ch].size()==0) continue;//ע��Ҫ��continue�������breakֱ�Ӿ�����ѭ���ˣ����ử����ͨ����ͼ��
			pDC->MoveTo(0,(int)((Result[ch][0]+150)*deltaY));//�������Ƶ����
			if(endf==startf) continue;
			for(unsigned int i=1;i<Result[ch].size();i++)
			{
				x=(int)(i*deltaf*deltaX*50/(endf-startf));
				y=(int)((Result[ch][i]+150)*deltaY);
				pDC->LineTo(x,y);//����������
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
	MessageBox("�������ʾѡ�����ͨ���Ĳ�������");

	/*Capture(ch,chcount);
	Sleep(delay+10);*/
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
	if(MessageBox("�����������?�Ƿ�ʼ������","��ʾ",MB_OKCANCEL)==IDCANCEL) return;
	f=startf;
	isMeasure=true;
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
				//������ʾС������
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
					i=i-1;//��ͨ�����²���һ��
					
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
			//����ĵ�ѹֵҪ���ԷŴ���
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
	MessageBox("�������ʾѡ�����ͨ���Ĳ�������");

	/*Capture(ch,chcount);
	Sleep(delay+10);*/
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
	if(MessageBox("�����������?�Ƿ�ʼ������","��ʾ",MB_OKCANCEL)==IDCANCEL) return;
	f=startf;
	isMeasure=true;
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
				//������ʾС������
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
					i=i-1;//��ͨ�����²���һ��
					
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
			//����ĵ�ѹֵҪ���ԷŴ���
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
	int deltaY=rect.Height()/100;//100�����ȡ�ģ����������Ϊ100��
	pDC->SetViewportOrg(rect.left,rect.bottom);//ԭ����Ϊ�����½ǵĵ�
	//��������
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
		str.Format("%d",50+2*y);//�����ʾ50~250��200���㣬�������ܹ����ֳ���100������ÿһ�����2y
		pDC->TextOutA(rect.left-30,-y*deltaY,str);
	}
	pDC->SelectObject(pOldPen);
	CPen pPen[4];
	pPen[0].CreatePen(PS_SOLID,2,RGB(255,165,0));//��ɫ����
	pPen[1].CreatePen(PS_SOLID,2,RGB(0,255,0));//��ɫ����
	pPen[2].CreatePen(PS_SOLID,2,RGB(0,0,255));//��ɫ����
	pPen[3].CreatePen(PS_SOLID,2,RGB(255,0,0));//��ɫ����
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
			for(unsigned int i=1;i<Result[ch].size();i++)
			{
				x=(int)(i*deltaf*deltaX*50/(endf-startf));
				y=-(int)((Result[ch][i]-50)/2*deltaY);
				pDC->LineTo(x,y);//����������
			}
		}
	}

	pDC->SelectObject(pOldPen);
}
void CMeasure::MeasureReciDir()
{
	f=startf;
	isMeasure=true;
	if(pturntable==NULL) 
	{
		AfxMessageBox("�������ӿ��ƻ�תϵͳ��");
		return;
	}
	pturntable->KillTimer(1);//�رն�ʱ�������⴮��ͨ�ų�ͻ��
	float angle=pturntable->ReadCurrentAngle();//������ǰ�ĽǶ�ֵ
	bool isRightDir=true;
	CreateBurst(f*1000,v/1000,Bwid/1000,Brep);//�����ź�Դ
	if(abs(angle-StartAngle)<=abs(angle-EndAngle))
	{
		pturntable->RotateTargetAngle(StartAngle);//�����ǰλ������ʼ�Ƕȿ�������ת����ʼ�Ƕ�
		isRightDir=true;
	}
	else 
	{
		pturntable->RotateTargetAngle(EndAngle);
		isRightDir=false;
	}
	angle=pturntable->ReadCurrentAngle();
	while(abs(angle-StartAngle)>0.1)//֮ǰ���Ե�ʱ����д��1���������1���е�����Ը�Ϊ0.1����
	{
		angle=pturntable->ReadCurrentAngle();
	}//�ȴ�ת��ָ���Ƕ����
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
	if(MessageBox("�����������?�Ƿ�ʼ������","��ʾ",MB_OKCANCEL)==IDCANCEL) return;
	if(isRightDir) pturntable->RotateRight();//˳ʱ��ת������
	else pturntable->RotateLeft();//��ʱ��ת��
	Sleep(200);
	angle=pturntable->ReadCurrentAngle();	
	while((isRightDir&&angle<EndAngle)||(!isRightDir&&angle>StartAngle))
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
				//������ʾС������
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
					i=i-1;//��ͨ�����²���һ��
					
				}
				angle=pturntable->ReadCurrentAngle();
				Sleep(200);
				MeaAngle.push_back(angle);
			}
		}
		//����ǶȺ͵�ѹֵ
		for(int i=0;i<4;i++)
		{
			if(!isChaChoose[i]) continue;
			Result[i].push_back(u[i]);
		}
		
		//���Ƽ�����ͼ
		huatu_recidir();
		angle=pturntable->ReadCurrentAngle();
		if(isRightDir&&angle>=EndAngle)
		{
			pturntable->StopRotateRight();
			break;
		}
		else if(!isRightDir&&angle<=StartAngle)
		{
			pturntable->StopRotateLeft();
			break;
		}		
	}
	if(isRightDir) pturntable->StopRotateRight();
	else pturntable->StopRotateLeft();
	viPrintf(vip,":timebase:mode main\n");
	pturntable->SetTimer(1,200,NULL);
}
void CMeasure::huatu_recidir()
{
	int cycle_num=10,redius_num=36;//cycle_num��ͬ��Բ�ĸ�����redius_num��ֱ��������
	CWnd *pWnd = GetDlgItem(IDC_picture);
	CRect rect;
	pWnd->GetClientRect(rect);
	CDC *pDC = pWnd->GetDC();
	CPen newPen;
	newPen.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	CPen* pOldPen = (CPen*)pDC->SelectObject(&newPen);
	pDC->SelectStockObject(NULL_BRUSH);//û�л�ˢ�ͱ���Բ�����ǣ����Ŀ���Բ
	int w=rect.Width(),h=rect.Height ();
	pDC->SetViewportOrg(w/2,h/2);//����ԭ��
	int R=(h-40)/2;//�뾶
	int deltaR=R/cycle_num;
	for(int i=1;i<=cycle_num;i++)
	{
		pDC->Ellipse(-i*deltaR,-i*deltaR,i*deltaR,i*deltaR);//��Բ����ʵ���ҵ�һ�����������ε����ϽǶ�������½Ƕ���
	}
	pDC->Ellipse(-cycle_num*deltaR,-cycle_num*deltaR,cycle_num*deltaR,cycle_num*deltaR);//�������Բ
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

	////debug
	//CPen ppen;
	//ppen.CreatePen(PS_SOLID, 1, RGB(255,0,0));
	//pOldPen=pDC->SelectObject(&ppen);
	//float Result[9]={3.2f,18.8f,10.5f,20.6f,30.0f,23.9f,9.6f,16.8f,7.5f};
	//float MeaAngle[9]={-28.7f,-22.6f,-18.9f,-9.2f,-0.1f,2.3f,8.4f,18.2f,29.9f};
	//pDC->MoveTo(0,0);
	//float max=Result[0];
	//for(unsigned int i=1;i<9;i++)
	//			if(max<Result[i]) max=Result[i];
	//for(unsigned int i=0;i<9;i++)
	//{
	//	deltaA=PI*MeaAngle[i]/180;
	//	x=-(int)(Result[i]/max*R*sin(deltaA));
	//	y=-(int)(Result[i]/max*R*cos(deltaA));
	//	pDC->LineTo(x,y);//����������
	//}

	////debug
	CPen pPen[4];
	pPen[0].CreatePen(PS_SOLID,2,RGB(255,165,0));//��ɫ����
	pPen[1].CreatePen(PS_SOLID,2,RGB(0,255,0));//��ɫ����
	pPen[2].CreatePen(PS_SOLID,2,RGB(0,0,255));//��ɫ����
	pPen[3].CreatePen(PS_SOLID,2,RGB(255,0,0));//��ɫ����
	
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