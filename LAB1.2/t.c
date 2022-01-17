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

main() { 
    u16    i, iblk;
    char   c, temp[64];
    char   *cp;

    prints("read block# 2 (GD)" RET);
    getblk(2, buf1);

    // get bg_inode_table block number
    gp = (GD *)buf1;
    iblk = (u16)gp->bg_inode_table;
    
    prints("inode_block= "); putc(iblk+'0'); prints(RET); 

    // get root inode
    prints("read inodes begin block to get root inode" RET);
    
    getblk(iblk, buf1);
    ip = (INODE *)buf1 + 1;

    // step through the data block of root inode
    // print out file names in root DIR
    prints("read data block of root DIR\n\r");
    
    setes(0x1000); // Set es register

    for (i = 0; i < 12; i++) { // Direct blocks
        getblk((u16)ip->i_block[i], 0); // Load to es
        putc("#");
        inces(); // increment es register
    }

    getblk(ip->i_block[12], buf2);
    if (ip->i_block[12]) { // indirect blocks
        //getblk(ip->i_block[12], buf2);
        u32 *up = (u32 *)buf2;
        while (*up) {
            getblk((u16)*up, 0);
            putc("*");
            inces(); // increment es reg
            up++;
        }
    }

    prints("Blocks loaded" RET);
    getc();

    /*for (i = 0; i < 12; i++) { // Direct blocks
        if ((u16)ip->i_block[i] == 0)
            break; // break if empty
        getblk(ip->i_block[i], buf2);
        dp = (DIR *)buf2;
        cp = buf2;
        while (cp < buf2 + BLK) { // print out names in DIR
            prints(dp->name); prints(RET);
            cp += dp->rec_len;
            dp = (DIR *)cp;
        }
    }*/
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
