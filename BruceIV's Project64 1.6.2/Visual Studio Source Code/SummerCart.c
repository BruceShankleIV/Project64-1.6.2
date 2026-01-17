#include "SummerCart.h"
#include <Windows.h>
#include <Shlobj.h>
#include <shlwapi.h>
extern BYTE*N64MEM,*RDRAM,*DMEM,*IMEM,*ROM;
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <io.h>
#define S8 3
struct summercart SummerCart;
static uint8_t*summercart_sd_addr(size_t size) {
    uint32_t addr=SummerCart.data0&0x1fffffff;
    if (addr>=0x1ffe0000&&addr+size<0x1ffe0000+8192)
    {
        return SummerCart.buffer+(addr-0x1ffe0000);
    }
    if (addr>=0x10000000&&addr+size<0x10000000+0x4000000)
    {
        return ROM+(addr-0x10000000);
    }
    return NULL;
}
static uint32_t summercart_sd_init()
{
    FILE*fp;
    if (!SummerCart.sd_path||!(fp=fopen(SummerCart.sd_path,"rb"))) return 0x40000000;
    fseek(fp,0,SEEK_END);
    SummerCart.sd_size=ftell(fp);
    fclose(fp);
    return 0;
}
static uint32_t summercart_sd_read()
{
    size_t i;
    FILE*fp;
    uint8_t*ptr;
    long offset=512*SummerCart.sd_sector;
    size_t size=512*SummerCart.data1;
    if ((unsigned long)(offset+size)>(unsigned long)SummerCart.sd_size||!(ptr=summercart_sd_addr(size))||!(fp=fopen(SummerCart.sd_path,"rb"))) return 0x40000000;
    fseek(fp,offset,SEEK_SET);
    for (i=0; i<size;++i)
    {
        int c=fgetc(fp);
        if (c<0)
        {
            fclose(fp);
            return 0x40000000;
        }
        ptr[i ^ SummerCart.sd_byteswap ^ S8]=c;
    }
    fclose(fp);
    return 0;
}
static uint32_t summercart_sd_write()
{
    size_t i;
    FILE*fp;
    uint8_t*ptr;
    long offset=512*SummerCart.sd_sector;
    size_t size=512*SummerCart.data1;
    if ((unsigned long)(offset+size)>(unsigned long)SummerCart.sd_size||!(ptr=summercart_sd_addr(size))||!(fp=fopen(SummerCart.sd_path,"r+b"))) return 0x40000000;
    fseek(fp,offset,SEEK_SET);
    for (i=0; i<size;++i)
    {
        int c=fputc(ptr[i ^ S8],fp);
        if (c<0)
        {
            fclose(fp);
            return 0x40000000;
        }
    }
    fclose(fp);
    return 0;
}
static void write_fat16_initial_image(const char*path)
{
    BYTE pt0[]={ 0xEB,0x3C,0x90,0x6D,0x6B,0x64,0x6F,0x73,0x66,0x73,0x00,0x00,0x02,0x08,0x01,0x00
                ,0x02,0x00,0x02,0x00,0x00,0xF8,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
                ,0x00,0xF8,0x07,0x00,0x00,0x00,0x29,0xD1,0x49,0x4B,0x66,0x53,0x44,0x43,0x41,0x52
                ,0x44,0x30,0x20,0x20,0x20,0x20,0x46,0x41,0x54,0x31,0x36,0x20,0x20,0x20,0x0E,0x1F
                ,0xBE,0x5B,0x7C,0xAC,0x22,0xC0,0x74,0x0B,0x56,0xB4,0x0E,0xBB,0x07,0x00,0xCD,0x10
                ,0x5E,0xEB,0xF0,0x32,0xE4,0xCD,0x16,0xCD,0x19,0xEB,0xFE,0x54,0x68,0x69,0x73,0x20
                ,0x69,0x73,0x20,0x6E,0x6F,0x74,0x20,0x61,0x20,0x62,0x6F,0x6F,0x74,0x61,0x62,0x6C
                ,0x65,0x20,0x64,0x69,0x73,0x6B,0x2E,0x20,0x20,0x50,0x6C,0x65,0x61,0x73,0x65,0x20
                ,0x69,0x6E,0x73,0x65,0x72,0x74,0x20,0x61,0x20,0x62,0x6F,0x6F,0x74,0x61,0x62,0x6C
                ,0x65,0x20,0x66,0x6C,0x6F,0x70,0x70,0x79,0x20,0x61,0x6E,0x64,0x0D,0x0A,0x70,0x72
                ,0x65,0x73,0x73,0x20,0x61,0x6E,0x79,0x20,0x6B,0x65,0x79,0x20,0x74,0x6F,0x20,0x74
                ,0x72,0x79,0x20,0x61,0x67,0x61,0x69,0x6E,0x20,0x2E,0x2E,0x2E,0x20,0x0D,0x0A,0x00 };
    BYTE pt1fe[]={ 0x55,0xAA,0xF8,0xFF,0xFF,0xFF };
    BYTE pt20000[]={ 0xF8,0xFF,0xFF,0xFF };
    BYTE pt3fe00[]={ 0x53,0x44,0x43,0x41,0x52,0x44,0x30,0x20,0x20,0x20,0x20,0x08,0x00,0x00,0x44,0xA8,0xB4,0x58,0xB4,0x58,0x00,0x00,0x44,0xA8,0xB4,0x58 };
    int fd=_open(path,O_CREAT|O_WRONLY|O_BINARY,0666);
    _write(fd,pt0,sizeof(pt0));
    _lseek(fd,0x1FE,SEEK_SET);
    _write(fd,pt1fe,sizeof(pt1fe));
    _lseek(fd,0x20000,SEEK_SET);
    _write(fd,pt20000,sizeof(pt20000));
    _lseek(fd,0x3FE00,SEEK_SET);
    _write(fd,pt3fe00,sizeof(pt3fe00));
    _lseek(fd,267386880,SEEK_SET);
    SetEndOfFile((HANDLE)_get_osfhandle(fd));
    _close(fd);
}
void init_summercart(struct summercart*summercart)
{
    static char _strPath[1024];
    SHGetFolderPath(NULL,
        CSIDL_APPDATA,
        NULL,
        0,
        _strPath);
    size_t len=strlen(_strPath);
    if (len>0&&_strPath[len-1]!='\\') {
        strcat(_strPath,"\\");
    }
    strcat(_strPath,"PJ64 1.6.2");
    CreateDirectory(_strPath,NULL); // can fail,ignore errors
    char strPath2[1024];
    strcpy(strPath2,_strPath);
    len=strlen(_strPath);
    if (len>0&&_strPath[len-1]!='\\') {
        strcat(_strPath,"\\");
    }
    strcat(_strPath,"AUTO0.iso");
    len=strlen(strPath2);
    if (len>0&&strPath2[len-1]!='\\') {
        strcat(strPath2,"\\");
    }
    strcat(strPath2,"AUTO0.vhd");
    if (_access(_strPath,0)&&_access(strPath2,0))
    {
        write_fat16_initial_image(_strPath);
    };
    summercart->sd_path=_strPath;
}
void poweron_summercart(struct summercart*summercart)
{
    memset(summercart->buffer,0,8192);
    summercart->sd_size=-1;
    summercart->status=0;
    summercart->data0=0;
    summercart->data1=0;
    summercart->sd_sector=0;
    summercart->cfg_rom_write=0;
    summercart->sd_byteswap=0;
    summercart->unlock=0;
    summercart->lock_seq=0;
}
int read_summercart_regs(void*opaque,uint32_t address,uint32_t*Value)
{
    struct pi_controller*pi=(struct pi_controller*)opaque;
    uint32_t addr=address&0xFFFF;
   *Value=0;
    if (SummerCart.unlock) {
        switch (address&0xFFFF)
        {
        case 0x00: *Value=SummerCart.status; break;
        case 0x04: *Value=SummerCart.data0;  break;
        case 0x08: *Value=SummerCart.data1;  break;
        case 0x0C: *Value=0x53437632;        break;
        }
    }
    return 0;
}
int write_summercart_regs(void*opaque,uint32_t address,uint32_t value,uint32_t mask)
{
    uint32_t addr=address&0xFFFF;
    if (addr==0x10)
    {
        switch (value&mask)
        {
        case 0xFFFFFFFF:
            SummerCart.unlock=0;
            break;
        case 0x5F554E4C:
            if (SummerCart.lock_seq==0)
            {
                SummerCart.lock_seq=2;
            }
            break;
        case 0x4F434B5F:
            if (SummerCart.lock_seq==2)
            {
                SummerCart.unlock=1;
                SummerCart.lock_seq=0;
            }
            break;
        default:
            SummerCart.lock_seq=0;
            break;
        }
        return 0;
    }
    if (SummerCart.unlock) {
        switch (addr)
        {
        case 0x00:
            SummerCart.status=0;
            switch (value&mask)
            {
            case 'c':
                switch (SummerCart.data0)
                {
                case 1:
                    SummerCart.data1=SummerCart.cfg_rom_write;
                    break;
                case 3:
                case 6:
                    SummerCart.data1=0;
                    break;
                default:
                    SummerCart.status=0x40000000;
                }
                break;
            case 'C':
                switch (SummerCart.data0)
                {
                case 1:
                    if (SummerCart.data1)
                    {
                        SummerCart.data1=SummerCart.cfg_rom_write;
                        SummerCart.cfg_rom_write=1;
                    }
                    else
                    {
                        SummerCart.data1=SummerCart.cfg_rom_write;
                        SummerCart.cfg_rom_write=0;
                    }
                    break;
                default:
                    SummerCart.status=0x40000000;
                }
                break;
            case 'i':
                switch (SummerCart.data1)
                {
                case 1:
                    SummerCart.status=summercart_sd_init();
                    break;
                case 4:
                    SummerCart.sd_byteswap=1;
                    break;
                case 5:
                    SummerCart.sd_byteswap=0;
                case 0: break;
                default:
                    SummerCart.status=0x40000000;
                }
                break;
            case 'I':
                SummerCart.sd_sector=SummerCart.data0;
                break;
            case 's':
                SummerCart.status=summercart_sd_read();
                break;
            case 'S':
                SummerCart.status=summercart_sd_write();
                break;
            default:
                SummerCart.status=0x40000000;
            }
            break;
        case 0x04:
            SummerCart.data0=value&mask;
            break;
        case 0x08:
            SummerCart.data1=value&mask;
        }
    }
    return 0;
}