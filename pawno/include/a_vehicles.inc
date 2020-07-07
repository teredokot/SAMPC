
#if defined _vehicles_included
	#endinput
#endif
#define _vehicles_included

#define CARMODTYPE_SPOILER 			(0)
#define CARMODTYPE_HOOD 			(1)
#define CARMODTYPE_ROOF 			(2)
#define CARMODTYPE_SIDESKIRT 		(3)
#define CARMODTYPE_LAMPS 			(4)
#define CARMODTYPE_NITRO 			(5)
#define CARMODTYPE_EXHAUST 			(6)
#define CARMODTYPE_WHEELS 			(7)
#define CARMODTYPE_STEREO 			(8)
#define CARMODTYPE_HYDRAULICS 		(9)
#define CARMODTYPE_FRONT_BUMPER 	(10)
#define CARMODTYPE_REAR_BUMPER		(11)
#define CARMODTYPE_VENT_RIGHT		(12)
#define CARMODTYPE_VENT_LEFT		(13)
#define CARMODTYPE_BULLBARS			(14)
#define CARMODTYPE_REAR_BULLBARS 	(15)
#define CARMODTYPE_FRONT_BULLBARS	(16)

#define VEHICLE_DOOR_FRONT_LEFT 	(1)
#define VEHICLE_DOOR_FRONT_RIGHT	(2)
#define VEHICLE_DOOR_REAR_LEFT 		(3)
#define VEHICLE_DOOR_REAR_RIGHT 	(4)

#define VEHICLE_MODEL_INFO_SIZE				(1)
#define VEHICLE_MODEL_INFO_FRONTSEAT		(2)
#define VEHICLE_MODEL_INFO_REARSEAT			(3)
#define VEHICLE_MODEL_INFO_PETROLCAP		(4)
#define VEHICLE_MODEL_INFO_WHEELSFRONT		(5)
#define VEHICLE_MODEL_INFO_WHEELSREAR		(6)
#define VEHICLE_MODEL_INFO_WHEELSMID		(7)
#define VEHICLE_MODEL_INFO_FRONT_BUMPER_Z	(8)
#define VEHICLE_MODEL_INFO_REAR_BUMPER_Z	(9)

native GetVehicleModelCount(modelid);
native GetVehicleModelsUsed();
native IsValidVehicle(vehicleid);
native CreateVehicle(modelid, Float:x, Float:y, Float:z, Float:a, color1, color2, respawndelay);
native DestroyVehicle(vehicleid);
native SetVehicleSpawnInfo(vehicleid, modelid, Float:x, Float:y, Float:z, Float:r, color1, color2, respawndelay = -2, interior = -2);
native GetVehicleSpawnInfo(vehicleid, &Float:x, &Float:y, &Float:z, &Float:r, &color1, &color2);
native GetVehiclePos(vehicleid, &Float:x, &Float:y, &Float:z);
native SetVehiclePos(vehicleid, Float:x, Float:y, Float:z);
native GetVehicleZAngle(vehicleid, &Float:a);
native Float:GetVehicleDistanceFromPoint(vehicleid, Float:X, Float:Y, Float:Z);
native SetVehicleZAngle(vehicleid, Float:a);
native SetVehicleParamsForPlayer(vehicleid, playerid, objective, locked);
native GetVehicleParamsCarDoors(vehicleid, &driver, &passenger, &backleft, &backright);
native SetVehicleParamsCarWindows(vehicleid, driver, passenger, backleft, backright);
native GetVehicleParamsCarWindows(vehicleid, &driver, &passenger, &backleft, &backright);
native SetVehicleToRespawn(vehicleid);
native LinkVehicleToInterior(vehicleid, interiorid);
native GetVehicleInterior(vehicleid);
native AddVehicleComponent(vehicleid, componentid);
native RemoveVehicleComponent(vehicleid, componentid);
native ChangeVehicleColor(vehicleid, color1, color2);
native GetVehicleColor(vehicleid, &color1, &color2);
native ChangeVehiclePaintjob(vehicleid, paintjobid);
native GetVehiclePaintjob(vehicleid);
native SetVehicleHealth(vehicleid, Float:health);
native GetVehicleHealth(vehicleid, &Float:health);
native AttachTrailerToVehicle(vehicleid, tovehicleid);
native DetachTrailerFromVehicle(vehicleid);
native IsTrailerAttachedToVehicle(vehicleid);
native GetVehicleTrailer(vehicleid);
native SetVehicleNumberPlate(vehicleid, const numberplate[]);
native GetVehicleNumberPlate(vehicleid, numberplate[], len = sizeof(numberplate));
native GetVehicleModel(vehicleid);
native RepairVehicle(vehicleid);
native SetVehicleVirtualWorld(vehicleid, worldid);
native GetVehicleVirtualWorld(vehicleid);
native ToggleTaxiLight(vehicleid, toggle);
native SetVehicleEngineState(vehicleid, engine_state);
native SetVehicleVelocity(vehicleid, Float:X, Float:Y, Float:Z);
native GetVehicleVelocity(vehicleid, &Float:x, &Float:y, &Float:z);
native IsVehicleOnItsSide(vehicleid);
native IsVehicleUpsideDown(vehicleid);
native GetVehicleSirenState(vehicle);
native IsVehicleWrecked(vehicle);
native IsVehicleSunked(vehicle);
native SetVehicleLightState(vehicleid, light_state);
native SetVehicleRespawnDelay(vehicleid, respawn_delay);
native GetVehicleRespawnDelay(vehicleid);
native SetVehicleSpawnPos(vehicleid, Float:x, Float:y, Float:z);
native GetVehicleSpawnPos(vehicleid, &Float:x, &Float:y, &Float:z);
native GetVehicleComponentInSlot(vehicleid, slot);
native GetVehicleComponentType(component);
native SetVehicleHoodState(vehicleid, bool:hood_state);
native SetVehicleTrunkState(vehicleid, bool:trunk_state);
native SetVehicleDoorState(vehicleid, doorid, bool:door_state);
native SetVehicleFeature(vehicleid, toggle);
native SetVehicleVisibility(vehicleid, visible);
native GetVehicleModelInfo(vehiclemodel, infotype, &Float:X, &Float:Y, &Float:Z);
