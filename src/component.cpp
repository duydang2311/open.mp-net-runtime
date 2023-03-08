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

	host_->invokeInitializeCore();
	core_->logLn(LogLevel::Message, "open.mp-net core intialized successfully, gamemode now starting");
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

void MainComponent::setTickDelegate(TickDelegate ptr)
{
	bool first_call = tick_delegate_ == nullptr;
	tick_delegate_ = ptr;
	if (first_call)
	{
		core_->getEventDispatcher().addEventHandler(this);
	}
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

void MainComponent::onReady()
{
	host_->invokeOnReady();
}

void MainComponent::onIncomingConnection(IPlayer& player, StringView ipAddress, unsigned short port)
{
	host_->invokeOnIncomingConnection(player, ipAddress, port);
}

void MainComponent::onPlayerConnect(IPlayer& player)
{
	host_->invokeOnPlayerConnect(player);
}

void MainComponent::onPlayerDisconnect(IPlayer& player, PeerDisconnectReason reason)
{
	host_->invokeOnPlayerDisconnect(player, reason);
}

void MainComponent::onPlayerClientInit(IPlayer& player)
{
	host_->invokeOnPlayerClientInit(player);
}

bool MainComponent::onPlayerRequestSpawn(IPlayer& player)
{
	return host_->invokeOnPlayerRequestSpawn(player);
}

void MainComponent::onPlayerSpawn(IPlayer& player)
{
	host_->invokeOnPlayerSpawn(player);
}

void MainComponent::onPlayerStreamIn(IPlayer& player, IPlayer& forPlayer)
{
	host_->invokeOnPlayerStreamIn(player, forPlayer);
}

void MainComponent::onPlayerStreamOut(IPlayer& player, IPlayer& forPlayer)
{
	host_->invokeOnPlayerStreamOut(player, forPlayer);
}

bool MainComponent::onPlayerText(IPlayer& player, StringView message)
{
	return host_->invokeOnPlayerText(player, message);
}

bool MainComponent::onPlayerCommandText(IPlayer& player, StringView message)
{
	return host_->invokeOnPlayerCommandText(player, message);
}

bool MainComponent::onPlayerShotMissed(IPlayer& player, const PlayerBulletData& bulletData)
{
	return host_->invokeOnPlayerShotMissed(player, bulletData);
}

bool MainComponent::onPlayerShotPlayer(IPlayer& player, IPlayer& target, const PlayerBulletData& bulletData)
{
	return host_->invokeOnPlayerShotPlayer(player, target, bulletData);
}

bool MainComponent::onPlayerShotVehicle(IPlayer& player, IVehicle& target, const PlayerBulletData& bulletData)
{
	return host_->invokeOnPlayerShotVehicle(player, target, bulletData);
}

bool MainComponent::onPlayerShotObject(IPlayer& player, IObject& target, const PlayerBulletData& bulletData)
{
	return host_->invokeOnPlayerShotObject(player, target, bulletData);
}

bool MainComponent::onPlayerShotPlayerObject(IPlayer& player, IPlayerObject& target, const PlayerBulletData& bulletData)
{
	return host_->invokeOnPlayerShotPlayerObject(player, target, bulletData);
}

void MainComponent::onPlayerScoreChange(IPlayer& player, int score)
{
	host_->invokeOnPlayerScoreChange(player, score);
}

void MainComponent::onPlayerNameChange(IPlayer& player, StringView oldName)
{
	host_->invokeOnPlayerNameChange(player, oldName);
}

void MainComponent::onPlayerInteriorChange(IPlayer& player, unsigned newInterior, unsigned oldInterior)
{
	host_->invokeOnPlayerInteriorChange(player, newInterior, oldInterior);
}

void MainComponent::onPlayerStateChange(IPlayer& player, PlayerState newState, PlayerState oldState)
{
	host_->invokeOnPlayerStateChange(player, newState, oldState);
}

void MainComponent::onPlayerKeyStateChange(IPlayer& player, uint32_t newKeys, uint32_t oldKeys)
{
	host_->invokeOnPlayerKeyStateChange(player, newKeys, oldKeys);
}

void MainComponent::onPlayerDeath(IPlayer& player, IPlayer* killer, int reason)
{
	host_->invokeOnPlayerDeath(player, killer, reason);
}

void MainComponent::onPlayerTakeDamage(IPlayer& player, IPlayer* from, float amount, unsigned weapon, BodyPart part)
{
	host_->invokeOnPlayerTakeDamage(player, from, amount, weapon, part);
}

void MainComponent::onPlayerGiveDamage(IPlayer& player, IPlayer& to, float amount, unsigned weapon, BodyPart part)
{
	host_->invokeOnPlayerGiveDamage(player, to, amount, weapon, part);
}

void MainComponent::onPlayerClickMap(IPlayer& player, Vector3 pos)
{
	host_->invokeOnPlayerClickMap(player, pos);
}

void MainComponent::onPlayerClickPlayer(IPlayer& player, IPlayer& clicked, PlayerClickSource source)
{
	host_->invokeOnPlayerClickPlayer(player, clicked, source);
}

void MainComponent::onClientCheckResponse(IPlayer& player, int actionType, int address, int results)
{
	host_->invokeOnClientCheckResponse(player, actionType, address, results);
}

bool MainComponent::onPlayerUpdate(IPlayer& player, TimePoint now)
{
	return host_->invokeOnPlayerUpdate(player, now);
}
