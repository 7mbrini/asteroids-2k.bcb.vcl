/******************************************************************************
	author:	Francesco Settembrini
	last update: 23/6/2021
	e-mail:	mailto:francesco.settembrini@poliba.it
******************************************************************************/

#ifndef _SDLSYSTEM_H_
#define _SDLSYSTEM_H_

#include <windows.h>
#include <string>

#include "vectors.h"

using namespace maths;

class TVideoManager
{
	public:
        TVideoManager(HWND hWnd, RECT Rect);
        ~TVideoManager();

		HDC GetDC();
        HWND GetHWnd();
        RECT GetClientArea();

        TVector2 GetScreenCenter();
        void DrawPoint(TVector2& Pt, COLORREF Color);
        void ClearScreen(COLORREF Color);
        bool LoadFont(std::string strFontPath, std::wstring strName, int nSize);

        void DrawLines(TVecPoints& Pts, int nLineWidth,
        	COLORREF Color, bool bClosed=false);
        void DrawLines(TVecVecPoints& Pts, int nLineWidth,
        	COLORREF Color, bool bClosed=false);

        void DrawText(char* pText, int nX, int nY,
        	COLORREF nColor = RGB(255,255,255), UINT nAlign = TA_CENTER);
        void DrawText(std::vector<std::string> StringList, int nX, int nY,
            int nTextH, COLORREF nColor = RGB(255,255,255), UINT nAlign = TA_CENTER);

	protected:
        HWND m_hWnd;
        RECT m_ClientArea;
        HDC m_hDC;
        HBITMAP m_hBmp;
};

#endif


