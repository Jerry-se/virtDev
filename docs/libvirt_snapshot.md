### 简介
快照是虚拟机操作系统及其在给定时间点的所有应用程序的视图。拥有虚拟机的可恢复快照是虚拟化环境的基本特征。快照允许用户及时保存虚拟机的状态并回滚到该状态。基本用例包括拍摄快照、安装新应用程序、更新或升级（发现它们很糟糕或坏了），然后回滚到之前的时间。

很明显，拍摄快照后发生的任何更改都不包含在快照中。快照不会持续更新。它表示虚拟机在单个时间点的状态。

### 创建快照
```
<domainsnapshot>
  <description>Snapshot of OS install and updates</description>
  <name>snap1</name>
  <disk name='hda' snapshot='external'>
    <driver type='qcow2'/>
  </disk>
  <disk name='vda' snapshot='no'/>
</domainsnapshot>
```
`virsh snapshot-create $DOMAIN /data/snapshot.xml --disk-only`

virsh snapshot-list 75YTRFrnxK8eSSqBetEkbY

//树形查看快照。
virsh snapshot-list 75YTRFrnxK8eSSqBetEkbY --tree

qemu-img info 75YTRFrnxK8eSSqBetEkbY.snap1

### 参考资料
* [libvirt snapshots](https://wiki.libvirt.org/page/Snapshots)
* [Snapshot XML format](https://libvirt.org/formatsnapshot.html)
* [libvirt 快照 （一）](https://blog.csdn.net/chenyulancn/article/details/12453199)
* [libvirt 快照 （二）](https://blog.csdn.net/chenyulancn/article/details/12453335)
* [libvirt/qemu特性之快照](https://blog.csdn.net/halcyonbaby/article/details/19709735)
* [KVM 介绍（7）](https://www.cnblogs.com/sammyliu/p/4468757.html)
