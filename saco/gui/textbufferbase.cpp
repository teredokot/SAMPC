#include "textbuffer.h"
#include "stateblockmanager.h"
#include <d3d9.h>

extern IDirect3DDevice9 *pD3DDevice;

namespace GUI
{
	CTextBufferBase::CTextBufferBase(uint uiMaxChars)
	{
		m_pVertexBuffer = NULL;

		m_uiMaxChars = uiMaxChars;		
		m_uiCount = 0;

		m_pFont = NULL;
		m_pBorderFont = NULL;

		m_iRenderX = 0;
		m_iRenderY = 0;

		m_iClipWidth = 0;
		m_iClipHeight = 0;

		m_bHasUpdates = true;

		OnRestoreDevice();
	}

	CTextBufferBase::~CTextBufferBase()
	{
		OnDestroyDevice();
	}

	
	void CTextBufferBase::SetPosition(int x, int y) 
	{ 
		m_iRenderX = x; 
		m_iRenderY = y; 

		m_bHasUpdates = true;
	}

	void CTextBufferBase::SetClipBounds(int width, int height) 
	{ 
		m_iClipWidth = width; 
		m_iClipHeight = height; 

		m_bHasUpdates = true;
	}

	void CTextBufferBase::SetFont(CFont *pFont) 
	{ 
		m_pFont = pFont; 

		m_bHasUpdates = true;
	}

	void CTextBufferBase::SetBorderFont(CFont *pBorderFont) 
	{ 
		m_pBorderFont = pBorderFont; 
	}

	void CTextBufferBase::AddTextToBuffer(const char *szText, Vertex *&pVertex, float &sx, float &sy, uint uiColor)
	{
		// Can't update render without a font
		if (!m_pFont)
			return;
		
		float fTextScale = m_pFont->GetTextScale();
		uint uiTextHeight = m_pFont->GetTextHeight();
		uint uiSpacing = m_pFont->GetTextSpacing();
		uint uiTexHeight = m_pFont->GetTexture()->GetHeight();
		uint uiTexWidth = m_pFont->GetTexture()->GetWidth();

	    float fStartX = 0.0f;

		uint uiPosition = 0;
		const byte *pText = (const byte*)szText;
		while (true)
		{
			byte c;

			c = *pText++;
			
			if (c == 0)
				break;

			uiPosition++;

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

			*pVertex++ = InitVertex(fX0, fY0, fZ, uiColor, tx1, ty1);
			*pVertex++ = InitVertex(fX1, fY0, fZ, uiColor, tx2, ty1);
			*pVertex++ = InitVertex(fX0, fY1, fZ, uiColor, tx1, ty2);
			*pVertex++ = InitVertex(fX0, fY1, fZ, uiColor, tx1, ty2);
			*pVertex++ = InitVertex(fX1, fY0, fZ, uiColor, tx2, ty1);
			*pVertex++ = InitVertex(fX1, fY1, fZ, uiColor, tx2, ty2);

			sx += w - (2 * uiSpacing);

			m_uiCount++;

			if (m_uiCount > m_uiMaxChars)
			{
#ifdef _DEBUG
				__debugbreak();
#endif
				break;
			}

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

	}

	float CTextBufferBase::GetTextWidth(const char *szText)
	{
		float fWidth = 0.0f;

		float fTextScale = m_pFont->GetTextScale();
		uint uiSpacing = m_pFont->GetTextSpacing();
		uint uiTexHeight = m_pFont->GetTexture()->GetHeight();
		uint uiTexWidth = m_pFont->GetTexture()->GetWidth();
		
		while(*szText)
		{
			byte c = *szText++;

			const float *pfChar = m_pFont->GetCharCoords(c);

			float tx1 = pfChar[0];
			float tx2 = pfChar[2];

			fWidth += (tx2-tx1) * uiTexWidth / fTextScale - (2 * uiSpacing);

		}

		return fWidth;
		
	}

	void CTextBufferBase::OnRender()
	{
		if (m_bHasUpdates)
			UpdateVertexBuffer();

		if (m_uiCount > 0)
		{

			IDirect3DStateBlock9 *pStateBlockSaved = CStateBlockManager::GetStateBlock(CStateBlockManager::STATE_CURRENT);
			IDirect3DStateBlock9 *pStateBlockText = CStateBlockManager::GetStateBlock(CStateBlockManager::STATE_TEXT);
			IDirect3DStateBlock9 *pStateBlockBorder = CStateBlockManager::GetStateBlock(CStateBlockManager::STATE_TEXT_BORDER);

			pStateBlockSaved->Capture();

			pD3DDevice->SetFVF( FVF );
			pD3DDevice->SetPixelShader( NULL );
			pD3DDevice->SetStreamSource( 0, m_pVertexBuffer, 0, sizeof(Vertex) );

			if (m_pBorderFont)
			{
				pStateBlockBorder->Apply();
				pD3DDevice->SetTexture(0, m_pBorderFont->GetTexture()->GetTexture());
				pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_uiCount*2);
			}

			pStateBlockText->Apply();
			pD3DDevice->SetTexture(0, m_pFont->GetTexture()->GetTexture());
			pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, m_uiCount*2);

			pStateBlockSaved->Apply();

		}
	}

	void CTextBufferBase::OnDestroyDevice()
	{
		if (m_pVertexBuffer)
			m_pVertexBuffer->Release();

		m_pVertexBuffer = NULL;
	}

	void CTextBufferBase::OnRestoreDevice()
	{

		pD3DDevice->CreateVertexBuffer(m_uiMaxChars*6*sizeof(Vertex), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 
			FVF, D3DPOOL_DEFAULT, &m_pVertexBuffer, NULL);

		m_bHasUpdates = true;

	}
}
