#pragma once

#include "common.h"
#include <d3d9.h>
#include "texture.h"
#include "resource.h"

namespace GUI
{
	class CFont :
		public IResource
	{
	private:
		const char *m_szCacheFilename;
		const char *m_szFontName;
		uint m_uiSize;
		bool m_bBold;
		bool m_bItalic;

		uint m_uiTextHeight;
		float m_fTextScale;
		uint m_uiSpacing;

		CTexture *m_pTexture;

		float m_fTexCoords[256][4];

	public:
		CFont(const char *szCacheFilename);
		CFont(const char *szFamily, uint uiSize, bool bBold, bool bItalic, bool bBorderFont = false);
		~CFont();

		void SaveToCache(const char *szFilename) const;

		const float *GetCharCoords(byte nChar) const { return &m_fTexCoords[nChar][0]; }
		const float *GetCharCoords(char cChar) const { return GetCharCoords((unsigned char)cChar); }
		
		float GetTextScale() const { return m_fTextScale; }
		uint GetTextHeight() const { return m_uiTextHeight; }
		uint GetTextSpacing() const { return m_uiSpacing; }

		CTexture *GetTexture() const { return m_pTexture; }
		
		virtual void OnDestroyDevice();
		virtual void OnRestoreDevice();
	};

}
