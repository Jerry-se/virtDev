#!/usr/bin/python3

import sys
import libvirt

connName = 'qemu+tcp://localhost/system'
stateList = ['NoState','Running','Blocked','Paused','Shutdown','Shutoff','Crashed','Pmsuspended']

def printDomInfo(dom):
  # print('function not avalible')
  print("dom %s state %s"%(dom.name(), stateList[dom.info()[0]]))

def list():
  conn = libvirt.open(connName)
  if conn == None:
    print('Failed to open connection to qemu', file=sys.stderr)
    return
  for id in conn.listDomainsID():
    dom = conn.lookupByID(id)
    printDomInfo(dom)
  conn.close()

def test(domid):
  conn = libvirt.open(connName)
  if conn == None:
    print('Failed to open connection to qemu', file=sys.stderr)
    return
  # print(conn.listDomainsID())
  dom = conn.lookupByName(domid)
  if dom == None:
    print('Failed to look up domid', file=sys.stderr)
    return
  printDomInfo(dom)
  conn.close()

def resume(domid):
  conn = libvirt.open(connName)
  if conn == None:
    print('Failed to open connection to qemu', file=sys.stderr)
    return
  # print(conn.listDomainsID())
  dom = conn.lookupByName(domid)
  if dom == None:
    print('Failed to look up domid', file=sys.stderr)
    return
  dom.resume()
  printDomInfo(dom)
  conn.close()

def suspend(domid):
  conn = libvirt.open(connName)
  if conn == None:
    print('Failed to open connection to qemu', file=sys.stderr)
    return
  dom = conn.lookupByName(domid)
  if dom == None:
    print('Failed to look up domid', file=sys.stderr)
    return
  dom.suspend()
  printDomInfo(dom)
  conn.close()

def process(op):
  domid = None
  if len(sys.argv) == 3:
    domid = sys.argv[2]
  print('operation: ', op)
  print('domain id: ', domid)
  if sys.argv[1] == 'list':
    list()
  elif sys.argv[1] == 'test':
    test(domid)
  elif sys.argv[1] == 'suspend':
    suspend(domid)
  elif sys.argv[1] == 'resume':
    resume(domid)

def main():
  if len(sys.argv) > 1:
    process(sys.argv[1])
  else:
    print('format test.py operation [domain id]')

print('test begin')
main()
print ('test end')
