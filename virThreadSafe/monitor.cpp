#include <iostream>
#include <string>
#include <signal.h>

#include <libvirt/libvirt.h>
#include <libvirt/virterror.h>

const static char* virUri = "qemu+tcp://localhost/system";
const char* arrayState[] = {"no state", "running", "blocked", "paused", "shutdown", "shutoff", "crashed", "pmsuspended", "last"};

int quit = 0;

void signal_handler(int sig);
int domain_event_cb(virConnectPtr conn, virDomainPtr dom, int event, int detail, void *opaque);

int main(int argc, char* argv[]) {
  int ret = virEventRegisterDefaultImpl();
  if (ret < 0) {
    printf("virEventRegisterDefaultImpl failed\n");
    return -1;
  }

  virConnectPtr conn_ = virConnectOpen(virUri);
  if (!conn_) {
    printf("connect failed\n");
    return -1;
  }

  signal(SIGINT, signal_handler);

  int callback_id = virConnectDomainEventRegisterAny(conn_, NULL,
    virDomainEventID::VIR_DOMAIN_EVENT_ID_LIFECYCLE, VIR_DOMAIN_EVENT_CALLBACK(domain_event_cb), NULL, NULL);

  printf("event loop begin\n");
  while (quit == 0) {
    if (virEventRunDefaultImpl() < 0) {
      virErrorPtr err = virGetLastError();
      printf("virEventRunDefaultImpl failed: %s\n", err->message);
    }
  }

  virConnectDomainEventDeregisterAny(conn_, callback_id);
  virConnectClose(conn_);
  printf("standard quit\n");
  return 0;
}

void signal_handler(int sig) {
  printf("catch ctrl+c\n");
  quit = 1;
}

int domain_event_cb(virConnectPtr conn, virDomainPtr dom, int event, int detail, void *opaque) {
  printf("event lifecycle cb called, event=%d, detail=%d\n", event, detail);
  const char* name = virDomainGetName(dom);
  switch (event)
  {
  case virDomainEventType::VIR_DOMAIN_EVENT_DEFINED:
    printf("domain %s defined\n", name);
    break;
  case virDomainEventType::VIR_DOMAIN_EVENT_UNDEFINED:
    printf("domain %s undefined\n", name);
    break;
  case virDomainEventType::VIR_DOMAIN_EVENT_STARTED:
    printf("domain %s started\n", name);
    break;
  case virDomainEventType::VIR_DOMAIN_EVENT_SUSPENDED:
    printf("domain %s suspended\n", name);
    break;
  case virDomainEventType::VIR_DOMAIN_EVENT_RESUMED:
    printf("domain %s resumed\n", name);
    break;
  case virDomainEventType::VIR_DOMAIN_EVENT_STOPPED:
    printf("domain %s stoped\n", name);
    break;
  case virDomainEventType::VIR_DOMAIN_EVENT_SHUTDOWN:
    printf("domain %s shutdown\n", name);
    break;
  case virDomainEventType::VIR_DOMAIN_EVENT_PMSUSPENDED:
    printf("domain %s pmsuspended\n", name);
    break;
  case virDomainEventType::VIR_DOMAIN_EVENT_CRASHED:
    printf("domain %s crashed\n", name);
    break;
  default:
    printf("unknowned event\n");
    break;
  }
}
