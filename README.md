# virtDev
libvirt development

## virThreadSafe
测试libvirt是否线程安全

[官方说明](https://wiki.libvirt.org/page/FAQ#Is_libvirt_thread_safe.3F)

Is libvirt thread safe?

Yes, libvirt is thread safe as of version 0.6.0. This means that multiple threads can act on a single virConnect instance without issue.

Previous libvirt versions required opening a separate connection for each thread: this method has several major drawbacks and is not recommended.

Google翻译为

libvirt 从 0.6.0 版开始是线程安全的。 这意味着多个线程可以毫无问题地作用于单个 virConnect 实例。

以前的 libvirt 版本需要为每个线程打开一个单独的连接：这种方法有几个主要缺点，不推荐使用。

## virTool
封装libvirt类

# 参考资料
1. [Reference Manual for libvirt](https://libvirt.org/html/index.html)
2. [libvirt Wiki](https://wiki.libvirt.org/page/Main_Page)
