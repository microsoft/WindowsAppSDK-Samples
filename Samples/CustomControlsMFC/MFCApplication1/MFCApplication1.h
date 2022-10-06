
// MFCApplication1.h : main header file for the MFCApplication1 application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


//#include <winrt/Windows.Foundation.Collections.h>
//#include <winrt/Windows.system.h>
//#include <winrt/windows.ui.xaml.hosting.h>
//#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>
//#include <winrt/windows.ui.xaml.controls.h>
//#include <winrt/Windows.ui.xaml.media.h>
//#include <winrt/Windows.UI.Core.h>
//#include <winrt/MyUWPApp.h>
//
//using namespace winrt;
//using namespace Windows::UI;
//using namespace Windows::UI::Composition;
//using namespace Windows::UI::Xaml::Hosting;
//using namespace Windows::Foundation::Numerics;
//using namespace Windows::UI::Xaml::Controls

// CMFCApplication1App:
// See MFCApplication1.cpp for the implementation of this class
//

class CMFCApplication1App : public CWinAppEx
{
public:
	CMFCApplication1App() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMFCApplication1App theApp;
