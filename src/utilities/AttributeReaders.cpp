#include "AttributeReaders.h"
#include "GameController.h"
#include "GameStateController.h"
#include <iostream>

u32 loadShipData(std::string path, gvReader& in, bool carrier)
{
	std::cout << "Reading ship in from: " << path << "... ";
	in.read(path);
	if (in.lines.empty()) {
		std::cout << "Could not read " << path << "!\n";
		return -1;
	}
	in.readLinesToValues();
	u32 id = std::stoi(in.values["id"]);
	std::string name = in.values["name"];
	ShipData* data = new ShipData;

	if (carrier) {
		delete data;
		data = new CarrierData;
	}

	data->id = id;
	data->name = name;
	data->description = in.values["description"];

	std::string meshpath = "models/" + in.values["model"];
	std::string texpath = "models/" + in.values["texture"];
	std::string normpath = "models/" + in.values["norm"];
	data->shipMesh = meshpath;
	data->shipTexture = texpath;
	data->shipNorm = normpath;

	std::string jetpath = "effects/" + in.values["jet"];
	std::string enginepath = "effects/" + in.values["engine"];

	data->engineTexture = enginepath;
	data->jetTexture = enginepath;

	data->thrustComponent.forward = std::stof(in.values["forwardThrust"]);
	data->thrustComponent.brake = std::stof(in.values["brakeThrust"]);
	data->thrustComponent.strafe = std::stof(in.values["strafeThrust"]);
	data->thrustComponent.pitch = std::stof(in.values["pitchThrust"]);
	data->thrustComponent.yaw = std::stof(in.values["yawThrust"]);
	data->thrustComponent.roll = std::stof(in.values["rollThrust"]);
	data->thrustComponent.velocityTolerance = std::stof(in.values["velocityTolerance"]);
	data->thrustComponent.linearMaxVelocity = std::stof(in.values["linearMaxVelocity"]);
	data->thrustComponent.angularMaxVelocity = std::stof(in.values["angularMaxVelocity"]);
	data->thrustComponent.boost = std::stof(in.values["afterburnerThrust"]);

	data->hardpointComponent.hardpointCount = std::stoi(in.values["hardpointCount"]);

	std::string val;
	for (unsigned int i = 0; i < data->hardpointComponent.hardpointCount; ++i) {
		val = "hardpoint" + std::to_string(i);
		data->hardpointComponent.hardpoints[i] = strToVec(in.values[val]);
		//data->shipComponent.weapons[i] = INVALID_ENTITY;
	}
	for (unsigned int i = 0; i < 2; ++i) {
		val = "upJetPos" + std::to_string(i);
		data->shipComponent.upJetPos[i] = strToVec(in.values[val]);
	}
	for (unsigned int i = 0; i < 2; ++i) {
		val = "downJetPos" + std::to_string(i);
		data->shipComponent.downJetPos[i] = strToVec(in.values[val]);
	}
	for (unsigned int i = 0; i < 2; ++i) {
		val = "leftJetPos" + std::to_string(i);
		data->shipComponent.leftJetPos[i] = strToVec(in.values[val]);
	}
	for (unsigned int i = 0; i < 2; ++i) {
		val = "rightJetPos" + std::to_string(i);
		data->shipComponent.rightJetPos[i] = strToVec(in.values[val]);
	}
	for (u32 i = 0; i < 2; ++i) {
		val = "reverseJetPos" + std::to_string(i);
		data->shipComponent.reverseJetPos[i] = strToVec(in.values[val]);
	}
	data->shipComponent.engineJetPos = strToVec(in.values["engineJetPos"]);
	data->hardpointComponent.physWeaponHardpoint = strToVec(in.values["physWeaponHardpoint"]);

	data->shipComponent.afterburnerFuel = std::stof(in.values["afterburnerFuel"]);
	data->shipComponent.maxAfterburnerFuel = data->shipComponent.afterburnerFuel;
	data->shipComponent.afterburnerFuelEfficiency = std::stof(in.values["afterburnerFuelEfficiency"]);

	data->shipComponent.afterburnerOn = false;
	data->thrustComponent.safetyOverride = false;
	data->shipComponent.shipDataId = id;

	if (carrier) {
		std::cout << "reading additional carrier data... ";
		CarrierData* carr = (CarrierData*)data;
		carr->health = in.getFloat("health");
		carr->scale = in.getVec("scale");
		carr->carrierComponent.reserveShips = in.getUint("reserveShips");
		carr->carrierComponent.spawnRate = in.getFloat("spawnRate");
		carr->carrierComponent.scale = carr->scale;
		carr->mass = in.getUint("mass");

		carr->carrierComponent.turretCount = in.getUint("turretCount");
		for (u32 i = 0; i < carr->carrierComponent.turretCount; ++i) {
			std::string pos = "turretPos" + std::to_string(i);
			std::string rot = "turretRot" + std::to_string(i);
			carr->carrierComponent.turretPositions[i] = in.getVec(pos);
			carr->carrierComponent.turretRotations[i] = in.getVec(rot);
		}

		carrierData[id] = carr;
	}
	else {
		shipData[id] = data;
	}
	std::cout << "Done.\n";
	return id;
}

u32 loadTurretData(std::string path, gvReader& in)
{
	std::cout << "Reading turret in from: " << path << "... ";
	in.read(path);
	if (in.lines.empty()) {
		std::cout << "Could not read " << path << "!\n";
		return -1;
	}
	in.readLinesToValues();
	u32 id = in.getUint("id");
	std::string name = in.getString("name");

	TurretData* data = new TurretData;

	std::string meshpath = "models/" + in.getString("model");
	std::string texpath = "models/" + in.getString("texture");
	std::string normpath = "models/" + in.getString("norm");
	data->mesh = meshpath;
	data->texture = texpath;
	data->norm = normpath;
	data->id = id;
	data->name = name;
	data->description = in.getString("description");

	data->hardpointComponent.hardpointCount = in.getUint("hardpointCount");
	for (u32 i = 0; i < data->hardpointComponent.hardpointCount; ++i) {
		data->hardpointComponent.hardpoints[i] = in.getVec("hardpoint" + std::to_string(i));
	}
	data->thrustComponent.pitch = in.getFloat("pitchThrust");
	data->thrustComponent.yaw = in.getFloat("yawThrust");

	turretData[id] = data;
	std::cout << "Done. \n";
	return id;
}


void loadAmmoData(WeaponData* data, gvReader& in)
{
	data->weaponComponent.maxAmmunition = std::stoi(in.values["maxAmmunition"]);
	data->weaponComponent.ammunition = data->weaponComponent.maxAmmunition;
	data->weaponComponent.maxClip = std::stoi(in.values["clip"]);
	data->weaponComponent.reloadTime = std::stof(in.values["reloadTime"]);
	data->weaponComponent.clip = data->weaponComponent.clip;

}
u32 loadWeaponData(std::string path, gvReader& in)
{
	std::cout << "Reading weapon in from: " << path << "... ";
	in.read(path);
	if (in.lines.empty()) {
		std::cout << "Could not read " << path << "!\n";
		return -1;
	}
	in.readLinesToValues();

	u32 id = std::stoi(in.values["id"]);
	std::string name = in.values["name"];
	WEAPON_TYPE type = weaponStrings.at(in.values["type"]);
	DAMAGE_TYPE dmgtype = damageStrings.at(in.values["dmgtype"]);
	WeaponData* data = new WeaponData;
	if (type == WEP_KINETIC) {
		delete data;
		data = new KineticData;
	}
	else if (type == WEP_MISSILE) {
		delete data;
		data = new MissileData;
	}
	else if (type == WEP_PHYS_BOLAS) {
		delete data;
		data = new BolasData;
	}
	data->weaponComponent.fireSound = in.getString("fireSound");
	data->weaponComponent.impactSound = in.getString("impactSound");

	data->id = id;
	data->name = name;
	data->description = in.values["description"];
	std::string meshpath = "models/" + in.values["model"];
	std::string texpath = "models/" + in.values["texture"];
	std::string normpath = "models/" + in.values["norm"];

	data->weaponMesh = meshpath;
	data->weaponTexture = texpath;
	data->weaponNorm = normpath;

	std::string effectpath = "effects/" + in.values["particle"];
	data->weaponEffect = effectpath;
	data->weaponComponent.usesAmmunition = false;
	data->weaponComponent.maxAmmunition = 0;
	data->weaponComponent.ammunition = 0;
	data->weaponComponent.maxClip = 0;
	data->weaponComponent.reloadTime = 0;
	data->weaponComponent.clip = 0;
	data->weaponComponent.timeReloading = 0;

	if (type == WEP_MISSILE) {
		data->weaponComponent.usesAmmunition = true;
		std::string misspath = "models/" + in.values["missilemodel"];
		std::string misstexpath = "models/" + in.values["missiletexture"];
		MissileData* miss = (MissileData*)data;

		miss->missileMesh = misspath;
		miss->missileTexture = misstexpath;
		miss->missileComponent.maxMissileVelocity = std::stof(in.values["maxMissileVelocity"]);
		miss->missileComponent.missileRotThrust = std::stof(in.values["missileRotThrust"]);
		miss->missileComponent.timeToLock = std::stof(in.values["timeToLock"]);
		loadAmmoData(data, in);
	}
	if (type == WEP_KINETIC) {
		data->weaponComponent.usesAmmunition = true;
		KineticData* kin = (KineticData*)data;

		loadAmmoData(data, in);
		kin->kineticComponent.accuracy = std::stof(in.values["accuracy"]);
		kin->kineticComponent.projectilesPerShot = std::stoi(in.values["projectilesPerShot"]);
	}

	if (type == WEP_PHYS_BOLAS) {
		BolasInfoComponent cmp;
		cmp.constraint = nullptr;
		cmp.currentDuration = 0.f;
		cmp.duration = in.getFloat("duration");
		//cmp.target1 = INVALID_ENTITY;
		//cmp.target2 = INVALID_ENTITY;
		cmp.timeToHit = in.getFloat("timeToHit");
		cmp.currentTimeToHit = 0.f;
		cmp.force = in.getFloat("force");
		BolasData* bdat = (BolasData*)data;
		cmp.latchSound = in.getString("latchSound");
		bdat->bolasComponent = cmp;

	}

	data->weaponComponent.isFiring = false;
	data->weaponComponent.type = type;
	data->weaponComponent.dmgtype = dmgtype;
	data->weaponComponent.firingSpeed = std::stof(in.values["firingSpeed"]);
	data->weaponComponent.projectileSpeed = std::stof(in.values["projectileSpeed"]);
	data->weaponComponent.damage = std::stof(in.values["damage"]);
	data->weaponComponent.lifetime = in.getFloat("lifetime");
	data->weaponComponent.timeSinceLastShot = 0.f;

	bool phys = std::stoi(in.values["phys"]);
	data->weaponComponent.phys = phys;
	data->weaponComponent.wepDataId = id;
	/*if (id == 0) {
		stateController->weaponData[id] = data;
		stateController->physWeaponData[id] = data;
	} else*/
	if (phys) {
		physWeaponData[id] = data;
	} else {
		weaponData[id] = data;
	}
	std::cout << "Done.\n";
	return id;
}

bool loadShip(u32 id, flecs::entity entity, bool carrier)
{
	ShipData* data = shipData[id];
	if (carrier) data = carrierData[id];

	if (!data) return false;

	IrrlichtComponent irr;
	irr.name = data->name;

	ITexture* norm = driver->getTexture(data->shipNorm.c_str());
	ITexture* tex = assets->getTextureAsset(data->name);
	IMesh* mesh = assets->getMeshAsset(data->name);
	if (norm) {
		if (!mesh) {
			auto initmesh = smgr->getMesh(data->shipMesh.c_str());
			mesh = smgr->getMeshManipulator()->createMeshWithTangents(initmesh);
			smgr->getMeshCache()->removeMesh(initmesh);
			assets->setMeshAsset(data->name, mesh);
		}
		irr.node = smgr->addMeshSceneNode(mesh);
		driver->makeNormalMapTexture(norm, 7.f);
		irr.node->setMaterialTexture(1, norm);
		irr.node->setMaterialType(EMT_PARALLAX_MAP_SOLID);
	}
	else {
		if (!mesh) {
			mesh = smgr->getMesh(data->shipMesh.c_str());
			assets->setMeshAsset(data->name, mesh);
		}
		irr.node = smgr->addMeshSceneNode(mesh);
	}
	if (!tex) {
		tex = driver->getTexture(data->shipTexture.c_str());
		assets->setTextureAsset(data->name, tex);
	}
	irr.node->setMaterialTexture(0, tex);

	irr.node->setName(idToStr(entity).c_str());
	irr.node->setID(ID_IsSelectable | ID_IsAvoidable);

	if (carrier) {
		CarrierData* cdata = (CarrierData*)data;
		entity.set<CarrierComponent>(cdata->carrierComponent);
		irr.node->setScale(cdata->carrierComponent.scale);
	}

	HardpointComponent hards = data->hardpointComponent;
	for (u32 i = 0; i < MAX_HARDPOINTS; ++i) {
		hards.weapons[i] = INVALID_ENTITY;
	}

	entity.set<ShipComponent>(data->shipComponent);
	entity.set<ThrustComponent>(data->thrustComponent);
	entity.set<HardpointComponent>(hards);
	entity.set<IrrlichtComponent>(irr);
	return true;
}

bool loadTurret(u32 id, flecs::entity entity)
{
	TurretData* data = turretData[id];
	if (!data) return false;

	IrrlichtComponent irr;
	irr.name = data->name;

	ITexture* norm = driver->getTexture(data->norm.c_str());
	ITexture* tex = assets->getTextureAsset(data->name);
	IMesh* mesh = assets->getMeshAsset(data->name);
	if (norm) {
		if (!mesh) {
			auto initmesh = smgr->getMesh(data->mesh.c_str());
			mesh = smgr->getMeshManipulator()->createMeshWithTangents(initmesh);
			smgr->getMeshCache()->removeMesh(initmesh);
			assets->setMeshAsset(data->name, mesh);
		}
		irr.node = smgr->addMeshSceneNode(mesh);
		driver->makeNormalMapTexture(norm, 7.f);
		irr.node->setMaterialTexture(1, norm);
		irr.node->setMaterialType(EMT_PARALLAX_MAP_SOLID);
	} else {
		if (!mesh) {
			mesh = smgr->getMesh(data->mesh.c_str());
			assets->setMeshAsset(data->name, mesh);
		}
		irr.node = smgr->addMeshSceneNode(mesh);
	}
	if (!tex) {
		tex = driver->getTexture(data->texture.c_str());
		assets->setTextureAsset(data->name, tex);
	}

	irr.node->setMaterialTexture(0, tex);
	irr.node->setName(idToStr(entity).c_str());
	irr.node->setID(ID_IsSelectable | ID_IsAvoidable);

	entity.set<IrrlichtComponent>(irr);
	entity.set<ThrustComponent>(data->thrustComponent);
	entity.set<HardpointComponent>(data->hardpointComponent);
	return true;
}

bool loadWeapon(u32 id, flecs::entity weaponEntity, bool phys)
{
	WeaponData* data = nullptr;

	if (phys) data = physWeaponData[id];
	else data = weaponData[id];

	if (!data) return false;

	IrrlichtComponent irr;

	IMesh* mesh = assets->getMeshAsset(data->name);
	ITexture* tex = assets->getTextureAsset(data->name);
	ITexture* norm = nullptr;
	if (data->weaponNorm != "") norm = driver->getTexture(data->weaponNorm.c_str());

	if (!mesh) {
		auto initmesh = smgr->getMesh(data->weaponMesh.c_str());
		mesh = smgr->getMeshManipulator()->createMeshWithTangents(initmesh);
		smgr->getMeshCache()->removeMesh(initmesh);
		assets->setMeshAsset(data->name, mesh);
	}
	irr.node = smgr->addMeshSceneNode(mesh);
	if (!tex) {
		tex = driver->getTexture(data->weaponTexture.c_str());
		assets->setTextureAsset(data->name, tex);
	}
	irr.node->setMaterialTexture(0, tex);

	if (norm) {
		driver->makeNormalMapTexture(norm, 7.f);
		irr.node->setMaterialTexture(1, norm);
		irr.node->setMaterialType(EMT_PARALLAX_MAP_SOLID);
	}

	irr.node->setName(idToStr(weaponEntity).c_str());
	irr.node->setID(ID_IsNotSelectable);

	//if(data->weaponComponent.phys) assets->setPhysWeaponFireSound(id, soundEngine->getSoundSource(data->weaponFireSound.c_str()));
	//else assets->setWeaponFireSound(id, soundEngine->getSoundSource(data->weaponFireSound.c_str()));

	WeaponInfoComponent wep = data->weaponComponent;

	if (data->weaponComponent.type == WEP_MISSILE) {
		MissileData* mdata = (MissileData*)data;
		weaponEntity.set<MissileInfoComponent>(mdata->missileComponent);
		auto miss = weaponEntity.get_mut<MissileInfoComponent>();
		miss->missileMesh = smgr->getMesh(mdata->missileMesh.c_str());
		miss->missileTexture = driver->getTexture(mdata->missileTexture.c_str());
		wep.usesAmmunition = true;
	}
	if (data->weaponComponent.type == WEP_KINETIC) {
		KineticData* kdata = (KineticData*)data;
		weaponEntity.set<KineticInfoComponent>(kdata->kineticComponent);
		wep.clip = wep.maxClip;
		wep.usesAmmunition = true; 
	}
	if (data->weaponComponent.type == WEP_PHYS_BOLAS) {
		BolasData* bdata = (BolasData*)data;
		weaponEntity.set<BolasInfoComponent>(bdata->bolasComponent);
		auto bolas = weaponEntity.get_mut<BolasInfoComponent>();
		BolasInfoComponent cmp = bdata->bolasComponent;
		*bolas = cmp;
	}
	wep.particle = driver->getTexture(data->weaponEffect.c_str());
	assets->setTextureAsset(data->weaponEffect, wep.particle);

	weaponEntity.set<WeaponInfoComponent>(wep);
	weaponEntity.set<IrrlichtComponent>(irr);
	return true; 
}

u32 loadObstacleData(std::string path, gvReader& in)
{
	std::cout << "Reading obstacle in from " << path << "... ";
	in.read(path);
	if (in.lines.empty()) {
		std::cout << "Could not read from " << path << ".\n";
		return -1;
	}
	in.readLinesToValues();
	u32 id = std::stoi(in.values["id"]);
	std::string name = in.values["name"];
	ObstacleData* data = new ObstacleData;

	data->id = id;
	data->name = name;

	data->health = std::stof(in.values["health"]);
	data->obstacleMesh = in.values["mesh"];
	data->obstacleTexture = in.values["texture"];
	data->obstacleNorm = in.values["norm"];
	if (data->obstacleNorm != "") {
		auto norm = driver->getTexture(data->obstacleNorm.c_str());
		driver->makeNormalMapTexture(norm, 5.f);
	}
	data->type = obstacleStrings.at(in.values["type"]);

	obstacleData[id] = data;
	std::cout << "Done.\n";
	return id;
}

bool loadObstacle(u32 id, flecs::entity entity)
{
	ObstacleData* data = obstacleData[id];
	if (!data) return false;

	ObstacleComponent obst;
	obst.type = data->type;

	IrrlichtComponent irr;
	irr.name = data->name;

	IMesh* mesh = nullptr;

	if (data->obstacleMesh != "") {
		mesh = assets->getMeshAsset(data->name);
		if (!mesh) {
			auto initmesh = smgr->getMesh(data->obstacleMesh.c_str());
			mesh = smgr->getMeshManipulator()->createMeshWithTangents(initmesh);
			smgr->getMeshCache()->removeMesh(initmesh);
			assets->setMeshAsset(data->name, mesh);
		}
	}
	ITexture* tex = assets->getTextureAsset(data->name);
	ITexture* norm = nullptr;
	if (data->obstacleNorm != "") {
		norm = driver->getTexture(data->obstacleNorm.c_str());
	}

	if (!tex) {
		tex = driver->getTexture(data->obstacleTexture.c_str());
		assets->setTextureAsset(data->name, tex);
	}

	if (obst.type == GAS_CLOUD) {
		auto ps = smgr->addParticleSystemSceneNode();
		irr.node = ps;
		IParticleAffector* paf = ps->createFadeOutParticleAffector();
		ps->addAffector(paf);
		paf->drop();
		ps->setMaterialFlag(EMF_LIGHTING, false);
		ps->setMaterialFlag(EMF_ZWRITE_ENABLE, false);
		ps->setMaterialTexture(0, tex);
		ps->setMaterialType(EMT_TRANSPARENT_ADD_COLOR);
	}
	else {
		irr.node = smgr->addMeshSceneNode(mesh);
		irr.node->setMaterialTexture(0, tex);
		if (norm) {
			irr.node->setMaterialTexture(1, norm);
			irr.node->setMaterialType(EMT_PARALLAX_MAP_SOLID);
		}
	}
	irr.node->setName(idToStr(entity).c_str());
	initializeHealth(entity, data->health);

	entity.set<ObstacleComponent>(obst);
	entity.set<IrrlichtComponent>(irr);
	return true;
}

btConvexHullShape createCollisionShapeFromMesh(IMesh* mesh)
{
	IMeshBuffer* buf = mesh->getMeshBuffer((u32)0);
	S3DVertex* bufverts = (S3DVertex*)buf->getVertices();
	btConvexHullShape shape;

	for (u32 i = 0; i < buf->getVertexCount(); ++i) {
		vector3df pos = bufverts[i].Pos;
		shape.addPoint(btVector3(pos.X, pos.Y, pos.Z));
	}
	shape.setMargin(0);
	btShapeHull* hull = new btShapeHull(&shape);
	hull->buildHull(0);
	btConvexHullShape ret((const btScalar*)hull->getVertexPointer(), hull->numVertices(), sizeof(btVector3));
	delete hull;
	return ret;
}

bool saveHull(std::string path, btConvexHullShape& shape)
{
	btDefaultSerializer serializer;
	serializer.startSerialization();
	shape.serializeSingleShape(&serializer);
	serializer.finishSerialization();

	FILE* f = fopen(path.c_str(), "wb");
	if (!f) return false;

	fwrite(serializer.getBufferPointer(), serializer.getCurrentBufferSize(), 1, f);
	fclose(f);
	return true;
}
bool loadHull(std::string path, btConvexHullShape& shape)
{
	btBulletWorldImporter importer(0);
	if (!importer.loadFile(path.c_str())) {
		std::cout << "Could not load hull file! \n";
		return false;
	}
	if (!importer.getNumCollisionShapes()) {
		std::cout << "Hull file not properly formatted! \n";
		return false;
	}
	btConvexHullShape* coll = (btConvexHullShape*)importer.getCollisionShapeByIndex(0);
	shape = *coll;

	return true;
	
}

bool loadWingman(std::string path, WingmanData& data)
{
	std::cout << "Reading in wingman data from " << path << "... ";
	gvReader in;
	in.read(path);
	if (in.lines.empty()) {
		std::cout << "Could not read " << path << "!\n";
		return false;
	}
	in.readLinesToValues();
	data.id = in.getInt("id");
	data.description = in.getString("description");
	data.name = in.getString("name");

	data.ai.aggressiveness = in.getFloat("aggressiveness");
	data.ai.type = aiTypeStrings.at(in.getString("type"));
	data.ai.aim = in.getFloat("aim");
	data.ai.cowardice = in.getFloat("cowardice");
	data.ai.reactionSpeed = in.getFloat("reactionSpeed");

	std::cout << "Done.\n";
	return true;
}