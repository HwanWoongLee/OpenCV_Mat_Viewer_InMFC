
// MatViewDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "MatView.h"
#include "MatViewDlg.h"
#include "afxdialogex.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMatViewDlg 대화 상자



CMatViewDlg::CMatViewDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MATVIEW_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMatViewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMatViewDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	
END_MESSAGE_MAP()


// CMatViewDlg 메시지 처리기

BOOL CMatViewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	
	if (!m_view.Create(NULL, NULL, WS_BORDER | WS_VISIBLE | WS_CHILD, CRect(), this, IDC_MAT_VIEW)) {
		return FALSE;
	}
	CWnd* pWnd = GetDlgItem(IDC_STATIC_VIEWER);
	CRect rect;
	pWnd->GetWindowRect(&rect);
	ScreenToClient(rect);

	m_view.MoveWindow(rect);
	m_view.ShowTool(true);
	pWnd->DestroyWindow();

	cv::Mat image = cv::imread("D:/Project/MatView/pepe.png");
	m_view.SetImage(image);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMatViewDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMatViewDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
