#include "virtDev.h"

#include <iostream>
#include <fstream>
#include <string>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <iomanip>

#include "virImpl.h"

namespace virTool {
  const static char* virUri = "qemu+tcp://localhost:16509/system";

  std::string stringPrintf(const char* format, ...) {
    char str[1024];
    va_list ap;
    va_start(ap, format);
    int n = vsnprintf(str, sizeof(str), format, ap);
    va_end(ap);
    return str;
  }

  std::string translateVirVer(unsigned long ver) {
    unsigned long major = ver / 1000000;
    ver = ver % 1000000;
    unsigned long minor = ver / 1000;
    unsigned long micro = ver % 1000;
    return stringPrintf("%lu.%lu.%lu", major, minor, micro);
  }

  static inline void PrintLastError() {
    virError *err = virGetLastError();
    if (err) {
      std::cout << "vir error occured: " << err->message << std::endl;
      virFreeError(err);
    }
  }

  class virToolDelegateImpl : public virToolDelegate {
  public:
    virToolDelegateImpl() {}
    ~virToolDelegateImpl() {}
    void PrintErrorMessage(virError *err) override {
      std::cout << "vir error occured: " << err->message << std::endl;
    }
  };

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
    // C语言方法
    // struct tm _tm{};
    // localtime_r(&obj.creationTime, &_tm);
    // char buf[256] = {0};
    // strftime(buf, sizeof(char) * 256, "%Y-%m-%d %H:%M:%S %z", &_tm);
    // std::cout << std::setw(28) << std::setfill(' ') << std::left << buf;
    std::stringstream   ss;
    ss.str("");
    ss << std::put_time(std::localtime(&obj.creationTime), "%Y-%m-%d %H:%M:%S %z");
    std::cout << std::setw(28) << std::setfill(' ') << std::left << ss.str();
    std::cout << std::setw(16) << std::setfill(' ') << std::left << obj.state;
    std::cout << std::setw(50) << std::setfill(' ') << std::left << obj.description;
    #if 0
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

/////////////////////////////////////////////////////////////////////////////////

  void parseVersion() {
    virTool virTool_;
    if (!virTool_.openConnect(virUri)) {
      PrintLastError();
      std::cout << "open connect failed" << std::endl;
      return;
    }
    unsigned long libVer = 0;
    if (virTool::getVersion(&libVer, NULL, NULL) == 0)
      std::cout << "virGetVersion: " << translateVirVer(libVer) << " " << libVer << std::endl;
    if (virTool_.getConnectVersion(&libVer) == 0)
      std::cout << "virConnectGetVersion: " << translateVirVer(libVer) << " " << libVer << std::endl;
    if (virTool_.getConnectLibVersion(&libVer) == 0)
      std::cout << "virConnectGetLibVersion: " << translateVirVer(libVer) << " " << libVer << std::endl;
  }

  void parseList() {
    virTool virTool_;
    if (!virTool_.openConnect(virUri)) {
      PrintLastError();
      std::cout << "open connect failed" << std::endl;
      return;
    }
    // 方法一，不推荐
    // int nums = virTool_.ConnectNumOfDomains();
    // if (nums < 1) {
    //   std::cout << "found none domain" << std::endl;
    //   return;
    // }
    // int *ids = new int[nums];
    // if (virTool_.ConnectListDomains(ids, nums) != -1) {
    //   for (int i = 0; i < nums; i++) {
    //     if (virTool_.DomainLookupByID(ids[i])) {
    //       virDomainInfo info;
    //       if (virTool_.DomainGetInfo(&info) == 0) {
    //         std::cout << stringPrintf("domain id: %d, state: %hhu, maxMem: %lu, memory: %lu, nrVirtCpu: %hu, cpuTime: %llu",
    //         ids[i], info.state, info.maxMem, info.memory, info.nrVirtCpu, info.cpuTime) << std::endl;
    //       }
    //       virTool_.DomainFree();
    //     }
    //   }
    // }
    // delete[] ids;
    // 方法二
    virDomainPtr *domains;
    unsigned int flags = 0b11111111111111;// VIR_CONNECT_LIST_DOMAINS_RUNNING | VIR_CONNECT_LIST_DOMAINS_PERSISTENT;
    int ret = virTool_.listAllDomains(&domains, flags);
    if (ret < 0)
      std::cout << "virConnectListAllDomains error" << std::endl;
    for (int i = 0; i < ret; i++) {
      // do_something_with_domain(domains[i]);
      virDomainInfo info;
      if (virDomainGetInfo(domains[i], &info) == 0) {
        std::cout << stringPrintf("domain id: %d, state: %hhu, maxMem: %lu, memory: %lu, nrVirtCpu: %hu, cpuTime: %llu",
        virDomainGetID(domains[i]), info.state, info.maxMem, info.memory, info.nrVirtCpu, info.cpuTime) << std::endl;
      }
      //here or in a separate loop if needed
      virDomainFree(domains[i]);
    }
    free(domains);
  }

  void parseCreateDomain(const char* xml_file) {
    std::string xml_content;
    {
      std::ifstream ifs(xml_file, std::ios::in);
      if (ifs.is_open()) {
        ifs.seekg(0, std::ios::end);
        int length = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        xml_content.resize(length);
        ifs.read(&xml_content[0], length);
        ifs.close();
      } else {
        std::cout << "open xml file failed" << std::endl;
        return;
      }
    }
    if (xml_content.empty()) {
      std::cout << "xml file is empty" << std::endl;
      return;
    }
    virTool virTool_(true);
    if (!virTool_.openConnect(virUri)) {
      PrintLastError();
      std::cout << "open connect failed" << std::endl;
      return;
    }
    auto domain = virTool_.createDomain(xml_content.c_str());
    if (!domain) {
      PrintLastError();
      std::cout << "create domain failed" << std::endl;
      return;
    }
    int32_t try_count = 0;
    while (try_count ++ < 100) {
      std::cout << "get vm local ip try_count: " << try_count << std::endl;
      if (domain->getDomainInterfaceAddress() < 0)
        PrintLastError();
      else
        break;
      sleep(3);
    }
    try_count = 0;
    while (try_count++ < 100) {
      std::cout << "set vm user password try_count: " << try_count << std::endl;
      if (domain->setDomainUserPassword("dbc", "vm123456") == 0)
        break;
      sleep(3);
    }
    std::cout << "create domain success" << std::endl;
    domain.reset();
  }

  void parseStatsDomain(const char* domainName) {
    virTool virTool_;
    if (!virTool_.openConnect(virUri)) {
      PrintLastError();
      std::cout << "open connect failed" << std::endl;
      return;
    }
    auto domain = virTool_.openDomainByName(domainName);
    if (domain) {
      int ret = domain->getDomainStatsList(VIR_DOMAIN_STATS_STATE | VIR_DOMAIN_STATS_CPU_TOTAL | VIR_DOMAIN_STATS_BALLOON |
        VIR_DOMAIN_STATS_VCPU | VIR_DOMAIN_STATS_INTERFACE | VIR_DOMAIN_STATS_BLOCK | VIR_DOMAIN_STATS_PERF);
      std::cout << "stats domain " << (ret < 0 ? "failed" : "ok") << std::endl;
    } else {
      std::cout << "can not find domain: " << domainName << std::endl;
    }
    domain.reset();
  }

  void parseFSInfoDomain(const char* domainName) {
    virTool virTool_;
    if (!virTool_.openConnect(virUri)) {
      PrintLastError();
      std::cout << "open connect failed" << std::endl;
      return;
    }
    auto domain = virTool_.openDomainByName(domainName);
    if (domain) {
      int ret = domain->getDomainFSInfo();
      std::cout << "fsinfo domain " << (ret < 0 ? "failed" : "ok") << std::endl;
    } else {
      std::cout << "can not find domain: " << domainName << std::endl;
    }
    domain.reset();
  }

  void parseBlklistDomain(const char* domainName) {
    virTool virTool_;
    if (!virTool_.openConnect(virUri)) {
      PrintLastError();
      std::cout << "open connect failed" << std::endl;
      return;
    }
    auto domain = virTool_.openDomainByName(domainName);
    if (domain) {
      // int ret = domain->getDomainBlockInfo("hda");
      // int ret = domain->getDomainBlockParameters();
      // int ret = domain->getDomainBlockIoTune("hda");
      // int ret = domain->getDomainStatsList();
      std::vector<domainDiskInfo> disks;
      if (domain->getDomainDisks(disks) < 0) {
        PrintLastError();
        std::cout << "list domain block failed" << std::endl;
      } else {
        std::cout << " ";
        std::cout << std::setw(8) << std::setfill(' ') << std::left << "Name";
        std::cout << std::setw(12) << std::setfill(' ') << std::left << "DriverName";
        std::cout << std::setw(12) << std::setfill(' ') << std::left << "DriverType";
        std::cout << std::setw(50) << std::setfill(' ') << std::left << "description";
        std::cout << std::endl;
        std::cout << std::setw(1 + 8 + 12 + 12 + 50) << std::setfill('-') << std::left << "" << std::endl;
        for (const auto& disk : disks) {
          std::cout << disk << std::endl;
        }
      }
    } else {
      std::cout << "can not find domain: " << domainName << std::endl;
    }
    domain.reset();
  }

  void parseSuspendDomain(const char* domainName) {
    virTool virTool_;
    if (!virTool_.openConnect(virUri)) {
      PrintLastError();
      std::cout << "open connect failed" << std::endl;
      return;
    }
    auto domain = virTool_.openDomainByName(domainName);
    if (domain) {
      int ret = domain->suspendDomain();
      std::cout << "suspend domain " << (ret < 0 ? "failed" : "ok") << std::endl;
    } else {
      std::cout << "can not find domain: " << domainName << std::endl;
    }
    domain.reset();
  }

  void parseResumeDomain(const char* domainName) {
    virTool virTool_;
    if (!virTool_.openConnect(virUri)) {
      PrintLastError();
      std::cout << "open connect failed" << std::endl;
      return;
    }
    auto domain = virTool_.openDomainByName(domainName);
    if (domain) {
      int ret = domain->resumeDomain();
      std::cout << "resume domain " << (ret < 0 ? "failed" : "ok") << std::endl;
    } else {
      std::cout << "can not find domain: " << domainName << std::endl;
    }
    domain.reset();
  }

  void parseRebootDomain(const char* domainName) {
    virTool virTool_;
    if (!virTool_.openConnect(virUri)) {
      PrintLastError();
      std::cout << "open connect failed" << std::endl;
      return;
    }
    auto domain = virTool_.openDomainByName(domainName);
    if (domain) {
      int ret = domain->rebootDomain(VIR_DOMAIN_REBOOT_ACPI_POWER_BTN | VIR_DOMAIN_REBOOT_GUEST_AGENT);
      std::cout << "reboot domain " << (ret < 0 ? "failed" : "ok") << std::endl;
    } else {
      std::cout << "can not find domain: " << domainName << std::endl;
    }
    domain.reset();
  }

  void parseIflistDomain(const char* domainName) {
    virTool virTool_;
    if (!virTool_.openConnect(virUri)) {
      PrintLastError();
      std::cout << "open connect failed" << std::endl;
      return;
    }
    auto domain = virTool_.openDomainByName(domainName);
    if (domain) {
      int ret = domain->getDomainInterfaceAddress(VIR_DOMAIN_INTERFACE_ADDRESSES_SRC_AGENT);
      std::cout << "list domain ifconfig " << (ret < 0 ? "failed" : "ok") << std::endl;
    } else {
      std::cout << "can not find domain: " << domainName << std::endl;
    }
    domain.reset();
  }

  void parseDomainSnapshotList(const char* domainName) {
    virTool virTool_;
    if (!virTool_.openConnect(virUri)) {
      PrintLastError();
      std::cout << "open connect failed" << std::endl;
      return;
    }
    auto domain = virTool_.openDomainByName(domainName);
    if (domain) {
      std::vector<std::shared_ptr<virDomainSnapshotImpl>> snaps;
      if (domain->listAllSnapshots(snaps, 1 << 10) < 0) {
        PrintLastError();
        std::cout << "list all snapshots failed" << std::endl;
      } else {
        std::cout << " ";
        std::cout << std::setw(8) << std::setfill(' ') << std::left << "Name";
        std::cout << std::setw(28) << std::setfill(' ') << std::left << "Creation Time";
        std::cout << std::setw(16) << std::setfill(' ') << std::left << "State";
        std::cout << std::setw(50) << std::setfill(' ') << std::left << "description";
        std::cout << std::endl;
        std::cout << std::setw(1 + 8 + 28 + 16 + 50) << std::setfill('-') << std::left << "" << std::endl;
        std::vector<domainSnapshotInfo> dsInfos;
        for (const auto& snap : snaps) {
          domainSnapshotInfo dsInfo;
          if (snap->getSnapshotInfo(dsInfo) == 0 && !dsInfo.name.empty()) {
            std::cout << dsInfo << std::endl;
            dsInfos.push_back(dsInfo);
          } else {
            std::cout << "get domain snapshot info failed" << std::endl;
          }
        }
        for (const auto& dsInfo : dsInfos) {
          std::cout << "get snap [" << dsInfo.name << "] disk details here:" << std::endl;
          for (int i = 0; i < dsInfo.disks.size(); i++) {
            std::cout << "disk name=" << dsInfo.disks[i].name << ", snapshot=" << dsInfo.disks[i].snapshot
                      << ", driver_type=" << dsInfo.disks[i].driver_type << ", source_file=" << dsInfo.disks[i].source_file;
            std::cout << std::endl;
          }
        }
      }
    }
    domain.reset();
  }
}
