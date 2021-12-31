### 描述
virsh程序是用于管理的virsh来宾域的主界面。该程序可用于创建、暂停和关闭域。它还可用于列出当前域。Libvirt 是一个 C 工具包，用于与最新版本的 Linux（和其他操作系统）的虚拟化功能进行交互。它是根据 GNU 宽松通用公共许可证提供的免费软件。Linux 操作系统的虚拟化意味着能够在单个硬件系统上同时运行多个操作系统实例，其中基本资源由一个 Linux 实例驱动。该库旨在提供长期稳定的 C API。目前支持 Xen、QEMU、KVM、LXC、OpenVZ、VirtualBox 和 VMware ESX。

### 概要
virsh [OPTION]... [COMMAND_STRING]

virsh [OPTION]... COMMAND DOMAIN [ARG]...

### 域的相关命令
```
# 列出所有域
virsh list --all

# 定义域(非临时域)
virsh define test.xml

# 取消域定义，会从域列表中移除
virsh undefine $DOMAIN

# 启动域
virsh start $DOMAIN

# 停止域(异步命令，会立即返回)
virsh shutdown $DOMAIN

# 停止域(同步命令，会等待域停止后才返回)
virsh destroy $DOMAIN

# 查看域的定义
virsh dumpxml domain_test

# 编辑域定义
virsh edit domain_test

# 查看域的网络ip地址
virsh domifaddr $DOMAIN

# 查看域的网络接口列表
virsh domiflist $DOMAIN

# 列出域的磁盘文件
virsh domblklist domain_test

# 列出域信息
virsh dominfo domain_test
```

### 网络的相关命令
参考[libvirt_network.md](./libvirt_network.md)

### network filter网络过滤相关命令
libvirt的`virsh nwfilter-*`系列命令提供了设置虚拟机防火墙的功能，它其实是封装了`iptables`过滤`Bridge`中数据包的命令(`-m physdev`)。它使用多个xml文件，每个xml文件中都可以定义一系列防火墙规则，然后把某个xml文件应用到某虚拟机的某张网卡(Bridge中的接口)，这样就完成了对此虚拟机的这张网卡的防火墙设置。当然可以把一个定义好防火墙规则的xml文件应用到多台虚拟机。
```
#查看用于设置
# virsh --help |grep nwfilter
    nwfilter-define                define or update a network filter from an XML file
    nwfilter-dumpxml               network filter information in XML
    nwfilter-edit                  edit XML configuration for a network filter
    nwfilter-list                  list network filters
    nwfilter-undefine              undefine a network filter

# 定义有防火墙规则的xml文件
# virsh nwfilter-dumpxml centos6.3_filter
<filter name='centos6.3_filter' chain='root'>
  <uuid>b1fdd87c-a44c-48fb-9a9d-e30f1466b720</uuid>
  <rule action='accept' direction='in' priority='400'>
    <tcp dstportstart='8000' dstportend='8002'/>
  </rule>
  <rule action='accept' direction='in' priority='400'>
    <tcp srcipaddr='172.16.1.0' srcipmask='24'/>
  </rule>
</filter>

#查看定义的防火墙xml文件
# virsh nwfilter-list
 UUID                                  Name
------------------------------------------------------------------
 69754f43-0325-453f-bd53-4a6e3ab5d456  centos6.3_filter

#在虚拟机xml文件中应用centos6.3_filter
    <interface type='bridge'>
      <mac address='f8:c9:79:ce:60:01'/>
      <source bridge='br0'/>
      <model type='virtio'/>
      <filterref filter='centos6.3_filter'/>
      <address type='pci' domain='0x0000' bus='0x00' slot='0x03' function='0x0'/>
    </interface>

```
