/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

int exec(char *cmdline) // cmdline=VA in Uspace
{
    int i, upa, usp;
    char *cp, kline[128], file[32], filename[32];
    PROC *p = running;

    strcpy(kline, cmdline);
    cp = kline; i = 0;
    while (*cp != ' ')
        filename[i++] = *cp++;

    filename[i] = 0;
    file[0] = 0;

    // if (filename[0] != '/')
    //     strcpy(file, "/bin/");
    kstrcat(file, filename);

    upa = p->pgdir[2048] & 0xFFFFF0000; // get PA of Umode image

    if (!load(file, p))
        return -1;

    usp = upa + 0x100000 - 128; // Umode stack at 1MB + pid*16KB - 128

    strcpy((char *)usp, kline); // copy cmdline to Umode stack
    p->usp = (int *)VA(0x100000 - 128); // Umode stack pointer

    for (i = 2; i < 14; i++)
        p->kstack[SSIZE - i] = 0; // clear kstack
    
    p->kstack[SSIZE - 1] = (int)VA(0); // resume to goUmode

    return (int)p->usp;
}
