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
#define VID "raspivid -p 500,100,640,480 -w 640 -h 480 -t 60000 -o "
#define FGB 9000000

const char *path = DIRPATH;

int main(int argc, char**argv)
{
	int i, j;
	int result;
	int count;
	int idx;
	int ret;
	long rnd;
	struct statfs s;
	long used_blocks;
	long used_percent;
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

	if (argc<2) {
        printf("Too few arguments\nUsage: ./finfo mount_point\n");
        return 1;
    }
    /*인자로 주어진 파일/디렉토리의 마운트된 곳의 정보를 가져옵니다.*/
    if ( statfs((const char*)argv[1], &s) !=0) {
        perror("statfs");
        return 1;
    }
     if (s.f_blocks >0) 
     {
        long free_percent;
        used_blocks = s.f_blocks - s.f_bfree;
        if (used_blocks ==0)
            used_percent = 0;
        else {
            used_percent = (long)
                (used_blocks * 100.0 / (used_blocks + s.f_bavail) + 0.5);
        }
        if (s.f_bfree ==0)
            free_percent = 0;
        else {
            free_percent = (long)
                (s.f_bavail * 100.0 / (s.f_blocks) + 0.5);
        }
        printf("blocks %ld%% used(%ld bytes %ldK)\n",
                used_percent,
                used_blocks * s.f_bsize,
                (long) (used_blocks * (s.f_bsize/(double)1024))
                );
        printf("blocks %ld%% remain(%ld bytes %ldK\n",
                free_percent,
                s.f_bavail * s.f_bsize,
                (long) (s.f_bavail * (s.f_bsize/(double)1024))
                );
     }
	rnd = (long)(s.f_bavail*(s.f_bsize/(double)1024));	

	if(rnd < FGB)
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

		
	
	}
       		
	}
	return 0;
}
