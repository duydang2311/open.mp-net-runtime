/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2022, open.mp team and contributors.
 */

#pragma once
#include "Server/Components/Objects/objects.hpp"
#include "Server/Components/TextDraws/textdraws.hpp"
#include "Server/Components/Vehicles/vehicles.hpp"
#include "core.hpp"

#include "player.hpp"
#include "src/c-api/player.hpp"
#include "src/nethost.hpp"
#include "values.hpp"

using TickDelegate = void* (*)();
class MainComponent final : public IComponent,
							public PlayerSpawnEventHandler,
							public PlayerConnectEventHandler,
							public CoreEventHandler
{
private:
	inline static MainComponent* instance_ = nullptr;

	ICore* core_ = nullptr;
	ITextDrawsComponent* text_draw_component_ = nullptr;
	IVehiclesComponent* vehicles_component_ = nullptr;
	IObjectsComponent* objects_component_ = nullptr;
	NetHost* host_ = nullptr;
	TickDelegate tick_delegate_ = nullptr;

public:
	PROVIDE_UID(0xEC3F51ACBB78719D);

	~MainComponent();

	StringView componentName() const override
	{
		return "open.mp-net";
	}

	SemanticVersion componentVersion() const override
	{
		return SemanticVersion(0, 0, 1, 0);
	}

	static MainComponent* getInstance();
	ICore* getCore();
	ITextDrawsComponent* getTextDrawComponent();
	IVehiclesComponent* getVehiclesComponent();
	IObjectsComponent* getObjectsComponent();
	void setTickDelegate(TickDelegate ptr);
	void free() override;

	void reset() override;
	void onLoad(ICore* c) override;
	void onInit(IComponentList* components) override;
	void onTick(Microseconds elapsed, TimePoint now) override;
	void onFree(IComponent* component) override;
	bool onPlayerRequestSpawn(IPlayer& player) override;
};
