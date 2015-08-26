
// mainDialog.h : 头文件
//
#pragma once
#include <mutex>
#include <vector>
#include "GraphicEntity.h"
#include <time.h>
#include <Windows.h>
#include <Psapi.h>
#include "PolygonClip.h"
#include "afxwin.h"
#include "scarlet\strconv.hpp"
#include "MemoryBitmap.h"
#include "Drawer.h"
#include "scarlet\io\ioutil.hpp"

#pragma comment(lib,"psapi.lib")

// CDemo_ClipView_VCDlg 对话框
class CDemo_ClipView_VCDlg : public CDialogEx
{
// 构造
public:
	CDemo_ClipView_VCDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_DEMO_CLIPVIEW_VC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnActivate(UINT, CWnd*, BOOL);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	static double maxMemory;
	static bool m_bStopTimer;
	static int timerId;
	CButton m_btn_clip;
	CStatic m_stc_drawing;
	CStatic m_stc_info1;
	CStatic m_stc_info2;
	afx_msg void OnBnClickedBtnClip();
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//TODO FROM private 这边拿带public 因为clipCircle和clipLine里面有绘图,我并不明白那是什么
	CBitmap* bmpMain;
	CClientDC* mainDC;
	CDC* cacheDC;
	
	CString curPath;
	CStringArray complexArray;
	
private:
	double startTime;
	double endTime;
	double startMemory;
	double endMemory;
	BOOL hasOutCanvasData;
	CList<int> beginTIdList;

	void ClearTestCaseData();
	BOOL LoadTestCaseData(CString xmlPath, CString caseID);
	void DrawTestCase(CString xmlPath, CString caseID);

	void EndTimeAndMemoryMonitor();
	void BeginTimeAndMemoryMonitor2();
	void EndTimeAndMemoryMonitor2();
	int  SplitCStringToArray(CString str,CStringArray& strArray, CString split);
	BOOL GetThreadIdList(CList<int>& tIdList);

	//读取
	BOOL IsPointOutCanvas(Point point);
	BOOL IsLineOutCanvas(Line line);
	BOOL IsCircleOutCanvas(Circle circle);
	BOOL IsBoundaryOutCanvas(Boundary boundary);
public:

	//void DrawLineInMem(Line line, CDC* memDc);
	//void DrawCircleInMem(Circle circle, CDC* memDc);
	//void DrawBoundaryInMem(Boundary boundary, COLORREF clr, CDC* memDc);
	
	//裁剪任务 算法核心
	PolygonClipTask clipTask;


};
