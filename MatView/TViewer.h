#pragma once

#define RATE_ZOOMING 0.25

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
	cv::Point2d		GetImagePts() { return m_ptImage; }
	CPoint			GetViewPts() { return m_ptView; }
	double			GetZoomRate() { return m_dZoom; }
	cv::Scalar		GetImageColor() { return m_imgColor; }

private:
	cv::Point2d     ClientToImage(CPoint clientPt, CRect clientRect, cv::Mat image);

	void            DisplayNavi(HDC& hdc, BITMAPINFO& bitmapInfo);
	void			CalcZoomRect(CPoint pt);

private:
	CTMatView*		m_pParent = nullptr;

	CRect			m_clientRect;
	CRect			m_rectDraw;			// 실제 그리고 있는 rect
	CRect           m_rectZoom;
	
	cv::Mat			m_orgImage;

	double			m_dZoom;

	bool			m_bLButton;

	CPoint			m_ptZoom;			// zoom 하는 중심(private)
	CPoint			m_ptLBStart;		// mouse clieck pos
	CPoint			m_ptOffset;			// mouse offset 
	CPoint			m_ptView;			// view point
	cv::Point2d     m_ptImage;			// image point

	cv::Scalar      m_imgColor;			// image color

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnPaint();
};


