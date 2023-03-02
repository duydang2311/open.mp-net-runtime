/*
 *  This Source Code Form is subject to the terms of the Mozilla Public License,
 *  v. 2.0. If a copy of the MPL was not distributed with this file, You can
 *  obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  The original code is copyright (c) 2022, open.mp team and contributors.
 */

#include "Server/Components/Objects/objects.hpp"
#include "Server/Components/TextDraws/textdraws.hpp"
#include "Server/Components/Vehicles/vehicles.hpp"
#include "core.hpp"

#include "src/component.hpp"
#include "src/nethost.hpp"
#include <chrono>
#include <ratio>

MainComponent::~MainComponent()
{
	core_->getPlayers().getPlayerConnectDispatcher().removeEventHandler(this);
	core_->getPlayers().getPlayerSpawnDispatcher().removeEventHandler(this);
}

void MainComponent::onLoad(ICore* c)
{
	core_ = c;
	instance_ = this;

	host_ = NetHost::getInstance();

	core_->getPlayers().getPlayerConnectDispatcher().addEventHandler(this);
	core_->getPlayers().getPlayerSpawnDispatcher().addEventHandler(this);

	assert(host_->invokeDllEntry());

	core_->logLn(LogLevel::Message, "open.mp-net loaded successfully, now starting gamemode");
	host_->invokeScriptStartEvent();
}

void MainComponent::onInit(IComponentList* components)
{
	text_draw_component_ = components->queryComponent<ITextDrawsComponent>();
	vehicles_component_ = components->queryComponent<IVehiclesComponent>();
	objects_component_ = components->queryComponent<IObjectsComponent>();
}

void MainComponent::onFree(IComponent* component)
{
	if (component == text_draw_component_)
	{
		text_draw_component_ = nullptr;
	}
	else if (component == vehicles_component_)
	{
		vehicles_component_ = nullptr;
	}
	else if (component == objects_component_)
	{
		objects_component_ = nullptr;
	}
}

void MainComponent::free()
{
	delete this;
}

void MainComponent::reset()
{
	tick_delegate_ = nullptr;
}

ITextDrawsComponent* MainComponent::getTextDrawComponent()
{
	return text_draw_component_;
}

IVehiclesComponent* MainComponent::getVehiclesComponent()
{
	return vehicles_component_;
}

IObjectsComponent* MainComponent::getObjectsComponent()
{
	return objects_component_;
}

ICore* MainComponent::getCore()
{
	return core_;
}

MainComponent* MainComponent::getInstance()
{
	return instance_;
}

bool MainComponent::onPlayerRequestSpawn(IPlayer& player)
{
	return host_->invokePlayerRequestSpawnEvent(player.getID());
}

void MainComponent::onTick(Microseconds elapsed, TimePoint now)
{
	auto us = elapsed.count();
	if (us > 30000)
	{
		core_->logLn(LogLevel::Warning, "Slow tick event: %lldms", us / 1000);
	}
	tick_delegate_();
}

void MainComponent::setTickDelegate(TickDelegate ptr)
{
	bool first_call = tick_delegate_ == nullptr;
	tick_delegate_ = ptr;
	if (first_call)
	{
		core_->getEventDispatcher().addEventHandler(this);
	}
}
