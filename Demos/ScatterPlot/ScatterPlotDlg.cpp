
// ScatterPlotDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "ScatterPlot.h"
#include "ScatterPlotDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include "ChartCtrl\ChartCtrl.h"
#include "ChartCtrl\ChartAxisLabel.h"
#include "ChartCtrl\ChartPointsSerie.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CScatterPlotDlg 对话框



CScatterPlotDlg::CScatterPlotDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SCATTERPLOT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CScatterPlotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CUSTOM_SHOW, m_ChartCtrlXZ);
	DDX_Control(pDX, IDC_CUSTOM_SHOW2, m_ChartCtrlYZ);
	DDX_Control(pDX, IDC_CUSTOM_SHOW3, m_ChartCtrlXY);
}

BEGIN_MESSAGE_MAP(CScatterPlotDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()



//初始化显示的区域大小
#define DATA_SHOW_LENGHT	2000	//总共显示的点个数
#define DATA_UPDATE_LENGHT	10    //每次更新的点个数
#define DATA_SHOW_X_AXIS	1000   //X轴显示的点最大值
#define DATA_SHOW_Y_AXIS	1000   //Y轴显示的点最大值
#define DATA_SHOW_Z_AXIS	1000   //Y轴显示的点最大值
//要显示点的缓冲数据
static double s_dXBuff[DATA_SHOW_LENGHT] = { 0 };
static double s_dYBuff[DATA_SHOW_LENGHT] = { 0 };
static double s_dZBuff[DATA_SHOW_LENGHT] = { 0 };




// CScatterPlotDlg 消息处理程序

BOOL CScatterPlotDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//获取显示的对话框大小
	CRect rect;
	GetClientRect(&rect);			//取客户大小
	m_ptOld.x = rect.right - rect.left;
	m_ptOld.y = rect.bottom - rect.top;
	int iCX = GetSystemMetrics(SM_CXFULLSCREEN);
	int iCY = GetSystemMetrics(SM_CXFULLSCREEN);
	CRect rt;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rt, 0);
	iCY = rt.bottom;
	MoveWindow(0, 0, iCX, iCY);

	DataBuffInit();
	ChartCtrlInit();
	SetTimer(0, 100, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CScatterPlotDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CScatterPlotDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CScatterPlotDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CScatterPlotDlg::ReSize()
{
	// TODO: 在此处添加实现代码.
	float fsp[2];
	POINT ptNew;//获取当前对话框大小
	CRect rectNew;//获取当前对话框的坐标
	GetClientRect(&rectNew);
	ptNew.x = rectNew.right - rectNew.left;
	ptNew.y = rectNew.bottom - rectNew.top;

	fsp[0] = (float)ptNew.x / m_ptOld.x;
	fsp[1] = (float)ptNew.y / m_ptOld.y;

	int iWoc;
	CRect rect;
	CPoint ptOldTL, ptNewTL;//左上角
	CPoint ptOldBR, ptNewBR;//右下角
	//列出所有的子空间
	HWND hwndChild = ::GetWindow(m_hWnd, GW_CHILD);
	while (hwndChild) {
		iWoc = ::GetDlgCtrlID(hwndChild);//取得ID
		GetDlgItem(iWoc)->GetWindowRect(rect);
		ScreenToClient(rect);

		ptOldTL = rect.TopLeft();
		ptNewTL.x = long(ptOldTL.x*fsp[0]);
		ptNewTL.y = long(ptOldTL.y*fsp[1]);
		ptOldBR = rect.BottomRight();
		ptNewBR.x = long(ptOldBR.x*fsp[0]);
		ptNewBR.y = long(ptOldBR.y*fsp[1]);

		rect.SetRect(ptNewTL, ptNewBR);
		GetDlgItem(iWoc)->MoveWindow(rect, TRUE);
		hwndChild = ::GetWindow(hwndChild, GW_HWNDNEXT);
	}
	m_ptOld = ptNew;
	return;
}


void CScatterPlotDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码

	//窗体大小发生变动。处理函数ReSize
	if (nType == SIZE_RESTORED || nType == SIZE_MAXIMIZED)
	{
		ReSize();//对每一个控件依次做调整
	}
}


void CScatterPlotDlg::ChartCtrlInit()
{
	// TODO: 在此处添加实现代码.
	//手动创建显示窗口
	//CRect rect, rectChart;
	//GetDlgItem(IDC_CUSTOM_SHOW)->GetWindowRect(&rect);
	//ScreenToClient(rect);
	//rectChart = rect;
	//rectChart.top = rect.bottom + 3;
	//rectChart.bottom = rectChart.top + rect.Height();
	//m_ChartCtrl2.Create(this, rectChart, IDC_CUSTOM_SHOW2);
	//m_ChartCtrl2.ShowWindow(SW_SHOWNORMAL);
	///////////////////////显示主题/////////////////////////////
	m_ChartCtrlXZ.GetTitle()->AddString(_T("主视图"));
	m_ChartCtrlYZ.GetTitle()->AddString(_T("左视图"));
	m_ChartCtrlXY.GetTitle()->AddString(_T("俯视图"));
	///////////////////////创建坐标xy标识/////////////////////////////
	//m_ChartCtrl.GetBottomAxis()->GetLabel()->SetText(_T("强度"));
	//m_ChartCtrl.GetLeftAxis()->GetLabel()->SetText(_T("采样点"));
	///////////////////////创建坐标显示范围/////////////////////////////
	CChartAxis *pAxisXZ = NULL;
	pAxisXZ = m_ChartCtrlXZ.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxisXZ->SetMinMax(0, DATA_SHOW_X_AXIS);
	pAxisXZ = m_ChartCtrlXZ.CreateStandardAxis(CChartCtrl::LeftAxis);
	pAxisXZ->SetMinMax(0, DATA_SHOW_Y_AXIS);

	CChartAxis *pAxisYZ = NULL;
	pAxisYZ = m_ChartCtrlYZ.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxisYZ->SetMinMax(0, DATA_SHOW_X_AXIS);
	pAxisYZ = m_ChartCtrlYZ.CreateStandardAxis(CChartCtrl::LeftAxis);
	pAxisYZ->SetMinMax(0, DATA_SHOW_Y_AXIS);

	CChartAxis *pAxisXY = NULL;
	pAxisXY = m_ChartCtrlXY.CreateStandardAxis(CChartCtrl::BottomAxis);
	pAxisXY->SetMinMax(0, DATA_SHOW_X_AXIS);
	pAxisXY = m_ChartCtrlXY.CreateStandardAxis(CChartCtrl::LeftAxis);
	pAxisXY->SetMinMax(0, DATA_SHOW_Y_AXIS);
}


void CScatterPlotDlg::DataBuffInit()
{
	// TODO: 在此处添加实现代码.

	//for (int i = 0; i < DATA_SHOW_LENGHT; i++) {
	//	s_dXBuff[i] = 0;
	//	s_dYBuff[i] = 0;
	//	s_dZBuff[i] = 0;
	//}
	for (int i = 0; i < DATA_SHOW_LENGHT; i++) {
		s_dXBuff[i] = rand() / 10000 * rand() / 100;
		s_dYBuff[i] = rand() / 10000 * rand() / 100;
		s_dZBuff[i] = rand() / 10000 * rand() / 100;
	}
}


void CScatterPlotDlg::DataShow(double *xb, double *yb, double *zb, int len)
{
	// TODO: 在此处添加实现代码.
	m_ChartCtrlXZ.EnableRefresh(false);
	m_ChartCtrlYZ.EnableRefresh(false);
	m_ChartCtrlXY.EnableRefresh(false);

	CChartPointsSerie *pPointsSerie;

	m_ChartCtrlXZ.RemoveAllSeries();
	pPointsSerie = m_ChartCtrlXZ.CreatePointsSerie();
	pPointsSerie->SetPointType(CChartPointsSerie::ptEllipse);
	pPointsSerie->AddPoints(xb, zb, len);

	m_ChartCtrlYZ.RemoveAllSeries();
	pPointsSerie = m_ChartCtrlYZ.CreatePointsSerie();
	pPointsSerie->SetPointType(CChartPointsSerie::ptEllipse);
	pPointsSerie->AddPoints(yb, zb, len);

	m_ChartCtrlXY.RemoveAllSeries();
	pPointsSerie = m_ChartCtrlXY.CreatePointsSerie();
	pPointsSerie->SetPointType(CChartPointsSerie::ptEllipse);
	pPointsSerie->AddPoints(xb, yb, len);

	UpdateWindow();
	m_ChartCtrlXZ.EnableRefresh(true);
	m_ChartCtrlYZ.EnableRefresh(true);
	m_ChartCtrlXY.EnableRefresh(true);
}


void CScatterPlotDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	for (int m = 0; m < DATA_SHOW_LENGHT - DATA_UPDATE_LENGHT; m++)
	{
		s_dXBuff[m] = s_dXBuff[DATA_UPDATE_LENGHT + m];
		s_dYBuff[m] = s_dYBuff[DATA_UPDATE_LENGHT + m];
		s_dZBuff[m] = s_dZBuff[DATA_UPDATE_LENGHT + m];
	}
	DataShow(s_dXBuff, s_dYBuff, s_dZBuff, DATA_SHOW_LENGHT);

	CDialogEx::OnTimer(nIDEvent);
}
