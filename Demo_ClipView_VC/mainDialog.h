
// mainDialog.h : ͷ�ļ�
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

// CDemo_ClipView_VCDlg �Ի���
class CDemo_ClipView_VCDlg : public CDialogEx
{
// ����
public:
	CDemo_ClipView_VCDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_DEMO_CLIPVIEW_VC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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

	//TODO FROM private ����ô�public ��ΪclipCircle��clipLine�����л�ͼ,�Ҳ�����������ʲô
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

	//��ȡ
	BOOL IsPointOutCanvas(Point point);
	BOOL IsLineOutCanvas(Line line);
	BOOL IsCircleOutCanvas(Circle circle);
	BOOL IsBoundaryOutCanvas(Boundary boundary);
public:

	//void DrawLineInMem(Line line, CDC* memDc);
	//void DrawCircleInMem(Circle circle, CDC* memDc);
	//void DrawBoundaryInMem(Boundary boundary, COLORREF clr, CDC* memDc);
	
	//�ü����� �㷨����
	PolygonClipTask clipTask;


};
