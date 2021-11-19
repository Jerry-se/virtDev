#include "virImpl.h"
#include <stdlib.h>
#include <stdio.h>

namespace virTool {

// enum virDomainState
static const char* arrayState[] = {"no state", "running", "blocked", "paused", "shutdown", "shutoff", "crashed", "pmsuspended", "last"};
// enum virDomainEventType
static const char* arrayEventType[] = {"VIR_DOMAIN_EVENT_DEFINED", "VIR_DOMAIN_EVENT_UNDEFINED", "VIR_DOMAIN_EVENT_STARTED",
  "VIR_DOMAIN_EVENT_SUSPENDED", "VIR_DOMAIN_EVENT_RESUMED", "VIR_DOMAIN_EVENT_STOPPED",
  "VIR_DOMAIN_EVENT_SHUTDOWN", "VIR_DOMAIN_EVENT_PMSUSPENDED", "VIR_DOMAIN_EVENT_CRASHED"};

int domain_event_cb(virConnectPtr conn, virDomainPtr dom, int event, int detail, void *opaque) {
  printf("event lifecycle cb called, event=%d, detail=%d\n", event, detail);
  if (event >= 0 && event <= virDomainEventType::VIR_DOMAIN_EVENT_CRASHED) {
    const char* name = virDomainGetName(dom);
    int domain_state = 0;
    if (virDomainGetState(dom, &domain_state, NULL, 0) < 0) {
      domain_state = 0;
    }
    printf("domain %s %s, state %s\n", name, arrayEventType[event], arrayState[domain_state]);
  }
  else {
    printf("unknowned event\n");
  }
}

virTool::virTool(virToolDelegate *delegate) :
 connectPtr_(nullptr),
 domainPtr_(nullptr),
 delegate_(delegate),
 thread_quit_(1),
 thread_event_loop_(nullptr) {
  int ret = virEventRegisterDefaultImpl();
  if (ret < 0) {
    printf("virEventRegisterDefaultImpl failed\n");
  }
}

virTool::~virTool() {
  thread_quit_ = 1;
  if (thread_event_loop_ && thread_event_loop_->joinable())
    thread_event_loop_->join();
  if (connectPtr_)
    virConnectDomainEventDeregisterAny(connectPtr_, callback_id_);
  DomainFree();
  ConnectClose();
}

int virTool::GetVersion(unsigned long *libVer, const char *type, unsigned long *typeVer) {
  return virGetVersion(libVer, type, typeVer);
}

bool virTool::ConnectOpen(const char *name) {
  connectPtr_ = virConnectOpen(name);
  if (connectPtr_) {
    callback_id_ = virConnectDomainEventRegisterAny(connectPtr_, NULL,
      virDomainEventID::VIR_DOMAIN_EVENT_ID_LIFECYCLE, VIR_DOMAIN_EVENT_CALLBACK(domain_event_cb), NULL, NULL);
    thread_quit_ = 0;
    thread_event_loop_ = new std::thread(&virTool::DefaultThreadFunc, this);
  }
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

void virTool::PrintLastError() {
  virError *err = virGetLastError();
  if (err) {
    if (delegate_)
      delegate_->PrintErrorMessage(err);
    printf("vir error occured: %s\n", err->message);
    virFreeError(err);
  }
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

virDomainPtr virTool::DomainDefineXML(const char *xml) {
  if (!connectPtr_) return nullptr;
  return virDomainDefineXML(connectPtr_, xml);
}

int virTool::DomainCreate(const char *xml) {
  if (domainPtr_) return -2;
  if (!connectPtr_) return -1;
  virDomainPtr domain = DomainDefineXML(xml);
  if (!domain) {
    PrintLastError();
    return -1;
  }
  int ret = virDomainCreate(domain);
  if (ret < 0) {
    PrintLastError();
    virDomainFree(domain);
    return ret;
  }
  domainPtr_ = domain;
  return ret;
}

int virTool::DomainInterfaceAddress() {
  if (!domainPtr_) return -1;
  virDomainInterfacePtr *ifaces = nullptr;
  int ifaces_count = 0;
  size_t i, j;

  if ((ifaces_count = virDomainInterfaceAddresses(domainPtr_, &ifaces,
          VIR_DOMAIN_INTERFACE_ADDRESSES_SRC_LEASE, 0)) < 0)
    goto cleanup;

  for (i = 0; i < ifaces_count; i++) {
    printf("name: %s", ifaces[i]->name);
    if (ifaces[i]->hwaddr)
        printf(" hwaddr: %s", ifaces[i]->hwaddr);

    for (j = 0; j < ifaces[i]->naddrs; j++) {
        virDomainIPAddressPtr ip_addr = ifaces[i]->addrs + j;
        printf("[addr: %s prefix: %d type: %d]",
               ip_addr->addr, ip_addr->prefix, ip_addr->type);
    }
    printf("\n");
  }

cleanup:
  if (ifaces && ifaces_count > 0) {
    for (i = 0; i < ifaces_count; i++) {
      virDomainInterfaceFree(ifaces[i]);
    }
  }
  free(ifaces);
  return ifaces_count;
}

int virTool::DomainSetUserPassword(const char *user, const char *password) {
  if (!domainPtr_) return -1;
  int ret = virDomainSetUserPassword(domainPtr_, user, password, 0);
  if (ret != 0) {
    PrintLastError();
  }
  return ret;
}

int virTool::DomainSuspend() {
  if (!domainPtr_) return -1;
  return virDomainSuspend(domainPtr_);
}

int virTool::DomainResume() {
  if (!domainPtr_) return -1;
  return virDomainResume(domainPtr_);
}

void virTool::DefaultThreadFunc() {
  printf("vir event loop thread begin\n");
  while (thread_quit_ == 0) {
    if (virEventRunDefaultImpl() < 0) {
      virErrorPtr err = virGetLastError();
      printf("virEventRunDefaultImpl failed: %s\n", err->message);
    }
  }
  printf("vir event loop thread end\n");
}

}
