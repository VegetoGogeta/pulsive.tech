#include "netvar.h"

#include "interfaces.h"

#include <ctype.h>
#include <format>


void SetupNetvars()
{
	for (auto clientClass = client->GetAllClasses(); clientClass; clientClass = clientClass->next)
		if (clientClass->recvTable)
			Dumb(clientClass->networkName, clientClass->recvTable);
}

void Dumb(const char* baseClass, RecvTable* table, std::int32_t offset)
{
	for (auto i = 0; i < table->propsCount; ++i)
	{
		const auto prop = &table->props[i];

		if (!prop || isdigit(prop->varName[0]))
			continue;

		if (fnv::Hash(prop->varName) == fnv::HashConst("baseclass"))
			continue;

		if (prop->recvType == SendPropType::DATATABLE &&
			prop->dataTable &&
			prop->dataTable->tableName[0] == 'D')
			Dumb(baseClass, prop->dataTable, offset + prop->offset);

		const auto netvarName = std::format("{}->{}", baseClass, prop->varName);

		netvars[fnv::Hash(netvarName.c_str())] = offset + prop->offset;
	}
}