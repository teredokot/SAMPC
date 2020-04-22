
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

native IsValidVehicle(vehicleid);
native CreateVehicle(modelid, Float:x, Float:y, Float:z, Float:a, color1, color2, respawndelay);
native DestroyVehicle(vehicleid);
native GetVehiclePos(vehicleid, &Float:x, &Float:y, &Float:z);
native SetVehiclePos(vehicleid, Float:x, Float:y, Float:z);
native GetVehicleZAngle(vehicleid, &Float:a);
native Float:GetVehicleDistanceFromPoint(vehicleid, Float:X, Float:Y, Float:Z);
native SetVehicleZAngle(vehicleid, Float:a);
native SetVehicleParamsForPlayer(vehicleid, playerid, objective, locked);
native SetVehicleParamsCarWindows(vehicleid, driver, passenger, backleft, backright);
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
