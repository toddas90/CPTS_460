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

//u16 NSEC = 2;
char buf1[BLK], buf2[BLK];
INODE *ip;
GD *gp;
DIR *dp;
u16 iblk;

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
        //prints(temp); putc(' ');
        if(strcmp(temp, name) == 0) {
            //prints("\n\r");
            return dp->inode;
        }
        cp += dp->rec_len;
        dp = (DIR *)cp;
    }
    error();
}

void getinode(u8 ino) {
    u8 blk, offset;
    
    blk = (ino - 1) / 8 + iblk;
    offset = (ino - 1) % 8;
    getblk(blk, buf1);
    ip = (INODE *)buf1 + offset; 
}

main() {
    u32 *up;
    u8 i, ino;

    getblk(2, buf1); // Get block into buf
    gp = (GD *)buf1;
    iblk = (u16)gp->bg_inode_table;

    getblk((u16)iblk, buf1); // Get root into buf
    ip = (INODE *)buf1 + 1;
   
    // Get /boot inode into buf
    ino = search(ip, "boot");
    getinode(ino);

    // get mtx into buf
    ino = search(ip, "mtx");
    getinode(ino);
    
    //prints("Loading mtx...\n\r");

    getblk((u16)ip->i_block[12], buf2);


    setes(0x1000);

    // load direct blocks
    for (i = 0; i < 12; i++){
        getblk((u16)ip->i_block[i], 0);
        //putc('*');
        inces();
     }

    // load indirect blocks
    if (ip->i_block[12]){
        up = (u32 *)buf2;      
        while(*up){
            getblk((u16)*up, 0);
            //putc('.');
            inces();
            up++;
        }
    }
    
    //prints("Kernel Loaded\n\r");
    return 1;
    // All done, go and execute kernel <3
}  
