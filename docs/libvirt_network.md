# libvirt默认的nat网络设置
```
virsh net-list
virsh net-dumpxml default
virsh net-edit default
virsh net-destroy default
virsh net-start default
```
```
# cat /etc/libvirt/qemu/networks/default.xml
<!--
WARNING: THIS IS AN AUTO-GENERATED FILE. CHANGES TO IT ARE LIKELY TO BE
OVERWRITTEN AND LOST. Changes to this xml configuration should be made using:
  virsh net-edit default
or other application using the libvirt API.
-->

<network>
  <name>default</name>
  <uuid>e8420337-a4d5-4e49-97a7-f750bb0f1d69</uuid>
  <forward mode='nat'/>
  <bridge name='virbr0' stp='on' delay='0'/>
  <mac address='52:54:00:f7:c3:b1'/>
  <ip address='192.168.122.1' netmask='255.255.255.0'>
    <dhcp>
      <range start='192.168.122.2' end='192.168.122.254'/>
    </dhcp>
  </ip>
</network>
```

# 虚拟机的dhcp ip 租用信息
```
# cat /var/lib/libvirt/dnsmasq/virbr0.macs
# cat /var/lib/libvirt/dnsmasq/virbr0.status
# cat /var/lib/libvirt/dnsmasq/default.conf
##WARNING:  THIS IS AN AUTO-GENERATED FILE. CHANGES TO IT ARE LIKELY TO BE
##OVERWRITTEN AND LOST.  Changes to this configuration should be made using:
##    virsh net-edit default
## or other application using the libvirt API.
##
## dnsmasq conf file created by libvirt
strict-order
user=libvirt-dnsmasq
pid-file=/run/libvirt/network/default.pid
except-interface=lo
bind-dynamic
interface=virbr0
dhcp-range=192.168.122.2,192.168.122.254,255.255.255.0
dhcp-no-override
dhcp-authoritative
dhcp-lease-max=253
dhcp-hostsfile=/var/lib/libvirt/dnsmasq/default.hostsfile
addn-hosts=/var/lib/libvirt/dnsmasq/default.addnhosts
```

# 手动设置静态ip地址
需要填入网卡的MAC地址，虚拟机的name以及需要设置的ip地址
```
virsh net-update default add ip-dhcp-host "<host mac='52:54:00:fb:55:cf' name='ubuntu18.04' ip='192.168.122.45' />" --live --config
```
然后查看default的定义
```
# cat /etc/libvirt/qemu/networks/default.xml
<!--
WARNING: THIS IS AN AUTO-GENERATED FILE. CHANGES TO IT ARE LIKELY TO BE
OVERWRITTEN AND LOST. Changes to this xml configuration should be made using:
  virsh net-edit default
or other application using the libvirt API.
-->

<network>
  <name>default</name>
  <uuid>fc76677d-da0a-47cb-b136-794a06229092</uuid>
  <forward mode='nat'/>
  <bridge name='virbr0' stp='on' delay='0'/>
  <mac address='52:54:00:85:00:58'/>
  <ip address='192.168.122.1' netmask='255.255.255.0'>
    <dhcp>
      <range start='192.168.122.2' end='192.168.122.254'/>
      <host mac='52:54:00:fb:55:cf' name='ubuntu18.04' ip='192.168.122.45'/>
    </dhcp>
  </ip>
</network>
```
参考官方资料[修改虚拟机的网络](https://wiki.libvirt.org/page/Networking#Applying_modifications_to_the_network)

# 获取kvm虚拟机的IP地址
```
virsh domifaddr domain_test
virsh net-list --all
virsh domiflist domain_test
Interface  Type       Source     Model       MAC
-------------------------------------------------------
vnet0      network    default    virtio      52:54:00:60:9b:be

virsh net-dhcp-leases default --mac 52:54:00:60:9b:be
```
同一个`ubuntu`镜像创建的2个虚拟机的IP相同，有三种解决方案:
1. 手动设置静态ip，不推荐。
2. 修改machine id，清空`/etc/machine-id`文件内容，注意不要删除文件，然后链接到`/var/lib/dbus/machine-id`。
```
rm /var/lib/dbus/machine-id
ln -s /etc/machine-id /var/lib/dbus/machine-id
```
3. 修改dhcp识别标志为mac。
- * [克隆的 VM 获取相同的 DHCP IP 地址](https://kb.vmware.com/s/article/82229)
- * [netplan does not allow dhcp client identifier type to be specified](https://bugs.launchpad.net/netplan/+bug/1738998)

### 参考资料
* [How To Find The IP Address Of A KVM Virtual Machine](https://ostechnix.com/how-to-find-the-ip-address-of-a-kvm-virtual-machine/)
* [How to find the IP address of a KVM Virtual Machine, that I can SSH into it?](https://unix.stackexchange.com/questions/33191/how-to-find-the-ip-address-of-a-kvm-virtual-machine-that-i-can-ssh-into-it)

