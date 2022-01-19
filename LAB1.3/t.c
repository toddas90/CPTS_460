/*******************************************************
*                  @t.c file                          *
*******************************************************/

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define TRK 18
#define CYL 36
#define BLK 1024

#include "ext2.h"

typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

u16 NSEC = 2;
char buf1[BLK], buf2[BLK];
int buf3[BLK];
INODE *ip;
GD *gp;
DIR *dp;

int strcmp(const char *s1, const char *s2) {
    while(*s1 && (*s1 == *s2)) {
        s1++; 
        s2++;
    }
    return *(u8*)s1 - *(u8*)s2;
}

int prints(char *s){
    while(*s) {
        putc(*s);
        s++;
    }
}

u16 getblk(u16 blk, char *buf) {
    readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);
}

// For now I am going to make this always search for "mtx", but it can be easily changed.
//u16 search(INODE *ip, char *name) {
u16 search(INODE *ip) {
    char *cp, temp[64];
    char *name = "mtx"; // remove this if switching to searching for specific name.

    u16 blk0 = ip->i_block[0];
    getblk((u16)blk0, buf2);
    
    dp = (DIR *)buf2;
    cp = buf2;

    while(cp < buf2 + BLK) {
        strncpy(temp, dp->name, dp->name_len);
        temp[dp->name_len] = 0;
        if(strcmp(temp, name) == 0) {
            return dp->inode;
        }
        cp += dp->rec_len;
        dp = (DIR *)cp;
    }
    error();
  //search for name in the data block of INODE; 
  //return its inumber if found
  //else error();
}

main() {
    u16 i, iblk;
    u32 *up;

    getblk(2, buf1); // Get block into buf
    gp = (GD *)buf1;
    iblk = (u16)gp->bg_inode_table;

    getblk((u16)iblk, buf1); // Get root into buf
    ip = (INODE *)buf1 + 1;

    ip = (INODE *)search(ip);

    setes(0x1000);

    // load direct blocks
    for (i=0; i<12; i++){
        getblk((u16)ip->i_block[i], 0);
        inces();
     }

    // load indirect blocks
    if (ip->i_block[12]){
        up = (u32 *)buf2;      
        while(*up){
            getblk((u16)*up, 0);
            inces();
            up++;
        }
    }
}  
