/*************************************************************************\
 *                  Copyright (C) Michael Kerrisk, 2014.                   *
 *                                                                         *
 * This program is free software. You may use, modify, and redistribute it *
 * under the terms of the GNU General Public License as published by the   *
 * Free Software Foundation, either version 3 or (at your option) any      *
 * later version. This program is distributed without any warranty.  See   *
 * the file COPYING.gpl-v3 for details.                                    *
* \*************************************************************************/

/************************************************************\
 *	Modified by: Nick Trusso
 *	Date: 9/18/2015
* \***********************************************************/


#define _BSD_SOURCE     /* Get major() and minor() from <sys/types.h> */
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include "file_perms.h"
#include "tlpi_hdr.h"
#include <stdio.h>
#include <stdlib.h>
#include <grp.h>
#include <pwd.h>

static void
displayStatInfo(const struct stat *sb,char filename[])
{
    //1st Line
    printf("   File: '%s'\n",filename);


    //2nd Line 
    printf("   Size: %lld", (long long) sb->st_size); 
    printf("		Blocks: %lld", (long long) sb->st_blocks); 
    printf("	    IO Block: %ld", (long) sb->st_blksize);
    switch (sb->st_mode & S_IFMT) {
    case S_IFREG:  printf(" regular file\n");            break;
    case S_IFDIR:  printf(" directory\n");               break;
    case S_IFCHR:  printf(" character device\n");        break;
    case S_IFBLK:  printf(" block device\n");            break;
    case S_IFLNK:  printf(" symbolic (soft) link\n");    break;
    case S_IFIFO:  printf(" FIFO or pipe\n");            break;
    case S_IFSOCK: printf(" socket\n");                  break;
    default:       printf(" unknown file type?\n");      break;
    }
    
    //3rd Line
    printf("Device: %lxh/%ldd ",
                (long) sb->st_dev, (long) sb->st_dev);
    printf("Inode: %ld ", (long) sb->st_ino);
    printf("  Links: %ld\n",(long)sb->st_nlink);

    //4th Line
    printf("Access: (%lo/",(unsigned long) sb->st_mode & 007777);  
    switch (sb->st_mode & S_IFMT) {
    case S_IFREG:  printf("-"); break;
    case S_IFDIR:  printf("d"); break;
    case S_IFCHR:  printf("c"); break;
    case S_IFBLK:  printf("b"); break;
    case S_IFLNK:  printf("l"); break;
    case S_IFIFO:  printf("p"); break;
    case S_IFSOCK: printf("s"); break;
    default:       printf("u"); break;
    }

    printf("%s)  ",filePermStr(sb->st_mode,1));
   
    //to display the callers user name and group
    struct passwd *pwd;
    struct group *gp;
    pwd = getpwuid(sb->st_uid);
    gp = getgrgid(sb->st_gid);
    
    printf("Uid: ( %ld/ %s)    ",(long)sb->st_uid,pwd->pw_name);	
    printf("Gid: ( %ld/ %s)\n",(long) sb->st_gid,gp->gr_name);
    
    //I dont see where stat uses this 
    if (sb->st_mode & (S_ISUID | S_ISGID | S_ISVTX))
        printf("special bits set: %s%s%s\n",  
                (sb->st_mode & S_ISUID) ? "set-UID " : "",
                (sb->st_mode & S_ISGID) ? "set-GID " : "",
                (sb->st_mode & S_ISVTX) ? "sticky " : "");

    
    //5th Line
    if (S_ISCHR(sb->st_mode) || S_ISBLK(sb->st_mode))
        printf("Device number (st_rdev):  major=%ld; minor=%ld\n",
                (long) major(sb->st_rdev), (long) minor(sb->st_rdev));
 
    char buf[64];
    //print last access time
    strftime(buf,sizeof(buf),"Access: %F %T %z\n",localtime(&sb->st_atime));
    printf("%s",buf);
    //print last modified time   
    strftime(buf,sizeof(buf),"Modify: %F %T %z\n",localtime(&sb->st_mtime));
    printf("%s",buf);
    //print last change time
    strftime(buf,sizeof(buf),"Modify: %F %T %z\n",localtime(&sb->st_ctime));
    printf("%s",buf);
    
}

int main(int argc, char *argv[])
{
    struct stat sb;
    Boolean statLink;           /* True if "-l" specified (i.e., use lstat) */
    int fname;                  /* Location of filename argument in argv[] */    
    statLink = (argc > 1) && strcmp(argv[1], "-l") == 0;
                                /* Simple parsing for "-l" */
    fname = statLink ? 2 : 1;

    if (fname >= argc || (argc > 1 && strcmp(argv[1], "--help") == 0))
        usageErr("%s [-l] file\n"
                "        -l = use lstat() instead of stat()\n", argv[0]);

    if (statLink) {
        if (lstat(argv[fname], &sb) == -1)
            errExit("lstat");
    } else {
        if (stat(argv[fname], &sb) == -1)
            errExit("stat");
    }
    
    displayStatInfo(&sb,argv[fname]); 
   
    exit(EXIT_SUCCESS);
}
