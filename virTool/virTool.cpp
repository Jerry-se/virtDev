#include "virTool.h"

namespace virtDev {

virTool::virTool() : connectPtr_(nullptr), domainPtr_(nullptr) {

}

virTool::~virTool() {
  DomainFree();
  ConnectClose();
}

int virTool::GetVersion(unsigned long *libVer, const char *type, unsigned long *typeVer) {
  return virGetVersion(libVer, type, typeVer);
}

bool virTool::ConnectOpen(const char *name) {
  connectPtr_ = virConnectOpen(name);
  return !!connectPtr_;
}

bool virTool::ConnectOpenReadOnly(const char *name) {
  connectPtr_ = virConnectOpenReadOnly(name);
  return !!connectPtr_;
}

int virTool::ConnectClose() {
  if (!connectPtr_) return -1;
  int ret = virConnectClose(connectPtr_);
  connectPtr_ = nullptr;
  return ret;
}

int virTool::ConnectGetVersion(unsigned long *hvVer) {
  if (!connectPtr_) return -1;
  return virConnectGetVersion(connectPtr_, hvVer);
}

int virTool::ConnectGetLibVersion(unsigned long *libVer) {
  if (!connectPtr_) return -1;
  return virConnectGetLibVersion(connectPtr_, libVer);
}

int virTool::ConnectListAllDomains(virDomainPtr **domains, unsigned int flags) {
  if (!connectPtr_) return -1;
  return virConnectListAllDomains(connectPtr_, domains, flags);
}

int virTool::ConnectListDomains(int *ids, int maxids) {
  if (!connectPtr_) return -1;
  return virConnectListDomains(connectPtr_, ids, maxids);
}

int virTool::ConnectNumOfDomains() {
  if (!connectPtr_) return -1;
  return virConnectNumOfDomains(connectPtr_);
}

int virTool::DomainFree() {
  if (!domainPtr_) return -1;
  int ret = virDomainFree(domainPtr_);
  domainPtr_ = nullptr;
  return ret;
}

int virTool::DomainGetInfo(virDomainInfoPtr info) {
  if (!domainPtr_) return -1;
  return virDomainGetInfo(domainPtr_, info);
}

int virTool::DomainGetState(int *state, int *reason, unsigned int flags) {
  if (!domainPtr_) return -1;
  return virDomainGetState(domainPtr_, state, reason, flags);
}

bool virTool::DomainLookupByID(int id) {
  if (!connectPtr_) return -1;
  domainPtr_ = virDomainLookupByID(connectPtr_, id);
  return !!domainPtr_;
}

bool virTool::DomainLookupByName(const char *name) {
  if (!connectPtr_) return -1;
  domainPtr_ = virDomainLookupByName(connectPtr_, name);
  return !!domainPtr_;
}

bool virTool::DomainLookupByUUID(const unsigned char *uuid) {
  if (!connectPtr_) return -1;
  domainPtr_ = virDomainLookupByUUID(connectPtr_, uuid);
  return !!domainPtr_;
}

bool virTool::DomainLookupByUUIDString(const char *uuid) {
  if (!connectPtr_) return -1;
  domainPtr_ = virDomainLookupByUUIDString(connectPtr_, uuid);
  return !!domainPtr_;
}

int virTool::DomainSuspend() {
  if (!domainPtr_) return -1;
  return virDomainSuspend(domainPtr_);
}

int virTool::DomainResume() {
  if (!domainPtr_) return -1;
  return virDomainResume(domainPtr_);
}

}
