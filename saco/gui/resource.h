#pragma once

namespace GUI
{

	class IResource
	{
	public:
		virtual void OnDestroyDevice() = 0;
		virtual void OnRestoreDevice() = 0;
	};

}