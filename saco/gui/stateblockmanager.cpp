
#include "stateblockmanager.h"

extern IDirect3DDevice9 *pD3DDevice;

namespace GUI
{

	IDirect3DStateBlock9 *CStateBlockManager::ms_ppStateBlocks[CStateBlockManager::MAX_STATES];

	void CStateBlockManager::Initialize()
	{
		memset(ms_ppStateBlocks, 0, sizeof(ms_ppStateBlocks));
		OnRestoreDevice();
	}

	void CStateBlockManager::Shutdown()
	{
		OnDestroyDevice();
	}

	IDirect3DStateBlock9 *CStateBlockManager::GetStateBlock(StateBlockType state)
	{
		return ms_ppStateBlocks[state];
	}

	void CStateBlockManager::OnDestroyDevice()
	{
		for(int i=0; i<MAX_STATES; i++)
			if (ms_ppStateBlocks[i])
			{
				ms_ppStateBlocks[i]->Release();
				ms_ppStateBlocks[i] = NULL;
			}
	}

	void CStateBlockManager::OnRestoreDevice()
	{
		//----------------------------------------------------------------------------
		// Default state...

		pD3DDevice->BeginStateBlock();

		//-- Vertex Pipeline --

		pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		pD3DDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
		//pD3DDevice->SetRenderState(D3DRS_FOGCOLOR, 0);
		//pD3DDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_NONE);
		//pD3DDevice->SetRenderState(D3DRS_FOGSTART, 0);
		//pD3DDevice->SetRenderState(D3DRS_FOGEND, 1);
		//pD3DDevice->SetRenderState(D3DRS_FOGDENSITY, 1);
		//pD3DDevice->SetRenderState(D3DRS_RANGEFOGENABLE, FALSE);
		//pD3DDevice->SetRenderState(D3DRS_AMBIENT, 0);
		//pD3DDevice->SetRenderState(D3DRS_COLORVERTEX, TRUE);
		//pD3DDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_NONE);
		pD3DDevice->SetRenderState(D3DRS_CLIPPING, TRUE);
		//pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		//pD3DDevice->SetRenderState(D3DRS_LOCALVIEWER, TRUE);
		//pD3DDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
		//pD3DDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
		//pD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
		//pD3DDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2);
		pD3DDevice->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
		pD3DDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, 0);
		//pD3DDevice->SetRenderState(D3DRS_POINTSIZE, 1);
		//pD3DDevice->SetRenderState(D3DRS_POINTSIZE_MIN, 1);
		//pD3DDevice->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
		//pD3DDevice->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
		//pD3DDevice->SetRenderState(D3DRS_POINTSCALE_A, 1);
		//pD3DDevice->SetRenderState(D3DRS_POINTSCALE_B, 0);
		//pD3DDevice->SetRenderState(D3DRS_POINTSCALE_C, 0);
		//pD3DDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
		//pD3DDevice->SetRenderState(D3DRS_MULTISAMPLEMASK, 0xffffffff);
		//pD3DDevice->SetRenderState(D3DRS_PATCHEDGESTYLE, D3DPATCHEDGE_DISCRETE);
		//pD3DDevice->SetRenderState(D3DRS_POINTSIZE_MAX, 1);
		pD3DDevice->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
		//pD3DDevice->SetRenderState(D3DRS_TWEENFACTOR, 0);
		//pD3DDevice->SetRenderState(D3DRS_POSITIONDEGREE, D3DDEGREE_CUBIC);
		//pD3DDevice->SetRenderState(D3DRS_NORMALDEGREE, D3DDEGREE_LINEAR);
		//pD3DDevice->SetRenderState(D3DRS_MINTESSELLATIONLEVEL, 1);
		//pD3DDevice->SetRenderState(D3DRS_MAXTESSELLATIONLEVEL, 1);
		//pD3DDevice->SetRenderState(D3DRS_ADAPTIVETESS_X, 0);
		//pD3DDevice->SetRenderState(D3DRS_ADAPTIVETESS_Y, 0);
		//pD3DDevice->SetRenderState(D3DRS_ADAPTIVETESS_Z, 1);
		//pD3DDevice->SetRenderState(D3DRS_ADAPTIVETESS_W, 0);
		//pD3DDevice->SetRenderState(D3DRS_ENABLEADAPTIVETESSELLATION, FALSE);

		//pD3DDevice->SetSamplerState(0, D3DSAMP_DMAPOFFSET, 256);

		//pD3DDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
		//pD3DDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

		//pD3DDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);
		//pD3DDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

		//-- Pixel Pipeline --

		pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		//pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
		pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		//pD3DDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		//pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		//pD3DDevice->SetRenderState(D3DRS_LASTPIXEL, TRUE);
		//pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		//pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
		//pD3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		pD3DDevice->SetRenderState(D3DRS_ALPHAREF, 0);
		pD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
		//pD3DDevice->SetRenderState(D3DRS_DITHERENABLE, FALSE);
		//pD3DDevice->SetRenderState(D3DRS_FOGSTART, 0);
		//pD3DDevice->SetRenderState(D3DRS_FOGEND, 1);
		//pD3DDevice->SetRenderState(D3DRS_FOGDENSITY, 1);
		pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		//pD3DDevice->SetRenderState(D3DRS_DEPTHBIAS, 0);
		pD3DDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
		//pD3DDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
		//pD3DDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
		//pD3DDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
		//pD3DDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
		//pD3DDevice->SetRenderState(D3DRS_STENCILREF, 0);
		//pD3DDevice->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
		//pD3DDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
		pD3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);
		//pD3DDevice->SetRenderState(D3DRS_WRAP0, 0);
		//pD3DDevice->SetRenderState(D3DRS_WRAP1, 0);
		//pD3DDevice->SetRenderState(D3DRS_WRAP2, 0);
		//pD3DDevice->SetRenderState(D3DRS_WRAP3, 0);
		//pD3DDevice->SetRenderState(D3DRS_WRAP4, 0);
		//pD3DDevice->SetRenderState(D3DRS_WRAP5, 0);
		//pD3DDevice->SetRenderState(D3DRS_WRAP6, 0);
		//pD3DDevice->SetRenderState(D3DRS_WRAP7, 0);
		//pD3DDevice->SetRenderState(D3DRS_WRAP8, 0);
		//pD3DDevice->SetRenderState(D3DRS_WRAP9, 0);
		//pD3DDevice->SetRenderState(D3DRS_WRAP10, 0);
		//pD3DDevice->SetRenderState(D3DRS_WRAP11, 0);
		//pD3DDevice->SetRenderState(D3DRS_WRAP12, 0);
		//pD3DDevice->SetRenderState(D3DRS_WRAP13, 0);
		//pD3DDevice->SetRenderState(D3DRS_WRAP14, 0);
		//pD3DDevice->SetRenderState(D3DRS_WRAP15, 0);
		//pD3DDevice->SetRenderState(D3DRS_LOCALVIEWER, TRUE);
		//pD3DDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);
		//pD3DDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
		//pD3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
		//pD3DDevice->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_COLOR2);
		pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0x0000000f);
		//pD3DDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		//pD3DDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
		//pD3DDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, 0);
		//pD3DDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, FALSE);
		//pD3DDevice->SetRenderState(D3DRS_TWOSIDEDSTENCILMODE, FALSE);
		//pD3DDevice->SetRenderState(D3DRS_CCW_STENCILFAIL, D3DSTENCILOP_KEEP);
		//pD3DDevice->SetRenderState(D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_KEEP);
		//pD3DDevice->SetRenderState(D3DRS_CCW_STENCILPASS, D3DSTENCILOP_KEEP);
		//pD3DDevice->SetRenderState(D3DRS_CCW_STENCILFUNC, D3DCMP_ALWAYS);
		//pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE1, 0x0000000f);
		//pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE2, 0x0000000f);
		//pD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE3, 0x0000000f);
		//pD3DDevice->SetRenderState(D3DRS_BLENDFACTOR, 0xffffffff);
		//pD3DDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, 0);
		//pD3DDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);
		pD3DDevice->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_ONE);
		pD3DDevice->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ZERO);
		//pD3DDevice->SetRenderState(D3DRS_BLENDOPALPHA, D3DBLENDOP_ADD);

		//pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		//pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		//pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
		//pD3DDevice->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0x00000000);
		pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		pD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		//pD3DDevice->SetSamplerState(0, D3DSAMP_MIPMAPLODBIAS, 0);
		//pD3DDevice->SetSamplerState(0, D3DSAMP_MAXMIPLEVEL, 0);
		//pD3DDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 1);
		//pD3DDevice->SetSamplerState(0, D3DSAMP_SRGBTEXTURE, 0);
		//pD3DDevice->SetSamplerState(0, D3DSAMP_ELEMENTINDEX, 0);

		pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
		pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
		pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
		//pD3DDevice->SetTextureStageState(0, D3DTSS_BUMPENVMAT00, 0);
		//pD3DDevice->SetTextureStageState(0, D3DTSS_BUMPENVMAT01, 0);
		//pD3DDevice->SetTextureStageState(0, D3DTSS_BUMPENVMAT10, 0);
		//pD3DDevice->SetTextureStageState(0, D3DTSS_BUMPENVMAT11, 0);
		pD3DDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
		//pD3DDevice->SetTextureStageState(0, D3DTSS_BUMPENVLSCALE, 0);
		//pD3DDevice->SetTextureStageState(0, D3DTSS_BUMPENVLOFFSET, 0);
		pD3DDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
		//pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG0, D3DTA_CURRENT);
		//pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG0, D3DTA_CURRENT);
		//pD3DDevice->SetTextureStageState(0, D3DTSS_RESULTARG, D3DTA_CURRENT);

		pD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		//pD3DDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		//pD3DDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
		pD3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		//pD3DDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		//pD3DDevice->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
		//pD3DDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT00, 0);
		//pD3DDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT01, 0);
		//pD3DDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT10, 0);
		//pD3DDevice->SetTextureStageState(1, D3DTSS_BUMPENVMAT11, 0);
		//pD3DDevice->SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 0);
		//pD3DDevice->SetTextureStageState(1, D3DTSS_BUMPENVLSCALE, 0);
		//pD3DDevice->SetTextureStageState(1, D3DTSS_BUMPENVLOFFSET, 0);
		//pD3DDevice->SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
		//pD3DDevice->SetTextureStageState(1, D3DTSS_COLORARG0, D3DTA_CURRENT);
		//pD3DDevice->SetTextureStageState(1, D3DTSS_ALPHAARG0, D3DTA_CURRENT);
		//pD3DDevice->SetTextureStageState(1, D3DTSS_RESULTARG, D3DTA_CURRENT);

		pD3DDevice->EndStateBlock(&ms_ppStateBlocks[STATE_CURRENT]);

		//----------------------------------------------------------------------------
		// Text drawing state block...

		pD3DDevice->BeginStateBlock();

		pD3DDevice->SetRenderState( D3DRS_ZENABLE,		  FALSE );
		pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pD3DDevice->SetRenderState( D3DRS_SRCBLEND,	      D3DBLEND_SRCALPHA );
		pD3DDevice->SetRenderState( D3DRS_DESTBLEND,	      D3DBLEND_INVSRCALPHA );
		pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
		pD3DDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
		pD3DDevice->SetRenderState( D3DRS_ALPHAFUNC,        D3DCMP_GREATEREQUAL );
		pD3DDevice->SetRenderState( D3DRS_FILLMODE,         D3DFILL_SOLID );
		pD3DDevice->SetRenderState( D3DRS_CULLMODE,         D3DCULL_CCW );
		pD3DDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
		pD3DDevice->SetRenderState( D3DRS_CLIPPING,         FALSE );
		pD3DDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,  FALSE );
		pD3DDevice->SetRenderState( D3DRS_VERTEXBLEND,      D3DVBF_DISABLE );
		pD3DDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
		pD3DDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );
		pD3DDevice->SetRenderState( D3DRS_COLORWRITEENABLE,
			D3DCOLORWRITEENABLE_RED  | D3DCOLORWRITEENABLE_GREEN |
			D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
		pD3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		pD3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		pD3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		pD3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		pD3DDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

		pD3DDevice->EndStateBlock(&ms_ppStateBlocks[STATE_TEXT]);

		//----------------------------------------------------------------------------
		// Text border drawing state block...

		pD3DDevice->BeginStateBlock();

		pD3DDevice->SetRenderState( D3DRS_ZENABLE,		  FALSE );
		pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pD3DDevice->SetRenderState( D3DRS_SRCBLEND,	      D3DBLEND_SRCALPHA );
		pD3DDevice->SetRenderState( D3DRS_DESTBLEND,	      D3DBLEND_INVSRCALPHA );
		pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
		pD3DDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
		pD3DDevice->SetRenderState( D3DRS_ALPHAFUNC,        D3DCMP_GREATEREQUAL );
		pD3DDevice->SetRenderState( D3DRS_FILLMODE,         D3DFILL_SOLID );
		pD3DDevice->SetRenderState( D3DRS_CULLMODE,         D3DCULL_CCW );
		pD3DDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
		pD3DDevice->SetRenderState( D3DRS_CLIPPING,         FALSE );
		pD3DDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,  FALSE );
		pD3DDevice->SetRenderState( D3DRS_VERTEXBLEND,      D3DVBF_DISABLE );
		pD3DDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
		pD3DDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );
		pD3DDevice->SetRenderState( D3DRS_COLORWRITEENABLE,
			D3DCOLORWRITEENABLE_RED  | D3DCOLORWRITEENABLE_GREEN |
			D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );
		// xx
		pD3DDevice->SetRenderState( D3DRS_TEXTUREFACTOR, 0xFF000000 );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TFACTOR );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
		pD3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		pD3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		pD3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		pD3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		pD3DDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

		pD3DDevice->EndStateBlock(&ms_ppStateBlocks[STATE_TEXT_BORDER]);

		//----------------------------------------------------------------------------
		// Sprite state block

		pD3DDevice->BeginStateBlock();

		pD3DDevice->SetRenderState( D3DRS_ZENABLE,		  FALSE );
		pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		pD3DDevice->SetRenderState( D3DRS_SRCBLEND,	      D3DBLEND_SRCALPHA );
		pD3DDevice->SetRenderState( D3DRS_DESTBLEND,	      D3DBLEND_INVSRCALPHA );
		pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE );
		//pD3DDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
		//pD3DDevice->SetRenderState( D3DRS_ALPHAFUNC,        D3DCMP_GREATEREQUAL );
		pD3DDevice->SetRenderState( D3DRS_FILLMODE,         D3DFILL_SOLID );
		pD3DDevice->SetRenderState( D3DRS_CULLMODE,         D3DCULL_CCW );
		pD3DDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
		pD3DDevice->SetRenderState( D3DRS_CLIPPING,         FALSE );
		pD3DDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,  FALSE );
		pD3DDevice->SetRenderState( D3DRS_VERTEXBLEND,      D3DVBF_DISABLE );
		pD3DDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
		pD3DDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );
		pD3DDevice->SetRenderState( D3DRS_COLORWRITEENABLE,
			D3DCOLORWRITEENABLE_RED  | D3DCOLORWRITEENABLE_GREEN |
			D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
		pD3DDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
		//pD3DDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
		//pD3DDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		//pD3DDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		//pD3DDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		//pD3DDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

		pD3DDevice->EndStateBlock(&ms_ppStateBlocks[STATE_SPRITE]);
		
	}



}