#include "virtDev.h"

#include <iostream>
#include <fstream>
#include <string>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>

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

  class virToolDelegateImpl : public virToolDelegate {
  public:
    virToolDelegateImpl() {}
    ~virToolDelegateImpl() {}
    void PrintErrorMessage(virError *err) override {
      std::cout << "vir error occured: " << err->message << std::endl;
    }
  };

  void parseVersion() {
    virTool virTool_;
    virTool_.ConnectOpen(virUri);
    unsigned long libVer = 0;
    if (virTool::GetVersion(&libVer, NULL, NULL) == 0)
      std::cout << "virGetVersion: " << translateVirVer(libVer) << " " << libVer << std::endl;
    if (virTool_.ConnectGetVersion(&libVer) == 0)
      std::cout << "virConnectGetVersion: " << translateVirVer(libVer) << " " << libVer << std::endl;
    if (virTool_.ConnectGetLibVersion(&libVer) == 0)
      std::cout << "virConnectGetLibVersion: " << translateVirVer(libVer) << " " << libVer << std::endl;
  }

  void parseList() {
    virTool virTool_;
    virTool_.ConnectOpen(virUri);
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
    int ret = virTool_.ConnectListAllDomains(&domains, flags);
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
      }
      else {
        std::cout << "open xml file failed" << std::endl;
        return;
      }
    }
    if (xml_content.empty()) {
      std::cout << "xml file is empty" << std::endl;
      return;
    }
    virTool virTool_;
    virTool_.ConnectOpen(virUri);
    if (virTool_.DomainCreate(xml_content.c_str()) < 0) {
      std::cout << "create domain failed" << std::endl;
      return;
    }
    int32_t try_count = 0;
    while (try_count ++ < 100) {
      virTool_.DomainInterfaceAddress();
      sleep(3);
    }
    try_count = 0;
    while (try_count++ < 10) {
      if (virTool_.DomainSetUserPassword("dbc", "vm123456") == 0)
        break;
    }
  }

  void parseSuspendDomain(const char* domainName) {
    virTool virTool_;
    virTool_.ConnectOpen(virUri);
    if (virTool_.DomainLookupByName(domainName)) {
      int ret = virTool_.DomainSuspend();
      std::cout << "suspend domain " << (ret == 0 ? "ok" : "failed") << std::endl;
    }
    else
      std::cout << "can not find domain: " << domainName << std::endl;
  }

  void parseResumeDomain(const char* domainName) {
    virTool virTool_;
    virTool_.ConnectOpen(virUri);
    if (virTool_.DomainLookupByName(domainName)) {
      int ret = virTool_.DomainResume();
      std::cout << "resume domain " << (ret == 0 ? "ok" : "failed") << std::endl;
    }
    else
      std::cout << "can not find domain: " << domainName << std::endl;
  }
}
