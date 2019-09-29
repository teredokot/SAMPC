#include "font.h"
#include <windows.h>
#include <math.h>

#include <stdio.h>

extern IDirect3DDevice9 *pD3DDevice;

namespace GUI
{

	CFont::CFont(const char *szCacheFilename)
	{
		// TODO: Write this.
	}
	
	CFont::CFont(const char *szFontName, uint uiSize, bool bBold, bool bItalic, bool bBorderFont)
	{
		// Save some stuff for later
		m_szFontName = szFontName;
		m_uiSize = uiSize;
		m_bBold = bBold;
		m_bItalic = bItalic;

		// Set up stuff
		m_fTextScale = 1.0f;	// Draw into texture without scaling
		m_uiSpacing = 0;
		uint uiTexWidth, uiTexHeight;

		// Large fonts need larger textures
		if (uiSize > 30)
			uiTexWidth = uiTexHeight = 1024;
		else if (m_uiSize >= 10)
			uiTexWidth = uiTexHeight = 512;
		else
			uiTexWidth = uiTexHeight = 256;

		// If requested texture is too big, use a smaller texture and smaller font,
		// and scale up when rendering.
		D3DCAPS9 d3dCaps;
		pD3DDevice->GetDeviceCaps( &d3dCaps );

		if( uiTexWidth > d3dCaps.MaxTextureWidth )
		{
			m_fTextScale = (float)d3dCaps.MaxTextureWidth / (float)uiTexWidth;
			uiTexWidth = uiTexHeight = d3dCaps.MaxTextureWidth;
		}

		m_pTexture = new CTexture(uiTexWidth, uiTexHeight, D3DFMT_A4R4G4B4);

		m_uiTextHeight = 0;

		//---------------------------------------
		// Do the windowsy stuff to create the bitmap

		DWORD*      pBitmapBits;
		BITMAPINFO bmi;
		ZeroMemory( &bmi.bmiHeader,  sizeof(BITMAPINFOHEADER) );
		bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth       =  (int)uiTexWidth;
		bmi.bmiHeader.biHeight      = -(int)uiTexHeight;
		bmi.bmiHeader.biPlanes      = 1;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biBitCount    = 32;

		// Create a DC and a bitmap for the font
		HDC     hDC       = CreateCompatibleDC( NULL );
		HBITMAP hbmBitmap = CreateDIBSection( hDC, &bmi, DIB_RGB_COLORS,
											(void**)&pBitmapBits, NULL, 0 );
		SetMapMode( hDC, MM_TEXT );

		// Create a font.  By specifying ANTIALIASED_QUALITY, we might get an
		// antialiased font, but this is not guaranteed.
		INT nHeight    = -MulDiv( uiSize, 
			(INT)(GetDeviceCaps(hDC, LOGPIXELSY) * m_fTextScale), 72 );
		DWORD dwBold   = (bBold)   ? FW_BOLD : FW_NORMAL;
		DWORD dwItalic = (bItalic) ? TRUE    : FALSE;
		HFONT hFont    = CreateFont( nHeight, 0, 0, 0, dwBold, dwItalic,
							FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
							CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
							VARIABLE_PITCH, szFontName );

		if( NULL==hFont )
			throw NewException(0);

		SelectObject( hDC, hbmBitmap );
		SelectObject( hDC, hFont );

		// Set text properties
		SetTextColor( hDC, RGB(255,255,255) );
		SetBkColor(   hDC, 0x00000000 );
		SetTextAlign( hDC, TA_TOP );

		// Loop through all printable character and output them to the bitmap..
		// Meanwhile, keep track of the corresponding tex coords for each character.
		DWORD x = 1;
		DWORD y = 1;
		BYTE str[2] = {0, 0};
		SIZE size;

		// Calculate the spacing between characters based on line height
		GetTextExtentPoint32( hDC, TEXT(" "), 1, &size );
		x = m_uiSpacing = (DWORD) ceil(size.cy * 0.25f);

		for( DWORD c=32; c<256; c++ )
		{
			str[0] = (BYTE)c;
			GetTextExtentPoint32( hDC, (LPCSTR)str, 1, &size );

			if( (DWORD)(x + size.cx + m_uiSpacing) > uiTexWidth )
			{
				x  = m_uiSpacing;
				y += size.cy+2;
			}
			
			ExtTextOut( hDC, x+0, y+0, ETO_OPAQUE, NULL, (LPCSTR)str, 1, NULL );

			m_fTexCoords[c][0] = ((FLOAT)(x + 0       - m_uiSpacing))/uiTexWidth;
			m_fTexCoords[c][1] = ((FLOAT)(y + 0       + 0          ))/uiTexHeight;
			m_fTexCoords[c][2] = ((FLOAT)(x + size.cx + m_uiSpacing))/uiTexWidth;
			m_fTexCoords[c][3] = ((FLOAT)(y + size.cy + 1          ))/uiTexHeight;

			x += size.cx + (2 * m_uiSpacing);
			
			if (m_uiTextHeight < (unsigned)(size.cy + 1))
				m_uiTextHeight = (size.cy + 1);
		}

		// Lock the surface and write the alpha values for the set pixels
		D3DLOCKED_RECT d3dlr;
		
		if (m_pTexture->GetTexture())
		{

			m_pTexture->GetTexture()->LockRect( 0, &d3dlr, 0, 0 );
			BYTE* pDstRow = (BYTE*)d3dlr.pBits;
			WORD* pDst16;
			
			if (!bBorderFont)
			{
				BYTE bAlpha; // 4-bit measure of pixel intensity
				for( y=0; y < uiTexHeight; y++ )
				{
					pDst16 = (WORD*)pDstRow;
					for( x=0; x < uiTexWidth; x++ )
					{
						bAlpha = (BYTE)((pBitmapBits[uiTexWidth*y + x] & 0xff) >> 4);
						if (bAlpha > 0)
						{
							*pDst16++ = (WORD) ((bAlpha << 12) | 0x0fff);
						}
						else
						{
							*pDst16++ = 0x0000;
						}
					}
					pDstRow += d3dlr.Pitch;
				}
			}
			else
			{
				BYTE bAlpha; // 4-bit measure of pixel intensity
				BYTE bAlphaU, bAlphaD, bAlphaL, bAlphaR;

				pDstRow += d3dlr.Pitch;	// for y=0
				for( y=1; y < uiTexHeight-1; y++ )
				{
					pDst16 = (WORD*)pDstRow;
					pDst16++;	// for x=0;
					for( x=1; x < uiTexWidth-1; x++ )
					{
						bAlphaU = (BYTE)((pBitmapBits[uiTexWidth*(y-1) + (x)] & 0xff) >> 4);
						bAlphaD = (BYTE)((pBitmapBits[uiTexWidth*(y+1) + (x)] & 0xff) >> 4);
						bAlphaL = (BYTE)((pBitmapBits[uiTexWidth*(y) + (x-1)] & 0xff) >> 4);
						bAlphaR = (BYTE)((pBitmapBits[uiTexWidth*(y) + (x+1)] & 0xff) >> 4);
						bAlpha = (BYTE)((float)(bAlphaU | bAlphaD | bAlphaL | bAlphaR) * 0.75f);
						bAlpha |= (BYTE)((pBitmapBits[uiTexWidth*y + x] & 0xff) >> 4);
						if (bAlpha > 0)
						{
							*pDst16++ = (WORD) ((bAlpha << 12) | 0x0fff);
						}
						else
						{
							*pDst16++ = 0x0000;
						}
					}
					pDstRow += d3dlr.Pitch;
				}
			}


			/*
		char szBuffer[1024];
		sprintf(szBuffer, "c:\\bmp-%s-%d-%s.bmp", szFontName, uiSize, bBorderFont?"b":"n");

		FILE *pFile = fopen(szBuffer, "wb");

		BITMAPFILEHEADER bmfh;
		int nBitsOffset = sizeof(BITMAPFILEHEADER) + bmi.bmiHeader.biSize; 
		LONG lImageSize = uiTexWidth * uiTexHeight * (bmi.bmiHeader.biBitCount/8);
		LONG lFileSize = nBitsOffset + lImageSize;
		bmfh.bfType = 'B'+('M'<<8);
		bmfh.bfOffBits = nBitsOffset;
		bmfh.bfSize = lFileSize;
		bmfh.bfReserved1 = bmfh.bfReserved2 = 0;
		//Write the bitmap file header
		UINT nWrittenFileHeaderSize = fwrite(&bmfh, 1, 
					sizeof(BITMAPFILEHEADER), pFile);
		//And then the bitmap info header
		UINT nWrittenInfoHeaderSize = fwrite(&bmi.bmiHeader, 
			1, sizeof(BITMAPINFOHEADER), pFile);
		//Finally, write the image data itself 
		//-- the data represents our drawing
		UINT nWrittenDIBDataSize = 
			fwrite(pBitmapBits, 1, lImageSize, pFile);

		fclose(pFile);*/


			m_pTexture->GetTexture()->UnlockRect(0);

		}

		// Done updating texture, so clean up used objects
		DeleteObject( hbmBitmap );
		DeleteDC( hDC );
		DeleteObject( hFont );

		//---------------------------------------

	}
	
	CFont::~CFont()
	{
		m_pTexture->OnDestroyDevice();
		delete m_pTexture;
		m_pTexture = NULL;
	}

	void CFont::SaveToCache(const char *szFilename) const
	{
		// TODO: Write this.
	}

	void CFont::OnDestroyDevice()
	{
		// Don't need to destory (managed)
		//m_pTexture->OnDestroyDevice();
	}

	void CFont::OnRestoreDevice()
	{
		// Don't need to restore (managed)
		//m_pTexture->OnRestoreDevice();
	}

}