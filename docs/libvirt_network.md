### 获取kvm虚拟机的IP地址
同一个镜像创建的2个虚拟机的IP相同，有三种解决方案，一是手动设置静态ip，二是修改machine id，三是修改dhcp识别标志为mac。
* [克隆的 VM 获取相同的 DHCP IP 地址](https://kb.vmware.com/s/article/82229)
* 设置静态IP或者[修改虚拟机的网络](https://wiki.libvirt.org/page/Networking#Applying_modifications_to_the_network)
* [netplan does not allow dhcp client identifier type to be specified](https://bugs.launchpad.net/netplan/+bug/1738998)

### 参考资料
* [How To Find The IP Address Of A KVM Virtual Machine](https://ostechnix.com/how-to-find-the-ip-address-of-a-kvm-virtual-machine/)
* [How to find the IP address of a KVM Virtual Machine, that I can SSH into it?](https://unix.stackexchange.com/questions/33191/how-to-find-the-ip-address-of-a-kvm-virtual-machine-that-i-can-ssh-into-it)

