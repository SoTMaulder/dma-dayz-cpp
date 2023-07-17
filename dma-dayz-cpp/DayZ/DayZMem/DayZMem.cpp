#include "DayZMem.h"

DMAMem::VmmManager* DayZ::Mem::getVMM()
{
	return vmmManager;
}

DayZ::Mem::Mem(DMAMem::VmmManager* vmmManager)
{
	this->staticManager = DMAMem::StaticManager(vmmManager);
	this->vmmManager = vmmManager;
	fetchBaseAddresses();
}

void DayZ::Mem::fetchBaseAddresses() {
	auto pid = getPid();
	auto baseModule = staticManager.getModule(pid, EXE_NAME);

	this->worldAddress = baseModule.pvmEntry->vaBase + 0x413B418;
	std::cout << "World Address: 0x" << std::hex << this->worldAddress << std::endl;
	this->networkManagerAddress = baseModule.pvmEntry->vaBase + 0xEE7A88;
	std::cout << "NetworkManager Address: 0x" << std::hex << this->networkManagerAddress << std::endl;

};

DWORD DayZ::Mem::getPid() {
	return staticManager.getPid(EXE_NAME);
}

DayZ::WorldPointer DayZ::Mem::getWorld()
{
	DayZ::WorldPointer wp;
	wp.resolveObject(getVMM(), getPid(), this->worldAddress);
	return wp;
}

DayZ::NetworkManager DayZ::Mem::getNetworkManager()
{
	DayZ::NetworkManager nm;
	nm.resolveObject(getVMM(), getPid(), this->networkManagerAddress);
	return nm;
}

std::shared_ptr<std::vector<std::shared_ptr<DayZ::Entity>>> DayZ::Mem::getAllUniqueEntities()
{
	auto map = std::map < QWORD, std::shared_ptr<DayZ::Entity>>();
	auto world = this->getWorld().WorldPtr;
	auto tempSet = std::unique_ptr<std::set<QWORD>>(new std::set<QWORD>());
	for (const auto ent : world->NearEntityTable->resolvedEntities) {
		tempSet->insert(ent->_remoteAddress);
		map[ent->_remoteAddress] = ent;
	}
	for (const auto ent : world->FarEntityTable->resolvedEntities) {
		tempSet->insert(ent->_remoteAddress);
		map[ent->_remoteAddress] = ent;
	}
	for (const auto ent : world->SlowEntityTable->resolvedEntities) {
		tempSet->insert(ent->_remoteAddress);
		map[ent->_remoteAddress] = ent;
	}
	for (const auto ent : world->ItemTable->resolvedEntities) {
		tempSet->insert(ent->_remoteAddress);
		map[ent->_remoteAddress] = ent;
	}
	
	auto returnSet = std::shared_ptr < std::vector<std::shared_ptr<DayZ::Entity>>>(new std::vector<std::shared_ptr<DayZ::Entity>>());
	for (const auto entry : *tempSet) {
		returnSet->push_back(map[entry]);
	}
	return returnSet;
}
