#pragma once

#include "common.h"
#include "font.h"

namespace GUI
{

	class CFontManager
	{
	private:
		static CFont *ms_ppFont[];

	public:

		enum FontType
		{
			FONT_ARIAL_NORMAL_8,
			FONT_ARIAL_BOLD_8,
			FONT_ARIAL_BOLD_10,
			FONT_ARIAL_BOLD_12,
			FONT_GTAWEAPON3_20,
			FONT_ARIAL_NORMAL_BORDER_8,
			FONT_ARIAL_BORDER_8,
			FONT_ARIAL_BORDER_10,
			FONT_ARIAL_BORDER_12,
			FONT_GTAWEAPON3_BORDER_20,
			MAX_FONTS			// always should be the last element
		};

		static void Initialize();
		static void Shutdown();

		static CFont *GetFont(FontType font);

		static void OnDestroyDevice();
		static void OnRestoreDevice();
	};

}
