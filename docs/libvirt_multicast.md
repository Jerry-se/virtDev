### multicast
设置多播组以表示虚拟网络。网络设备在同一个多播组中的任何虚拟机甚至可以跨主机相互通信。此模式也适用于非特权用户。没有默认的 DNS 或 DHCP 支持，也没有传出网络访问。要提供传出网络访问，其中一个 VM 应具有第二个 NIC，该 NIC 连接到前 4 种网络类型之一并执行适当的路由。多播协议也与用户模式 linux 来宾使用的协议兼容。使用的源地址必须来自多播地址块。
```
<devices>
  <interface type='mcast'>
    <mac address='52:54:00:6d:90:01'/>
    <source address='230.0.0.1' port='5558'/>
  </interface>
</devices>
```
在Windows虚拟机中此网卡会自动生成一个169.254.x.x网段的[本地链接地址](https://en.wikipedia.org/wiki/Link-local_address)。

### Zero-configuration networking
必须为网络上的主机分配IP 地址，以便将它们唯一地标识给同一网络上的其他设备。在某些网络上，有一个中央机构在添加新设备时分配这些地址。引入了自动处理此任务的机制，IPv4 和 IPv6 现在都包括地址自动配置系统，它允许设备通过简单的机制确定要使用的安全地址。对于链路本地寻址，IPv4 使用RFC 3927 中描述的特殊块169.254.0.0/16而 IPv6 主机使用前缀fe80::/10。更常见的地址是由 DHCP 服务器，通常内置于计算机主机或路由器等常见网络硬件中。

大多数 IPv4 主机仅在 DHCP 服务器不可用时才使用链路本地寻址作为最后的手段。否则，IPv4 主机将使用其 DHCP 分配的地址进行所有通信，无论是全局通信还是本地链路通信。原因之一是 IPv4 主机不需要支持每个接口的多个地址，尽管很多都支持。另一个是不是每个 IPv4 主机都实现分布式名称解析（例如，多播 DNS），因此发现网络上另一台主机的自动配置链接本地地址可能很困难。发现另一台主机的 DHCP 分配地址需要分布式名称解析或具有此信息的单播 DNS 服务器；某些网络具有 DNS 服务器，这些服务器会使用 DHCP 分配的主机和地址信息自动更新。

IPv6 主机需要支持每个接口的多个地址；此外，即使全局地址可用，每个 IPv6 主机都需要配置链路本地地址。IPv6 主机可以在收到路由器通告消息时额外地自行配置额外的地址，从而消除对 DHCP 服务器的需要。

IPv4 和 IPv6 主机都可以随机生成自动配置地址的主机特定部分。IPv6 主机通常将最多 64 位的前缀与源自工厂分配的 48 位IEEE MAC 地址的 64 位 EUI-64 组合在一起。MAC 地址具有全球唯一的优势，这是 EUI-64 的基本属性。IPv6 协议栈还包括重复地址检测，以避免与其他主机发生冲突。在 IPv4 中，该方法称为链路本地地址自动配置。但是，Microsoft将此称为 自动私有 IP 寻址( APIPA ) 或Internet 协议自动配置(IPAC)。Windows 至少从Windows 98开始支持该功能。

http://www.zeroconf.org/

### ubuntu无法取到169.254的ip地址
```
# avahi
sudo apt install avahi-daemon avahi-autoipd
sudo avahi-autoipd -w ens4
sudo avahi-daemon -D
```
avahi使用需要网卡名称做为参数，此方法不合适。
```
# dhcpcd
sudo apt install dhcpcd5
sudo dhcpcd5 -L 或 sudo dhcpcd5 --noipv4ll
sudo dhcpcd5 --reboot 1
echo "nameserver 127.0.0.1" > /etc/resolv.conf.head
echo "nameserver 192.168.122.1" > /etc/resolv.conf.head
sudo systemctl restart dhcpcd.service 或 sudo /etc/init.d/dhcpcd restart
sudo dhcpcd -o domain_name_servers -T
```
使用dhcpcd后无法连接外网，发现是dns解析的问题
```
sudo apt install resolvconf
sudo systemctl start resolvconf
sudo vim /etc/resolvconf/resolv.conf.d/head
```
```
dbc@ubuntu-1804:~$ ip route
default via 192.168.122.1 dev ens3 proto dhcp metric 101
default dev ens10 scope link metric 1003
169.254.0.0/16 dev ens10 proto kernel scope link src 169.254.8.115
169.254.0.0/16 dev ens3 scope link metric 1000
192.168.122.0/24 dev ens3 proto kernel scope link src 192.168.122.229 metric 101
```

* [archlinux avahi](https://wiki.archlinux.org/title/Avahi)
* [archlinux dhcpcd](https://wiki.archlinux.org/title/Dhcpcd)
* [dhcpcd manpages](http://manpages.ubuntu.com/manpages/xenial/man8/dhcpcd5.8.html)
* [ubuntu dhcp scope](https://ubuntu.com/blog/dhcp-scope)
* [ZeroConfNetworking](https://wiki.ubuntu.com/ZeroConfNetworking)
### Network Manager
```
sudo apt install network-manager
sudo vim /etc/netplan/xxx.yaml
network:
  version: 2
  renderer: NetworkManager
sudo netplan apply
```

### 制作镜像
```
sudo apt install network-manager dhcpcd5 resolvconf
# sudo dhclient -r ens3
sudo vim /etc/netplan/xxx.yaml
# network:
#   version: 2
#   renderer: NetworkManager
sudo netplan apply
systemctl is-enabled network-manager.service
systemctl status network-manager.service
systemctl start network-manager.service
sudo ip link set ens3 down
sudo ip link set ens3 up
sudo dhcpcd --release ens3
sudo dhcpcd -L
sudo dhcpcd --noipv4ll
sudo rm -f /etc/machine-id
sudo touch /etc/machine-id
sudo mv /etc/dhcpcd.duid /etc/dhcpcd.duid.bak
```

### 参考资料
* [multicast-tunnel](https://libvirt.org/formatdomain.html#multicast-tunnel)
* [APIPA（Automatic Private IP Addressing，自动专用IP寻址）](https://www.cnblogs.com/sddai/p/6045577.html)
* [Zero-configuration networking](https://en.wikipedia.org/wiki/Zero-configuration_networking)
* [Avahi (software)](https://en.wikipedia.org/wiki/Avahi_(software))
