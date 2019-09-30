#include "texture.h"

#include <d3dx9.h>

extern IDirect3DDevice9 *pD3DDevice;

namespace GUI
{
	CTexture::CTexture(const char *szFilename)
	{
		m_pTexture = NULL;
		m_szFilename = szFilename;

		OnRestoreDevice();
	}

	CTexture::CTexture(uint uiWidth, uint uiHeight, D3DFORMAT format)
	{
		m_pTexture = NULL;
		m_szFilename = NULL;
		m_uiWidth = uiWidth;
		m_uiHeight = uiHeight;
		m_Format = format;

		OnRestoreDevice();
	}

	CTexture::~CTexture()
	{
		if (m_pTexture)
			m_pTexture->Release();
	}

	IDirect3DTexture9 *CTexture::GetTexture()
	{
		if (!m_pTexture)
			OnRestoreDevice();
		return m_pTexture;
	}

	uint CTexture::GetWidth() const
	{
		return m_uiWidth;
	}

	uint CTexture::GetHeight() const
	{
		return m_uiHeight;
	}

	void CTexture::OnDestroyDevice()
	{
		if (m_pTexture)
			m_pTexture->Release();
		m_pTexture = NULL;
	}

	void CTexture::OnRestoreDevice()
	{
		if (m_szFilename)
		{
			D3DXCreateTextureFromFile(pD3DDevice, m_szFilename, &m_pTexture);

			D3DSURFACE_DESC desc;
			m_pTexture->GetLevelDesc(0, &desc);
			m_uiWidth = desc.Width;
			m_uiHeight = desc.Height;
			m_Format = desc.Format;
		}
		else
		{
			pD3DDevice->CreateTexture(m_uiWidth, m_uiHeight, 1, 0, m_Format, D3DPOOL_MANAGED, &m_pTexture, NULL);
		}
	}

}