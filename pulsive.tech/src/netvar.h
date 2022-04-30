#pragma once

#include "fnv.h"
#include "datatable.h"

#include <unordered_map>

//call once to dumo
void SetupNetvars();

//recursive dump
void Dumb(const char* baseClass, RecvTable* table, std::int32_t offset = 0);

//hold offsets
inline std::unordered_map<std::uint32_t, std::uint32_t> netvars;

#define NETVAR(func_name, netvar, type) type& func_name() \
{ \
	static auto offset = netvars[fnv::HashConst(netvar)]; \
	return *reinterpret_cast<type*>(std::uint32_t(this) + offset); \
}


