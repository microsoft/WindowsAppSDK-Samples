
// MFCdotNETHostView.cpp : implementation of the CMFCdotNETHostView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MFCdotNETHost.h"
#endif

#include "MFCdotNETHostDoc.h"
#include "MFCdotNETHostView.h"

#undef GetCurrentTime

//#include "..\WinUIClassLibraryExp\TestUserControl.xaml.h"

using namespace winrt;
//using namespace Windows::UI;
//using namespace Windows::UI::Composition;
//Microsoft.UI.Interop.h
using namespace Windows::UI::Xaml::Hosting;
//using namespace Windows::Foundation::Numerics;
//using namespace Windows::UI::Xaml::Controls;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//winrt::Microsoft::UI::Xaml::Hosting::DesktopWindowXamlSource _desktopWindowXamlSource{ nullptr };
winrt::Windows::UI::Xaml::Hosting::DesktopWindowXamlSource _desktopWindowXamlSource{ nullptr };
// CMFCdotNETHostView

IMPLEMENT_DYNCREATE(CMFCdotNETHostView, CView)

BEGIN_MESSAGE_MAP(CMFCdotNETHostView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMFCdotNETHostView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CMFCdotNETHostView construction/destruction

CMFCdotNETHostView::CMFCdotNETHostView() noexcept
{
	// TODO: add construction code here

}

CMFCdotNETHostView::~CMFCdotNETHostView()
{
}

BOOL CMFCdotNETHostView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CMFCdotNETHostView drawing
HWND xamlHostHwnd = NULL;
void CMFCdotNETHostView::OnDraw(CDC* /*pDC*/)
{
	CMFCdotNETHostDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

  //TODO
  if (_desktopWindowXamlSource == nullptr)
  {
    _desktopWindowXamlSource = DesktopWindowXamlSource{};
    auto interop = _desktopWindowXamlSource.as<IDesktopWindowXamlSourceNative>();
    check_hresult(interop->AttachToWindow(GetSafeHwnd()));

    check_hresult(interop->get_WindowHandle(&xamlHostHwnd));

    RECT windowRect;
    GetWindowRect(&windowRect);
    ::SetWindowPos(xamlHostHwnd, NULL, 200, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_SHOWWINDOW);

    CreateOFC();
  }
	// TODO: add draw code for native data here
}

void CMFCdotNETHostView::CreateOFC()
{
  auto control = winrt::WinUIClassLibraryExp::TestUserControl();
  control;
  _desktopWindowXamlSource.Content(control);
  
}


// CMFCdotNETHostView printing


void CMFCdotNETHostView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMFCdotNETHostView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMFCdotNETHostView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMFCdotNETHostView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CMFCdotNETHostView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMFCdotNETHostView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMFCdotNETHostView diagnostics

#ifdef _DEBUG
void CMFCdotNETHostView::AssertValid() const
{
	CView::AssertValid();
}

void CMFCdotNETHostView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCdotNETHostDoc* CMFCdotNETHostView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCdotNETHostDoc)));
	return (CMFCdotNETHostDoc*)m_pDocument;
}
#endif //_DEBUG


// CMFCdotNETHostView message handlers
