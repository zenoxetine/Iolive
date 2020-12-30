#pragma once

#include "Ioface/Ioface.hpp"

namespace Iolive {
class IofaceBridge
{
public:
	IofaceBridge() = delete;

	static Ioface& Get() { return s_Ioface; }
	static void UpdateIoface()
	{
		s_Ioface.UpdateAll();
	}

	static bool OpenCamera(int deviceId) { return s_Ioface.OpenCamera(deviceId); }
	static void CloseCamera() { s_Ioface.CloseCamera(); }

	static bool IsCameraOpened() { return s_Ioface.IsCameraOpened(); }

private:
	inline static Ioface s_Ioface = Ioface(); // initialize directly
};
}