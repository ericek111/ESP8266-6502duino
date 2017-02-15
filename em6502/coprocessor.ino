/* I've hijacked unused 6502 opcode 0x02 as a coprocessor instruction. The byte
 * following 0x02 is an operation code which will be passed to this function.
 * The function can use what's in A, X and Y, and may modify the value in A
 * if necessary. If A is modified then the N and Z flags should also be modified
 * appropriately (using setnz(A)).
 */

#include "FS.h"

enum
{
    /* Toggles emulator debug flag. This may or may not cause 
     * stuff to happen!
     */
    COP_DEBUG = 0,

    /* Returns, in A, the number of pages of RAM we have. RAM
     * starts at page 0 and is contiguous.
     */
    COP_RAMTOP = 1,

    /* Reboot.
     */
    COP_REBOOT = 2,

    /* A=0: Zero icounter.
     * Retrieve icounter into $YYXX .. $YYXX+3
     */
    COP_ICOUNTER = 3,

    /* Clear IFLAG from A (iflag &= ~A)
     * and return resulting flag in A.
     * So call it with 0 to read, or a bitmask to clear.
     */
    COP_IFLAG = 4,

    /* Implements OSFIND.
     */
    COP_OSFIND = 5,

    /* Implements OSFILE.
     */
    COP_OSFILE = 6,

    /* Implements OSBGET.
     */
    COP_OSBGET = 7,

    /* Implements OSBPUT.
     */
    COP_OSBPUT = 8,

    /* Implements OSGBPB.
     */
    COP_OSGBPB = 9,

    /* Implements OSWRCH.
     */
    COP_SENDCHAR = 10,

    /* Poll for keyboard input
     */
    COP_GETCHAR = 11,

    /* Implements OSARGS.
     */
    COP_OSARGS = 12,
};

const byte MAXFH = 8;
static File filehandles[MAXFH];

/* Called from setup()
 */
void init_coprocessor(void)
{
    SPIFFS.begin();
}

/* Get a CR-terminated string.
 * At most 255 characters. Returns a static buffer!!!
 */
char *getCRstring(word addr6502)
{
    static char s[256];
    word i;

    for (i = 0; i < 256; i++)
    {
        byte m = memread(addr6502 + i);
        if (m == '\r')
        {
            s[i] = 0;
            break;
        }
        s[i] = m;
    }
    if (i == 256)
    {
        s[255] = 0;
    }
    return s;
}

/* This is called from op_COP and passed the byte 
 * following the COP instruction.
 */
void do_coprocessor_instruction(byte operation)
{
    switch (operation)
    {
        case COP_DEBUG:
            /* Just toggle debugflag. Could be used to
             * enable debugging of parts of the emulator.
             */
            debugflag = ! !A;
            break;

        case COP_RAMTOP:
            /* Return the number of pages of RAM. It's
             * assumed that RAM is contigous, starting at
             * zero.
             */
            A = RAMSIZE >> 8;
            setnz(A);
            break;

        case COP_REBOOT:
            /* Reboots the emulator.
             */
            cop_reboot();
            break;

        case COP_ICOUNTER:
            /* If A=0 the reset instruction counter, otherwise
             * store current value into 4 bytes at $YYXX.
             */
            cop_icounter();
            break;

        case COP_IFLAG:
            /* Read or clear interrupt flags.
             * Performs iflag &= ~A and returns
             * resulting iflag in A.
             */
            cop_iflag();
            break;

        case COP_OSFIND:
            /* Implements a subset of OSFIND on SPIFFS filesystem.
             */
            osfind();
            break;

        case COP_OSFILE:
            /* Implements a subset of OSFILE on SPIFFS filesystem
             */
            osfile();
            break;

        case COP_OSBGET:
            /* Implements OSBGET on SPIFFS filesystem
             */
            osbget();
            break;

        case COP_OSBPUT:
            /* Implements OSBPUT on SPIFFS filesystem
             */
            osbput();
            break;

        case COP_OSGBPB:
            /* Implements a subset of OSGBPB on SPIFFS filesystem
             */
            osgbpb();
            break;

        case COP_SENDCHAR:
            /* Send a character to serial and telnet port.
             */
            cop_sendchar();
            break;

        case COP_GETCHAR:
            /* Poll serial or telnet port, resets serial interrupt flag.
             */
            cop_getchar();
            break;

        case COP_OSARGS:
            /* A subset of OSARGS.
             */
            cop_osargs();
            break;
    }
}

void osfind(void)
{
    byte fh;

    /* Close file(s).
     */
    if (A == 0)
    {
        if (Y == 0)
        {
            /* Close all files.
             */
            for (fh = 0; fh < MAXFH; fh++)
            {
                if (filehandles[fh])
                {
                    filehandles[fh].close();
                }
            }
        }
        else
        {
            if ((Y <= MAXFH) && (filehandles[Y - 1]))
            {
                filehandles[Y - 1].close();
            }
        }
        return;
    }

    /* Have we got a spare slot?
     */
    for (fh = 0; fh < MAXFH; fh++)
    {
        if (!filehandles[fh])
        {
            break;
        }
    }

    /* No spare slots.
     */
    if (fh == MAXFH)
    {
        A = 0;
        setnz(A);
        return;
    }

    /* Open file. Name is stored in $YYXX and is CR-terminated.
     */
    char *filename = getCRstring(((word) Y << 8) | (word) X);

    switch (A)
    {
        case 0x40:
            /* Open for input.
             */
            filehandles[fh] = SPIFFS.open(filename, "r");
            break;

        case 0x80:
            /* Open for output.
             */
            filehandles[fh] = SPIFFS.open(filename, "w");
            break;

        case 0xC0:
            /* Open for both. Create if non-existant.
             */
            filehandles[fh] = SPIFFS.open(filename, "r+");
            if (!filehandles[fh])
            {
                filehandles[fh] = SPIFFS.open(filename, "w+");
            }
            break;
    }

    /* Error?
     */
    if (!filehandles[fh])
    {
        A = 0;
        setnz(A);
        return;
    }

    /* Return filehandle. Offset of one, since FH=0 means error.
     */
    A = fh + 1;
    setnz(A);
}

/* A subset of OSFILE.
 */
void osfile(void)
{
    word cb = ((word) Y << 8) | (word) X;
    char *filename =
        getCRstring(((word) memread(cb + 1) << 8) | (word) memread(cb));

    word loadaddr = getword(cb + 2);
    word execaddr = getword(cb + 6);
    word startaddr = getword(cb + 10);
    word endaddr = getword(cb + 14);

    SR &= ~bit_C;
    switch (A)
    {
        case 0:
	{
            /* Save memory to file.
             */
            File f = SPIFFS.open(filename, "w");
            if (!f)
            {
                SR |= bit_C;
                return;
            }
            else
            {
                word i;
                for (i = startaddr; i < endaddr; i++)
                {
                    f.write(memread(i));
                }
                f.close();
            }
            return;
	}
        case 6:
	{
            /* Delete named file.
             */
            Serial.print("File: ");
            Serial.println(filename);
            if (!SPIFFS.remove(filename))
            {
                SR |= bit_C;
            }
            return;
	}
        case 0xFF:
	{
            /* Load file to memory.
             */
            File f = SPIFFS.open(filename, "r");
            if (!f)
            {
                SR |= bit_C;
                return;
            }
            else
            {
                word i;
                word size = f.size();
                if (execaddr & 0xFF)
                {
                    SR |= bit_C;
                    return;
                }
                for (i = loadaddr; i < loadaddr + size; i++)
                {
                    memwrite(i, f.read());
                }
                f.close();
            }
            return;
	}
    }
}

/* Get byte from filehandle. Set C on error.
 */
void osbget(void)
{
    SR &= ~bit_C;
    if (filehandles[Y])
    {
        signed int c = filehandles[Y].read();
        if (c < 0)
        {
            SR |= bit_C;
        }
        A = c;
    }
    else
    {
        SR |= bit_C;
    }
}

/* Put byte to filehandle. 
 * Set C on error.
 */
void osbput(void)
{
    SR &= ~bit_C;
    if (filehandles[Y])
    {
        if (filehandles[Y].write(A) != 1)
        {
            SR |= bit_C;
        }
    }
    else
    {
        SR |= bit_C;
    }
}

/* Only a small subset of OSGBPB, necessary for getting file list.
 */
void osgbpb(void)
{
    word cb = ((word) Y << 8) | (word) X;

    /* Read entries from current directory.
     */
    if (A == 8)
    {
        word datapointer = getword(cb + 1);
        word items = getword(cb + 5);
        uint32 fptr = getword32(cb + 9);
        int i;
        bool atend = false;

        SR &= ~bit_C;

        /* This is terribly time-inefficient, but about the best I can
         * come up with in terms of memory!
         */
        Dir d = SPIFFS.openDir("");
        for (i = 0; i < fptr; i++)
        {
            if (!d.next())
            {
                break;
            }
        }

        for (i = 0; i < items; i++)
        {
            if (!d.next())
            {
                atend = true;
                break;
            }
            String f = d.fileName();
            memwrite(datapointer++, (byte) f.length());
            for (int j = 0; f[j]; j++)
            {
                memwrite(datapointer++, f[j]);
            }
            fptr++;
        }
        if (atend)
        {
            if (i == 0)
            {
                SR |= bit_C;
            }
        }

        items -= i;
        putword(cb + 5, items);
        putword(cb + 1, datapointer);
        putword32(cb + 9, fptr);
    }
}

/* Send character in A to serial and telnet client.
 */
void cop_sendchar(void)
{
    Serial.write(A);
    if (client && client.connected())
    {
        client.write(A);
    }
}

/* Read character from serial or telnet client
 * and clear serial interrupt flag. Returns with
 * A=0 if nothing waiting.
 */
void cop_getchar(void)
{
    if (Serial.available())
    {
        A = Serial.read();
        iflag &= ~IFLAG_SERIAL;
    }
    else if (tchar_waiting)
    {
        A = tchar;
        tchar_waiting = false;
        iflag &= ~IFLAG_SERIAL;
    }
    else
    {
        A = 0;
    }
}

/* Implements a small subset of OSARGS, just
 * enough to allow *FREE to work.
 */
void cop_osargs()
{
    /* File handle == 0?
     */
    if (Y == 0)
    {
        switch (A)
        {
            case 4:
            {
                /* Get disk space used.
                 */
                FSInfo fs_info;
                SPIFFS.info(fs_info);
                putword32((word)X, fs_info.usedBytes);
                break;
            }
            case 5:
	    {
                /* Get disk space free.
                 */
                FSInfo fs_info;
                SPIFFS.info(fs_info);
                putword32((word)X, fs_info.totalBytes - fs_info.usedBytes);
                break;
            }
        }
    }
}

/* Reboot the ESP.
 */
void cop_reboot(void)
{
    ESP.restart();
}

/* Clear interrupt flags from A and return result.
 * If A=0 then nothing gets cleared, so you can use 
 * this to read the interrupt flag.
 */
void cop_iflag(void)
{
    iflag &= ~A;
    A = iflag;
    setnz(A);
}

/* Reset (if A=0) or read the instruction counter (otherwise).
 */
void cop_icounter(void)
{
    if (A == 0)
    {
        icounter = 0;
    }
    else
    {
        putword32(((word) Y << 8) | (word) X, icounter);
    }
}
