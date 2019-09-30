
#include "fontmanager.h"

namespace GUI
{

	CFont *CFontManager::ms_ppFont[CFontManager::MAX_FONTS];

	void CFontManager::Initialize()
	{
		for(int i=0; i<MAX_FONTS; i++)
			ms_ppFont[i] = NULL;
	}

	void CFontManager::Shutdown()
	{
		for(int i=0; i<MAX_FONTS; i++)
			delete ms_ppFont[i];
	}

	CFont *CFontManager::GetFont(CFontManager::FontType font)
	{
		if (!ms_ppFont[font])
		{
			switch(font)
			{
				case FontType::FONT_ARIAL_NORMAL_8:
					ms_ppFont[font] = new CFont("Arial", 8, false, false);
					break;
				case FontType::FONT_ARIAL_BOLD_8:
					ms_ppFont[font] = new CFont("Arial", 8, true, false);
					break;
				case FontType::FONT_ARIAL_BOLD_10:
					ms_ppFont[font] = new CFont("Arial", 10, true, false);
					break;
				case FontType::FONT_ARIAL_BOLD_12:
					ms_ppFont[font] = new CFont("Arial", 12, true, false);
					break;
				case FontType::FONT_GTAWEAPON3_20:
					ms_ppFont[font] = new CFont("GTAWEAPON3", 20, false, false);
					break;
				case FontType::FONT_ARIAL_NORMAL_BORDER_8:
					ms_ppFont[font] = new CFont("Arial", 8, false, false, true);
					break;
				case FontType::FONT_ARIAL_BORDER_8:
					ms_ppFont[font] = new CFont("Arial", 8, true, false, true);
					break;
				case FontType::FONT_ARIAL_BORDER_10:
					ms_ppFont[font] = new CFont("Arial", 10, true, false, true);
					break;
				case FontType::FONT_ARIAL_BORDER_12:
					ms_ppFont[font] = new CFont("Arial", 12, true, false, true);
					break;
				case FontType::FONT_GTAWEAPON3_BORDER_20:
					ms_ppFont[font] = new CFont("GTAWEAPON3", 20, false, false, true);
					break;
			}
		}
		
		return ms_ppFont[font];
	}

	void CFontManager::OnDestroyDevice()
	{
		for(int i=0; i<MAX_FONTS; i++)
			if (ms_ppFont[i])
				ms_ppFont[i]->OnDestroyDevice();
	}

	void CFontManager::OnRestoreDevice()
	{
		for(int i=0; i<MAX_FONTS; i++)
			if (ms_ppFont[i])
				ms_ppFont[i]->OnRestoreDevice();
	}



}