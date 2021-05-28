#include "pch.h"
#include "TMatView.h"


IMPLEMENT_DYNAMIC(CTMatView, CWnd)

BEGIN_MESSAGE_MAP(CTMatView, CWnd)
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_SHOWWINDOW()
    ON_WM_ACTIVATE()
    ON_WM_MOUSEMOVE()
    ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_TIMER()
    ON_WM_MOUSEWHEEL()
    ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


CTMatView::CTMatView() {
    InitMatView();
}

CTMatView::CTMatView(cv::Mat image) {
    InitMatView();
    SetImage(image);
}

CTMatView::CTMatView(cv::Mat image, CRect rect) {
    InitMatView();
    SetImage(image);
    SetRectArea(rect);
}

CTMatView::~CTMatView() {
    if (m_pwndParent)
        m_pwndParent = nullptr;
}

void CTMatView::InitMatView() {
    m_orgImage          = cv::Mat();
    m_showImage         = cv::Mat();
    m_navImage          = cv::Mat();

    for (int i = 0; i < eRECT_NUM; ++i) {
        m_rect[i]       = CRect();
    }

    m_rectImage         = CRect();
    m_rectZoom          = CRect();
    m_dZoom             = 1.0;
    m_bFocus            = false;
}

void CTMatView::DisplayImage(HDC& hdc, cv::Mat& image) {
    if (image.empty())
        return;

    // 영상 가로길이는 4바이트의 배수여야한다...
    cv::resize(image, image, cv::Size(GDI_WIDTHBYTES(image.cols * 8), image.rows));
    
    BITMAPINFO bitmapInfo;
    bitmapInfo.bmiHeader.biYPelsPerMeter    = 0;
    bitmapInfo.bmiHeader.biXPelsPerMeter    = 0;
    bitmapInfo.bmiHeader.biBitCount         = 24;
    bitmapInfo.bmiHeader.biWidth            = image.cols;
    bitmapInfo.bmiHeader.biHeight           = image.rows;
    bitmapInfo.bmiHeader.biPlanes           = 1;
    bitmapInfo.bmiHeader.biSize             = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biCompression      = BI_RGB;
    bitmapInfo.bmiHeader.biClrImportant     = 0;
    bitmapInfo.bmiHeader.biSizeImage        = 0;
    
    if (image.channels() == 3) {

    }
    else if (image.channels() == 1) {
        cvtColor(image, image, cv::COLOR_GRAY2RGB);
    }
    else if (image.channels() == 4) {
    }

    cv::Mat drawImage;
    flip(image, drawImage, 0);

    double image_w      = drawImage.cols;
    double image_h      = drawImage.rows;
    double rect_w       = m_rect[eRECT_PICTURE].Width();
    double rect_h       = m_rect[eRECT_PICTURE].Height();
    double dRectRate    = rect_w / rect_h;
    double dImageRate   = image_w / image_h;
    int iGap            = m_rect[eRECT_PICTURE].top - m_rect[eRECT_WND].top;

    // 비율에 맞춰서 크기 조정. 
    int dx = 0, dy = 0, dw = 0, dh = 0;  // 그리는 좌표
    if (dRectRate > dImageRate) {
        dh = rect_h;
        dw = dh * image_w / image_h;
        dx = (rect_w - dw) / 2;
        dy = 0;
    }
    else {
        dw = rect_w;
        dh = dw * image_h / image_w;
        dy = (rect_h - dh) / 2;
        dx = 0;
    }

    // 이미지 출력
    ::SetStretchBltMode(hdc, COLORONCOLOR);		//모드 설정(안해주면 영상 축소 시 깨짐 현상)

    // 검은색으로 초기화
    CBrush brush;
    brush.CreateSolidBrush(RGB(0, 0, 0));
    FillRect(hdc, CRect(0, iGap, rect_w, rect_h + iGap), brush);

    dy += iGap;
    m_rectImage = CRect(dx, dy, dx + dw, dy + dh);

    if (m_rectZoom.IsRectEmpty())
        m_rectZoom = m_rectImage;

    if (m_dZoom > 1.0) {
        double x = m_rectZoom.left;
        double y = m_rectZoom.top;
    
        dx = dx - ((x - dx) * m_dZoom);
        dy = dy - ((y - dy) * m_dZoom);
        dw = m_dZoom * dw;
        dh = m_dZoom * dh;
    }

    // 실직적으로 그리는 부분
    ::StretchDIBits(hdc,			            //출력대상 핸들
        dx, dy, dw, dh,				            //출력대상 좌표
        0, 0, drawImage.cols, drawImage.rows,	//원본의 좌표
        drawImage.data,							//데이터 시작 주소
        &bitmapInfo,							//BITMAPINFO 구조체 시작 주소
        DIB_RGB_COLORS,
        SRCCOPY);

    // Navigation
    DisplayNavImage(hdc, bitmapInfo);

    return;
}


void CTMatView::DisplayNavImage(HDC& hdc, BITMAPINFO& bitmapInfo) {
    if (m_navImage.empty())
        return;

    cv::Mat showNavImage = m_navImage.clone();

    cv::Point lt = ClientToImage(m_rectZoom.TopLeft(), m_rectImage, m_orgImage);
    cv::Point br = ClientToImage(m_rectZoom.BottomRight(), m_rectImage, m_orgImage);

    cv::rectangle(showNavImage, cv::Rect(lt.x, lt.y, br.x - lt.x, br.y - lt.y), cv::Scalar(0, 0, 255), showNavImage.cols * 0.01);
    cv::resize(showNavImage, showNavImage, cv::Size(GDI_WIDTHBYTES(showNavImage.cols * 8), showNavImage.rows));
    flip(showNavImage, showNavImage, 0);

    CRect rect;
    GetWindowRect(rect);
    ScreenToClient(rect);

    ::StretchDIBits(hdc,
        rect.right - m_rectImage.Width() * 0.3,
        rect.bottom - m_rectImage.Height() * 0.3,
        m_rectImage.Width() * 0.3, 
        m_rectImage.Height() * 0.3,
        0, 0, showNavImage.cols, showNavImage.rows,
        showNavImage.data,
        &bitmapInfo,
        DIB_RGB_COLORS,
        SRCCOPY);

    return;
}

void CTMatView::SetImage(cv::Mat image) {
    if (!image.empty()) {
        m_orgImage = image.clone();
        m_navImage = m_orgImage.clone();
        if (m_navImage.channels() == 1)
            cv::cvtColor(m_navImage, m_navImage, cv::COLOR_GRAY2BGR);
    }
}

void CTMatView::MoveWindow(CRect rect) {
    SetParentWnd();
    SetRectArea(rect);

    CreateMenu();
}
void CTMatView::SetParentWnd() {
    this->m_pwndParent = this->GetParent();
}

void CTMatView::SetRectArea(CRect rect) {
    //Set RectArea
    m_rect[eRECT_WND]       = rect;
    m_rect[eRECT_PICTURE]   = CRect(rect.left, rect.top + 25,   rect.right, rect.bottom);
    m_rect[eRECT_MENU]      = CRect(rect.left, rect.top,        rect.right, rect.top + 25);
    
    m_rect[eRECT_PALETTE]   = CRect(0, 25, m_rect[eRECT_PICTURE].Width(), m_rect[eRECT_PICTURE].Height() + 25);

    //Set window size    
    SetWindowPos(NULL,
        rect.left, rect.top,
        rect.Width(), rect.Height(),
        SWP_NOREPOSITION);
}

void CTMatView::CreateMenu() {
    CRect rect = m_rect[eRECT_MENU];

    double dh = rect.Height();
    double dw = rect.Width() / 30;

    CRect rectLoad          = CRect(dw * 0, 0, dw * 2, dh);
    CRect rectSave          = CRect(dw * 2, 0, dw * 4, dh);
    m_rect[eRECT_ZOOM]      = CRect(dw * 5, 0, dw * 8, dh);
    m_rect[eRECT_COORD]     = CRect(dw * 8, 0, dw * 30, dh);

    CreateButton(m_btnLoad, rectLoad, eBTN_LOAD, _T("Load"));
    CreateButton(m_btnSave, rectSave, eBTN_SAVE, _T("Save"));

    return;
}

void CTMatView::CreateButton(CMFCButton& button, CRect rect, eBTN_ID btnID, LPCTSTR str) {
    button.Create(NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        rect, this, btnID);

    button.m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
    button.SetWindowTextW(str);

    //버튼 문자열이 없으면 이미지.
    if (str == _T("")) {
        CImage btnImg;
        CString cstrIconPath;
        HRESULT hres;

        switch (btnID) {
            case eBTN_LOAD:
                cstrIconPath = _T("..\\icon\\open.ico");
                break;
            case eBTN_SAVE:
                cstrIconPath = _T("..\\icon\\save.ico");
                break;
        }
        //CImage로 불러옴.
        hres = btnImg.Load(cstrIconPath);
        if (hres != S_OK)
            return;

        //CImage -> BITMAP
        HBITMAP hbit;
        hbit = btnImg.Detach();
        BITMAP bit;
        ::GetObject(hbit, sizeof(BITMAP), &bit);

        //BITMAP -> Mat(크기변경을 위해...)
        Mat tempMat;
        tempMat.create(bit.bmWidth, bit.bmHeight, CV_8UC4);
        memcpy(tempMat.data, bit.bmBits, tempMat.cols * tempMat.rows * tempMat.channels());
        flip(tempMat, tempMat, 0);
        resize(tempMat, tempMat, cv::Size(rect.Width(), rect.Height()));

        //Mat-> BITMAP

        button.SetImage(hbit);
    }
}

BOOL CTMatView::LoadImageFile() {
    CString szFilter = _T("Image (*.BMP, *.PNG, *.JPG) | *.BMP;*.PNG;*.JPG;*.jpg | All Files(*.*)|*.*||");
    CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);

    Mat loadImg;

    if (IDOK == dlg.DoModal()) {
        CString cstrPath = dlg.GetPathName();
        CT2CA pszConvertedAnsiString(cstrPath);
        string strPath(pszConvertedAnsiString);

        loadImg = imread(strPath, IMREAD_UNCHANGED);
        m_rectZoom = CRect();
    }

    if (loadImg.empty()) {
        return FALSE;
    }
    else {
        SetImage(loadImg);
        //loadImg.copyTo(m_orgImage);
    }

    m_showImage = m_orgImage.clone();
    m_dZoom = 1.0;
    Invalidate(FALSE);

    return TRUE;
}

BOOL CTMatView::SaveImageFile() {
    if (m_orgImage.empty()) {
        MessageBox(_T("Not have image"));
        return FALSE;
    }

    CString szFilter = _T("Image (*.BMP, *.PNG, *.JPG) | *.BMP;*.PNG;*.JPG;*.jpg | All Files(*.*)|*.*||");
    CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, szFilter);

    if (IDOK == dlg.DoModal()) {
        CString cstrPath = dlg.GetPathName();
        if (-1 == cstrPath.ReverseFind('.')) {
            cstrPath += ".jpg";
        }
        CT2CA pszConvertedAnsiString(cstrPath);
        string strPath(pszConvertedAnsiString);

        imwrite(strPath, m_orgImage);
    }

    return TRUE;
}

cv::Point2d CTMatView::ClientToImage(CPoint clientPt, CRect clientRect, cv::Mat image) {
    cv::Point2d ptImage(-1, -1);
    
    if (clientRect.IsRectEmpty() || image.empty())
        return ptImage;

    double dRateToImage = (double)image.cols / (double)clientRect.Width();
    clientPt -= clientRect.TopLeft();
    
    ptImage = cv::Point2d(clientPt.x * dRateToImage, clientPt.y * dRateToImage);
    // cv::drawMarker(m_orgImage, ptImage, cv::Scalar(0, 255, 0), 0, 500, 5);
    return ptImage;
}

void CTMatView::SetZoomRect(double dZoom, bool bZoomin) {
    double dw, dh, dx, dy;

    if (bZoomin) {
        dw = m_rectZoom.Width();
        dh = m_rectZoom.Height();
        dx = m_rectZoom.left;
        dy = m_rectZoom.top;
    }
    else {
        dw = m_rectImage.Width();
        dh = m_rectImage.Height();
        dx = m_rectImage.left;
        dy = m_rectImage.top;
    }
    double zw, zh, zx, zy;

    zw = m_rectImage.Width() / dZoom;
    zh = m_rectImage.Height() / dZoom;

    if (dZoom > 1.0) {
        zx = m_ptView.x - (zw / 2);
        zy = m_ptView.y - (zh / 2);
    }
    else {
        zx = (dx + (dw / 2)) - (zw / 2);
        zy = (dy + (dh / 2)) - (zh / 2);
    }

    if (dZoom > 1.0) {
        zx = zx < dx ? dx : zx;
        zy = zy < dy ? dy : zy;
        zx = zx + zw > dx + dw ? dx + dw - zw : zx;
        zy = zy + zh > dy + dh ? dy + dh - zh : zy;
    }

    m_rectZoom = CRect(zx, zy, zx + zw, zy + zh);

    InvalidateRect(m_rect[eRECT_PALETTE], FALSE);
    
    return;
}


/////////////////////////////////////////////   Message


int CTMatView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    SetTimer(T_CHECK_FOCUS, 100, nullptr);
    
    return 0;
}

void CTMatView::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CWnd::OnShowWindow(bShow, nStatus);

    if (bShow) {
        SetTimer(T_CHECK_FOCUS, 500, nullptr);
        Invalidate(FALSE);
    }
}

void CTMatView::OnLButtonDown(UINT nFlags, CPoint point)
{

    CWnd::OnLButtonDown(nFlags, point);
}

void CTMatView::OnLButtonUp(UINT nFlags, CPoint point)
{

    CWnd::OnLButtonUp(nFlags, point);
}

void CTMatView::OnPaint()
{
    CPaintDC dc(this);
    CRect rectClient;
    GetClientRect(rectClient);

    CMemDC		memDC(dc, this);
    CDC&        pDC = memDC.GetDC();

    // background
    CRect rect = CRect(0, 0, m_rect[eRECT_WND].Width(), m_rect[eRECT_WND].Height());;
    pDC.FillSolidRect(rect, COLOR_BACKGROUND);

    // display
    DisplayImage(pDC.m_hDC, m_showImage);

    // menu
    rect = CRect(0, 0, m_rect[eRECT_MENU].Width(), m_rect[eRECT_MENU].Height());
    pDC.FillSolidRect(rect, COLOR_MENU);
    
    // zoom
    CString str;
    str.Format(_T("%.1lf"), m_dZoom);
    pDC.Rectangle(m_rect[eRECT_ZOOM]);
    pDC.SetTextColor(RGB(0, 0, 0));
    pDC.SetBkColor(RGB(255, 255, 255));
    pDC.DrawText(str, m_rect[eRECT_ZOOM], DT_CENTER | DT_TABSTOP | DT_VCENTER | DT_SINGLELINE);
    
    //// test zoom rect
    //CPen myPen, * pOldPen;
    //CBrush clsBrush, *pclsBrush;
    //myPen.CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
    //pOldPen = pDC.SelectObject(&myPen);
    //clsBrush.CreateStockObject(NULL_BRUSH);
    //pclsBrush = pDC.SelectObject(&clsBrush);
    ////pDC.Rectangle(m_rectImage);
    //pDC.Rectangle(m_rectZoom);
    //pDC.SelectObject(pOldPen);
    //pDC.SelectObject(pclsBrush);

    // Coordtrans
    str.Format(_T("Image [%.1lf, %.1lf] / View [%d, %d]"), m_ptImage.x, m_ptImage.y, m_ptView.x, m_ptView.y);
    pDC.Rectangle(m_rect[eRECT_COORD]);
    pDC.SetTextColor(RGB(0, 0, 0));
    pDC.SetBkColor(RGB(255, 255, 255));
    pDC.DrawText(str, m_rect[eRECT_COORD], DT_CENTER | DT_TABSTOP | DT_VCENTER | DT_SINGLELINE);
    
    //str.Format(_T("Rect : %d %d %d %d"), m_rectZoom.left, m_rectZoom.top, m_rectZoom.Width(), m_rectZoom.Height());
    //pDC.TextOutW(0, 100, str);

    return;
}

void CTMatView::OnTimer(UINT_PTR nIDEvent)
{
    switch (nIDEvent) {
        case T_CHECK_FOCUS: {
            Invalidate(FALSE);
            KillTimer(T_CHECK_FOCUS);
            break;
        }
    }

    CWnd::OnTimer(nIDEvent);
}

BOOL CTMatView::OnCommand(WPARAM wParam, LPARAM lParam)
{
    switch (wParam) {
        case eBTN_SAVE: {
            SaveImageFile();
            break;
        }
        case eBTN_LOAD: {
            LoadImageFile();
            break;
        }
    }
    return CWnd::OnCommand(wParam, lParam);
}

BOOL CTMatView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    if (!m_orgImage.empty()) {
        CPoint ptClient = pt;
        ScreenToClient(&ptClient);
        m_ptView = ptClient;

        if (PtInRect(m_rect[eRECT_PALETTE], ptClient)) {
            if (zDelta > 0) {
                if (MAX_ZOOM > m_dZoom) {
                    m_dZoom += 0.1;
                }
                else {
                    m_dZoom = MAX_ZOOM;
                }
                SetZoomRect(m_dZoom, true);
            }
            else {
                if (MIN_ZOOM < m_dZoom) {
                    m_dZoom -= 0.1;
                }
                else {
                    m_dZoom = MIN_ZOOM;
                }
                SetZoomRect(m_dZoom, false);
            }

            Invalidate(FALSE);
        }
    }
  
    return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CTMatView::OnMouseMove(UINT nFlags, CPoint point)
{
    if (!m_orgImage.empty()) {
        //ScreenToClient(&point);

        if (PtInRect(m_rectImage, point)) {
            m_ptView = point;
            m_ptImage = ClientToImage(point, m_rectImage, m_orgImage);
            //m_ptImageToCenter = m_ptImage - cv::Point2d(m_orgImage.cols / 2, m_orgImage.rows / 2);
            InvalidateRect(m_rect[eRECT_COORD], FALSE);
        }
    }

    CWnd::OnMouseMove(nFlags, point);
}
