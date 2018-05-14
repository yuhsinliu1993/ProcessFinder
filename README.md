# Process Finder

Implement a simple process finder just like **ps** command in Linux system.


About the program
-----------------
1. List all the processes, and display the following info for each process: pid, uid, gid, ppid, pgid, sid, tty, status, and (image) command line.
2. By default, your tool listed only the processes belong to the current user and have an associated terminal.
3. A "**-a**" option can be used to list processes from all the users.
4. A "**-x**" option can be used to list processes without an associated terminal
5. Provide **sort** options,**-p**, **-q**, **-r**, and **-s**, which sort the listed processes by pid (default), ppid, pgid, and sid, respectively.


### A running example:

```
  pid   uid   gid  ppid  pgid   sid      tty St (img) cmd
    1     0     0     0     1     1        -  S (systemd) /sbin/init splash
    2     0     0     0     0     0        -  S (kthreadd)
    3     0     0     2     0     0        -  S (ksoftirqd/0)
    5     0     0     2     0     0        -  S (kworker/0:0H)
    7     0     0     2     0     0        -  S (rcu_sched)
  ...
  402  1000  1000     1   402   402        -  S (tmux) tmux
  418  1000  1000   402   418   418   pts/13  S (bash) -bash
 1000     0     0     1  1000  1000   pts/13  S (ModemManager) /usr/sbin/ModemManager
 7541  1000  1000     1  7541  7541   pts/13  S (systemd) /lib/systemd/systemd --user
 7795  1000  1000  7541  7541  7541   pts/13  S ((sd-pam)) (sd-pam)          
10286  1000  1000 10257 10257 10257   pts/13  S (sshd) sshd: chuang@pts/2
10287  1000  1000 10286 10287 10287    pts/2  S (bash) -bash
10299  1000  1000 10287 10299 10287    pts/2  S (tmux) tmux at
14021  1000  1000 28846 14021 28846    pts/4  R (psf) ./psf -ax
26372  1000  1000 26343 26343 26343    pts/4  S (sshd) sshd: chuang@pts/1
26373  1000  1000 26372 26373 26373    pts/1  S (bash) -bash
28846  1000  1000   402 28846 28846    pts/4  S (bash) -bash

```
