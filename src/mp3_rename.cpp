/*
 *
 * Windows version batch file name changes utility.
 * Function: change all .exe files in directory to .exe.wjj file name.
 * Usage: apps my_directory
 * 
 */

#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <dirent.h> 
#include <sys/types.h> 
#include <sys/stat.h> 

#define MAX_FILE_LEN 1024*2
void dir_scan(char *path, char *file); 
void get_mp3_name(char* src_mp3_name,char* new_mp3_name);

void dump_hex(char *pBuf,unsigned long length);
int count = 0; 

int main(int argc, char *argv[]) 
{ 
    struct stat s; 

    if(argc != 2){ 
        printf("usage: %s my_directory_name\n",argv[0]);
        printf("one directory requried\n"); 
        exit(1); 
    } 
    if(stat(argv[1], &s) < 0){ 
        printf("stat error %s\n",argv[1]); 
        exit(2); 
    } 
    if(!S_ISDIR(s.st_mode)){ 
        printf("%s is not a directory name\n", argv[1]); 
        exit(3); 
    } 

    dir_scan("", argv[1]); 

    printf("total: %d files\n", count); 

    exit(0); 
} 

void dir_scan(char *path, char *file) 
{ 
    struct stat s; 
    DIR   *dir; 
    struct dirent *dt; 
    char dirname[MAX_FILE_LEN]; 

    memset(dirname, 0, 50*sizeof(char)); 
    strcpy(dirname, path); 

    if(stat(file, &s) < 0){ 
        printf("stat error %s%s\n",dirname,file); fflush(stdout);
        dump_hex(file,strlen(file));
    } 

    if(S_ISDIR(s.st_mode)){ 
        strcpy(dirname+strlen(dirname), file); 
        if (*(dirname + strlen(dirname) - 1) != '\\')
        {
            strcpy(dirname+strlen(dirname), "\\"); 
        }
        if((dir = opendir(file)) == NULL){ 
            printf("opendir %s\\%s error\n"); 
            exit(4); 
        } 
        if(chdir(file) < 0) { 
            printf("chdir error\n"); 
            exit(5); 
        } 
        while((dt = readdir(dir)) != NULL){ 
            if(dt->d_name[0] == '.'){ 
                continue; 
            } 

            dir_scan(dirname, dt->d_name); 
        } 
        if(chdir("..") < 0){ 
            printf("chdir error\n"); 
            exit(6); 
        } 
    }else{ 
        char* pExt = file+strlen(file)- 4;
        char src_path_name[MAX_FILE_LEN] = {0};
        char new_file[MAX_FILE_LEN]= {0};
        printf("src file: %s\n",file);
        if (!strcasecmp(pExt,".mp3"))
        {
            //sprintf(new_file,"%s%s",dirname,file);
            sprintf(new_file,"%s",file);
            pExt = new_file + strlen(new_file);
            *(pExt++) = '.';
            *(pExt++) = 'w';
            *(pExt++) = 'j';
            *(pExt++) = 'j';
            char sys_cmd[MAX_FILE_LEN] = {0};
            sprintf(src_path_name,"%s%s",dirname,file);
            get_mp3_name(src_path_name,new_file);      
            printf("%s\n",sys_cmd);fflush(stdout);
            if (strcmp(new_file,""))
            {
                sprintf(sys_cmd,"rename \"%s%s\" \"%s\"\n",dirname, file,new_file);
                system(sys_cmd);
            }
        }
        count++; 
    } 
}

void dump_hex(char *pBuf,unsigned long length)
{
    unsigned long i = 0;
    unsigned long j = 0;

    printf("Hex dump\n");
    printf("====================\n");
    for (i=0;i<length;i++)
    {
        int lineLen = (i + 16 < length)?16:(length - i); 
        for (j=0;j<lineLen;j++)
        {
            printf("%02x ",(unsigned char)*(pBuf+i+j));
        }

        for (j=0;j<16 - lineLen;j++)
        {
            printf("   ");
        }

        printf(" ");

        for (j=0;j<lineLen;j++)
        {
            char ch = *(pBuf+i+j);
            printf("%c",(ch > 31 && ch < 127)? ch : '.');
        }
        i += j;
        printf("\n");
    }
    fflush(stdout);
    printf("====================\n");
}
/*
 * 11172-3 audio header format
header()
{
    syncword           12    bits    bslbf
    ID                   1    bit    bslbf
    layer               2    bits    bslbf
    protection_bit     1    bit    bslbf

    bitrate_index       4    bits    bslbf
    sampling_frequency 2    bits    bslbf
    padding_bit           1    bit    bslbf
    private_bit           1    bit    bslbf

    mode               2    bits    bslbf
    mode_extension       2    bits    bslbf
    copyright           1    bit    bslbf
    original/home       1    bit    bslbf
    emphasis           2    bits    bslbf
}
*/

typedef struct mp3_hdr_t {
    unsigned long syncword;         // 12 bits   bslbf
    unsigned char ID;               // 1  bit    bslbf
    unsigned char layer;            // 2  bits   bslbf
    unsigned char protection_bit;   // 1  bit    bslbf

    unsigned char bitrate_index;     // 4    bits   bslbf
    unsigned char sampling_frequency;// 2    bits   bslbf
    unsigned char padding_bit;       // 1    bit    bslbf
    unsigned char private_bit;       // 1    bit    bslbf

    unsigned char mode;               // 2    bits    bslbf
    unsigned char mode_extension;     // 2    bits    bslbf
    unsigned char copyright;          // 1    bit     bslbf
    unsigned char original;           // 1    bit     bslbf
    unsigned char emphasis;           // 2    bits    bslbf
} MP3_HDR;

void dump_mp3_hdr(MP3_HDR mp3_hdr)
{
    printf("dump mp3 header:\n");
    printf("====================\n");
    printf("syncword: 0x%x\n"         ,mp3_hdr.syncword);        
    printf("ID: 0x%x\n"               ,mp3_hdr.ID);              
    printf("layer: 0x%x\n"            ,mp3_hdr.layer);            
    printf("protection_bit: 0x%x\n"   ,mp3_hdr.protection_bit);   

    printf("bitrate_index: 0x%x\n"    ,mp3_hdr.bitrate_index);    
    printf("sampling_frequency: 0x%x\n"  ,mp3_hdr.sampling_frequency);
    printf("padding_bit: 0x%x\n"      ,mp3_hdr.padding_bit);      
    printf("private_bit: 0x%x\n"      ,mp3_hdr.private_bit);      

    printf("mode: 0x%x\n"             ,mp3_hdr.mode);             
    printf("mode_extension: 0x%x\n"   ,mp3_hdr.mode_extension);   
    printf("copyright: 0x%x\n"        ,mp3_hdr.copyright);        
    printf("original: 0x%x\n"         ,mp3_hdr.original);         
    printf("emphasis: 0x%x\n"         ,mp3_hdr.emphasis);         
    printf("====================\n");
}

void get_mp3_name(char* src_mp3_name,char* new_mp3_name)
{
    FILE* fp = NULL;
    unsigned char pBuf[4];
    MP3_HDR mp3_hdr = {0};
    unsigned int layer[] = {0,3,2,1};
    unsigned int bitrate[][3] = 
    {
        //bit_rate_index Layer I Layer II Layer III
        /*'0000'*/ 0,              0,              0 ,
        /*'0001'*/ 32, /*kbit/s*/ 32, /*kbit/s*/ 32, /*kbit/s*/
        /*'0010'*/ 64, /*kbit/s*/ 48, /*kbit/s*/ 40, /*kbit/s*/
        /*'0011'*/ 96, /*kbit/s*/ 56, /*kbit/s*/ 48, /*kbit/s*/
        /*'0100'*/ 128, /*kbit/s*/ 64, /*kbit/s*/ 56, /*kbit/s*/
        /*'0101'*/ 160, /*kbit/s*/ 80, /*kbit/s*/ 64, /*kbit/s*/
        /*'0110'*/ 192, /*kbit/s*/ 96, /*kbit/s*/ 80, /*kbit/s*/
        /*'0111'*/ 224, /*kbit/s*/ 112, /*kbit/s*/ 96, /*kbit/s*/
        /*'1000'*/ 256, /*kbit/s*/ 128, /*kbit/s*/ 112, /*kbit/s*/
        /*'1001'*/ 288, /*kbit/s*/ 160, /*kbit/s*/ 128, /*kbit/s*/
        /*'1010'*/ 320, /*kbit/s*/ 192, /*kbit/s*/ 160, /*kbit/s*/
        /*'1011'*/ 352, /*kbit/s*/ 224, /*kbit/s*/ 192, /*kbit/s*/
        /*'1100'*/ 384, /*kbit/s*/ 256, /*kbit/s*/ 224, /*kbit/s*/
        /*'1101'*/ 416, /*kbit/s*/ 320, /*kbit/s*/ 256, /*kbit/s*/
        /*'1110'*/ 448, /*kbit/s*/ 384, /*kbit/s*/ 320, /*kbit/s*/
        /*'1111'*/ 0,   /*kbit/s*/ 0,   /*kbit/s*/ 0    /*kbit/s*/
    };
    unsigned int sample_rate[] = { 44100,48000,32000,0};
    const char *channel[] = {"stereo","jstereo","dual","mono"};

    char* short_src_file_name = strrchr(src_mp3_name,'\\') + 1;

    fp = fopen(src_mp3_name,"rb");
    if (fp == NULL)
    {
        printf("read file %s error\n",src_mp3_name);
        return;
    }
    fread(pBuf,1,4,fp);

    //sync word
    mp3_hdr.syncword = (pBuf[0] << 4) + ((pBuf[1] >> 4) & 0x0F);
    mp3_hdr.ID = (pBuf[1] & 0x0F) >> 3;
    mp3_hdr.layer = (pBuf[1] & 0x06) >> 1;
    mp3_hdr.protection_bit = (pBuf[1] & 0x01);

    mp3_hdr.bitrate_index = (pBuf[2] >> 4) & 0x0F;
    mp3_hdr.sampling_frequency = (pBuf[2] >> 2) & 0x03;
    mp3_hdr.padding_bit = (pBuf[2] >> 1) & 0x01;
    mp3_hdr.private_bit = (pBuf[2]) & 0x01;

    mp3_hdr.mode = (pBuf[3] >> 6) & 0x03;
    mp3_hdr.mode_extension = (pBuf[3] >> 4) & 0x03;
    mp3_hdr.copyright = (pBuf[3] >> 3) & 0x01;
    mp3_hdr.original = (pBuf[3] >> 2) & 0x01;
    mp3_hdr.emphasis = (pBuf[3]) & 0x03;

    dump_mp3_hdr(mp3_hdr);
    struct stat s;
    int index = 0;
    do {
        sprintf(new_mp3_name,"L%d_%dK_%.1f_%s_%04d.mp3",
                layer[mp3_hdr.layer],
                bitrate[mp3_hdr.bitrate_index][layer[mp3_hdr.layer] - 1],
                sample_rate[mp3_hdr.sampling_frequency]/1000.0,
                channel[mp3_hdr.mode],
                index
               );
        printf("%s == %s\n",src_mp3_name + strlen(src_mp3_name) - strlen(new_mp3_name),new_mp3_name);
        if (!strcmp(short_src_file_name,new_mp3_name))
        {
            printf("the same file,break\n");
            strcpy(new_mp3_name,"");
            break;
        }

        if(stat(new_mp3_name, &s) < 0){ 
            printf("new name: %s\n",new_mp3_name);
            break;
        }
        index++;
    }while(1);

    fclose(fp);
}
