/*******************************************************
*                      t.c file                        *
*******************************************************/
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define TRK 18
#define CYL 36
#define BLK 1024
#define RET "\n\r"

#include "ext2.h"

typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

GD    *gp;
INODE *ip;
DIR   *dp;

char buf1[BLK], buf2[BLK];
int color = 0x0A;
u8 ino;
char *msg = "Data Block =   \n\r";

main() { 
    u16    i, iblk, blk0, lo, hi;
    u32    *up;
    char   c, temp[64], *cp;

    prints("read block# 2 (GD)" RET);
    getblk((u16)2, buf1);

    // get bg_inode_table block number
    gp = (GD *)buf1;
    iblk = (u16)gp->bg_inode_table;
    
    prints("inode_block= "); putc(iblk+'0'); prints(RET); 

    // get root inode
    prints("read inodes begin block to get root inode" RET);
    
    getblk((u16)iblk, buf1);
    ip = (INODE *)buf1 + 1;

    // step through the data block of root inode
    // print out file names in root DIR
    prints("read data block of root DIR" RET);
    
    blk0 = (u16)ip->i_block[0];

    lo = blk0 % 10; // little trick to print numbers
    hi = blk0 / 10;
    msg[13] = hi+'0'; msg[14] = lo+'0';
    prints(msg);

    getblk(blk0, buf2);
    dp = (DIR *)buf2;
    cp = buf2;
    while (cp < buf2 + BLK) { // print out names in DIR
        strncpy(temp, dp->name, dp->name_len);
        temp[dp->name_len] = 0;
        prints(temp); putc(' ');
        cp += dp->rec_len;
        dp = (DIR *)cp;
    }
    prints(RET);

    prints("Done." RET);
    return 1;
}  

int prints(char *s) {
    while(*s) {
        putc(*s);
        s++;
    }
}

int gets(char *s) {
    char c;
    while((c = getc()) && c != '\r') {
        *s = c;
        putc(c);
        s++;
    }
    *s = 0;
}

int getblk(u16 blk, char *buf) {
    readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);
    //readfd( blk/18, ((blk)%18)/9, ( ((blk)%18)%9)<<1, buf);
}
