#pragma once

class CTMatView;
class TViewer : public CFrameWnd
{
	DECLARE_DYNCREATE(TViewer)
public:
	TViewer();
	TViewer(CWnd* pParent);
	virtual ~TViewer();

	void			DrawView(const cv::Mat& image);

	void			FitImage();

	cv::Mat			GetImage();
	cv::Point		GetImagePts() { return m_ptImage; }
	CPoint			GetViewPts() { return m_ptView; }
	double			GetZoomRate() { return m_dZoom; }


	CPoint			m_ptZoom;			// zoom 하는 중심(private)

private:
	cv::Point2d     ClientToImage(CPoint clientPt, CRect clientRect, cv::Mat image);
	//cv::Point       ViewToImage(CPoint pt);

	void            DisplayNavi(HDC& hdc, BITMAPINFO& bitmapInfo);
	void			CalcZoomRect(CPoint pt);

private:
	CTMatView*		m_pParent = nullptr;
	CRect			m_clientRect;
	cv::Mat			m_orgImage;

	CRect			m_rectDraw;			// 실제 그리고 있는 rect
	CRect           m_rectZoom;			
	CRect			m_rectBitmap;
	

	double			m_dZoom;


	CPoint          m_ptOffset;
	CPoint          m_ptView;
	cv::Point       m_ptImage;


protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnPaint();
};


