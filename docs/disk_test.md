# fio
`sudo apt install fio`

### 随机读IOPS
```
sudo fio -direct=1 -iodepth=32 -rw=randread -ioengine=libaio -bs=4k -size=4G -numjobs=4 -time_based=1 -runtime=90 -group_reporting -filename=/dev/sda -name=test
test: (g=0): rw=randread, bs=(R) 4096B-4096B, (W) 4096B-4096B, (T) 4096B-4096B, ioengine=libaio, iodepth=32
...
fio-3.1
Starting 4 processes
Jobs: 4 (f=4): [r(4)][100.0%][r=16.5MiB/s,w=0KiB/s][r=4235,w=0 IOPS][eta 00m:00s]
test: (groupid=0, jobs=4): err= 0: pid=29114: Mon Dec 27 06:27:20 2021
   read: IOPS=4297, BW=16.8MiB/s (17.6MB/s)(1511MiB/90036msec)
    slat (usec): min=2, max=6643, avg=37.00, stdev=75.11
    clat (usec): min=205, max=314919, avg=29725.40, stdev=68562.23
     lat (usec): min=430, max=315114, avg=29768.75, stdev=68560.69
    clat percentiles (usec):
     |  1.00th=[   652],  5.00th=[  1188], 10.00th=[  1876], 20.00th=[  3261],
     | 30.00th=[  4686], 40.00th=[  6128], 50.00th=[  7570], 60.00th=[  8979],
     | 70.00th=[ 10552], 80.00th=[ 12256], 90.00th=[ 20055], 95.00th=[235930],
     | 99.00th=[242222], 99.50th=[246416], 99.90th=[254804], 99.95th=[283116],
     | 99.99th=[308282]
   bw (  KiB/s): min= 1824, max= 6112, per=25.01%, avg=4299.76, stdev=1064.43, samples=720
   iops        : min=  456, max= 1528, avg=1074.85, stdev=266.09, samples=720
  lat (usec)   : 250=0.01%, 500=0.18%, 750=1.57%, 1000=1.78%
  lat (msec)   : 2=7.37%, 4=14.28%, 10=41.32%, 20=23.48%, 50=0.10%
  lat (msec)   : 100=0.01%, 250=9.68%, 500=0.23%
  cpu          : usr=3.80%, sys=15.08%, ctx=292494, majf=0, minf=172
  IO depths    : 1=0.1%, 2=0.1%, 4=0.1%, 8=0.1%, 16=0.1%, 32=100.0%, >=64=0.0%
     submit    : 0=0.0%, 4=100.0%, 8=0.0%, 16=0.0%, 32=0.0%, 64=0.0%, >=64=0.0%
     complete  : 0=0.0%, 4=100.0%, 8=0.0%, 16=0.0%, 32=0.1%, 64=0.0%, >=64=0.0%
     issued rwt: total=386943,0,0, short=0,0,0, dropped=0,0,0
     latency   : target=0, window=0, percentile=100.00%, depth=32

Run status group 0 (all jobs):
   READ: bw=16.8MiB/s (17.6MB/s), 16.8MiB/s-16.8MiB/s (17.6MB/s-17.6MB/s), io=1511MiB (1585MB), run=90036-90036msec

Disk stats (read/write):
  sda: ios=386353/29, merge=22/3, ticks=11385188/452, in_queue=11399316, util=99.96%

test: (g=0): rw=randread, bs=(R) 4096B-4096B, (W) 4096B-4096B, (T) 4096B-4096B, ioengine=libaio, iodepth=32
...
fio-3.1
Starting 4 processes
Jobs: 4 (f=4): [r(4)][100.0%][r=142MiB/s,w=0KiB/s][r=36.2k,w=0 IOPS][eta 00m:00s]
test: (groupid=0, jobs=4): err= 0: pid=16691: Mon Dec 27 06:27:32 2021
   read: IOPS=35.8k, BW=140MiB/s (147MB/s)(12.3GiB/90001msec)
    slat (usec): min=10, max=22430, avg=33.32, stdev=38.30
    clat (usec): min=54, max=32610, avg=3490.11, stdev=551.70
     lat (usec): min=111, max=32708, avg=3540.58, stdev=558.85
    clat percentiles (usec):
     |  1.00th=[ 2704],  5.00th=[ 2900], 10.00th=[ 3130], 20.00th=[ 3261],
     | 30.00th=[ 3294], 40.00th=[ 3359], 50.00th=[ 3392], 60.00th=[ 3458],
     | 70.00th=[ 3523], 80.00th=[ 3621], 90.00th=[ 3916], 95.00th=[ 4228],
     | 99.00th=[ 5669], 99.50th=[ 6128], 99.90th=[ 7308], 99.95th=[ 8160],
     | 99.99th=[21627]
   bw (  KiB/s): min=21208, max=43096, per=25.00%, avg=35796.55, stdev=3151.89, samples=720
   iops        : min= 5302, max=10774, avg=8949.11, stdev=787.96, samples=720
  lat (usec)   : 100=0.01%, 250=0.01%, 500=0.01%, 750=0.01%, 1000=0.01%
  lat (msec)   : 2=0.01%, 4=91.78%, 10=8.20%, 20=0.01%, 50=0.01%
  cpu          : usr=4.69%, sys=95.30%, ctx=837, majf=0, minf=159
  IO depths    : 1=0.1%, 2=0.1%, 4=0.1%, 8=0.1%, 16=0.1%, 32=100.0%, >=64=0.0%
     submit    : 0=0.0%, 4=100.0%, 8=0.0%, 16=0.0%, 32=0.0%, 64=0.0%, >=64=0.0%
     complete  : 0=0.0%, 4=100.0%, 8=0.0%, 16=0.0%, 32=0.1%, 64=0.0%, >=64=0.0%
     issued rwt: total=3221862,0,0, short=0,0,0, dropped=0,0,0
     latency   : target=0, window=0, percentile=100.00%, depth=32

Run status group 0 (all jobs):
   READ: bw=140MiB/s (147MB/s), 140MiB/s-140MiB/s (147MB/s-147MB/s), io=12.3GiB (13.2GB), run=90001-90001msec

Disk stats (read/write):
  vda: ios=3217764/35, merge=0/7, ticks=344720/20, in_queue=164292, util=84.56%
```

### 随机读写IOPS
```
sudo fio -direct=1 -iodepth=32 -rw=randrw -ioengine=libaio -bs=4k -size=4G -numjobs=4 -time_based=1 -runtime=90 -group_reporting -filename=/dev/sda -name=test
```

### 顺序写吞吐量
```
sudo fio -direct=1 -iodepth=128 -rw=write -ioengine=libaio -bs=128k -size=4G -numjobs=1 -time_based=1 -runtime=180 -group_reporting -filename=/dev/sda -name=test
```

### 顺序读吞吐量
```
sudo fio -direct=1 -iodepth=128 -rw=read -ioengine=libaio -bs=128k -size=4G -numjobs=1 -time_based=1 -runtime=180 -group_reporting -filename=/dev/sda -name=test
```

## 参考资料
* [测试块存储性能](https://help.aliyun.com/document_detail/147897.html)
