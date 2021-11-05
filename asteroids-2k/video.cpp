/*!****************************************************************************

	@file	video.h
	@file	video.cpp

	@brief	Video manager

	@noop	author:	Francesco Settembrini
	@noop	last update: 23/6/2021
	@noop	e-mail:	mailto:francesco.settembrini@poliba.it

******************************************************************************/

#include <vcl.h>
#include <assert.h>
#include <system.hpp>
#include <string>
#pragma hdrstop

#include "video.h"


/*!****************************************************************************
* @brief	Initialize the video system
* @param	hWnd Handle to the game main window
* @param	Rect Size of the game client area
* @return	Returns true for success, false otherwise
******************************************************************************/
TVideoManager::TVideoManager(HWND hWnd, RECT Rect)
{
	assert(hWnd);
	assert(Rect.right > 0);
	assert(Rect.bottom > 0);

	m_hWnd = hWnd;
	m_ClientArea = Rect;

	HDC hDC = ::GetDC(hWnd);
	if( !hDC ) throw;

	m_hDC = ::CreateCompatibleDC(hDC);
	if( !m_hDC ) throw;

	m_hBmp = ::CreateCompatibleBitmap(hDC,
		m_ClientArea.right, m_ClientArea.bottom);
	if( !m_hBmp ) throw;

	::SelectObject(m_hDC, m_hBmp);
	::SetBkMode(m_hDC, TRANSPARENT);

	::ReleaseDC(hWnd, hDC);
}

/*!****************************************************************************
* @brief	Class destructor
******************************************************************************/
TVideoManager::~TVideoManager()
{
	assert(m_hDC);
	assert(m_hBmp);

	::DeleteDC(m_hDC);
	::DeleteObject(m_hBmp);
}

/*!****************************************************************************
* @brief	Gets the coordinates of the screen center
* @return	Returns the screen center coordinates
******************************************************************************/
TVector2 TVideoManager::GetScreenCenter()
{
	TVector2 RetVal(m_ClientArea.right/2.0, m_ClientArea.bottom/2.0);

    return RetVal;
}

/*!****************************************************************************
* @brief	Draw a polyline
* @param	Pts Reference to a vector of points
* @param	nLineWidth Specifies the width of the polyline
* @param	Color Specifies the color of the polyline
* @param	bClosed If true draw a closed polyline
******************************************************************************/
void TVideoManager::DrawLines(TVecPoints& Pts, int nLineWidth,
	COLORREF Color, bool bClosed)
{
	assert(m_hDC);

	HDC hDC = m_hDC;
	assert(hDC);

	HPEN hPen = ::CreatePen(PS_SOLID, nLineWidth, Color);
	assert(hPen);

	HPEN hOldPen = (HPEN) ::SelectObject(hDC, hPen);
	assert(hOldPen);

	for(int i=1; i<Pts.size(); i++)
	{
		::MoveToEx(hDC, Pts[i-1].X, Pts[i-1].Y, (LPPOINT) NULL);
		::LineTo(hDC, Pts[i].X, Pts[i].Y);
	}

	if( bClosed && (Pts.size() > 2) )
	{
		::MoveToEx(hDC, Pts[Pts.size()-1].X, Pts[Pts.size()-1].Y, (LPPOINT) NULL);
		::LineTo(hDC, Pts[0].X, Pts[0].Y);
	}

	::SelectObject(hDC, hOldPen);
	::DeleteObject(hPen);
}

/*!****************************************************************************
* @brief	Draw a series of polylines
* @param	Pts Reference to a vector of polylines
* @param	nLineWidth Specifies the width of the polyline
* @param	Color Specifies the color of the polyline
* @param	bClosed If true draw a closed polyline
******************************************************************************/
void TVideoManager::DrawLines(TVecVecPoints& Pts, int nLineWidth,
	COLORREF Color, bool bClosed)
{
	for (int i = 0; i < Pts.size(); ++i)
	{
		DrawLines(Pts[i], nLineWidth, Color, bClosed);
	}
}

/*!****************************************************************************
* @brief	Draw a point
* @param	Pt Coordinates of point to be drawn
* @param	Color Specifies the color of the point
******************************************************************************/
void TVideoManager::DrawPoint(TVector2& Pt, COLORREF Color)
{
	assert(m_hDC);

	::SetPixel(m_hDC, Pt.X, Pt.Y, Color);
}

/*!****************************************************************************
* @brief	Clears the screen by filling it to the specified color
* @param	Color The color to fill the graphics area
******************************************************************************/
void TVideoManager::ClearScreen(COLORREF Color)
{
	assert(m_hDC);

	HBRUSH hBrush = ::CreateSolidBrush(Color);
	assert(hBrush);

	HBRUSH hOldBrush = (HBRUSH) ::SelectObject(m_hDC, hBrush);

	::FillRect(m_hDC, &m_ClientArea, hBrush);

	::SelectObject(m_hDC, hOldBrush);
	::DeleteObject(hBrush);
}

/*!****************************************************************************
* @brief	Loads the font specified by a file name
* @param	strFontPath Full path to the true-type (ttf) font file name
* @param	strName The name of the font
* @param	nSize The size of the font
******************************************************************************/
bool TVideoManager::LoadFont(std::string strFontPath, std::wstring strName, int nSize)
{
	assert(m_hDC);

	bool bResult = false;
    WideString wstrFontPath(strFontPath.c_str());
	int nResults = AddFontResourceEx( (wchar_t*) wstrFontPath.c_bstr(), FR_PRIVATE, NULL);

	if( nResults )
	{
		LOGFONT LF;
		memset(&LF, 0, sizeof(LF));

		LF.lfHeight = nSize;
		LF.lfWeight = FW_NORMAL;
		LF.lfOutPrecision = OUT_TT_ONLY_PRECIS;
		wcscpy(LF.lfFaceName, strName.c_str());

		HFONT hFont = ::CreateFontIndirect(&LF);

		::SelectObject(m_hDC, hFont);

		bResult = true;
	}

	return bResult;
}

/*!****************************************************************************
* @brief	Draws text
* @param	pText Pointer to a text string
* @param	nX X position for text
* @param	nY Y position for text
* @param	nColor Color for text
* @param	nAlign Alignment for text
******************************************************************************/
void TVideoManager::DrawText(char* pText, int nX, int nY, COLORREF nColor, UINT nAlign)
{
	assert(pText);
	assert(m_hDC);

	::SetTextAlign(m_hDC, nAlign);
	::SetTextColor(m_hDC, nColor);

	::TextOutA(m_hDC, nX, nY, (LPCSTR) pText, strlen(pText));
}

/*!****************************************************************************
* @brief	Draws mulitple texts
* @param	StringList A series of text strings
* @param	nX X position for text
* @param	nY Y position for text
* @param	nLineHeight	Height of text line
* @param	nColor Color for text
* @param	nAlign Alignment for text
******************************************************************************/
void TVideoManager::DrawText(std::vector<std::string> StringList,
	int nX, int nY, int nLineHeight, COLORREF nColor, UINT nAlign)
{
	for(int i=0; i<StringList.size(); i++)
	{
		DrawText((char*) StringList[i].c_str(), nX, nY, nColor, nAlign);

		nY += 1.25 * nLineHeight;
	}
}

/*!****************************************************************************
* @brief	Gets the handle to the game main window
* @return	Returns the handle to the game main window
******************************************************************************/
HWND TVideoManager::GetHWnd()
{
 	return m_hWnd;
}

/*!****************************************************************************
* @brief	Gets the Device Context
* @return	Returns the handle to a Device Context
******************************************************************************/
HDC TVideoManager::GetDC()
{
	return m_hDC;
}

/*!****************************************************************************
* @brief	Gets the size of the client area
* @return	Returns the size for the client area
******************************************************************************/
RECT TVideoManager::GetClientArea()
{
	return m_ClientArea;
}


