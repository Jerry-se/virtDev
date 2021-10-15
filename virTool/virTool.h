#ifndef _VIRDEV_VIR_TOOL_H_
#define _VIRDEV_VIR_TOOL_H_

#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

namespace virtDev {

class virTool {
public:
  virTool();
  ~virTool();

  // host
  static int GetVersion(unsigned long *libVer, const char *type, unsigned long *typeVer);

  bool ConnectOpen(const char *name);
  bool ConnectOpenReadOnly(const char *name);

  int ConnectClose();
  int ConnectGetVersion(unsigned long *hvVer);
  int ConnectGetLibVersion(unsigned long *libVer);

  // domain
  // returns the number of domains found or -1 and sets domains to NULL in case of error.
  int ConnectListAllDomains(virDomainPtr **domains, unsigned int flags);
  // returns -1 in case of error
  int ConnectListDomains(int *ids, int maxids);
  int ConnectNumOfDomains();

  int DomainFree();

  // returns 0 in case of success and -1 in case of failure.
  int DomainGetInfo(virDomainInfoPtr info);
  // returns 0 in case of success and -1 in case of failure.
  int DomainGetState(int *state, int *reason, unsigned int flags);

  bool DomainLookupByID(int id);
  bool DomainLookupByName(const char *name);
  bool DomainLookupByUUID(const unsigned char *uuid);
  bool DomainLookupByUUIDString(const char *uuid);

  // Suspends an active domain, the process is frozen without further access to CPU resources and I/O but the memory used by the domain at the hypervisor
  // level will stay allocated. Use virDomainResume() to reactivate the domain. This function may require privileged access. Moreover, suspend may not be
  // supported if domain is in some special state like VIR_DOMAIN_PMSUSPENDED.
  // returns 0 in case of success and -1 in case of failure.
  int DomainSuspend();
  // Resume a suspended domain, the process is restarted from the state where it was frozen by calling virDomainSuspend(). This function may require
  // privileged access Moreover, resume may not be supported if domain is in some special state like VIR_DOMAIN_PMSUSPENDED.
  // Returns 0 in case of success and -1 in case of failure.
  int DomainResume();

public:
  virConnectPtr GetConnectPtr() { return connectPtr_; }
  virDomainPtr GetDomainPtr() { return domainPtr_; }

private:
  virConnectPtr connectPtr_;
  virDomainPtr domainPtr_;
};
} // namespace virtDev

#endif //_VIRDEV_VIR_TOOL_H_
