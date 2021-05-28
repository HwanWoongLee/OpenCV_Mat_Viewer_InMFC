
// MatViewDlg.h: 헤더 파일
//

#pragma once
#define IDC_MAT_VIEW		12345

// CMatViewDlg 대화 상자
class CTMatView;
class CMatViewDlg : public CDialogEx
{
// 생성입니다.
public:
	CMatViewDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MATVIEW_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


private:
	CTMatView m_view;

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	
};
