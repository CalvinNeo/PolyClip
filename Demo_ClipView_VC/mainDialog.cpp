//
// mainDialog.cpp : 实现文件
//
#include "stdafx.h"
#include "prjClip.h"
#include "mainDialog.h"
#include "afxdialogex.h"
#include <TlHelp32.h>
#include "PolygonClip.h"
#include <thread>
#include <memory>
#include "DataCollection.h"
#include "XMLParser.h"
#include <mutex>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#undef min
#undef max

// mainDialog 主窗体

#define INFO_HEIGHT		80
#define TESTDATA_XML0  L"TestData0.xml"
#define TESTDATA_XML1  L"TestData1.xml"

#define TESTDATA_XML2  L"TestData2.xml"
#define TESTDATA_XML3  L"TestData3.xml"

#define TESTDATA_XML_S1  L"TestData_Efficiency_CircleInBoundary.xml"
#define TESTDATA_XML_S2  L"TestData_Efficiency_CircleOutOfBoundary.xml"
#define TESTDATA_XML_S3  L"TestData_Efficiency_LineInBoundary.xml"
#define TESTDATA_XML_S4  L"TestData_Efficiency_LineOutOfBoundary.xml"

CDemo_ClipView_VCDlg::CDemo_ClipView_VCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDemo_ClipView_VCDlg::IDD, pParent) {
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
}

void CDemo_ClipView_VCDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_CLIP, m_btn_clip);
	DDX_Control(pDX, IDC_STATIC_DRAWING, m_stc_drawing);
	DDX_Control(pDX, IDC_STATIC_INFO_1, m_stc_info1);
	DDX_Control(pDX, IDC_STATIC_INFO_2, m_stc_info2);
}

BEGIN_MESSAGE_MAP(CDemo_ClipView_VCDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_ACTIVATE()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_CLIP, &CDemo_ClipView_VCDlg::OnBnClickedBtnClip)
	ON_WM_NCLBUTTONDOWN()
END_MESSAGE_MAP()


BOOL CDemo_ClipView_VCDlg::OnInitDialog() {
	CDialogEx::OnInitDialog();
	mainDC = new CClientDC(this);
	cacheDC = new CDC;
	bmpMain = new CBitmap;
	cacheDC->CreateCompatibleDC(mainDC);
	bmpMain->CreateCompatibleBitmap(mainDC, CANVAS_WIDTH, CANVAS_HEIGHT);
	cacheDC->SelectObject(bmpMain);

	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	CRect clientRect;
	GetClientRect(&clientRect);
	CRect windowRect;
	GetWindowRect(&windowRect);
	int width = CANVAS_WIDTH + windowRect.Width() - clientRect.Width();
	int height = CANVAS_HEIGHT + INFO_HEIGHT + windowRect.Height() - clientRect.Height();
	SetWindowPos(NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);

	m_btn_clip.SetWindowPos(NULL, 50, CANVAS_HEIGHT + 10, 80, 30, SWP_NOZORDER);
	m_stc_drawing.SetWindowPos(NULL, 140, CANVAS_HEIGHT + 15, 200, 20, SWP_NOZORDER);
	m_stc_info1.SetWindowPos(NULL, 250, CANVAS_HEIGHT + 5, 550, 50, SWP_NOZORDER);
	m_stc_info2.SetWindowPos(NULL, 250, CANVAS_HEIGHT + 60, 550, 20, SWP_NOZORDER);

	hasOutCanvasData = FALSE;

	GetCurrentDirectory(MAX_PATH, curPath.GetBufferSetLength(MAX_PATH + 1));
	curPath.ReleaseBuffer();
	curPath += L"\\";

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDemo_ClipView_VCDlg::OnActivate(UINT nstate, CWnd* pWnd, BOOL bMin) {
	CDialogEx::OnActivate(nstate, pWnd, bMin);
	mainDC->BitBlt(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, cacheDC, 0, 0, SRCCOPY);
}


void CDemo_ClipView_VCDlg::OnPaint() {
	if (IsIconic()) {
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
	} else {
		mainDC->BitBlt(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, cacheDC, 0, 0, SRCCOPY);
		CDialogEx::OnPaint();
	}
}

HCURSOR CDemo_ClipView_VCDlg::OnQueryDragIcon() {
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CDemo_ClipView_VCDlg::OnCommand(WPARAM wParam, LPARAM lParam) {
	UINT uMsg = LOWORD(wParam);
	switch (uMsg) {
	case ID_TESTCASE1:
	{
		CString xmlPath = curPath + TESTDATA_XML1;
		DrawTestCase(xmlPath, L"1");
	}
	break;
	case ID_TESTCASE2:
	{
		CString xmlPath = curPath + TESTDATA_XML1;
		DrawTestCase(xmlPath, L"2");
	}
	break;
	case ID_TESTCASE3:
	{
		CString xmlPath = curPath + TESTDATA_XML1;
		DrawTestCase(xmlPath, L"3");
	}
	break;
	case ID_TESTCASE4:
	{
		CString xmlPath = curPath + TESTDATA_XML1;
		DrawTestCase(xmlPath, L"4");
	}
	break;
	case ID_TESTCASE5_0:
	{
		CString xmlPath = curPath + TESTDATA_XML0;
		DrawTestCase(xmlPath, L"5");
	}
	break;
	case ID_TESTCASE5_1:
	{
		CString xmlPath = curPath + TESTDATA_XML2;
		DrawTestCase(xmlPath, L"5");
	}
	break;
	case ID_TESTCASE5_2:
	{
		CString xmlPath = curPath + TESTDATA_XML3;
		DrawTestCase(xmlPath, L"5");
	}
	break;
	case ID_TESTCASE_S1:
	{
		CString xmlPath = curPath + TESTDATA_XML_S1;
		DrawTestCase(xmlPath, L"5");
	}
	break;
	case ID_TESTCASE_S2:
	{
		CString xmlPath = curPath + TESTDATA_XML_S2;
		DrawTestCase(xmlPath, L"5");
	}
	break;
	case ID_TESTCASE_S3:
	{
		CString xmlPath = curPath + TESTDATA_XML_S3;
		DrawTestCase(xmlPath, L"5");
	}
	break;
	case ID_TESTCASE_S4:
	{
		CString xmlPath = curPath + TESTDATA_XML_S4;
		DrawTestCase(xmlPath, L"5");
	}
	break;
	break;
	default:
		break;
	}
	return CDialogEx::OnCommand(wParam, lParam);
}

void CDemo_ClipView_VCDlg::OnNcLButtonDown(UINT nHitTest, CPoint point) {
	CDialogEx::OnNcLButtonDown(nHitTest, point);
}


void CDemo_ClipView_VCDlg::OnBnClickedBtnClip() {
	BeginTimeAndMemoryMonitor2();
	clipTask.performClip();
	HDC hdc = cacheDC->GetSafeHdc();
	auto bitmapInfoHeader = clipTask.memoryBitmap_.BitmapInfoHeader();
	auto err = SetDIBitsToDevice(hdc, 0, 0, 800, 600, 0, 0, 0, 600, clipTask.memoryBitmap_.Data(), (BITMAPINFO*)&bitmapInfoHeader, DIB_RGB_COLORS);
	EndTimeAndMemoryMonitor2();
}


BOOL CDemo_ClipView_VCDlg::IsPointOutCanvas(Point point) {
	if (point.x < 0 || point.y < 0 || point.x > CANVAS_WIDTH || point.y > CANVAS_HEIGHT) {
		return TRUE;
	}
	return FALSE;
}
BOOL CDemo_ClipView_VCDlg::IsLineOutCanvas(Line line) {
	if (IsPointOutCanvas(line.startpoint) || IsPointOutCanvas(line.endpoint)) {
		return TRUE;
	}
	return FALSE;
}
BOOL CDemo_ClipView_VCDlg::IsCircleOutCanvas(Circle circle) {
	if (IsPointOutCanvas(circle.center)) {
		return TRUE;
	}
	if ((circle.center.x - circle.radius) < 0 || (circle.center.x + circle.radius) > CANVAS_WIDTH
		|| (circle.center.y - circle.radius) < 0 || (circle.center.y + circle.radius) > CANVAS_HEIGHT) {
		return TRUE;
	}
	return FALSE;
}
BOOL CDemo_ClipView_VCDlg::IsBoundaryOutCanvas(Boundary boundary) {
	vector<Point>::iterator iter = boundary.vertexs.begin();
	for (; iter != boundary.vertexs.end(); iter++) {
		if (IsPointOutCanvas(*iter)) {
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CDemo_ClipView_VCDlg::LoadTestCaseData(CString xmlPath, CString caseID) {
	parser::DataCollection dc;
	std::shared_ptr<parser::Mapping> mapping = std::make_shared<parser::Mapping>();
	auto err = mapping->MappingFile(xmlPath.GetBuffer());
	if (!err.second) {
		return FALSE;
	}
	parser::XMLParser xmlp;
	err = xmlp.Parse((const char*)mapping->Buffer(), dc);
	if (!err.second) {
		return FALSE;
	}

	if ((caseID == L"5" && (dc.end() - dc.begin()) != 1) ||
		(caseID != L"5" && (dc.end() - dc.begin()) != 4)) {
		return FALSE;
	}

	auto id = _wtoi(caseID);
	auto iter = dc.begin();
	if (id != 5) {
		iter += (id - 1);
	}

	CString strComplex = scarlet::strconv::stringToWstring(iter->_comp).c_str();
	if (SplitCStringToArray(strComplex, complexArray, L",") != 6) {
		complexArray.RemoveAll();
		complexArray.InsertAt(0, L" ");
		complexArray.InsertAt(0, L" ");
		complexArray.InsertAt(0, L" ");
		complexArray.InsertAt(0, L" ");
		complexArray.InsertAt(0, L" ");
		complexArray.InsertAt(0, L" ");
	}

	for (auto & k : iter->lines) {
		Line line;
		line.startpoint.x = k.start.first;
		line.startpoint.y = k.start.second;
		line.endpoint.x = k.end.first;
		line.endpoint.y = k.end.second;

		if (IsLineOutCanvas(line)) {
			hasOutCanvasData = TRUE;
			continue;
		}
		clipTask.lines.push_back(line);
	}

	for (auto & k : iter->circles) {
		Circle circle;

		circle.center.x = k.p.first;
		circle.center.y = k.p.second;
		circle.radius = k.r;

		if (IsCircleOutCanvas(circle)) {
			hasOutCanvasData = TRUE;
			continue;
		}
		clipTask.circles.push_back(circle);
	}


	for (auto &k : iter->boundarys) {
#ifndef SINGLE_BOUNDARIES
#define FOR_BOUNDARY bound
		Boundary bound;
#else
#define FOR_BOUNDARY boundary
#endif
		FOR_BOUNDARY.isConvex = false;
		for (auto &i : k.p) {
			Point point;
			point.x = i.first;
			point.y = i.second;
			FOR_BOUNDARY.AddVertex(point);
		}
		if (IsBoundaryOutCanvas(FOR_BOUNDARY)) {
			hasOutCanvasData = TRUE;
			FOR_BOUNDARY.vertexs.clear();
		}
#ifndef SINGLE_BOUNDARIES
		clipTask.boundaries.push_back(FOR_BOUNDARY);
#endif
	}


	return TRUE;
}
void CDemo_ClipView_VCDlg::ClearTestCaseData() {
	hasOutCanvasData = FALSE;
	complexArray.RemoveAll();

	clipTask.clearTest();

	CClientDC dc(this);
	dc.FillSolidRect(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, RGB(0, 0, 0));
}

//新版本变更*********************************************
double CDemo_ClipView_VCDlg::maxMemory = 0;
bool CDemo_ClipView_VCDlg::m_bStopTimer = false;
int CDemo_ClipView_VCDlg::timerId = 0;
void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime);
void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT nTimerid, DWORD dwTime) {
	HANDLE handle = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
	//double curMemory = pmc.PeakPagefileUsage;
	double curMemory = pmc.PagefileUsage;
	if (curMemory > CDemo_ClipView_VCDlg::maxMemory) {
		CDemo_ClipView_VCDlg::maxMemory = curMemory;
	}
}
UINT ThreadProc(LPVOID lParam);
UINT ThreadProc(LPVOID lParam) {
	SetTimer(NULL, ++CDemo_ClipView_VCDlg::timerId, /*频率*/10/*毫秒*/, (TIMERPROC)TimerProc);
	int itemp;
	MSG msg;
	while ((itemp = GetMessage(&msg, NULL, NULL, NULL)) && (-1 != itemp)) {
		if (CDemo_ClipView_VCDlg::m_bStopTimer) {
			KillTimer(NULL, CDemo_ClipView_VCDlg::timerId);
			return 0;
		}
		if (msg.message == WM_TIMER) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return 0;
}

void CDemo_ClipView_VCDlg::BeginTimeAndMemoryMonitor2() {
	m_btn_clip.EnableWindow(FALSE);
	m_stc_drawing.SetWindowText(L"裁剪中...");
	m_stc_info1.ShowWindow(SW_HIDE);
	m_stc_info2.ShowWindow(SW_HIDE);
	HANDLE handle = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
	startMemory = pmc.PagefileUsage;
	maxMemory = 0;
	CDemo_ClipView_VCDlg::m_bStopTimer = false;
	AfxBeginThread(ThreadProc, NULL);
	GetThreadIdList(beginTIdList);
	startTime = clock();
}
void CDemo_ClipView_VCDlg::EndTimeAndMemoryMonitor() {
	endTime = clock();
	HANDLE handle = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
	endMemory = pmc.PagefileUsage;
	double useTimeS = (endTime - startTime) / 1000;
	double useMemoryM = (endMemory - startMemory) / 1024 / 1024;
	m_stc_drawing.SetWindowText(L"裁剪完毕！");
	m_stc_info1.ShowWindow(SW_SHOW);
	CString strTime;
	strTime.Format(L"耗时:  %lfs", useTimeS);
	m_stc_info1.SetWindowText(strTime);
	m_stc_info2.ShowWindow(SW_SHOW);
	CString strMemory;
	strMemory.Format(L"占用内存:  %lfM", useMemoryM);
	m_stc_info2.SetWindowText(strMemory);
}

void CDemo_ClipView_VCDlg::EndTimeAndMemoryMonitor2() {
	endTime = clock();
	m_bStopTimer = true;
	double useTimeS = (endTime - startTime) / 1000;
	double useMemoryM = (maxMemory - startMemory) / 1024 / 1024;
	if (useMemoryM < 0) {
		useMemoryM = 0.0;
	}
	m_stc_drawing.SetWindowText(L"裁剪完毕！");
	m_stc_info1.ShowWindow(SW_SHOW);
	CString str1;
	str1.Format(L"共%d个图形，%d个边界，有%d个图形与边界相交（%d个交点），%d个无交点，其中%d个在界内，%d个在界外",
		clipTask.shapeCount, clipTask.boundaryCount, clipTask.crossCount, clipTask.crossPointCount, clipTask.noCrossCount, clipTask.insideCount, clipTask.outsiedCount);
	m_stc_info1.SetWindowText(str1);
	m_stc_info2.ShowWindow(SW_SHOW);
	CString str2;
	str2.Format(L"耗时:  %lfs   占用内存:  %lfM", useTimeS, useMemoryM);
	m_stc_info2.SetWindowText(str2);

}

//绘制图形
void CDemo_ClipView_VCDlg::DrawTestCase(CString xmlPath, CString caseID) {
	m_stc_drawing.ShowWindow(SW_SHOW);
	m_stc_drawing.SetWindowText(L"图形绘制中...");
	m_stc_info1.ShowWindow(SW_HIDE);
	m_stc_info2.ShowWindow(SW_HIDE);
	m_btn_clip.EnableWindow(FALSE);
	ClearTestCaseData();
	if (!LoadTestCaseData(xmlPath, caseID)) {
		MessageBox(L"读取数据失败!", L"Demo_CliView_VC", MB_OK);
		m_stc_drawing.SetWindowText(L"");
		return;
	}

	MemoryBitmap mbitmap(800, 600);
	vector<Line>::iterator iterLine = clipTask.lines.begin();
	for (; iterLine != clipTask.lines.end(); iterLine++) {
		Drawer::DrawLine3(mbitmap,
			std::uint32_t(iterLine->startpoint.x), std::uint32_t(iterLine->startpoint.y),
			std::uint32_t(iterLine->endpoint.x), std::uint32_t(iterLine->endpoint.y),
			MemoryBitmap::kGreenColor);
	}
	vector<Circle>::iterator iterCircle = clipTask.circles.begin();
	for (; iterCircle != clipTask.circles.end(); iterCircle++) {
		Drawer::DrawCircle1(mbitmap,
			std::uint32_t(iterCircle->center.x), std::uint32_t(iterCircle->center.y), std::uint32_t(iterCircle->radius),
			MemoryBitmap::kBlueColor);
	}

#ifndef SINGLE_BOUNDARIES
	for (size_t boundary_index = 0; boundary_index < clipTask.boundaries.size(); boundary_index++) {
		auto & boundary = clipTask.boundaries[boundary_index];
		vector<Point>::iterator iter = boundary.vertexs.begin();
		if (iter == boundary.vertexs.end()) 
			continue;
		Drawer::DrawLine3(mbitmap,
			std::uint32_t(boundary.vertexs[0].x), std::uint32_t(boundary.vertexs[0].y),
			std::uint32_t(boundary.vertexs[boundary.vertexs.size() - 1].x), std::uint32_t(boundary.vertexs[boundary.vertexs.size() - 1].y),
			MemoryBitmap::kRedColor);
		for (std::size_t i = 0; i < boundary.vertexs.size() - 1;++i) {
			Drawer::DrawLine3(mbitmap,
				std::uint32_t(boundary.vertexs[i].x), std::uint32_t(boundary.vertexs[i].y),
				std::uint32_t(boundary.vertexs[i + 1].x), std::uint32_t(boundary.vertexs[i + 1].y),
				MemoryBitmap::kRedColor);
		}
	}
#else
	DrawBoundaryInMem(boundary, clrBoundary, memDc_main);
#endif

	//scarlet::io::ioutil::writeFile(L"genBmp02.bmp", (char*)mbitmap.Data(), mbitmap.Size());

	HDC hdc = cacheDC->GetSafeHdc();
	auto bitmapInfoHeader = mbitmap.BitmapInfoHeader();
	auto err = SetDIBitsToDevice(hdc, 0, 0, 800, 600, 0, 0, 0, 600, mbitmap.Data(), (BITMAPINFO*)&bitmapInfoHeader, DIB_RGB_COLORS);

	if (hasOutCanvasData) {
		m_stc_drawing.SetWindowText(L"存在超出边界数据！！！");
	} else {
		m_btn_clip.EnableWindow(TRUE);
		m_stc_drawing.SetWindowText(L"图形绘制完成：");
		m_stc_info1.ShowWindow(SW_SHOW);
		CString strInfo1;
		strInfo1.Format(L"共有%s个图形和%s个边界，其中%s个图形与边界相交。", complexArray.GetAt(0), complexArray.GetAt(1), complexArray.GetAt(2));
		m_stc_info1.SetWindowText(strInfo1);
		m_stc_info2.ShowWindow(SW_SHOW);
		CString strInfo2;
		strInfo2.Format(L"共有%s个图形与边界无交点，其中%s个位于边界内，%s个位于边界外。", complexArray.GetAt(3), complexArray.GetAt(4), complexArray.GetAt(5));
		m_stc_info2.SetWindowText(strInfo2);
	}
}

/*
void CDemo_ClipView_VCDlg::DrawLineInMem(Line line, CDC* memDc) {
	memDc->MoveTo(line.startpoint.ToCPoint());
	memDc->LineTo(line.endpoint.ToCPoint());
}

void CDemo_ClipView_VCDlg::DrawCircleInMem(Circle circle, CDC* memDc) {
	memDc->Arc((int)(circle.center.x - circle.radius), (int)(circle.center.y - circle.radius), (int)(circle.center.x + circle.radius), (int)(circle.center.y + circle.radius), 0, 0, 0, 0);
}

void CDemo_ClipView_VCDlg::DrawBoundaryInMem(Boundary boundary, COLORREF clr, CDC* memDc) {
	CPen penUse;
	penUse.CreatePen(PS_SOLID, 1, clr);
	CPen* penOld = memDc->SelectObject(&penUse);

	vector<Point>::iterator iter = boundary.vertexs.begin();
	if (iter == boundary.vertexs.end()) return;
	Point startPoint = *iter;
	memDc->MoveTo(startPoint.ToCPoint());
	for (; iter != boundary.vertexs.end(); iter++) {
		memDc->LineTo((*iter).ToCPoint());
	}
	memDc->LineTo(startPoint.ToCPoint());

	memDc->SelectObject(penOld);
}
*/

int CDemo_ClipView_VCDlg::SplitCStringToArray(CString str, CStringArray& strArray, CString split) {
	int curPos = 0;
	while (TRUE) {
		CString resToken = str.Tokenize(split, curPos);
		if (resToken.IsEmpty())
			break;
		strArray.Add(resToken);
	}
	return strArray.GetSize();
}

BOOL CDemo_ClipView_VCDlg::GetThreadIdList(CList<int>& tIdList) {
	int pID = _getpid();
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pID);
	if (hThreadSnap == INVALID_HANDLE_VALUE) {
		return FALSE;
	}
	te32.dwSize = sizeof(THREADENTRY32);
	BOOL bGetThread = Thread32First(hThreadSnap, &te32);
	while (bGetThread) {
		if (te32.th32OwnerProcessID == pID) {
			tIdList.AddTail(te32.th32ThreadID);
		}
		bGetThread = Thread32Next(hThreadSnap, &te32);
	}
	CloseHandle(hThreadSnap);
	return TRUE;
}