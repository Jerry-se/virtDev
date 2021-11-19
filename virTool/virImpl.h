#ifndef _VIRDEV_VIR_TOOL_H_
#define _VIRDEV_VIR_TOOL_H_

#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

#include <thread>

namespace virTool {

class virToolDelegate {
public:
  virtual void PrintErrorMessage(virError *err) = 0;
};

class virTool {
public:
  virTool(virToolDelegate *delegate = nullptr);
  ~virTool();

  // host
  /**
   * @brief Provides version information. libVer is the version of the library and will always be set unless an error occurs,
   * in which case an error code will be returned. typeVer exists for historical compatibility;
   * if it is not NULL it will duplicate libVer (it was originally intended to return hypervisor information based on type,
   * but due to the design of remote clients this is not reliable).
   * To get the version of the running hypervisor use the virConnectGetVersion() function instead.
   * To get the libvirt library version used by a connection use the virConnectGetLibVersion() instead.
   * This function includes a call to virInitialize() when necessary.
   * @param libVer   return value for the library version (OUT)
   * @param type     ignored; pass NULL
   * @param typeVer  pass NULL; for historical purposes duplicates libVer if non-NULL
   *
   * @return -1 in case of failure, 0 otherwise, and values for libVer and typeVer have the format major * 1,000,000 + minor * 1,000 + release.
   *     -<em>-1</em> fail
   *     -<em>0</em> succeed
   */
  static int GetVersion(unsigned long *libVer, const char *type, unsigned long *typeVer);

  bool ConnectOpen(const char *name);
  bool ConnectOpenReadOnly(const char *name);

  /**
   * @brief This function closes the connection to the Hypervisor. This should not be called if further interaction with the Hypervisor are needed
   * especially if there is running domain which need further monitoring by the application.
   * @param conn     pointer to the hypervisor connection
   *
   * @return a positive number if at least 1 reference remains on success. The returned value should not be assumed to be the total reference count.
   * A return of 0 implies no references remain and the connection is closed and memory has been freed. A return of -1 implies a failure.
   * It is possible for the last virConnectClose to return a positive value if some other object still has a temporary reference to the connection,
   * but the application should not try to further use a connection after the virConnectClose that matches the initial open.
   *     -<em>-1</em> fail
   *     -<em>0</em> succeed
   *     -<em>1</em> still has a temporary reference to the connection
   */
  int ConnectClose();
  /**
   * @brief Get the version level of the Hypervisor running. This may work only with hypervisor call, i.e. with privileged access to the hypervisor,
   * not with a Read-Only connection.
   * @param conn     pointer to the hypervisor connection
   * @param hvVer    return value for the version of the running hypervisor (OUT)
   *
   * @return -1 in case of error, 0 otherwise. if the version can't be extracted by lack of capacities returns 0 and hvVer is 0,
   * otherwise hvVer value is major * 1,000,000 + minor * 1,000 + release
   *     -<em>-1</em> fail
   *     -<em>0</em> succeed
   */
  int ConnectGetVersion(unsigned long *hvVer);
  /**
   * @brief Provides libVer, which is the version of libvirt used by the daemon running on the conn host
   * @param conn     pointer to the hypervisor connection
   * @param libVer   returns the libvirt library version used on the connection (OUT)
   *
   * @return -1 in case of failure, 0 otherwise, and values for libVer have the format major * 1,000,000 + minor * 1,000 + release.
   *     -<em>-1</em> fail
   *     -<em>0</em> succeed
   */
  int ConnectGetLibVersion(unsigned long *libVer);

  void PrintLastError();

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

  virDomainPtr DomainDefineXML(const char *xml);
  int DomainCreate(const char *xml);

  int DomainInterfaceAddress();

  int DomainSetUserPassword(const char *user, const char *password);
  /**
   * @brief Suspends an active domain, the process is frozen without further access to CPU resources and I/O but the memory used by the domain
   * at the hypervisor level will stay allocated. Use virDomainResume() to reactivate the domain. This function may require privileged access.
   * Moreover, suspend may not be supported if domain is in some special state like VIR_DOMAIN_PMSUSPENDED.
   * @param domain   a domain object
   *
   * @return 0 in case of success and -1 in case of failure.
   *     -<em>-1</em> fail
   *     -<em>0</em> succeed
   */
  int DomainSuspend();
  /**
   * @brief Resume a suspended domain, the process is restarted from the state where it was frozen by calling virDomainSuspend().
   * This function may require privileged access Moreover, resume may not be supported if domain is in some special state like VIR_DOMAIN_PMSUSPENDED.
   * @param domain   a domain object
   *
   * @return 0 in case of success and -1 in case of failure.
   *     -<em>-1</em> fail
   *     -<em>0</em> succeed
   */
  int DomainResume();

public:
  virConnectPtr GetConnectPtr() { return connectPtr_; }
  virDomainPtr GetDomainPtr() { return domainPtr_; }

protected:
  void DefaultThreadFunc();

private:
  virConnectPtr connectPtr_;
  virDomainPtr domainPtr_;
  virToolDelegate *delegate_;
  int callback_id_;
  int thread_quit_;
  std::thread *thread_event_loop_;
};
} // namespace virTool

#endif //_VIRDEV_VIR_TOOL_H_
