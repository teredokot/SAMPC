#pragma once

#include "common.h"
#include "renderable.h"
#include "font.h"
#include <d3d9.h>

namespace GUI
{
	
	class CTextBufferBase :
		public IRenderable
	{
	protected:
		static const uint FVF = D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1;

		struct Vertex
		{
			float x, y, z, w;
			uint color;
			float tu, tv;
		};

		inline Vertex InitVertex(float x, float y, float z, uint color, float tu, float tv)
		{
			Vertex v; 
			v.x = x; v.y = y; v.z = z; v.w = 1.0f;
			v.color = color;
			v.tu = tu; v.tv = tv;
			return v;
		}

		CFont *m_pFont;
		CFont *m_pBorderFont;

		uint m_uiMaxChars;
		uint m_uiCount;		// Vertex Buffer Glyph Count

		int m_iRenderX, m_iRenderY;
		int m_iClipWidth, m_iClipHeight;

		bool m_bHasUpdates;

		IDirect3DVertexBuffer9 *m_pVertexBuffer;

		virtual void UpdateVertexBuffer() = 0;
		
		void AddTextToBuffer(const char *szText, Vertex *&pVertex, float &sx, float &sy, uint uiColor);
		float GetTextWidth(const char *szText);

	public:
		CTextBufferBase(uint uiMaxChars);
		virtual ~CTextBufferBase();

		void SetPosition(int x, int y);
		void SetClipBounds(int width, int height);

		void SetBorderFont(CFont *pBorderFont);
		CFont *GetBorderFont() { return m_pFont; }

		void SetFont(CFont *pFont); 
		CFont *GetFont() { return m_pFont; }

		virtual void OnRender();
		virtual void OnDestroyDevice();
		virtual void OnRestoreDevice();

	};

}
