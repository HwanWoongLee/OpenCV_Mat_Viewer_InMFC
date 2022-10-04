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
	cv::Point		GetImagePts()		{ return m_ptImage;			}
	CPoint			GetViewPts()		{ return m_ptView;			}
	double			GetZoomRate()		{ return m_dZoom;			}
	cv::Scalar		GetImageColor()		{ return m_imgColor;		}
	cv::Rect		GetSelectedRect()	{ return m_rectSelected;	}

private:
	cv::Point		ClientToImage(CPoint clientPt, CRect clientRect, cv::Mat image);

	void            DisplayNavi(HDC& hdc, BITMAPINFO& bitmapInfo);
	void			CalcZoomRect(CPoint pt);
	void			Zooming(short zDelta);
	
	void			SetDrawImage(const cv::Mat& image);
	BITMAPINFO		m_bitmapInfo;

private:
	CTMatView*		m_pParent = nullptr;

	CRect			m_clientRect;
	CRect			m_rectView;			// fit between client(view) and image
	CRect           m_rectZoom;			// zoom area
	
	cv::Mat			m_orgImage;
	cv::Mat			m_drawImage;
	cv::Mat			m_showNavImage;

	double			m_dZoom;

	bool			m_bLButton;
	bool			m_bRButton;

	CPoint			m_ptZoom;			// zoom 하는 중심
	CPoint			m_ptLBStart;		// mouse clieck pos		
	CPoint			m_ptOffset;			// mouse offset 
	CPoint			m_ptView;			// view point
	cv::Point		m_ptImage;			// image point

	cv::Scalar      m_imgColor;			// current mouse point image color

	cv::Rect		m_rectSelected;
	cv::Point		m_ptRStart	= cv::Point(-1, -1);
	cv::Point		m_ptREnd	= cv::Point(-1, -1);

	// add critical section
	static CCriticalSection m_cs;

	//// marker
	//cv::Point		m_ptMark[3];
	//int				m_iMark = 0;


protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};


