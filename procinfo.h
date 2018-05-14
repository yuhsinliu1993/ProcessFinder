#include <limits.h>     // _POSIX_PATH_MAX, _POSIX2_LINE_MAX

#ifndef _PROCINFO_H_
#define _PROCINFO_H_

typedef struct statstruct_proc {
  int           pid;                       /** The process id. **/
  char          exName [_POSIX_PATH_MAX];  /** The filename of the executable **/
  char          state; /** 1 **/           /** R is running,
                                               S is sleeping,
			                                   D is sleeping in an uninterruptible wait,
			                                   Z is zombie,
                                               T is traced or stopped **/
  int           ppid;                      /** The pid of the parent. **/
  int           pgrp;                      /** The pgrp of the process. **/
  int           session;                   /** The session id of the process. **/
  int           tty;                       /** The tty the process uses **/
  unsigned      euid,                      /** effective user id **/
                egid;                      /** effective group id */
  char          cmdline[_POSIX2_LINE_MAX]; /** Command **/
  struct statstruct_proc *next;            /** For sorting **/
} procinfo;

struct proc_pid_list {
    procinfo *_p_info;
};

#endif
