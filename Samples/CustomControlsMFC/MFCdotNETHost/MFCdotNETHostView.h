
// MFCdotNETHostView.h : interface of the CMFCdotNETHostView class
//

#pragma once
#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>


class CMFCdotNETHostView : public CView
{
  //TODO
  
  void CreateOFC();

protected: // create from serialization only
	CMFCdotNETHostView() noexcept;
	DECLARE_DYNCREATE(CMFCdotNETHostView)

// Attributes
public:
	CMFCdotNETHostDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CMFCdotNETHostView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in MFCdotNETHostView.cpp
inline CMFCdotNETHostDoc* CMFCdotNETHostView::GetDocument() const
   { return reinterpret_cast<CMFCdotNETHostDoc*>(m_pDocument); }
#endif

