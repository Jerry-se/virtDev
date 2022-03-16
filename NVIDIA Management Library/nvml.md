# 简介

# nvidia-smi
nvidia-sim简称NVSMI，提供监控GPU使用情况和更改GPU状态的功能，是一个跨平台工具，支持所有标准的NVIDIA驱动程序支持的Linux和WindowsServer 2008 R2 开始的64位系统。这个工具是N卡驱动附带的，只要装好驱动，就会有这个命令。

[GPU之nvidia-smi命令详解](https://www.jianshu.com/p/ceb3c020e06b)

# 安装
The NVIDIA Management Library can be downloaded as part of the [GPU Deployment Kit](https://developer.nvidia.com/tesla-deployment-kit).

CUDA Toolkit 11.6 Update 1 Downloads

### ubuntu 18.04
```
wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu1804/x86_64/cuda-ubuntu1804.pin
sudo mv cuda-ubuntu1804.pin /etc/apt/preferences.d/cuda-repository-pin-600
wget https://developer.download.nvidia.com/compute/cuda/11.6.1/local_installers/cuda-repo-ubuntu1804-11-6-local_11.6.1-510.47.03-1_amd64.deb
sudo dpkg -i cuda-repo-ubuntu1804-11-6-local_11.6.1-510.47.03-1_amd64.deb
sudo apt-key add /var/cuda-repo-ubuntu1804-11-6-local/7fa2af80.pub
sudo apt-get update
sudo apt-get -y install cuda
```
### windows 10
https://developer.download.nvidia.cn/compute/cuda/11.6.1/local_installers/cuda_11.6.1_511.65_windows.exe

# 使用NVML做开发
cuda自带的nvml示例代码位于 `/usr/local/cuda-11.6/nvml/example/` 文件夹下，链接库需要`/usr/lib/x86_64-linux-gnu/libnvidia-ml.so`，此动态库在`/usr/local/cuda/targets/x86_64-linux/lib/stubs/`文件夹下也有一份，示例可以使用`make`命令编译。

- [nvapi](https://developer.nvidia.com/nvapi)
- [用NVAPI获取GPU信息](https://blog.csdn.net/fduffyyg/article/details/86505051)
- [NVML查询显卡信息](https://www.cnblogs.com/betterwgo/p/6858806.html)
- [通过NVML获取GPU信息](https://blog.csdn.net/caowei880123/article/details/103475232)

# 参考资料
- [NVIDIA System Management Interface](https://developer.nvidia.com/nvidia-system-management-interface)
- [NVIDIA Management Library](https://developer.nvidia.com/nvidia-management-library-nvml)
- [NVML API Reference Guide](https://docs.nvidia.com/deploy/nvml-api/index.html)
