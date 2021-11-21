#include "virImpl.h"
#include <stdlib.h>
#include <stdio.h>

// void virConnectDeleter(virConnectPtr conn) {
//     virConnectClose(conn);
//     printf("vir connect close\n");
// }

// void virDomainDeleter(virDomainPtr domain) {
//     virDomainFree(domain);
//     printf("vir domain free\n");
// }

struct virConnectDeleter {
    inline void operator()(virConnectPtr conn) {
        virConnectClose(conn);
        printf("vir connect close\n");
    }
};

struct virDomainDeleter {
    inline void operator()(virDomainPtr domain) {
        virDomainFree(domain);
        printf("vir domain free\n");
    }
};

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

static inline void PrintLastError() {
  virError *err = virGetLastError();
  if (err) {
    printf("vir error occured: %s\n", err->message);
    virFreeError(err);
  }
}

virDomainImpl::virDomainImpl(virDomainPtr domain)
  : domain_(std::shared_ptr<virDomain>(domain, virDomainDeleter())) {
}

virDomainImpl::~virDomainImpl() {

}

int32_t virDomainImpl::startDomain() {
  if (!domain_) return -1;
  return virDomainCreate(domain_.get());
}

int32_t virDomainImpl::suspendDomain() {
  if (!domain_) return -1;
  return virDomainSuspend(domain_.get());
}

int32_t virDomainImpl::resumeDomain() {
  if (!domain_) return -1;
  return virDomainResume(domain_.get());
}

int32_t virDomainImpl::rebootDomain() {
  if (!domain_) return -1;
  return virDomainReboot(domain_.get(), VIR_DOMAIN_REBOOT_DEFAULT);
}

int32_t virDomainImpl::shutdownDomain() {
  if (!domain_) return -1;
  return virDomainShutdown(domain_.get());
}

int32_t virDomainImpl::destroyDomain() {
  if (!domain_) return -1;
  return virDomainDestroy(domain_.get());
}

int32_t virDomainImpl::resetDomain() {
  if (!domain_) return -1;
  return virDomainReset(domain_.get(), 0);
}

int32_t virDomainImpl::undefineDomain() {
  if (!domain_) return -1;
  return virDomainUndefine(domain_.get());
}

int virDomainImpl::getDomainInfo(virDomainInfoPtr info) {
  if (!domain_) return -1;
  return virDomainGetInfo(domain_.get(), info);
}

int virDomainImpl::getDomainState(int *state, int *reason, unsigned int flags) {
  if (!domain_) return -1;
  return virDomainGetState(domain_.get(), state, reason, flags);
}

int virDomainImpl::getDomainInterfaceAddress() {
  if (!domain_) return -1;
  virDomainInterfacePtr *ifaces = nullptr;
  int ifaces_count = 0;
  size_t i, j;

  if ((ifaces_count = virDomainInterfaceAddresses(domain_.get(), &ifaces,
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

int virDomainImpl::setDomainUserPassword(const char *user, const char *password) {
  if (!domain_) return -1;
  return virDomainSetUserPassword(domain_.get(), user, password, 0);
}

/////////////////////////////////////////////////////////////////////////////////

virTool::virTool(bool enableEvent)
  : conn_(nullptr)
  , enable_event_(enableEvent)
  , callback_id_(-1)
  , thread_quit_(1)
  , thread_event_loop_(nullptr) {
  if (enableEvent) {
    int ret = virEventRegisterDefaultImpl();
    if (ret < 0) {
      printf("virEventRegisterDefaultImpl failed\n");
    }
  }
}

virTool::~virTool() {
  thread_quit_ = 1;
  if (thread_event_loop_) {
    if (thread_event_loop_->joinable())
      thread_event_loop_->join();
    delete thread_event_loop_;
  }
  if (conn_ && enable_event_)
    virConnectDomainEventDeregisterAny(conn_.get(), callback_id_);
}

int virTool::getVersion(unsigned long *libVer, const char *type, unsigned long *typeVer) {
  return virGetVersion(libVer, type, typeVer);
}

int virTool::getConnectVersion(unsigned long *hvVer) {
  if (!conn_) return -1;
  return virConnectGetVersion(conn_.get(), hvVer);
}

int virTool::getConnectLibVersion(unsigned long *libVer) {
  if (!conn_) return -1;
  return virConnectGetLibVersion(conn_.get(), libVer);
}

bool virTool::openConnect(const char *name) {
  // if (conn_) return true;
  virConnectPtr connectPtr = virConnectOpen(name);
  if (connectPtr == nullptr) {
      return false;
  }
  conn_.reset(connectPtr, virConnectDeleter());
  if (connectPtr && enable_event_) {
    callback_id_ = virConnectDomainEventRegisterAny(connectPtr, NULL,
      virDomainEventID::VIR_DOMAIN_EVENT_ID_LIFECYCLE, VIR_DOMAIN_EVENT_CALLBACK(domain_event_cb), NULL, NULL);
    thread_quit_ = 0;
    thread_event_loop_ = new std::thread(&virTool::DefaultThreadFunc, this);
  }
  return !!conn_;
}

bool virTool::openConnectReadOnly(const char *name) {
  // virConnectPtr connectPtr = virConnectOpenReadOnly(name);
  // return !!connectPtr;
  return false;
}

int virTool::listAllDomains(virDomainPtr **domains, unsigned int flags) {
  if (!conn_) return -1;
  return virConnectListAllDomains(conn_.get(), domains, flags);
}

int virTool::listDomains(int *ids, int maxids) {
  if (!conn_) return -1;
  return virConnectListDomains(conn_.get(), ids, maxids);
}

int virTool::numOfDomains() {
  if (!conn_) return -1;
  return virConnectNumOfDomains(conn_.get());
}

std::shared_ptr<virDomainImpl> virTool::openDomainByID(int id) {
  if (!conn_) return nullptr;
  virDomainPtr domainPtr = virDomainLookupByID(conn_.get(), id);
  if (nullptr == domainPtr) {
    return nullptr;
  }
  return std::make_shared<virDomainImpl>(domainPtr);
}

std::shared_ptr<virDomainImpl> virTool::openDomainByName(const char *domain_name) {
  if (!conn_) return nullptr;
  virDomainPtr domainPtr = virDomainLookupByName(conn_.get(), domain_name);
  if (nullptr == domainPtr) {
    return nullptr;
  }
  return std::make_shared<virDomainImpl>(domainPtr);
}

std::shared_ptr<virDomainImpl> virTool::openDomainByUUID(const unsigned char *uuid) {
  if (!conn_) return nullptr;
  virDomainPtr domainPtr = virDomainLookupByUUID(conn_.get(), uuid);
  if (nullptr == domainPtr) {
    return nullptr;
  }
  return std::make_shared<virDomainImpl>(domainPtr);
}

std::shared_ptr<virDomainImpl> virTool::openDomainByUUIDString(const char *uuid) {
  if (!conn_) return nullptr;
  virDomainPtr domainPtr = virDomainLookupByUUIDString(conn_.get(), uuid);
  if (nullptr == domainPtr) {
    return nullptr;
  }
  return std::make_shared<virDomainImpl>(domainPtr);
}

std::shared_ptr<virDomainImpl> virTool::createDomain(const char *xml_content) {
  if (!conn_) return nullptr;
  virDomainPtr domainPtr = virDomainDefineXML(conn_.get(), xml_content);
  if (nullptr == domainPtr) {
    return nullptr;
  }
  std::shared_ptr<virDomainImpl> dom = std::make_shared<virDomainImpl>(domainPtr);
  if (dom && dom->startDomain() < 0) {
    return nullptr;
  }
  return std::move(dom);
}

std::shared_ptr<virDomainImpl> virTool::defineDomain(const char *xml_content) {
  if (!conn_) return nullptr;
  virDomainPtr domainPtr = virDomainDefineXML(conn_.get(), xml_content);
  if (nullptr == domainPtr) {
    return nullptr;
  }
  return std::make_shared<virDomainImpl>(domainPtr);
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
