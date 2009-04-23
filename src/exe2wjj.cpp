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
        char new_file[MAX_FILE_LEN]= {0};
        if (!strcasecmp(pExt,".exe"))
        {
            //sprintf(new_file,"%s%s",dirname,file);
            sprintf(new_file,"%s",file);
            pExt = new_file + strlen(new_file);
            *(pExt++) = '.';
            *(pExt++) = 'w';
            *(pExt++) = 'j';
            *(pExt++) = 'j';
            char sys_cmd[MAX_FILE_LEN] = {0};
            sprintf(sys_cmd,"ATTRIB -S -R -H \"%s%s\"\n",dirname, file);
            system(sys_cmd);
            
            sprintf(sys_cmd,"rename \"%s%s\" \"%s\"\n",dirname, file,new_file);
            printf("%s\n",sys_cmd);fflush(stdout);
            system(sys_cmd);

            system(sys_cmd);
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


