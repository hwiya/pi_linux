#include<stdio.h>
#include<unistd.h>
#include <libgen.h>
#include<stdlib.h>
#include<time.h>
#include<sys/time.h>
#include<string.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<errno.h>
#include<sys/vfs.h>
#include<dirent.h>

#define DEBUG
#define BUFSIZE 100
#define DIRPATH "/home/pi/user/blackbox/"
#define VID "raspivid -w 640 -h 480 -t 60000 -o "
#define FGB 5200000

const char *path = DIRPATH;
const char *MMOUNT = "/proc/mounts";

struct f_size
{
    long blocks;
    long avail; 
};

typedef struct _mountinfo 
{
    FILE *fp;                // 파일 스트림 포인터    
    char devname[80];        // 장치 이름
    char mountdir[80];        // 마운트 디렉토리 이름
    char fstype[12];        // 파일 시스템 타입
    struct f_size size;        // 파일 시스템의 총크기/사용율 
} MOUNTP;

MOUNTP *dfopen()
{
    MOUNTP *MP;

    // /proc/mounts 파일을 연다.
    MP = (MOUNTP *)malloc(sizeof(MOUNTP));
    if(!(MP->fp = fopen(MMOUNT, "r")))
    {
        return NULL;
    }
    else
        return MP;
}

MOUNTP *dfget(MOUNTP *MP)
{
    char buf[256];
    struct statfs lstatfs;
    struct stat lstat; 
    int is_root = 0;

    // /proc/mounts로 부터 마운트된 파티션의 정보를 얻어온다.
    fgets(buf, 255, MP->fp);
           is_root = 0;
        sscanf(buf, "%s%s%s",MP->devname, MP->mountdir, MP->fstype);
         if (strcmp(MP->mountdir,"/") == 0) is_root=1;
        if (stat(MP->devname, &lstat) == 0 || is_root)
        {
            if (strstr(buf, MP->mountdir) && S_ISBLK(lstat.st_mode) || is_root)
            {
                // 파일시스템의 총 할당된 크기와 사용량을 구한다.        
                statfs(MP->mountdir, &lstatfs);
                MP->size.blocks = lstatfs.f_blocks * (lstatfs.f_bsize/1024); 
                MP->size.avail  = lstatfs.f_bavail * (lstatfs.f_bsize/1024); 
                return MP;
            }
        }
    
    rewind(MP->fp);
    return NULL;
}

int dfclose(MOUNTP *MP)
{
    fclose(MP->fp);
}


int main(int argc, char**argv)
{
	int i, j;
	int result;
	int count;
	int idx;
	time_t UTCtime;
	struct tm *tm;
	char dirName[BUFSIZE];
	char dirDel[BUFSIZE];
	char dirVid[BUFSIZE];
	char temp[BUFSIZE];
	char temp2[BUFSIZE];
	struct dirent **namelist;
	
	while(1)
	{
	//1.read current time from kernel
	time(&UTCtime); 
	
	//2. transform struct tm	
	tm = localtime(&UTCtime);

	strftime(temp, sizeof(temp), "%Y%m%e%H", tm);

	sprintf(dirName, DIRPATH);
	strcat(dirName, temp);
	
	if((result = mkdir(dirName,0777)) == -1)
	{
		if(errno != EEXIST)
		{	
			perror("mkdir error");
			return 1;
		}

		printf("%s is exist\n", dirName);

	}
	
	strftime(temp2, sizeof(temp2), "/%Y%m%e_%H%M%S.h264", tm);

	sprintf(dirVid, VID);
	strcat(dirVid, dirName);
	strcat(dirVid, temp2);
	system(dirVid);	

	
	MOUNTP *MP;
    	if ((MP=dfopen()) == NULL)
    	{
        	perror("error");
        	return 1;
   	}



        dfget(MP);
        printf("available capacity : %10lu\n", MP->size.avail);
        sleep(1);
	
	if((MP->size.avail) < FGB)
	{
		if((count = scandir(path, &namelist, NULL, alphasort)) == -1)
		{
			fprintf(stderr, "%s Directory Scan Error : %s\n", path, strerror(errno));
			return 1;
		}
	
		for(idx = 0; idx < count; idx++)
		{
			printf("%s\n", namelist[idx]->d_name);
		}
		
		for(idx = 0; idx < count; idx++)
		{
			free(namelist[idx]);
		}
	
		sprintf(dirDel, "rm -rf ");
		strcat(dirDel, DIRPATH);
		strcat(dirDel, namelist[2]->d_name);
		printf("Old Folder Del - %s\n", namelist[2]->d_name);
		system(dirDel);

		free(namelist[idx]);
	}
	}
	return 0;
}
