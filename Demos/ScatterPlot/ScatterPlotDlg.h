
// ScatterPlotDlg.h: 头文件
//

#pragma once
#include "ChartCtrl/ChartCtrl.h"

// CScatterPlotDlg 对话框
class CScatterPlotDlg : public CDialogEx
{
// 构造
public:
	CScatterPlotDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SCATTERPLOT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
public:
	void ReSize();

public:
	CChartCtrl m_ChartCtrlXZ;		// 主视图
	CChartCtrl m_ChartCtrlYZ;		// 左视图
	CChartCtrl m_ChartCtrlXY;		// 俯视图
	POINT m_ptOld;
	
	void ChartCtrlInit();
	void DataBuffInit();
	void DataShow(double *xb, double *yb, double *zb, int len);

};
