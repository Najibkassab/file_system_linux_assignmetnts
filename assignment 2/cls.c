/*Inlcude------------------------------------------------------*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdbool.h>
#include <dirent.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
/*Functions Prototype-------------------------------------------*/
char *GetUSERNAME (uid_t uid);
char *GetGROUPNAME (gid_t gid);
char *GetTime (time_t time, int *Day, int *hour, int *minute);
void displayStatInfo (struct stat *sb);
char *filePermStr (mode_t perm, int flags);
char getfiletype (struct stat *sb);
int PrintDIREntries (char *path, uint8_t flag);
void lsRecursive (char *name, uint8_t flag);
int listfiles (char *path);
int printFiles(char *name,uint8_t flag);
_Bool ISHiddenfile(char * filename );
#define STR_SIZE sizeof("rwxrwxrwx")
#define FP_SPECIAL 1
/*Global Variables----------------------------------------------*/
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define MAX_PATH 1024
/*     FLAGS                                      */
#define NO_FLAG 0x00
#define HIDDEN_FLAG 0x01
#define LIST_FLAG 0x02
#define RECURSIVE_FLAG 0x04
     int
       indexx = 1;
     char *
       Month[] =
       { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JYU", "AUG", "SPT",
       "OCT", "NOV", "DEC"
     };

/*main function-------------------------------------------------*/
int
main (int argc, char **argv)
{
  uint8_t flag;
  int opt;
  int indexx = 0;

  if(argc==2){
  while ((opt = getopt (argc, argv, ":rla")) != -1)
    {
      switch (opt)
	{
	case 'a':
	  flag |= HIDDEN_FLAG;
	  break;
	case 'l':
	  flag |= LIST_FLAG;
	  break;
	case 'r':
	  flag = RECURSIVE_FLAG;
	  break;
	case '?':
	  printf ("unknown option: %c\n", optopt);
	  flag = NO_FLAG;
	  break;
      }
    }
if(flag==0)
  PrintDIREntries (argv[1], flag);
 else
 PrintDIREntries (".", flag);
  
  }
  
  else if(argc>2){
  while ((opt = getopt (argc, argv, ":rla")) != -1)
    {
      switch (opt)
	{
	case 'a':
	  flag |= HIDDEN_FLAG;
	  break;
	case 'l':
	  flag |= LIST_FLAG;
	  break;
	case 'r':
	  flag = RECURSIVE_FLAG;
	  break;
	case '?':
	  printf ("unknown option: %c\n", optopt);
	  flag = NO_FLAG;
	  break;
      }
    }
  
  if (flag != 0)
    indexx++;
    
for(int i=0;i<argc-1-indexx;i++){
  printf("Ls for %s\n",argv[1 + indexx+i]);
  PrintDIREntries (argv[1 + indexx+i], flag);  
  }
  } 
  else {
  printFiles(".",0);
}
  return 0;
}

int
PrintDIREntries (char *path, uint8_t flag)
{

  DIR *dfd;
  if ((dfd = opendir (path)) == NULL)
    {
      printf ("can't open this path -->%s\n", path);
      return -1;
    }
  struct dirent *dp;
  struct stat mystat;
  switch (flag)
    {
    default:
    case 0x00:			//show files 
      printFiles(path,flag);
      break;
    case 0x01:			//show hidden files
     printFiles (path,flag);
      break;
    case 0x02:			//list all details
     printFiles(path,flag);
      break;
    case 0x03:			//list all details with hidden files
      printFiles(path,flag);
      break;
    case 0x04:			//Recursive
    lsRecursive(path,flag);
      break;
    case 0x05:			//Recursive+hidden files
    lsRecursive(path,flag);
      break;
    case 0x06:			//list all details+Recursive
    lsRecursive(path,flag);
      break;
    case 0x07:			//Hidden LIST all details Recusive
    lsRecursive(path,flag);
      break;
    }


}

char *GetUSERNAME (uid_t uid)
{
  struct passwd *mypasswd;
  mypasswd = getpwuid (uid);
  return mypasswd->pw_name;
}

char *GetGROUPNAME (gid_t gid)
{
  struct group *mygroup = getgrgid (gid);
  return mygroup->gr_name;
}

char *GetTime (time_t time, int *Day, int *hour, int *minute)
{

  struct tm *mytime = gmtime (&time);
  *Day = mytime->tm_mday;
  *hour = mytime->tm_hour;
  *minute = mytime->tm_min;
  return Month[mytime->tm_mon];
}

char getfiletype (struct stat *sb)
{
  char res;
  switch (sb->st_mode & S_IFMT)
    {
    case S_IFREG:
      res = '-';
      break;
    case S_IFDIR:
      res = 'd';
      break;
    case S_IFCHR:
      res = 'c';
      break;
    case S_IFBLK:
      res = 'b';
      break;
    case S_IFLNK:
      res = 'l';
      break;
    case S_IFIFO:
      res = 'p';
      break;
    case S_IFSOCK:
      res = 's';
      break;
    }

  return res;
}

char *filePermStr (mode_t perm, int flags)
{
  static char str[STR_SIZE];

/* If FP_SPECIAL was specified, we emulate the trickery of ls(1) in
returning set-user-ID, set-group-ID, and sticky bit information in
the user/group/other execute fields. This is made more complex by
the fact that the case of the character displayed for this bits
depends on whether the corresponding execute bit is on or off. */

  snprintf (str, STR_SIZE, "%c%c%c%c%c%c%c%c%c",
	    (perm & S_IRUSR) ? 'r' : '-', (perm & S_IWUSR) ? 'w' : '-',
	    (perm & S_IXUSR) ?
	    (((perm & S_ISUID) && (flags & FP_SPECIAL)) ? 's' : 'x') :
	    (((perm & S_ISUID) && (flags & FP_SPECIAL)) ? 'S' : '-'),
	    (perm & S_IRGRP) ? 'r' : '-', (perm & S_IWGRP) ? 'w' : '-',
	    (perm & S_IXGRP) ?
	    (((perm & S_ISGID) && (flags & FP_SPECIAL)) ? 's' : 'x') :
	    (((perm & S_ISGID) && (flags & FP_SPECIAL)) ? 'S' : '-'),
	    (perm & S_IROTH) ? 'r' : '-', (perm & S_IWOTH) ? 'w' : '-',
	    (perm & S_IXOTH) ?
	    (((perm & S_ISVTX) && (flags & FP_SPECIAL)) ? 't' : 'x') :
	    (((perm & S_ISVTX) && (flags & FP_SPECIAL)) ? 'T' : '-'));

  return str;
}


int listdetials(struct dirent *file){
struct stat mystat;
  if (stat (file->d_name, &mystat) < 0)
    {
      perror ("stat");
      return -1;
    }
  printf ("(%c%s)\n", getfiletype (&mystat),
	  filePermStr (mystat.st_mode, 0));
  if (mystat.st_mode & (S_ISUID | S_ISGID | S_ISVTX))
    {
      printf ("special bits set:\t\t%s%s%s\n",
	      (mystat.st_mode & S_ISUID) ? "set-UID " : "",
	      (mystat.st_mode & S_ISGID) ? "set-GID " : "",
	      (mystat.st_mode & S_ISVTX) ? "sticky " : "");
    }

  printf ("Inode Number:\t\t\t%lu\n", mystat.st_ino);
  printf ("User ID:\t\t\t%s\n", GetUSERNAME (mystat.st_uid));
  printf ("Group ID:\t\t\t%s\n", GetGROUPNAME (mystat.st_gid));
  printf ("Hard Links:\t\t\t%lu \n", mystat.st_nlink);
  printf ("File size:\t\t\t%lld bytes\n", (long long) mystat.st_size);
  printf ("Last file access:\t\t%s", ctime (&mystat.st_atime));
  printf ("Last file modification:\t\t%s", ctime (&mystat.st_mtime));
  printf ("Last status change:\t\t%s", ctime (&mystat.st_ctime));
  printf ("=======================================================\n");



}

void lsRecursive (char *name, uint8_t flag)
{
  DIR *dir;
  char path[1024];
  struct dirent *entry;
   
   printFiles(name,flag);
  
    if (!(dir = opendir (name)))
    return;
  while ((entry = readdir (dir)) != NULL)
    {
      if (strcmp (entry->d_name, ".") == 0 || strcmp (entry->d_name, "..") == 0)
	  continue;
	
      if (entry->d_type == DT_DIR)
	{
	  snprintf (path, sizeof (path), "%s/%s", name, entry->d_name);
	  printf ("%s:\n", path);
	  lsRecursive (path, flag);
	}
    }   
  closedir (dir);
}


int printFiles(char *name,uint8_t flag){



   DIR *dir;
  char path[1024];
  struct dirent *entry;
 uint8_t Hidden_flag= flag & 0x01;
 uint8_t list_flag= flag & 0x02;
 
 
  if(chdir(name)<0) perror("chdir");
  
  if (!(dir = opendir (name)))
    return -1;

  while ((entry = readdir (dir)) != NULL)
    {
      if (ISHiddenfile(entry->d_name))
	{
	  if (Hidden_flag){	  
	    printf ("%s%s\t",KBLU, entry->d_name);
	   if(list_flag)
	    {
	    listdetials(entry);
	    }
	    continue;
	  }
	}
	if (entry->d_type == DT_DIR){
	   
	   if(!ISHiddenfile(entry->d_name))
	    {
	    printf ("%s%s\t",KBLU, entry->d_name);
	    if(list_flag)
	    listdetials(entry);
	    }
	   }
	 else	{

	   if(!ISHiddenfile(entry->d_name))
	    {
	    	 printf ("%s%s\t",KWHT, entry->d_name);
	   if(list_flag)
	    {
	    listdetials(entry);
	    }
	    }
	 }
    }
    
  closedir (dir); 
  printf("%s\n",KWHT);	
}



_Bool ISHiddenfile(char * filename ){

if((strcmp (filename, ".") == 0 || strcmp (filename, "..") == 0 || filename[0]=='.')) return 1;
else return 0;
}
