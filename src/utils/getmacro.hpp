#pragma once

#define GET_PLAYER_CHECKED(player_output, playerid)                                               \
	IPlayer* player_output = MainComponent::getInstance()->getCore()->getPlayers().get(playerid); \
	if (player_output == nullptr)

#define GET_PLAYER_CHECKED(player_output, playerid)                                               \
	IPlayer* player_output = MainComponent::getInstance()->getCore()->getPlayers().get(playerid); \
	if (player_output == nullptr)

#define GET_VEHICLE_CHECKED(vehicle_output, vehicleid)                                               \
	IVehicle* vehicle_output = MainComponent::getInstance()->getVehiclesComponent()->get(vehicleid); \
	if (vehicle_output == nullptr)

#define GET_OBJECT_CHECKED(object_output, objectid)                                              \
	IObject* object_output = MainComponent::getInstance()->getObjectsComponent()->get(objectid); \
	if (object_output == nullptr)

#define GET_PLAYER_TEXTDRAW_CHECKED(player, textdraw_data_output, textid)                    \
	IPlayerTextDrawData* textdraw_data_output = queryExtension<IPlayerTextDrawData>(player); \
	if (textdraw_data_output == nullptr)

#define GET_PLAYER_TEXTDRAW_BY_ID_CHECKED(player, textdraw_data, textdraw_output, textid) \
	IPlayerTextDraw* textdraw_output = nullptr;                                           \
	for (auto it = textdraw_data->begin(), end = textdraw_data->end(); it != end; ++it)   \
	{                                                                                     \
		auto td = *it;                                                                    \
		if (td->getID() == textid)                                                        \
		{                                                                                 \
			textdraw_output = td;                                                         \
		}                                                                                 \
	}
