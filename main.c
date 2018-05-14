#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>     // getopt
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <fnmatch.h>
#include <errno.h>

#include "procinfo.h"
#include "tty_devices.h"

#define MAX_PROCS 32768

static struct proc_pid_list p_info_list[MAX_PROCS] = { 0 };


void err_sys(const char* msg){
  perror(msg);
  // exit(1);
}

static int get_proc_info(pid_t pid, procinfo *pinfo){
    char proc_path[_POSIX_PATH_MAX], proc_stat_path[_POSIX_PATH_MAX], proc_cmd_path[_POSIX_PATH_MAX];
    char stat_str[_POSIX2_LINE_MAX], cmd_str[_POSIX2_LINE_MAX];
    char *s, *t;
    FILE *proc_fp, *cmd_fp;
    struct stat st;

    if (pinfo == NULL) {
        errno = EINVAL;
        return -1;
    }

    snprintf(proc_path, _POSIX_PATH_MAX, "/proc/%u", (unsigned) pid);
    snprintf(proc_stat_path, _POSIX_PATH_MAX, "/proc/%u/stat", (unsigned) pid);
    snprintf(proc_cmd_path, _POSIX_PATH_MAX, "/proc/%u/cmdline", (unsigned) pid);

    if (access(proc_stat_path, R_OK) == -1)
        return (pinfo->pid = -1);

    if (access(proc_cmd_path, R_OK) == -1)
        return (pinfo->pid = -1);

    if (stat(proc_stat_path, &st) != -1) {
  	     pinfo->euid = st.st_uid;
  	     pinfo->egid = st.st_gid;
    } else {
  	     pinfo->euid = -1;
         pinfo->egid = -1;
    }

    if((proc_fp = fopen(proc_stat_path, "r")) == NULL)
        return (pinfo->pid = -1);

    if((cmd_fp = fopen(proc_cmd_path, "r")) == NULL)
        return (pinfo->pid = -1);

    if((s = fgets (stat_str, _POSIX2_LINE_MAX, proc_fp)) == NULL){
        fclose (proc_fp);
        return (pinfo->pid = -1);
    }

    if((s = fgets (cmd_str, _POSIX2_LINE_MAX, cmd_fp)) == NULL){
        fclose (cmd_fp);
        return (pinfo->pid = -1);
    }
    strncpy (pinfo->cmdline, cmd_str, strlen(cmd_str));
    pinfo->cmdline[strlen(cmd_str)] = '\0';

    /** pid **/
    sscanf (stat_str, "%u", &(pinfo->pid));

    /** exName **/
    s = strchr (stat_str, '(') + 1;
    t = strchr (stat_str, ')');
    strncpy (pinfo->exName, s, t - s);
    pinfo->exName [t - s] = '\0';

    /** rest **/
    sscanf (t + 2, "%c %d %d %d %d",
	              /* 1  2  3  4  5 */
	  &(pinfo->state),
	  &(pinfo->ppid),
	  &(pinfo->pgrp),
	  &(pinfo->session),
	  &(pinfo->tty)
    );

    pinfo->next = NULL;

    fclose (proc_fp);
    fclose (cmd_fp);
    return 0;
}

void _strcpy(int minor, char *prefix, char *ret){
    int len;
    char tmp[4];

    if ((len = sprintf(tmp, "%d", minor)) != -1){
        strncpy(ret, prefix, strlen(prefix));
        ret[strlen(prefix)] = '\0';

        if (len > 0)
            strncat(ret, tmp, len);

        ret[strlen(prefix) + len] = '\0';
        return;
    } else {
        err_sys("sprintf");
    }
}

void tty_devices_mapping(int tty, char *dev_str){
    int i;
    int _minor = tty % 256;
    int _major = (tty - _minor) / 256;

    for(i = 0; tty_device_dict[i].dev; i++){
        if (_major == 4 && _minor < 64){ // tty
            _strcpy(_minor, "tty", dev_str);
            return;
        } else if (_major == 4 && _minor > 64) { // ttyS
            _strcpy(_minor, "ttyS", dev_str);
            return;
        } else if (_major == 136) { // pts
            _strcpy(_minor, "pts/", dev_str);
            return;
        } else if (_major == 7 && _minor < 128) { // vcs
            if (_minor == 0) { strncpy(dev_str, "vcs", 3); dev_str[3] = '\0'; }
            else _strcpy(_minor, "vcs", dev_str);
            return;
        } else if (_major == 7 && _minor > 128) { // vcsa
            if (_minor == 0) { strncpy(dev_str, "vcsa", 4); dev_str[4] = '\0'; }
            else _strcpy(_minor, "vcsa", dev_str);
            return;
        }else if (_major == 21) { // sg
            _strcpy(_minor, "sg", dev_str);
            return;
        } else if (_major == 89) { // i2c-0
            _strcpy(_minor, "i2c-", dev_str);
            return;
        } else if (tty_device_dict[i].major == _major){
            strncpy(dev_str, tty_device_dict[i].dev, strlen(tty_device_dict[i].dev));
            dev_str[strlen(tty_device_dict[i].dev)] = '\0';
            return;
        }
    }

    // No match device
    dev_str[0] = '-';
    dev_str[1] = '\0';
    // if (tty != 0) printf("%d\n", tty);

}

void show_proc_info(procinfo *pinfo, char type, int euid){
    char tty_str[20];
    /** print process info **/
    //  pid   uid   gid  ppid  pgid   sid      tty St (img) cmd
    //    1     0     0     0     1     1        -  S (systemd) /sbin/init splash
    tty_devices_mapping(pinfo->tty, tty_str);

    if (type == 'a' && euid == -1){ // -a
        if (tty_str[0] != '-')
            printf("%5d %5u %5u %5d %5d %5d %8s %2c (%s) %s\n", pinfo->pid, pinfo->euid, pinfo->egid, pinfo->ppid, pinfo->pgrp, pinfo->session, tty_str, pinfo->state, pinfo->exName, pinfo->cmdline);
    } else if (type == 'x' && euid == -1) { // -ax
        printf("%5d %5u %5u %5d %5d %5d %8s %2c (%s) %s\n", pinfo->pid, pinfo->euid, pinfo->egid, pinfo->ppid, pinfo->pgrp, pinfo->session, tty_str, pinfo->state, pinfo->exName, pinfo->cmdline);
    } else if (type == 'x' && euid != -1) { // -x
        if (euid == pinfo->euid)
            printf("%5d %5u %5u %5d %5d %5d %8s %2c (%s) %s\n", pinfo->pid, pinfo->euid, pinfo->egid, pinfo->ppid, pinfo->pgrp, pinfo->session, tty_str, pinfo->state, pinfo->exName, pinfo->cmdline);
    } else {
        // default
        if (euid == pinfo->euid && tty_str[0] != '-')
            printf("%5d %5u %5u %5d %5d %5d %8s %2c (%s) %s\n", pinfo->pid, pinfo->euid, pinfo->egid, pinfo->ppid, pinfo->pgrp, pinfo->session,tty_str, pinfo->state, pinfo->exName, pinfo->cmdline);
    }
}

void sort_process_by(int key, procinfo *p_info){
    procinfo *p = p_info_list[key]._p_info;

    if (p == NULL) {
        p_info_list[key]._p_info = p_info;
    } else {
        for (;;) {
            procinfo *q = p;
            p = p->next;
            if (p == NULL) {
                q->next = p_info;
                break;
            }
        }
    }
}

int main(int argc, char* argv[]){
    int i;
    char ch, mode = 'p', type = 'u';
    uid_t euid = geteuid();
    DIR *dirp;
    struct dirent *dp;

    while ((ch = getopt(argc, argv, "axpqrst")) != -1){
        switch (ch) {
            case 'a': // list processes from all the users
                type = 'a';
                euid = -1;
                break;
            case 'x': // list processes without an associated terminal
                type = 'x';
                break;
            case 'p':
                mode = 'p';
            break;
            case 'q':
                mode = 'q';
                break;
            case 'r':
                mode = 'r';
                break;
            case 's':
                mode = 's';
                break;
            case 't': // list processes in a tree-relationship
                mode = 't';
                break;
        }
    }

    if((dirp = opendir("/proc")) == NULL)
        err_sys("opendir");  // if filename cannot be accessed, or if it cannot malloc(3) enough memory

    /* Scan entries under "/proc" directory */
    if (mode == 't') { /** pstress **/ }
    else {
        printf("  pid   uid   gid  ppid  pgid   sid      tty St (img) cmd\n");

        for (;;) {
            errno = 0;   /* To distinguish error from end-of-directory */
            dp = readdir(dirp);

            if (dp == NULL) {
                if (errno != 0)
                    err_sys("readdir");
                else
                    break;  // end-of-directory
            }

            /* Since we are looking for "/proc/PID" directories, skip entries that are not directories, or don't begin with a digit. */
            if(dp->d_type != DT_DIR || !isdigit((unsigned char) dp->d_name[0]))
                continue;

            procinfo *p_info = malloc(sizeof(procinfo));
            if (get_proc_info((pid_t) atoi(dp->d_name), p_info) != 0)
                continue;
                // err_sys("get_proc_info");

            if (mode == 'q') {
                // sort by ppid
                sort_process_by(p_info->ppid, p_info);
            } else if (mode == 'r') {
                // sort by pgid
                sort_process_by(p_info->pgrp, p_info);
            } else if (mode == 's') {
                // sort by session id
                sort_process_by(p_info->session, p_info);
            } else {
                // default: sort by pid
                sort_process_by(p_info->pid, p_info);
            }
        }

        // Show processes info (sorted)
        for (i = 0; i < MAX_PROCS; i++) {
            if (p_info_list[i]._p_info != NULL) {
                procinfo *_p_info = p_info_list[i]._p_info;
                for (;;) {
                    show_proc_info(_p_info, type, euid);

                    _p_info = _p_info->next;
                    if (_p_info == NULL) break;
                }
            }
        }
    }

    return 0;
}
