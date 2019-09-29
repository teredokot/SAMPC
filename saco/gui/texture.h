#pragma once

#include "common.h"
#include <d3d9.h>
#include "resource.h"

namespace GUI
{
	class CTexture :
		public IResource
	{
	protected:
		IDirect3DTexture9 *m_pTexture;
		const char *m_szFilename;
		uint m_uiWidth;
		uint m_uiHeight;
		D3DFORMAT m_Format;
		
		//CTexture() {}

	public:
		CTexture(const char *szFilename);
		CTexture(uint uiWidth, uint uiHeight, D3DFORMAT format);
		virtual ~CTexture();

		IDirect3DTexture9 *GetTexture();

		uint GetWidth() const;
		uint GetHeight() const;

		virtual void OnDestroyDevice();
		virtual void OnRestoreDevice();
	};

}
