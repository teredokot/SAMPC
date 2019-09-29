#include "textbuffer.h"
#include "stateblockmanager.h"
#include <d3dx9.h>
#include <d3d9.h>
#include <stdarg.h>
#include <stdio.h>

extern IDirect3DDevice9 *pD3DDevice;

namespace GUI
{
	CTextBuffer::CTextBuffer(uint uiMaxChars) : CTextBufferBase(uiMaxChars)
	{
		m_szBuffer = new char[uiMaxChars+1];
		m_szBuffer[0] = 0;
				
		m_uiColor = 0xFFFFFFFF;
		m_iTextStartPos = 0;

		m_bCaretEnable = false;
		m_uiCaretColor = m_uiCaretColor;
		m_uiCaretPosition = 0;
	}

	CTextBuffer::~CTextBuffer()
	{
		if (m_szBuffer)
			delete m_szBuffer;
	}

	void CTextBuffer::UpdateVertexBuffer()
	{
		// Can't update render without a font
		if (!m_pFont)
			return;

		float sx, sy;
		
		float fTextScale = m_pFont->GetTextScale();
		uint uiTextHeight = m_pFont->GetTextHeight();
		uint uiSpacing = m_pFont->GetTextSpacing();
		uint uiTexHeight = m_pFont->GetTexture()->GetHeight();
		uint uiTexWidth = m_pFont->GetTexture()->GetWidth();

		m_uiCount = 0;

		sx = 0.0f;
		sy = 0.0f;

	    float fStartX = sx;

		Vertex *pVertex = NULL;
		m_pVertexBuffer->Lock(0, 0, (void**)&pVertex, D3DLOCK_DISCARD);

#ifdef _DEBUG
		if (!pVertex)
			__debugbreak();
#endif

		bool bRenderCaret = false;
		uint uiPosition = 0;
		const byte *pText = (const byte*)m_szBuffer + m_iTextStartPos;
		while (true)
		{
			byte c;

			if (!bRenderCaret && m_bCaretEnable && uiPosition == m_uiCaretPosition)
			{
				c = '|';
				bRenderCaret = true;

				sx -= (float)uiSpacing;
				sx++;
			}
			else
			{
				c = *pText++;
				
				if (c == 0)
					break;

				bRenderCaret = false;
				uiPosition++;
			}

			if (c == (byte)'\n')
			{
				sx = fStartX;
				sy += uiTextHeight;

				// Check for overflowing the clip height
				if (m_iClipHeight > 0 && (sy - m_iRenderY) > m_iClipHeight)
					break;
			}

			if ((c < 32 || c >= 256))
				continue;

			const float *pfChar = m_pFont->GetCharCoords(c);

			float tx1 = pfChar[0];
			float ty1 = pfChar[1];
			float tx2 = pfChar[2];
			float ty2 = pfChar[3];

			float w = (tx2-tx1) * uiTexWidth / fTextScale;
			float h = (ty2-ty1) * uiTexHeight / fTextScale;

			// Now lets make the vertices
			//
			//	0.....1  4
			//	.    .  ..
			//	.   .  . .
			//	.  .  .  .
			//	. .  .   .
			//	..  .    .
			//	2  3.....5
			//

			float fX0 = sx + 0 - 0.5f + m_iRenderX;
			float fX1 = sx + w - 0.5f + m_iRenderX;
			float fY0 = sy + 0 - 0.5f + m_iRenderY;
			float fY1 = sy + h - 0.5f + m_iRenderY;
			float fZ = 0.9f;

			uint color;
			if (bRenderCaret)
				color = m_uiCaretColor;
			else
				color = m_uiColor;

			*pVertex++ = InitVertex(fX0, fY0, fZ, color, tx1, ty1);
			*pVertex++ = InitVertex(fX1, fY0, fZ, color, tx2, ty1);
			*pVertex++ = InitVertex(fX0, fY1, fZ, color, tx1, ty2);
			*pVertex++ = InitVertex(fX0, fY1, fZ, color, tx1, ty2);
			*pVertex++ = InitVertex(fX1, fY0, fZ, color, tx2, ty1);
			*pVertex++ = InitVertex(fX1, fY1, fZ, color, tx2, ty2);

			if (bRenderCaret)
				sx += (uiSpacing - 1);
			else
				sx += w - (2 * uiSpacing);

			m_uiCount++;

			// Check for overflowing the clip width
			if (m_iClipWidth > 0 && (fX1 - m_iRenderX) > m_iClipWidth)
			{
				do
				{
					c = *pText++;
				} while (c != 0 && c != '\n');

				if (c == '\n')
				{
					pText--;
					continue;
				}
				else
				{
					break;
				}
			}

		}

		m_pVertexBuffer->Unlock();

		m_bHasUpdates = false;

	}

	void CTextBuffer::AddText(const byte nChar)
	{
		char szText[2];
		szText[0] = *reinterpret_cast<const char*>(&nChar);
		szText[1] = 0;
		AddText(szText);
	}

	void CTextBuffer::AddText(const char *szText)
	{
		strcat(m_szBuffer, szText);

		m_bHasUpdates = true;
	}

	void CTextBuffer::InsertText(uint uiPosition, const byte nChar)
	{
		char szText[2];
		szText[0] = *reinterpret_cast<const char*>(&nChar);
		szText[1] = 0;
		InsertText(uiPosition, szText);
	}

	void CTextBuffer::InsertText(uint uiPosition, const char *szText)
	{
		uint uiLength = GetLength();
		if (uiLength == 0 || uiPosition == uiLength)
		{
			AddText(szText);
		}
		else
		{
			char *szTempBuffer = new char[uiLength - uiPosition + 1];
			strcpy(szTempBuffer, m_szBuffer+uiPosition);
			strcpy(m_szBuffer+uiPosition, szText);
			strcat(m_szBuffer, szTempBuffer);
			delete szTempBuffer;

			m_bHasUpdates = true;
		}

	}

	void CTextBuffer::RemoveText(uint uiPosition, uint uiLength)
	{
		uint uiBufferLength = GetLength();
		if (uiPosition > uiBufferLength - 1)
			return;

		char *szTempBuffer = new char[uiBufferLength - (uiPosition + uiLength) + 1];
		strcpy(szTempBuffer, m_szBuffer+uiPosition+uiLength);
		strcpy(m_szBuffer+uiPosition, szTempBuffer);
		delete szTempBuffer;

		m_bHasUpdates = true;
	}

	void CTextBuffer::ClearText()
	{
		m_szBuffer[0] = 0;
		
		m_bHasUpdates = true;
	}

	uint CTextBuffer::GetLength() const
	{ 
		return (uint)strlen(m_szBuffer); 
	}

	void CTextBuffer::PrintF(const char *szFormat, ...)
	{
		va_list ap;
		va_start(ap, szFormat);
		vsprintf(m_szBuffer, szFormat, ap);
		va_end(ap);
	}
		
	void CTextBuffer::SetColor(uint color) 
	{ 
		m_uiColor = color; 

		m_bHasUpdates = true;
	}

	void CTextBuffer::SetTextStart(int startPos) 
	{ 
		m_iTextStartPos = startPos; 

		m_bHasUpdates = true;
	}

	void CTextBuffer::SetCaretEnable(bool enabled)
	{
		m_bCaretEnable = true;

		m_bHasUpdates = true;
	}

	void CTextBuffer::SetCaretColor(uint color)
	{
		m_uiCaretColor = color;

		m_bHasUpdates = true;

	}

	void CTextBuffer::SetCaretPosition(int pos)
	{
		m_uiCaretPosition = pos;

		m_bHasUpdates = true;
	}

}
