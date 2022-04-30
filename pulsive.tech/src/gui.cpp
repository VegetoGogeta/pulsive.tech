#include "gui.h"

#include "../ext/imgui/imgui.h"
#include "../ext/imgui/imgui_impl_win32.h"
#include "../ext/imgui/imgui_impl_dx9.h"

#include <stdexcept>
#include "cfg_file.h"




extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT message,
	WPARAM wideParam,
	LPARAM longParam
);

//window process
LRESULT CALLBACK WindowProcess(
	HWND window,
	UINT message,
	WPARAM wideParam,
	LPARAM longParam
);

bool gui::SetupWindowClass(const char* windowClassName) noexcept
{
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = DefWindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.hIcon = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = windowClassName;
	windowClass.hIconSm = NULL;

	//register
	if (!RegisterClassEx(&windowClass))
		return false;

	return true;
}

void gui::DestroyWindowClass() noexcept
{
	UnregisterClass(
		windowClass.lpszClassName,
		windowClass.hInstance
	);
}

bool gui::SetupWindow(const char* windowName) noexcept
{
	//creat temp window
	window = CreateWindow(
		windowClass.lpszClassName,
		windowName,
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		100,
		100,
		0,
		0,
		windowClass.hInstance,
		0
	);

	if (!window)
		return false;

	return true;
}

void gui::DestroyWindow() noexcept
{
	if (window)
		DestroyWindow(window);
}

bool gui::SetupDirectX() noexcept
{
	const auto handle = GetModuleHandle("d3d9.dll");

	if (!handle)
		return false;

	using CreateFn = LPDIRECT3D9(__stdcall*)(UINT);

	const auto create = reinterpret_cast<CreateFn>(GetProcAddress(
		handle,
		"Direct3DCreate9"
	));

	if (!create)
		return false;

	d3d9 = create(D3D_SDK_VERSION);

	if (!d3d9)
		return false;

	D3DPRESENT_PARAMETERS params = {  };
	params.BackBufferWidth = 0;
	params.BackBufferHeight = 0;
	params.BackBufferFormat = D3DFMT_UNKNOWN;
	params.BackBufferCount = 0;
	params.MultiSampleType = D3DMULTISAMPLE_NONE;
	params.MultiSampleQuality = NULL;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.hDeviceWindow = window;
	params.Windowed = 1;
	params.EnableAutoDepthStencil = 0;
	params.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
	params.Flags = NULL;
	params.FullScreen_RefreshRateInHz = 0;
	params.PresentationInterval = 0;

	if (d3d9->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_NULLREF,
		window,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT,
		&params,
		&device
	) < 0) return false;

	return true;
}

void gui::DestroyDirectX() noexcept
{
	if (device)
	{
		device->Release();
		device = NULL;
	}

	if (d3d9)
	{
		d3d9->Release();
		d3d9 = NULL;
	}
}

void gui::Setup()
{
	if (!SetupWindowClass("hackClass001"))
		throw std::runtime_error("Failed to create window class.");

	if (!SetupWindow("Hack Window"))
		throw std::runtime_error("Failed to create window");

	if (!SetupDirectX())
		throw std::runtime_error("Failed to create device");

	DestroyWindow();
	DestroyWindowClass();
}

void gui::SetupMenu(LPDIRECT3DDEVICE9 device) noexcept
{
	auto params = D3DDEVICE_CREATION_PARAMETERS{ };
	device->GetCreationParameters(&params);

	window = params.hFocusWindow;

	originalWindowProcess = reinterpret_cast<WNDPROC>(
		SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProcess))
		);

	ImGui::CreateContext();
	ImGui::StyleColorsDark();
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.157f, 0.157f, 0.157f, 0.941f));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.000f, 0.000f, 0.000f, 0.540f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.541f, 0.541f, 0.541f, 1.000f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.442f, 0.442f, 0.442f, 0.670f));
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.611f, 0.584f, 0.769f, 1.000f));
		ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(0.899f, 0.899f, 0.899f, 1.000f));
		ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.757f, 0.757f, 0.757f, 0.400f));
		ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.541f, 0.541f, 0.541f, 1.000f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.757f, 0.757f, 0.757f, 0.400f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.541f, 0.541f, 0.541f, 1.000f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.541f, 0.541f, 0.541f, 1.000f));
		ImGui::PushStyleColor(ImGuiCol_Tab, ImVec4(0.489f, 0.428f, 0.591f, 1.000f));
		ImGui::PushStyleColor(ImGuiCol_TabHovered, ImVec4(0.543f, 0.369f, 0.735f, 1.000f));
		ImGui::PushStyleColor(ImGuiCol_TabActive, ImVec4(0.366f, 0.072f, 0.691f, 1.000f));

		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.80f);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(11.0f,6.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, 8.00f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(20.0f, 6.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowTitleAlign, ImVec2(0.50f, 0.50f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12.0f, 2.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(10.0f, 2.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 21.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 7.0);


	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);

		setup = true;
}

void gui::Destroy() noexcept
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	SetWindowLongPtr(
		window,
		GWLP_WNDPROC,
		reinterpret_cast<LONG_PTR>(originalWindowProcess)
	);

	DestroyDirectX();
}

void gui::Render() noexcept
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (ImGui::Begin("pulsive.tech", &open))
	{
		if (ImGui::BeginTabBar("Categories"))
		{
			if (ImGui::BeginTabItem("Exploits"))
			{

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Visuals"))
			{
				ImGui::Checkbox("Radar Hack", &config::radar);
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::End();
	}
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

}

LRESULT CALLBACK WindowProcess(
	HWND window,
	UINT message,
	WPARAM wideParam,
	LPARAM longParam
)
{
	//toggle menu
	if (GetAsyncKeyState(VK_DELETE) & 1)
		gui::open = !gui::open;

	//pass messages to gui
	if (gui::open && ImGui_ImplWin32_WndProcHandler(
		window,
		message,
		wideParam,
		longParam
	)) return 1L;

	return CallWindowProc(
		gui::originalWindowProcess,
		window,
		message,
		wideParam,
		longParam
	);
}