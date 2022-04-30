#define Win32_LEAN_AND_HEAN
#include <Windows.h>

#include <thread>
#include <cstdint>
#include <iostream>

#include "hooks.h"
#include "interfaces.h"
#include "cfg_file.h"


template <typename T>
T* GetInterface(const char* name, const char* library)
{
	const auto handle = GetModuleHandle(library);
	if (!handle)
		return nullptr;

	const auto functionAddress = GetProcAddress(handle, "CreateInterface");

	if (!functionAddress)
		return nullptr;

	using Fn = T * (*)(const char*, int*);
	const auto CreateInterface = reinterpret_cast<Fn>(functionAddress);

	return CreateInterface(name, nullptr);
}
// setup
void Setup(const HMODULE instance)
{
	try 
	{
		gui::Setup();
		hooks::Setup(); 
	}
	catch (const std::exception& error)
	{
		MessageBeep(MB_ICONERROR);
		MessageBox(
			0,
			error.what(),
			"pulsive.tech failed to load correctly!",
			MB_OK | MB_ICONEXCLAMATION
		);

		goto UNLOAD;

	}

		client = GetInterface<IClient>("VClient018", "client.dll");
		entitylist = GetInterface<IEntityList>("VClientEntityList003", "client.dll");
		SetupNetvars();
		for (auto i = 1; i <= 64; ++i)
		{
			const auto entity = entitylist->GetClientEntity(i);

			if (!entity)
				continue;

			if (config::radar) {
				entity->Spotted() = true;
			}
			else {
				entity->Spotted() = false;
			}
		}
	while (!GetAsyncKeyState(VK_END))
	{

		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}

UNLOAD: 
	hooks::Destroy();
	gui::Destroy(); 
	FreeLibraryAndExitThread(instance, 0);
}


//entry point
BOOL WINAPI DllMain(
	const HMODULE instance,
	const std::uintptr_t reason,
	const void* reserved
)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(instance);

		const auto thread = CreateThread(
			nullptr,
			0,
			reinterpret_cast<LPTHREAD_START_ROUTINE>(Setup),
			instance,
			0,
			nullptr
		);
			if (thread)
				CloseHandle(thread);
	}
	return TRUE;
}