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
INODE *ip;
GD *gp;
DIR *dp;

u16 strcmpr(char *s1, char *s2) {
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

u16 search(INODE *ip, char *name) {
    char *cp, temp[64];

    u16 blk0 = ip->i_block[0];
    getblk((u16)blk0, buf2);
    
    dp = (DIR *)buf2;
    cp = buf2;

    while(cp < buf2 + BLK) {
        strncpy(temp, dp->name, dp->name_len);
        temp[dp->name_len] = 0;
        prints(temp); putc(' '); prints(name); prints("\n\r");
        if(strcmpr(temp, name) == 0) {
            return dp->inode;
        }
        cp += dp->rec_len;
        dp = (DIR *)cp;
    }
    error();
}

main() {
    u16 i, iblk;
    u32 *up;

    getblk(2, buf1); // Get block into buf
    gp = (GD *)buf1;
    iblk = (u16)gp->bg_inode_table;

    getblk((u16)iblk, buf1); // Get root into buf
    ip = (INODE *)buf1 + 1;
    
    ip = (INODE *)search(ip, "boot");
    ip = (INODE *)search(ip, "mtx");

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
