#pragma once

#include "Server/Components/Vehicles/vehicles.hpp"
#include "src/utils/export.hpp"
#include "src/interop.hpp"

/// Set the inital spawn data of the vehicle
DLL_EXPORT void Vehicle_SetSpawnData(IEntity* vehicle, const VehicleSpawnData& data);

/// Get the initial spawn data of the vehicle
DLL_EXPORT CVehicleSpawnData Vehicle_GetSpawnData(IEntity* vehicle);

/// Checks if player has the vehicle streamed in for themselves
DLL_EXPORT bool Vehicle_IsStreamedInForPlayer(IEntity* vehicle, IEntity* player);

/// Streams in the vehicle for a specific player
DLL_EXPORT void Vehicle_StreamInForPlayer(IEntity* vehicle, IEntity* player);

/// Streams out the vehicle for a specific player
DLL_EXPORT void Vehicle_StreamOutForPlayer(IEntity* vehicle, IEntity* player);

/// Set the vehicle's colour
DLL_EXPORT void Vehicle_SetColour(IEntity* vehicle, int col1, int col2);

/// Get the vehicle's colour
DLL_EXPORT void Vehicle_GetColour(IEntity* vehicle, int* primaryColor, int* secondaryColor);

/// Set the vehicle's health
DLL_EXPORT void Vehicle_SetHealth(IEntity* vehicle, float health);

/// Get the vehicle's health
DLL_EXPORT float Vehicle_GetHealth(IEntity* vehicle);

/// Update the vehicle from a sync packet
DLL_EXPORT bool Vehicle_UpdateFromDriverSync(IEntity* vehicle, const VehicleDriverSyncPacket& vehicleSync, IEntity* player);

/// Update the vehicle from a passenger sync packet
DLL_EXPORT bool Vehicle_UpdateFromPassengerSync(IEntity* vehicle, const VehiclePassengerSyncPacket& passengerSync, IEntity* player);

/// Update the vehicle from an unoccupied sync packet
DLL_EXPORT bool Vehicle_UpdateFromUnoccupied(IEntity* vehicle, const VehicleUnoccupiedSyncPacket& unoccupiedSync, IEntity* player);

/// Update the vehicle from a trailer sync packet
DLL_EXPORT bool Vehicle_UpdateFromTrailerSync(IEntity* vehicle, const VehicleTrailerSyncPacket& unoccupiedSync, IEntity* player);

/// Get the players which the vehicle is streamed for
DLL_EXPORT IEntity** Vehicle_StreamedForPlayers(IEntity* vehicle);

/// Returns the current driver of the vehicle
DLL_EXPORT IEntity* Vehicle_GetDriver(IEntity* vehicle);

/// Returns the passengers of the vehicle
DLL_EXPORT IEntity** Vehicle_GetPassengers(IEntity* vehicle);

/// Sets the vehicle's number plate
DLL_EXPORT void Vehicle_SetPlate(IEntity* vehicle, const char* plate);

/// Get the vehicle's number plate
DLL_EXPORT const char* Vehicle_GetPlate(IEntity* vehicle);

/// Sets the vehicle's damage status
DLL_EXPORT void Vehicle_SetDamageStatus(IEntity* vehicle, int panelStatus, int doorStatus, uint8_t lightStatus, uint8_t tyreStatus, IEntity* vehicleUpdater = nullptr);

/// Gets the vehicle's damage status
DLL_EXPORT void Vehicle_GetDamageStatus(IEntity* vehicle, int* panelStatus, int* doorStatus, int* lightStatus, int* tyreStatus);

/// Sets the vehicle's paintjob
DLL_EXPORT void Vehicle_SetPaintJob(IEntity* vehicle, int paintjob);

/// Gets the vehicle's paintjob
DLL_EXPORT int Vehicle_GetPaintJob(IEntity* vehicle);

/// Adds a component to the vehicle.
DLL_EXPORT void Vehicle_AddComponent(IEntity* vehicle, int component);

/// Gets the vehicle's component in a designated slot
DLL_EXPORT int Vehicle_GetComponentInSlot(IEntity* vehicle, int slot);

/// Removes a vehicle's component.
DLL_EXPORT void Vehicle_RemoveComponent(IEntity* vehicle, int component);

/// Puts the player inside this vehicle.
DLL_EXPORT void Vehicle_PutPlayer(IEntity* vehicle, IEntity* player, int seatId);

/// Set the vehicle's Z angle.
DLL_EXPORT void Vehicle_SetZAngle(IEntity* vehicle, float angle);

/// Gets the vehicle's Z angle.
DLL_EXPORT float Vehicle_GetZAngle(IEntity* vehicle);

// Set the vehicle's parameters.
DLL_EXPORT void Vehicle_SetParams(IEntity* vehicle, const VehicleParams& params);

// Set the vehicle's parameters for a specific player.
DLL_EXPORT void Vehicle_SetParamsForPlayer(IEntity* vehicle, IEntity* player, const VehicleParams& params);

// Get the vehicle's parameters.
DLL_EXPORT CVehicleParams Vehicle_GetParams(IEntity* vehicle);

/// Checks if the vehicle is dead.
DLL_EXPORT bool Vehicle_IsDead(IEntity* vehicle);

/// Respawns the vehicle.
DLL_EXPORT void Vehicle_Respawn(IEntity* vehicle);

/// Get the vehicle's respawn delay.
DLL_EXPORT int64_t Vehicle_GetRespawnDelay(IEntity* vehicle);

/// Set the vehicle respawn delay.
DLL_EXPORT void Vehicle_SetRespawnDelay(IEntity* vehicle, int64_t seconds);

/// Checks if the vehicle is respawning.
DLL_EXPORT bool Vehicle_IsRespawning(IEntity* vehicle);

// Sets (links) the vehicle to an interior.
DLL_EXPORT void Vehicle_SetInterior(IEntity* vehicle, int interior);

// Gets the vehicle's interior.
DLL_EXPORT int Vehicle_GetInterior(IEntity* vehicle);

/// Attaches a vehicle as a trailer to this vehicle.
DLL_EXPORT void Vehicle_AttachTrailer(IEntity* vehicle, IEntity* trailer);

/// Detaches a vehicle from this vehicle.
DLL_EXPORT void Vehicle_DetachTrailer(IEntity* vehicle);

/// Checks if the current vehicle is a trailer.
DLL_EXPORT bool Vehicle_IsTrailer(IEntity* vehicle);

/// Get the current vehicle's attached trailer.
DLL_EXPORT IEntity* Vehicle_GetTrailer(IEntity* vehicle);

/// Get the current vehicle's cab.
DLL_EXPORT IEntity* Vehicle_GetCab(IEntity* vehicle);

/// Fully repair the vehicle.
DLL_EXPORT void Vehicle_Repair(IEntity* vehicle);

/// Adds a train carriage to the vehicle (ONLY FOR TRAINS).
DLL_EXPORT void Vehicle_AddCarriage(IEntity* vehicle, IEntity* carriage, int pos);
DLL_EXPORT void Vehicle_UpdateCarriage(IEntity* vehicle, Vector3 pos, Vector3 veloc);
DLL_EXPORT IEntity** Vehicle_GetCarriages(IEntity* vehicle);

/// Sets the velocity of the vehicle.
DLL_EXPORT void Vehicle_SetVelocity(IEntity* vehicle, Vector3 velocity);

/// Gets the current velocity of the vehicle.
DLL_EXPORT Vector3 Vehicle_GetVelocity(IEntity* vehicle);

/// Sets the angular velocity of the vehicle.
DLL_EXPORT void Vehicle_SetAngularVelocity(IEntity* vehicle, Vector3 velocity);

/// Gets the current angular velocity of the vehicle.
DLL_EXPORT Vector3 Vehicle_GetAngularVelocity(IEntity* vehicle);

/// Gets the current model ID of the vehicle.
DLL_EXPORT int Vehicle_GetModel(IEntity* vehicle);

/// Gets the current landing gear state from a ID_VEHICLE_SYNC packet from the latest driver.
DLL_EXPORT uint8_t Vehicle_GetLandingGearState(IEntity* vehicle);

/// Get if the vehicle was occupied since last spawn.
DLL_EXPORT bool Vehicle_HasBeenOccupied(IEntity* vehicle);

/// Get the last time the vehicle was occupied.
DLL_EXPORT int64_t Vehicle_GetLastOccupiedTime(IEntity* vehicle);

/// Get the last time the vehicle was spawned.
DLL_EXPORT int64_t Vehicle_GetLastSpawnTime(IEntity* vehicle);

/// Get if vehicle is occupied.
DLL_EXPORT bool Vehicle_IsOccupied(IEntity* vehicle);

/// Set vehicle siren status.
DLL_EXPORT void Vehicle_SetSiren(IEntity* vehicle, bool status);

/// Get vehicle siren status.
DLL_EXPORT uint8_t Vehicle_GetSirenState(IEntity* vehicle);

/// Get hydra thrust angle
DLL_EXPORT uint32_t Vehicle_GetHydraThrustAngle(IEntity* vehicle);

/// Get train speed
DLL_EXPORT float Vehicle_GetTrainSpeed(IEntity* vehicle);

/// Get last driver's pool id
DLL_EXPORT int Vehicle_GetLastDriverPoolID(IEntity* vehicle);
