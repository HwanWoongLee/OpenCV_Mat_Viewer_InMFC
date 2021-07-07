
#include "pch.h"
#include "MatView.h"
#include "TViewer.h"
#include "TMatView.h"

// TViewer

IMPLEMENT_DYNCREATE(TViewer, CFrameWnd)

TViewer::TViewer(CWnd* pParent)
{
    m_pParent = (CTMatView*)pParent;
}

TViewer::TViewer()
{
}
TViewer::~TViewer()
{
}

void TViewer::DrawView(const cv::Mat& image) {
    m_orgImage = image.clone();
    m_rectZoom = CRect();

	Invalidate(FALSE);
}

cv::Mat TViewer::GetImage() {
    return m_orgImage.clone();
}

void TViewer::FitImage() {
    m_dZoom = 1.0;
    m_ptOffset = CPoint(0, 0);
    m_rectZoom = m_rectDraw;

    Invalidate(FALSE);

    return;
}

cv::Point2d TViewer::ClientToImage(CPoint clientPt, CRect clientRect, cv::Mat image) {
    cv::Point2d ptImage(-1, -1);
    
    if (clientRect.IsRectEmpty() || image.empty())
        return ptImage;

    double dRateToImage_x = (double)image.cols / (double)clientRect.Width();
    double dRateToImage_y = (double)image.rows / (double)clientRect.Height();
    double dRateToImage = dRateToImage_x > dRateToImage_y ? dRateToImage_x : dRateToImage_y;

    clientPt -= clientRect.TopLeft();
    
    ptImage = cv::Point2d(clientPt.x * dRateToImage, clientPt.y * dRateToImage);
    return ptImage;
}


//cv::Point TViewer::ViewToImage(CPoint pt) {
//    cv::Point ptImage(0, 0);
//    if (!m_rectDraw.IsRectEmpty()) {
//        ptImage = cv::Point(pt.x - m_rectDraw.TopLeft().x, pt.y - m_rectDraw.TopLeft().y);
//        int x = (m_orgImage.cols * ptImage.x) / m_rectDraw.Width();
//        int y = (m_orgImage.rows * ptImage.y) / m_rectDraw.Height();
//
//        x /= m_dZoom;
//        y /= m_dZoom;
//
//        if (x < 0) x = 0;
//        else if (x > m_orgImage.cols) x = m_orgImage.cols;
//        
//        if (y < 0) y = 0;
//        else if (y > m_orgImage.rows) y = m_orgImage.rows;
//
//        ptImage.x = x;
//        ptImage.y = y;
//    }
//    return ptImage;
//}


void TViewer::CalcZoomRect(CPoint pt) {
    double dw = m_rectDraw.Width() / m_dZoom;
    double dh = m_rectDraw.Height() / m_dZoom;

    double dx = pt.x - dw / 2;
    double dy = pt.y - dh / 2;

    if (dx < m_rectDraw.left) dx = m_rectDraw.left; 
    else if (dx + dw > m_rectDraw.right) dx = m_rectDraw.right - dw;

    if (dy < m_rectDraw.top) dy = m_rectDraw.top; 
    else if (dy + dh > m_rectDraw.bottom) dy = m_rectDraw.bottom - dh;
    
    m_rectZoom = CRect(dx, dy, dx + dw, dy + dh);
}

BEGIN_MESSAGE_MAP(TViewer, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// TViewer 메시지 처리기


void TViewer::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CFrameWnd::OnLButtonDown(nFlags, point);
}


void TViewer::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CFrameWnd::OnLButtonUp(nFlags, point);
}


void TViewer::OnMouseMove(UINT nFlags, CPoint point)
{
    if (!m_orgImage.empty()) {
        m_ptView = point;
        
        CPoint pt = m_ptView - m_rectDraw.TopLeft();
        m_ptZoom = CPoint(pt.x / m_dZoom, pt.y / m_dZoom);
        m_ptZoom += m_ptOffset;
        m_ptZoom += m_rectZoom.TopLeft();

        m_ptImage = ClientToImage(m_ptZoom, m_rectDraw, m_orgImage);

        Invalidate(FALSE);
        m_pParent->UpdateUI();
    }
	CFrameWnd::OnMouseMove(nFlags, point);
}


BOOL TViewer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
    if (!m_orgImage.empty()) {
        CPoint ptClient = pt;
        ScreenToClient(&ptClient);
    
        if (PtInRect(m_clientRect, ptClient)) {
            if (zDelta > 0) {           // zoom in
                if (MAX_ZOOM > m_dZoom)
                    m_dZoom += m_dZoom * 0.26;
                else
                    m_dZoom = MAX_ZOOM;
            }   
            else {                      // zoom out
                if (MIN_ZOOM < m_dZoom)
                    m_dZoom -= m_dZoom * 0.26;
                else {
                    m_dZoom = MIN_ZOOM;
                }
            }
    
            CalcZoomRect(m_ptZoom);
            Invalidate(FALSE);
        }
    }
	return CFrameWnd::OnMouseWheel(nFlags, zDelta, pt);
}


void TViewer::OnPaint()
{
	CPaintDC dc(this);
	//CRect rectClient;
	GetClientRect(m_clientRect);

	CMemDC		memDC(dc, this);
	CDC& pDC = memDC.GetDC();

	// background
	pDC.FillSolidRect(m_clientRect, COLOR_BACKGROUND);

    // draw
	if (!m_orgImage.empty()) {
        // 그릴 이미지 준비
        cv::Mat drawImage = m_orgImage.clone();

        cv::resize(drawImage, drawImage, cv::Size(GDI_WIDTHBYTES(drawImage.cols * 8), drawImage.rows));     // 영상 가로길이는 4바이트의 배수여야한다...

        BITMAPINFO bitmapInfo;
        bitmapInfo.bmiHeader.biYPelsPerMeter    = 0;
        bitmapInfo.bmiHeader.biXPelsPerMeter    = 0;
        bitmapInfo.bmiHeader.biBitCount         = 24;
        bitmapInfo.bmiHeader.biWidth            = drawImage.cols;
        bitmapInfo.bmiHeader.biHeight           = drawImage.rows;
        bitmapInfo.bmiHeader.biPlanes           = 1;
        bitmapInfo.bmiHeader.biSize             = sizeof(BITMAPINFOHEADER);
        bitmapInfo.bmiHeader.biCompression      = BI_RGB;
        bitmapInfo.bmiHeader.biClrImportant     = 0;
        bitmapInfo.bmiHeader.biSizeImage        = 0;

        if (drawImage.channels() == 3) {

        }
        else if (drawImage.channels() == 1) {
            cvtColor(drawImage, drawImage, cv::COLOR_GRAY2RGB);
        }
        else if (drawImage.channels() == 4) {
        }

        flip(drawImage, drawImage, 0);

        double image_w      = drawImage.cols;
        double image_h      = drawImage.rows;
        double rect_w       = m_clientRect.Width();
        double rect_h       = m_clientRect.Height();

        double dRectRate    = rect_w / rect_h;
        double dImageRate   = image_w / image_h;
        
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
        ::SetStretchBltMode(pDC.m_hDC, COLORONCOLOR);		//모드 설정(안해주면 영상 축소 시 깨짐 현상)

        // 검은색으로 초기화
        CBrush brush;
        brush.CreateSolidBrush(RGB(0, 0, 0));
        FillRect(pDC.m_hDC, CRect(0, 0, rect_w, rect_h), brush);

        m_rectDraw = CRect(dx, dy, dx + dw, dy + dh);

        if (m_rectZoom.IsRectEmpty())
            m_rectZoom = m_rectDraw;

        if (m_dZoom > 1.0) {
            double x = m_rectZoom.left;
            double y = m_rectZoom.top;
        
            dx = dx - ((x - dx) * m_dZoom);
            dy = dy - ((y - dy) * m_dZoom);
            dw = m_dZoom * dw;
            dh = m_dZoom * dh;
        }

        // offset
        dx -= m_ptOffset.x;
        dy -= m_ptOffset.y;

        // 실직적으로 그리는 부분
        ::StretchDIBits(pDC.m_hDC,			        //출력대상 핸들
            dx, dy, dw, dh,				            //출력대상 좌표
            0, 0, drawImage.cols, drawImage.rows,	//원본의 좌표
            drawImage.data,							//데이터 시작 주소
            &bitmapInfo,							//BITMAPINFO 구조체 시작 주소
            DIB_RGB_COLORS,
            SRCCOPY);

        m_rectBitmap = CRect(dx, dy, dw, dh);

        // Navigation
        if (m_pParent->GetCheck()) {
            DisplayNavi(pDC.m_hDC, bitmapInfo);
        }
	}


    //// test
    CString str;
    str.Format(_T("test [%d, %d]"), m_ptZoom.x, m_ptZoom.y);
    //TRACE(str);

    pDC.FillSolidRect(CRect(0, 0, 200, 25), RGB(255, 255, 255));
    pDC.DrawText(str, CRect(0, 0, 200, 25), DT_CENTER | DT_TABSTOP | DT_VCENTER | DT_SINGLELINE);

}


void TViewer::DisplayNavi(HDC& hdc, BITMAPINFO& bitmapInfo) {
    if (m_orgImage.empty())
        return;

    cv::Mat showNavImage = m_orgImage.clone();

    if (showNavImage.channels() == 3) {

    }
    else if (showNavImage.channels() == 1) {
        cvtColor(showNavImage, showNavImage, cv::COLOR_GRAY2RGB);
    }
    else if (showNavImage.channels() == 4) {

    }

    cv::Point lt = ClientToImage(m_rectZoom.TopLeft(),      m_rectDraw, m_orgImage);
    cv::Point br = ClientToImage(m_rectZoom.BottomRight(),  m_rectDraw, m_orgImage);

    cv::drawMarker(showNavImage, m_ptImage, cv::Scalar(0, 255, 0), 0, showNavImage.cols / 10, showNavImage.cols / 100);

    cv::rectangle(showNavImage, cv::Rect(lt.x, lt.y, br.x - lt.x, br.y - lt.y), cv::Scalar(0,255,0), showNavImage.cols * 0.01);
    cv::resize(showNavImage, showNavImage, cv::Size(GDI_WIDTHBYTES(showNavImage.cols * 8), showNavImage.rows));
    flip(showNavImage, showNavImage, 0);

    CRect rect;
    GetWindowRect(rect);
    ScreenToClient(rect);

    ::StretchDIBits(hdc,
        rect.right - m_rectDraw.Width() * 0.3,
        rect.bottom - m_rectDraw.Height() * 0.3,
        m_rectDraw.Width() * 0.3,
        m_rectDraw.Height() * 0.3,
        0, 0, showNavImage.cols, showNavImage.rows,
        showNavImage.data,
        &bitmapInfo,
        DIB_RGB_COLORS,
        SRCCOPY);

    return;
}