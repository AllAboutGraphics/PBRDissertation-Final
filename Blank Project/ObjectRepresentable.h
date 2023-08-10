#pragma once

#include "IMGUI\imgui.h"
#include "IMGUI\imgui_impl_opengl3.h"
#include "IMGUI\imgui_impl_win32.h"

class ObjectRepresentable
{
	public:
		ObjectRepresentable() {}
		virtual ~ObjectRepresentable() {}

		virtual void ImGuiRender(bool shouldShow = true) {}
};