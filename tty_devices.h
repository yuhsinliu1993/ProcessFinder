#ifndef _TTY_DEVICES_H_
#define _TTY_DEVICES_H_

typedef struct tty_devices_struct {
    int major;
    int minor;
    char *dev;
} tty_devices;

tty_devices tty_device_dict[] = {
    10,   1, "psaux",
    10,  56, "memory_bandwidth",
    10,  57, "network_throughput",
    10,  58, "network_latency",
    10,  59, "cpu_dma_latency",
    10,  61, "ecryptfs",
    10,  62, "rfkill",
    10,  63, "vga_arbiter",
    10, 137, "vhci",
    10, 183, "hwrng",
    10, 203, "cuse",
    10, 227, "mcelog",
    10, 228, "hpet",
    10, 229, "fuse",
    10, 231, "snapshot",
    10, 234, "btrfs-control",
    10, 235, "autofs",
    10, 237, "loop-control",
    10, 239, "uhid",
     5,   0, "tty",
     5,   1, "console",
     5,   2, "ptmx",
     5,   3, "ttyprintk",
    29,   0, "fb0",
   246,   0, "hidraw0",
    89,   0, "i2c-0",
     1,   1, "mem",
     1,   3, "null",
     1,   4, "port",
     1,   5, "zero",
     1,   7, "full",
     1,   8, "random",
     1,  11, "kmsg",
   108,   0, "ppp",
   249,   0, "rtc0",
    11,   0, "sr0",
     0,   0, 0
};

#endif
