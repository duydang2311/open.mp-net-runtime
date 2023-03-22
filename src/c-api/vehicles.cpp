#pragma once

#include "src/c-api/vehicles.hpp"

void Vehicle_SetSpawnData(IEntity* vehicle, const CVehicleSpawnData& data)
{
	static_cast<IVehicle*>(vehicle)->setSpawnData(VehicleSpawnData {
		Seconds(data.respawnDelay),
		data.modelID,
		data.position,
		data.zRotation,
		data.colour1,
		data.colour2,
		data.siren,
		data.interior,
	});
}

CVehicleSpawnData Vehicle_GetSpawnData(IEntity* vehicle)
{
	auto data = static_cast<IVehicle*>(vehicle)->getSpawnData();
	return CVehicleSpawnData {
		data.respawnDelay.count(),
		data.modelID,
		data.position,
		data.zRotation,
		data.colour1,
		data.colour2,
		data.siren,
		data.interior
	};
}

bool Vehicle_IsStreamedInForPlayer(IEntity* vehicle, IEntity* player)
{
	return static_cast<IVehicle*>(vehicle)->isStreamedInForPlayer(*static_cast<IPlayer*>(player));
}

void Vehicle_StreamInForPlayer(IEntity* vehicle, IEntity* player)
{
	static_cast<IVehicle*>(vehicle)->streamInForPlayer(*static_cast<IPlayer*>(player));
}

void Vehicle_StreamOutForPlayer(IEntity* vehicle, IEntity* player)
{
	static_cast<IVehicle*>(vehicle)->streamOutForPlayer(*static_cast<IPlayer*>(player));
}

void Vehicle_SetColour(IEntity* vehicle, int col1, int col2)
{
	static_cast<IVehicle*>(vehicle)->setColour(col1, col2);
}

void Vehicle_GetColour(IEntity* vehicle, int* primaryColor, int* secondaryColor)
{
	auto pair = static_cast<IVehicle*>(vehicle)->getColour();
	*primaryColor = pair.first;
	*secondaryColor = pair.second;
}

void Vehicle_SetHealth(IEntity* vehicle, float health)
{
	static_cast<IVehicle*>(vehicle)->setHealth(health);
}

float Vehicle_GetHealth(IEntity* vehicle)
{
	return static_cast<IVehicle*>(vehicle)->getHealth();
}

bool Vehicle_UpdateFromDriverSync(IEntity* vehicle, const CVehicleDriverSyncPacket& vehicleSync, IEntity* player)
{
	auto packet = VehicleDriverSyncPacket {
		vehicleSync.PlayerID,
		vehicleSync.VehicleID,
		vehicleSync.LeftRight,
		vehicleSync.UpDown,
		vehicleSync.Keys,
		GTAQuat(vehicleSync.Rotation),
		vehicleSync.Position,
		vehicleSync.Velocity,
		vehicleSync.Health,
		vehicleSync.PlayerHealthArmour,
		vehicleSync.Siren,
		vehicleSync.LandingGear,
		vehicleSync.TrailerID,
		vehicleSync.HasTrailer
	};
	packet.AdditionalKeyWeapon = vehicleSync.AdditionalKeyWeapon;
	packet.WeaponID = vehicleSync.WeaponID;
	packet.AdditionalKey = vehicleSync.AdditionalKey;
	packet.HydraThrustAngle = vehicleSync.HydraThrustAngle;
	packet.TrainSpeed = vehicleSync.TrainSpeed;
	return static_cast<IVehicle*>(vehicle)->updateFromDriverSync(packet, *static_cast<IPlayer*>(player));
}

bool Vehicle_UpdateFromPassengerSync(IEntity* vehicle, const VehiclePassengerSyncPacket& passengerSync, IEntity* player)
{
	return static_cast<IVehicle*>(vehicle)->updateFromPassengerSync(passengerSync, *static_cast<IPlayer*>(player));
}

bool Vehicle_UpdateFromUnoccupied(IEntity* vehicle, const VehicleUnoccupiedSyncPacket& unoccupiedSync, IEntity* player)
{
	return static_cast<IVehicle*>(vehicle)->updateFromUnoccupied(unoccupiedSync, *static_cast<IPlayer*>(player));
}

bool Vehicle_UpdateFromTrailerSync(IEntity* vehicle, const VehicleTrailerSyncPacket& unoccupiedSync, IEntity* player)
{
	return static_cast<IVehicle*>(vehicle)->updateFromTrailerSync(unoccupiedSync, *static_cast<IPlayer*>(player));
}

std::size_t Vehicle_StreamedForPlayers(IEntity* vehicle, IEntity*** players_ptr)
{
	auto players = static_cast<IVehicle*>(vehicle)->streamedForPlayers();
	auto size = players.size();
	*players_ptr = new IEntity*[size];
	std::size_t i = 0;
	for (auto ptr : players)
	{
		(*players_ptr)[i++] = ptr;
	}
	return size;
}

IEntity* Vehicle_GetDriver(IEntity* vehicle)
{
	return static_cast<IVehicle*>(vehicle)->getDriver();
}

std::size_t Vehicle_GetPassengers(IEntity* vehicle, IEntity*** passengers_ptr)
{
	auto passengers = static_cast<IVehicle*>(vehicle)->getPassengers();
	auto size = passengers.size();
	*passengers_ptr = new IEntity*[size];
	std::size_t i = 0;
	for (auto ptr : passengers)
	{
		(*passengers_ptr)[i++] = ptr;
	}
	return size;
}

void Vehicle_SetPlate(IEntity* vehicle, const char* plate)
{
	static_cast<IVehicle*>(vehicle)->setPlate(plate);
}

const char* Vehicle_GetPlate(IEntity* vehicle)
{
	return static_cast<IVehicle*>(vehicle)->getPlate().data();
}

void Vehicle_SetDamageStatus(IEntity* vehicle, int panelStatus, int doorStatus, uint8_t lightStatus, uint8_t tyreStatus, IEntity* vehicleUpdater)
{
	static_cast<IVehicle*>(vehicle)->setDamageStatus(panelStatus, doorStatus, lightStatus, tyreStatus, static_cast<IPlayer*>(vehicleUpdater));
}

void Vehicle_GetDamageStatus(IEntity* vehicle, int* panelStatus, int* doorStatus, int* lightStatus, int* tyreStatus)
{
	static_cast<IVehicle*>(vehicle)->getDamageStatus(*panelStatus, *doorStatus, *lightStatus, *tyreStatus);
}

void Vehicle_SetPaintJob(IEntity* vehicle, int paintjob)
{
	static_cast<IVehicle*>(vehicle)->setPaintJob(paintjob);
}

int Vehicle_GetPaintJob(IEntity* vehicle)
{
	return static_cast<IVehicle*>(vehicle)->getPaintJob();
}

void Vehicle_AddComponent(IEntity* vehicle, int component)
{
	static_cast<IVehicle*>(vehicle)->addComponent(component);
}

int Vehicle_GetComponentInSlot(IEntity* vehicle, int slot)
{
	return static_cast<IVehicle*>(vehicle)->getComponentInSlot(slot);
}

void Vehicle_RemoveComponent(IEntity* vehicle, int component)
{
	static_cast<IVehicle*>(vehicle)->removeComponent(component);
}

void Vehicle_PutPlayer(IEntity* vehicle, IEntity* player, int seatId)
{
	static_cast<IVehicle*>(vehicle)->putPlayer(*static_cast<IPlayer*>(player), seatId);
}

void Vehicle_SetZAngle(IEntity* vehicle, float angle)
{
	static_cast<IVehicle*>(vehicle)->setZAngle(angle);
}

float Vehicle_GetZAngle(IEntity* vehicle)
{
	return static_cast<IVehicle*>(vehicle)->getZAngle();
}

// Set the vehicle's parameters.
void Vehicle_SetParams(IEntity* vehicle, const CVehicleParams& params)
{
	static_cast<IVehicle*>(vehicle)->setParams(VehicleParams {
		params.engine,
		params.lights,
		params.alarm,
		params.doors,
		params.bonnet,
		params.boot,
		params.objective,
		params.siren,
		params.doorDriver,
		params.doorPassenger,
		params.doorBackLeft,
		params.doorBackRight,
		params.windowDriver,
		params.windowPassenger,
		params.windowBackLeft,
		params.windowBackRight });
}

// Set the vehicle's parameters for a specific player.
void Vehicle_SetParamsForPlayer(IEntity* vehicle, IEntity* player, const CVehicleParams& params)
{
	static_cast<IVehicle*>(vehicle)->setParamsForPlayer(*static_cast<IPlayer*>(player), VehicleParams { params.engine, params.lights, params.alarm, params.doors, params.bonnet, params.boot, params.objective, params.siren, params.doorDriver, params.doorPassenger, params.doorBackLeft, params.doorBackRight, params.windowDriver, params.windowPassenger, params.windowBackLeft, params.windowBackRight });
}

// Get the vehicle's parameters.
CVehicleParams Vehicle_GetParams(IEntity* vehicle)
{
	auto params = static_cast<IVehicle*>(vehicle)->getParams();
	return CVehicleParams {
		params.engine,
		params.lights,
		params.alarm,
		params.doors,
		params.bonnet,
		params.boot,
		params.objective,
		params.siren,
		params.doorDriver,
		params.doorPassenger,
		params.doorBackLeft,
		params.doorBackRight,
		params.windowDriver,
		params.windowPassenger,
		params.windowBackLeft,
		params.windowBackRight
	};
}

bool Vehicle_IsDead(IEntity* vehicle)
{
	return static_cast<IVehicle*>(vehicle)->isDead();
}

void Vehicle_Respawn(IEntity* vehicle)
{
	static_cast<IVehicle*>(vehicle)->respawn();
}

int64_t Vehicle_GetRespawnDelay(IEntity* vehicle)
{
	return static_cast<IVehicle*>(vehicle)->getRespawnDelay().count();
}

void Vehicle_SetRespawnDelay(IEntity* vehicle, int64_t seconds)
{
	static_cast<IVehicle*>(vehicle)->setRespawnDelay(Seconds(seconds));
}

bool Vehicle_IsRespawning(IEntity* vehicle)
{
	return static_cast<IVehicle*>(vehicle)->isRespawning();
}

// Sets (links) the vehicle to an interior.
void Vehicle_SetInterior(IEntity* vehicle, int interior)
{
	static_cast<IVehicle*>(vehicle)->setInterior(interior);
}

// Gets the vehicle's interior.
int Vehicle_GetInterior(IEntity* vehicle)
{
	return static_cast<IVehicle*>(vehicle)->getInterior();
}

void Vehicle_AttachTrailer(IEntity* vehicle, IEntity* trailer)
{
	static_cast<IVehicle*>(vehicle)->attachTrailer(*static_cast<IVehicle*>(trailer));
}

void Vehicle_DetachTrailer(IEntity* vehicle)
{
	static_cast<IVehicle*>(vehicle)->detachTrailer();
}

bool Vehicle_IsTrailer(IEntity* vehicle)
{
	return static_cast<IVehicle*>(vehicle)->isTrailer();
}

IEntity* Vehicle_GetTrailer(IEntity* vehicle)
{
	return static_cast<IVehicle*>(vehicle)->getTrailer();
}

IEntity* Vehicle_GetCab(IEntity* vehicle)
{
	return static_cast<IVehicle*>(vehicle)->getCab();
}

void Vehicle_Repair(IEntity* vehicle)
{
	static_cast<IVehicle*>(vehicle)->repair();
}

void Vehicle_AddCarriage(IEntity* vehicle, IEntity* carriage, int pos)
{
	static_cast<IVehicle*>(vehicle)->addCarriage(static_cast<IVehicle*>(carriage), pos);
}

void Vehicle_UpdateCarriage(IEntity* vehicle, Vector3 pos, Vector3 veloc)
{
	static_cast<IVehicle*>(vehicle)->updateCarriage(pos, veloc);
}

std::size_t Vehicle_GetCarriages(IEntity* vehicle, IEntity*** carriages_ptr)
{
	auto carriages = static_cast<IVehicle*>(vehicle)->getCarriages();
	auto size = carriages.size();
	*carriages_ptr = new IEntity*[size];
	std::size_t i = 0;
	for (auto ptr : carriages)
	{
		(*carriages_ptr)[i++] = ptr;
	}
	return size;
}

void Vehicle_SetVelocity(IEntity* vehicle, Vector3 velocity)
{
	static_cast<IVehicle*>(vehicle)->setVelocity(velocity);
}

Vector3 Vehicle_GetVelocity(IEntity* vehicle)
{
	return static_cast<IVehicle*>(vehicle)->getVelocity();
}

void Vehicle_SetAngularVelocity(IEntity* vehicle, Vector3 velocity)
{
	static_cast<IVehicle*>(vehicle)->setAngularVelocity(velocity);
}

Vector3 Vehicle_GetAngularVelocity(IEntity* vehicle)
{
	return static_cast<IVehicle*>(vehicle)->getAngularVelocity();
}

int Vehicle_GetModel(IEntity* vehicle)
{
	return static_cast<IVehicle*>(vehicle)->getModel();
}

uint8_t Vehicle_GetLandingGearState(IEntity* vehicle)
{
	return static_cast<IVehicle*>(vehicle)->getLandingGearState();
}

bool Vehicle_HasBeenOccupied(IEntity* vehicle)
{
	return static_cast<IVehicle*>(vehicle)->hasBeenOccupied();
}

int64_t Vehicle_GetLastOccupiedTime(IEntity* vehicle)
{
	return static_cast<IVehicle*>(vehicle)->getLastOccupiedTime().time_since_epoch().count();
}

int64_t Vehicle_GetLastSpawnTime(IEntity* vehicle)
{
	return static_cast<IVehicle*>(vehicle)->getLastSpawnTime().time_since_epoch().count();
}

bool Vehicle_IsOccupied(IEntity* vehicle)
{
	return static_cast<IVehicle*>(vehicle)->isOccupied();
}

void Vehicle_SetSiren(IEntity* vehicle, bool status)
{
	static_cast<IVehicle*>(vehicle)->setSiren(status);
}

uint8_t Vehicle_GetSirenState(IEntity* vehicle)
{
	return static_cast<IVehicle*>(vehicle)->getSirenState();
}

uint32_t Vehicle_GetHydraThrustAngle(IEntity* vehicle)
{
	return static_cast<IVehicle*>(vehicle)->getHydraThrustAngle();
}

float Vehicle_GetTrainSpeed(IEntity* vehicle)
{
	return static_cast<IVehicle*>(vehicle)->getTrainSpeed();
}

int Vehicle_GetLastDriverPoolID(IEntity* vehicle)
{
	return static_cast<IVehicle*>(vehicle)->getLastDriverPoolID();
}
