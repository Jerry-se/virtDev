#include "virImpl.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include <tinyxml2.h>

#define __DEBUG__ 1

#ifdef __DEBUG__
#define DebugPrintf(format, ...) printf(format, ##__VA_ARGS__)
#else
#define DebugPrintf(format, ...)
#endif

// void virConnectDeleter(virConnectPtr conn) {
//     virConnectClose(conn);
//     printf("vir connect close\n");
// }

// void virDomainDeleter(virDomainPtr domain) {
//     virDomainFree(domain);
//     printf("vir domain free\n");
// }

// void virDomainSnapshotDeleter(virDomainSnapshotPtr snapshot) {
//     virDomainSnapshotFree(snapshot);
//     printf("vir domain snapshot free\n");
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

struct virDomainSnapshotDeleter {
  inline void operator()(virDomainSnapshotPtr snapshot) {
    virDomainSnapshotFree(snapshot);
    printf("vir domain snapshot free\n");
  }
};

namespace virTool {

// enum virDomainState
static const char* arrayDomainState[] = {"no state", "running", "blocked", "paused", "shutdown", "shutoff", "crashed", "pmsuspended", "last"};
// enum virDomainEventType
static const char* arrayEventType[] = {"VIR_DOMAIN_EVENT_DEFINED", "VIR_DOMAIN_EVENT_UNDEFINED", "VIR_DOMAIN_EVENT_STARTED",
  "VIR_DOMAIN_EVENT_SUSPENDED", "VIR_DOMAIN_EVENT_RESUMED", "VIR_DOMAIN_EVENT_STOPPED",
  "VIR_DOMAIN_EVENT_SHUTDOWN", "VIR_DOMAIN_EVENT_PMSUSPENDED", "VIR_DOMAIN_EVENT_CRASHED"};
// enum event agent state
static const char* arrayEventAgentState[] = {"no state", "agent connected", "agent disconnected", "last"};
// enum event agent lifecycle reason
static const char* arrayEventAgentReason[] = {"unknown state change reason", "state changed due to domain start", "channel state changed", "last"};

int domain_event_cb(virConnectPtr conn, virDomainPtr dom, int event, int detail, void *opaque) {
  printf("event lifecycle cb called, event=%d, detail=%d\n", event, detail);
  if (event >= 0 && event <= virDomainEventType::VIR_DOMAIN_EVENT_CRASHED) {
    const char* name = virDomainGetName(dom);
    int domain_state = 0;
    if (virDomainGetState(dom, &domain_state, NULL, 0) < 0) {
      domain_state = 0;
    }
    printf("domain %s %s, state %s\n", name, arrayEventType[event], arrayDomainState[domain_state]);
  }
  else {
    printf("unknowned event lifecycle\n");
  }
}

void domain_event_agent_cb(virConnectPtr conn, virDomainPtr dom, int state, int reason, void *opaque) {
  printf("event agent lifecycle cb called, state=%d, reason=%d\n", state, reason);
  if (state >= 0 && state <= virConnectDomainEventAgentLifecycleState::VIR_CONNECT_DOMAIN_EVENT_AGENT_LIFECYCLE_STATE_DISCONNECTED) {
    const char* name = virDomainGetName(dom);
    int domain_state = 0;
    if (virDomainGetState(dom, &domain_state, NULL, 0) < 0) {
      domain_state = 0;
    }
    printf("event agent state: %s, reason: %s, domain state: %s\n", arrayEventAgentState[state], arrayEventAgentReason[reason], arrayDomainState[domain_state]);
  }
  else {
    printf("unknowned event agent state\n");
  }
}

static inline void PrintLastError() {
  virError *err = virGetLastError();
  if (err) {
    printf("vir error occured: %s\n", err->message);
    virFreeError(err);
  }
}

static inline void PrintTypedParameter(virTypedParameterPtr params, int nparams) {
  for (int i = 0; i < nparams; i++) {
    printf("parameter[%d]: field=%s, type=%d, value=", i, params[i].field, params[i].type);
    switch (params[i].type)
    {
    case 1:
      printf("%d", params[i].value.i);
      break;
    case 2:
      printf("%ud", params[i].value.ui);
      break;
    case 3:
      printf("%lld", params[i].value.l);
      break;
    case 4:
      printf("%llu", params[i].value.ul);
      break;
    case 5:
      printf("%lf", params[i].value.d);
      break;
    case 6:
      printf("%c", params[i].value.b);
      break;
    case 7:
      printf("%s", params[i].value.s);
      break;

    default:
      printf("unknown value");
      break;
    }
    printf("\n");
  }
}

/////////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& out, const domainDiskInfo& obj) {
  std::cout << " ";
  std::cout << std::setw(8) << std::setfill(' ') << std::left << obj.targetDev;
  std::cout << std::setw(12) << std::setfill(' ') << std::left << obj.driverName;
  std::cout << std::setw(12) << std::setfill(' ') << std::left << obj.driverType;
  std::cout << std::setw(50) << std::setfill(' ') << std::left << obj.sourceFile;
  return out;
}

std::ostream& operator<<(std::ostream& out, const domainSnapshotInfo& obj) {
  if (!obj.name.empty()) {
    std::cout << " ";
    std::cout << std::setw(8) << std::setfill(' ') << std::left << obj.name;
    // boost::posix_time::ptime ctime = boost::posix_time::from_time_t(obj.creationTime);
    // // std::cout << std::setw(28) << std::setfill(' ') << std::left << boost::posix_time::to_simple_string(ctime);
    // // 时间格式 2021-11-30 11:03:55 +0800
    // boost::posix_time::ptime  now = boost::posix_time::second_clock::local_time();
    // boost::posix_time::ptime  utc = boost::posix_time::second_clock::universal_time();
    // boost::posix_time::time_duration tz_offset = (now - utc);

    // std::stringstream   ss;
    // boost::local_time::local_time_facet* output_facet = new boost::local_time::local_time_facet();
    // ss.imbue(std::locale(std::locale::classic(), output_facet));

    // output_facet->format("%H:%M:%S");
    // ss.str("");
    // ss << tz_offset;

    // boost::local_time::time_zone_ptr    zone(new boost::local_time::posix_time_zone(ss.str().c_str()));
    // boost::local_time::local_date_time  ldt(ctime, zone);
    // output_facet->format("%Y-%m-%d %H:%M:%S %Q");
    // ss.str("");
    // ss << ldt;
    // std::cout << std::setw(28) << std::setfill(' ') << std::left << ss.str();
    // delete output_facet;
    std::cout << std::setw(16) << std::setfill(' ') << std::left << obj.state;
    std::cout << std::setw(50) << std::setfill(' ') << std::left << obj.description;
    #if 1
    std::cout << std::endl;
    for (int i = 0; i < obj.disks.size(); i++) {
      std::cout << " disk name=" << obj.disks[i].name << ", snapshot=" << obj.disks[i].snapshot
                << ", driver_type=" << obj.disks[i].driver_type << ", source_file=" << obj.disks[i].source_file;
      if (i != obj.disks.size() - 1)
        std::cout << std::endl;
    }
    #endif
  }
  return out;
}

int getDomainSnapshotInfo(virDomainSnapshotPtr snapshot, domainSnapshotInfo &info) {
  int ret = -1;
  if (snapshot == nullptr) return ret;
  info.name = virDomainSnapshotGetName(snapshot);
  if (info.name.empty()) return ret;
  char *xmlDesc = virDomainSnapshotGetXMLDesc(snapshot, 0);
  if (xmlDesc) {
    do {
      tinyxml2::XMLDocument doc;
      tinyxml2::XMLError err = doc.Parse(xmlDesc);
      if (err != tinyxml2::XML_SUCCESS) {
        std::cout << "parse domain snapshot xml desc error: " << err << std::endl;
        break;
      }
      tinyxml2::XMLElement *root = doc.RootElement();
      tinyxml2::XMLElement *desc = root->FirstChildElement("description");
      if (desc) {
        info.description = desc->GetText();
      }
      tinyxml2::XMLElement *state = root->FirstChildElement("state");
      if (state) {
        info.state = state->GetText();
      }
      tinyxml2::XMLElement *creationTime = root->FirstChildElement("creationTime");
      if (creationTime) {
        info.creationTime = atoll(creationTime->GetText());
      }
      tinyxml2::XMLElement *disks = root->FirstChildElement("disks");
      if (disks) {
        tinyxml2::XMLElement *disk = disks->FirstChildElement("disk");
        while (disk) {
          domainSnapshotDiskInfo dsinfo;
          dsinfo.name = disk->Attribute("name");
          dsinfo.snapshot = disk->Attribute("snapshot");
          tinyxml2::XMLElement *driver = disk->FirstChildElement("driver");
          if (driver) {
            dsinfo.driver_type = driver->Attribute("type");
          }
          tinyxml2::XMLElement *source = disk->FirstChildElement("source");
          if (source) {
            dsinfo.source_file = source->Attribute("file");
          }
          info.disks.push_back(dsinfo);
          disk = disk->NextSiblingElement("disk");
        }
      }
      ret = 0;
    } while(0);
    free(xmlDesc);
  }
  return ret;
}

/////////////////////////////////////////////////////////////////////////////////

virDomainSnapshotImpl::virDomainSnapshotImpl(virDomainSnapshotPtr snapshot)
  : snapshot_(std::shared_ptr<virDomainSnapshot>(snapshot, virDomainSnapshotDeleter())) {

}

virDomainSnapshotImpl::~virDomainSnapshotImpl() {

}

int32_t virDomainSnapshotImpl::revertDomainToThisSnapshot(int flags) {
  if (!snapshot_) return -1;
  return virDomainRevertToSnapshot(snapshot_.get(), flags);
}

int32_t virDomainSnapshotImpl::deleteSnapshot(int flags) {
  if (!snapshot_) return -1;
  return virDomainSnapshotDelete(snapshot_.get(), flags);
}

int32_t virDomainSnapshotImpl::getSnapshotName(std::string &name) {
  if (!snapshot_) return -1;
  name = virDomainSnapshotGetName(snapshot_.get());
  return name.empty() ? -1 : 0;
}

std::shared_ptr<virDomainSnapshotImpl> virDomainSnapshotImpl::getSnapshotParent() {
  if (!snapshot_) return nullptr;
  virDomainSnapshotPtr snap = virDomainSnapshotGetParent(snapshot_.get(), 0);
  if (snap == nullptr) {
    return nullptr;
  }
  return std::make_shared<virDomainSnapshotImpl>(snap);
}

int32_t virDomainSnapshotImpl::getSnapshotXMLDesc(std::string &desc) {
  if (!snapshot_) return -1;
  char *xml = virDomainSnapshotGetXMLDesc(snapshot_.get(), 0);
  desc = xml;
  free(xml);
  return desc.empty() ? -1 : 0;
}

int32_t virDomainSnapshotImpl::listAllSnapshotChilden() {
  if (!snapshot_) return -1;
  virDomainSnapshotPtr *snaps = nullptr;
  int snaps_count = 0;
  if ((snaps_count = virDomainSnapshotListAllChildren(snapshot_.get(), &snaps, 1 << 10)) < 0)
    goto cleanup;
  for (int i = 0; i < snaps_count; i++) {
    const char *name = virDomainSnapshotGetName(snaps[i]);
    if (name) {
      printf("list all childen snapshots names[%d]: %s\n", i, name);
      // free(name);
    }
  }
cleanup:
  if (snaps && snaps_count > 0) {
    for (int i = 0; i < snaps_count; i++) {
      virDomainSnapshotFree(snaps[i]);
    }
  }
  if (snaps)
    free(snaps);
  return snaps_count;
}

int32_t virDomainSnapshotImpl::listAllSnapshotChildenNames(std::vector<std::string> *names) {
  return -1;
}

int virDomainSnapshotImpl::getSnapshotChildenNums() {
  if (!snapshot_) return -1;
  return virDomainSnapshotNumChildren(snapshot_.get(), 1 << 10);
}

int32_t virDomainSnapshotImpl::getSnapshotInfo(domainSnapshotInfo &info) {
  if (!snapshot_) return -1;
  return getDomainSnapshotInfo(snapshot_.get(), info);
}

/////////////////////////////////////////////////////////////////////////////////

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

int32_t virDomainImpl::rebootDomain(int flag) {
  if (!domain_) return -1;
  return virDomainReboot(domain_.get(), flag);
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

int32_t virDomainImpl::deleteDomain() {
  if (!domain_) return -1;
  virDomainInfo info;
  int ret = virDomainGetInfo(domain_.get(), &info);
  if (ret < 0) return ret;
  std::vector<domainDiskInfo> disks;
  getDomainDisks(disks);
  if (info.state == VIR_DOMAIN_RUNNING) {
    ret = destroyDomain();
    if (ret < 0) return ret;
  }
  ret = undefineDomain();
  if (ret < 0) return ret;
  for (const auto& disk_file : disks) {
    if (access(disk_file.sourceFile.c_str(), F_OK) != -1) {
      remove(disk_file.sourceFile.c_str());
      printf("delete file: %s\n", disk_file.sourceFile.c_str());
    }
  }
  return 0;
}

int32_t virDomainImpl::getDomainDisks(std::vector<domainDiskInfo> &disks) {
  int ret = -1;
  if (!domain_) return ret;
  char* pContent = virDomainGetXMLDesc(domain_.get(), VIR_DOMAIN_XML_SECURE);
  if (!pContent) return ret;
  do {
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError err = doc.Parse(pContent);
    if (err != tinyxml2::XML_SUCCESS) break;
    tinyxml2::XMLElement* root = doc.RootElement();
    tinyxml2::XMLElement* devices_node = root->FirstChildElement("devices");
    if (!devices_node) break;
    tinyxml2::XMLElement* disk_node = devices_node->FirstChildElement("disk");
    while (disk_node) {
      domainDiskInfo ddInfo;
      tinyxml2::XMLElement* disk_driver_node = disk_node->FirstChildElement("driver");
      if (disk_driver_node) {
        ddInfo.driverName = disk_driver_node->Attribute("name");
        ddInfo.driverType = disk_driver_node->Attribute("type");
      }
      tinyxml2::XMLElement* disk_source_node = disk_node->FirstChildElement("source");
      if (disk_source_node) {
        ddInfo.sourceFile = disk_source_node->Attribute("file");
      }
      tinyxml2::XMLElement* disk_target_node = disk_node->FirstChildElement("target");
      if (disk_target_node) {
        ddInfo.targetDev = disk_target_node->Attribute("dev");
        ddInfo.targetBus = disk_target_node->Attribute("bus");
      }
      disks.push_back(ddInfo);
      disk_node = disk_node->NextSiblingElement("disk");
    }
    ret = 0;
  } while(0);

  free(pContent);
  return ret;
}

int virDomainImpl::getDomainInfo(virDomainInfoPtr info) {
  if (!domain_) return -1;
  return virDomainGetInfo(domain_.get(), info);
}

int virDomainImpl::getDomainState(int *state, int *reason, unsigned int flags) {
  if (!domain_) return -1;
  return virDomainGetState(domain_.get(), state, reason, flags);
}

int virDomainImpl::getDomainStatsList() {
  if (!domain_) return -1;
  virDomainStatsRecordPtr *retStats = NULL;
  virDomainPtr doms[] = {domain_.get(), NULL};
  int retStats_count = virDomainListGetStats(doms, VIR_DOMAIN_STATS_BLOCK, &retStats, VIR_CONNECT_GET_ALL_DOMAINS_STATS_ENFORCE_STATS);
  if (retStats_count < 0)
    goto error;
  for (int i = 0; i < retStats_count; i++) {
    // virTypedParameterPtr params = retStats[i]->params;
    // for (int j = 0; j < retStats[i]->nparams; j++) {
    //   printf("block parameter[%d]: field=%s, type=%d\n", j, params[j].field, params[j].type);
    // }
    PrintTypedParameter(retStats[i]->params, retStats[i]->nparams);
  }
error:
  if (retStats) {
    virDomainStatsRecordListFree(retStats);
  }
  return retStats_count;
}

int virDomainImpl::getDomainBlockInfo(const char *disk) {
  if (!domain_) return -1;
  virDomainBlockInfo info;
  int ret = virDomainGetBlockInfo(domain_.get(), disk, &info, 0);
  if (ret < 0) return ret;
  if (info.allocation == info.physical) {
    // If the domain is no longer active,
    // then the defaults are being returned.
    if (!virDomainIsActive(domain_.get())) {
      printf("domain is not active, block info is default\n");
      return ret;
    }
  }
  // Do something with the allocation and physical values
  printf("device:%s, block capacity:%llu, allocation:%llu, physical:%llu\n", disk, info.capacity, info.allocation, info.physical);
  return ret;
}

int virDomainImpl::getDomainBlockParameters() {
  if (!domain_) return -1;
  virTypedParameterPtr params = nullptr;
  int nparams = 0;
  if (virDomainGetBlkioParameters(domain_.get(), NULL, &nparams, 0) == 0 && nparams != 0) {
    if ((params = (virTypedParameterPtr)malloc(sizeof(*params) * nparams)) == NULL)
      goto error;
    memset(params, 0, sizeof(*params) * nparams);
    if (virDomainGetMemoryParameters(domain_.get(), params, &nparams, 0) < 0)
      goto error;
    // for (int i = 0; i < nparams; i++) {
    //   printf("block parameter[%d]: field=%s, type=%d\n", i, params[i].field, params[i].type);
    // }
    PrintTypedParameter(params, nparams);
  }
error:
  if (params)
    free(params);
  return nparams;
}

int virDomainImpl::getDomainBlockIoTune(const char *disk) {
  if (!domain_) return -1;
  virTypedParameterPtr params = nullptr;
  int nparams = 0;
  if (virDomainGetBlockIoTune(domain_.get(), disk, NULL, &nparams, 0) == 0 && nparams != 0) {
    if ((params = (virTypedParameterPtr)malloc(sizeof(*params) * nparams)) == NULL)
      goto error;
    memset(params, 0, sizeof(*params) * nparams);
    if (virDomainGetBlockIoTune(domain_.get(), disk, params, &nparams, 0) < 0)
      goto error;
    // for (int i = 0; i < nparams; i++) {
    //   printf("block parameter[%d]: field=%s, type=%d\n", i, params[i].field, params[i].type);
    // }
    PrintTypedParameter(params, nparams);
  }
error:
  if (params)
    free(params);
  return nparams;
}

int virDomainImpl::getDomainFSInfo() {
  if (!domain_) return -1;
  virDomainFSInfoPtr *fsInfos = nullptr;
  int fsInfos_count = virDomainGetFSInfo(domain_.get(), &fsInfos, 0);
  if (fsInfos_count < 0)
    goto cleanup;
  for (int i = 0; i < fsInfos_count; i++) {
    printf("device name:%s, mountpoint:%s, fstype:%s", fsInfos[i]->name, fsInfos[i]->mountpoint, fsInfos[i]->fstype);
    for (int j = 0; j < fsInfos[i]->ndevAlias; j++) {
      printf(", devAlias%d:%s", j, fsInfos[i]->devAlias[j]);
    }
    printf("\n");
  }
cleanup:
  if (fsInfos && fsInfos_count > 0) {
    for (int i = 0; i < fsInfos_count; i++) {
      virDomainFSInfoFree(fsInfos[i]);
    }
  }
  free(fsInfos);
  return fsInfos_count;
}

int virDomainImpl::getDomainGuestInfo() {
  if (!domain_) return -1;
  virTypedParameterPtr *params = nullptr;
  int nparams = 0;
  // https://libvirt.org/news.html
  // v5.7.0 (2019-09-03) add virDomainGetGuestInfo method
//   int ret = virDomainGetGuestInfo(domain_.get(), VIR_DOMAIN_GUEST_INFO_DISKS, &params, &nparams, 0);
//   if (ret < 0)
//     goto cleanup;
//   for (int i = 0; i < nparams; i++) {
//     // TODO
//     printf("\n");
//   }
// cleanup:
//   if (params && nparams > 0) {
//     for (int i = 0; i < nparams; i++) {
//       virTypedParamsFree(params[i]);
//     }
//   }
//   free(params);
  return 0;
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
      printf("[addr: %s prefix: %d type: %d]", ip_addr->addr, ip_addr->prefix, ip_addr->type);
    }
    printf("\n");
  }

cleanup:
  if (ifaces && ifaces_count > 0) {
    for (i = 0; i < ifaces_count; i++) {
      virDomainInterfaceFree(ifaces[i]);
    }
  }
  if (ifaces)
    free(ifaces);
  return ifaces_count;
}

int virDomainImpl::setDomainUserPassword(const char *user, const char *password) {
  if (!domain_) return -1;
  return virDomainSetUserPassword(domain_.get(), user, password, 0);
}

std::shared_ptr<virDomainSnapshotImpl> virDomainImpl::createSnapshot(const char *xmlDesc, unsigned int flags) {
  if (!domain_) return nullptr;
  virDomainSnapshotPtr snapshot = virDomainSnapshotCreateXML(domain_.get(), xmlDesc, flags);
  if (snapshot == nullptr) {
    // print last error
    return nullptr;
  }
  return std::make_shared<virDomainSnapshotImpl>(snapshot);
}

std::shared_ptr<virDomainSnapshotImpl> virDomainImpl::getSnapshotByName(const char *name) {
  if (!domain_) return nullptr;
  virDomainSnapshotPtr snapshot = virDomainSnapshotLookupByName(domain_.get(), name, 0);
  if (snapshot == nullptr) {
    // print last error
    return nullptr;
  }
  return std::make_shared<virDomainSnapshotImpl>(snapshot);
}

int32_t virDomainImpl::listAllSnapshots(std::vector<std::shared_ptr<virDomainSnapshotImpl>> &snapshots, unsigned int flags) {
  if (!domain_) return -1;
  virDomainSnapshotPtr *snaps = nullptr;
  int snaps_count = 0;
  if ((snaps_count = virDomainListAllSnapshots(domain_.get(), &snaps, flags)) < 0)
    goto cleanup;
  std::cout << " ";
  std::cout << std::setw(8) << std::setfill(' ') << std::left << "Name";
  std::cout << std::setw(28) << std::setfill(' ') << std::left << "Creation Time";
  std::cout << std::setw(16) << std::setfill(' ') << std::left << "State";
  std::cout << std::setw(50) << std::setfill(' ') << std::left << "description";
  std::cout << std::endl;
  std::cout << std::setw(1 + 8 + 28 + 16 + 50) << std::setfill('-') << std::left << "" << std::endl;
  // printf(" Name    Creation Time               State\n");
  // printf("----------------------------------------------------\n");
  for (int i = 0; i < snaps_count; i++) {
    domainSnapshotInfo dsInfo;
    getDomainSnapshotInfo(snaps[i], dsInfo);
    std::cout << dsInfo << std::endl;
  }
cleanup:
  if (snaps && snaps_count > 0) {
    for (int i = 0; i < snaps_count; i++) {
      // virDomainSnapshotFree(snaps[i]);
      snapshots.push_back(std::make_shared<virDomainSnapshotImpl>(snaps[i]));
    }
  }
  if (snaps)
    free(snaps);
  return snaps_count;
}

int32_t virDomainImpl::listSnapshotNames(std::vector<std::string> &names, int nameslen, unsigned int flags) {
  if (!domain_ && nameslen < 1) return -1;
  char **ns = (char**)malloc(sizeof(char*) * nameslen);
  int nlen = 0;
  // libvirt官网不鼓励使用此API，而是推荐使用virDomainListAllSnapshots ()。
  if ((nlen = virDomainSnapshotListNames(domain_.get(), ns, nameslen, flags)) < 0)
    goto cleanup;
  // for (int i = 0; i < nlen; i++) {
  //   printf("domain snapshot names[%d]: %s\n", i, ns[i]);
  // }
cleanup:
  if (ns && nlen > 0) {
    for (int i = 0; i < nlen; i++) {
      names.push_back(ns[i]);
      free(ns[i]);
    }
  }
  if (ns)
    free(ns);
  return nlen;
}

int32_t virDomainImpl::getSnapshotNums(unsigned int flags) {
  if (!domain_) return -1;
  return virDomainSnapshotNum(domain_.get(), flags);
}

/////////////////////////////////////////////////////////////////////////////////

virTool::virTool(bool enableEvent)
  : conn_(nullptr)
  , enable_event_(enableEvent)
  , callback_id_(-1)
  , agent_callback_id_(-1)
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
  if (conn_ && enable_event_) {
    virConnectDomainEventDeregisterAny(conn_.get(), callback_id_);
    virConnectDomainEventDeregisterAny(conn_.get(), agent_callback_id_);
  }
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
    agent_callback_id_ = virConnectDomainEventRegisterAny(connectPtr, NULL,
      virDomainEventID::VIR_DOMAIN_EVENT_ID_AGENT_LIFECYCLE, VIR_DOMAIN_EVENT_CALLBACK(domain_event_agent_cb), NULL, NULL);
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
      printf("virEventRunDefaultImpl failed: %s\n", err ? err->message : "");
      if (err) virFreeError(err);
    }
  }
  printf("vir event loop thread end\n");
}

}
