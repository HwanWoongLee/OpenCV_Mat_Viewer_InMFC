#pragma once
#include "opencv2\opencv.hpp"

using namespace cv;
using namespace std;

#define T_CHECK_FOCUS	    3009
#define IDC_CHECK_BOX	    3010

#define COLOR_MENU          RGB(45, 45, 48)
#define COLOR_BACKGROUND    RGB(10, 10, 18)
#define MAX_ZOOM            10.0
#define MIN_ZOOM            1.0

class TViewer;

class CTMatView : public CWnd {
    DECLARE_DYNAMIC(CTMatView)

    enum eBTN_ID {
        eBTN_SAVE = 4009    ,
        eBTN_LOAD           ,
        eBTN_FIT            ,
    };

    enum eRECT {
        eRECT_MENU          ,
        eRECT_VIEW          ,
        eRECT_ZOOM_RATE     ,
        eRECT_COORD         ,
        eRECT_NUM           ,
    };

public:
    CTMatView();
    CTMatView(cv::Mat image);
    CTMatView(cv::Mat image, CRect rect);
    virtual ~CTMatView();

    void            InitMatView();
    void            MoveWindow(CRect rect);

    void            UpdateUI();
    bool            GetCheck() { return m_checkBox.GetCheck(); }

private:
    void            SetImage(cv::Mat image);
    void            SetRectArea(CRect rect);
    void            SetParentWnd();

    BOOL            LoadImageFile();
    BOOL            SaveImageFile();

    // UI
    void            CreateView();
    void            CreateMenu();
    void            CreateButton(CMFCButton& button, CRect rect, eBTN_ID btnID, LPCTSTR str = _T(""));

private:
    CWnd*           m_pwndParent = nullptr;
    TViewer*        m_pViewer = nullptr;

    CRect           m_rect[eRECT_NUM];

    bool            m_bLBDown = false;      // 마우스 왼클릭

    CBrush          m_brush;

    // Menu
    CMFCButton	    m_btnLoad;
    CMFCButton	    m_btnSave;
    CMFCButton	    m_btnFit;
    CButton         m_checkBox;




public:
    DECLARE_MESSAGE_MAP()
    afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void	OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void	OnPaint();
    afx_msg void	OnTimer(UINT_PTR nIDEvent);
    virtual BOOL	OnCommand(WPARAM wParam, LPARAM lParam);
    afx_msg BOOL    OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};