## OpenCV_Mat_Viewer_InMFC
MFC Project to View OpenCV(C++) Mat.

## Version
+ OS : Windows10
+ VS : VS2019
+ OpenCV : 4.5.2

## Example
### Create Mat View
~~~
  // 중략...
  CTMatView view;
  if (view.Create(NULL, NULL, WS_VISIBLE | WS_CHILD | WS_BORDER, CRect(), this, IDC_MAT_VIEW)) {
    view.MoveWindow(rect);    // attach view window to rect
    view.ShowTool(true);      // toolbar show or not
  }
  
~~~

### Show Mat Image
~~~
  /// 중략...
  cv::Mat imgLoad = cv::imread(strPath);
  view.SetImage(image);
  
~~~
