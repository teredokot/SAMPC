#pragma once

#include "common.h"
#include <d3d9.h>

namespace GUI
{

	class CStateBlockManager
	{
	private:
		static IDirect3DStateBlock9 *ms_ppStateBlocks[];

	public:

		enum StateBlockType
		{
			STATE_CURRENT,
			STATE_TEXT,
			STATE_TEXT_BORDER,
			STATE_SPRITE,
			// Add your own here...
			MAX_STATES			// always should be the last element
		};

		static void Initialize();
		static void Shutdown();

		static IDirect3DStateBlock9 *GetStateBlock(StateBlockType state);

		static void OnDestroyDevice();
		static void OnRestoreDevice();
	};

}
