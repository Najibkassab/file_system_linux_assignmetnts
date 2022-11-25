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
char *filePermStr (mode_t perm);
char getfiletype (struct stat *sb);
int PrintDIREntries (char *path, uint8_t flag);
int lsRecursive (char *name, uint8_t flag);
int listfiles (char *path);
int printFiles (char *name, uint8_t flag);
_Bool ISHiddenfile (char *filename);

/*Global Variables----------------------------------------------*/
#define STR_SIZE sizeof("rwxrwxrwx")
#define FP_SPECIAL 1
//COLORS definatio to color the files and directories
#define KGRN  "\x1B[32m"	//for hidden files
#define KBLU  "\x1B[34m"	//for directories
#define KWHT  "\x1B[37m"	//for normal files
/* FLAGS       */
#define NO_FLAG 0x00
#define HIDDEN_FLAG 0x01	//to show hidden files
#define LIST_FLAG 0x02		//to list details about files & directories
#define RECURSIVE_FLAG 0x04	//to list files & directories recursivly

char *Month[] =
  { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JYU", "AUG", "SPT",
  "OCT", "NOV", "DEC" };

/*main function-------------------------------------------------*/
int main (int argc, char **argv)
{
  uint8_t flag;
  int opt;
  int indexx = 0;

  if (argc == 2)
    {
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
      if (flag == 0)
	PrintDIREntries (argv[1], flag);
      else
	PrintDIREntries (".", flag);

    }

  else if (argc > 2)
    {
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

      for (int i = 0; i < argc - 1 - indexx; i++)
	{
	  printf ("Ls for %s\n", argv[1 + indexx + i]);
	  PrintDIREntries (argv[1 + indexx + i], flag);
	}
    }
  else
    {
      printFiles (".", 0);
    }
  return 0;
}

int PrintDIREntries (char *path, uint8_t flag)
{
printf("flag=%d/n",flag);

  switch (flag)
    {
    default:
    case 0x00:
    case 0x01:			
    case 0x02:
    case 0x03:
      printFiles (path, flag);
      break;
    case 0x04:			
    case 0x05:
    case 0x06:
    case 0x07:			
    lsRecursive (path, flag);
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

char *filePermStr (mode_t perm)
{
  static char str[STR_SIZE];
  snprintf (str, STR_SIZE, "%c%c%c%c%c%c%c%c%c",
	    (perm & S_IRUSR) ? 'r' : '-', 
	    (perm & S_IWUSR) ? 'w' : '-',
	    (perm & S_IXUSR) ? 'x' : '-',
	    (perm & S_IRGRP) ? 'r' : '-',
	    (perm & S_IWGRP) ? 'w' : '-',
	    (perm & S_IXGRP) ? 'x' : '-',
	    (perm & S_IROTH) ? 'r' : '-',
	    (perm & S_IWOTH) ? 'w' : '-',
	    (perm & S_IXOTH) ? 'x' : '-');

  return str;
}


int listdetials (char *file)
{
  
  struct stat mystat;
  if (stat (file, &mystat) < 0)
    {
      perror ("stat");
      return -1;
    }
  printf ("(%c%s)\n", getfiletype (&mystat), filePermStr (mystat.st_mode));
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

int  lsRecursive (char *name, uint8_t flag)
{
  DIR *dir;
  char path[1024];
  struct dirent *entry;
  
  printFiles (name, flag);
  
  if (!(dir = opendir (name))){
  printf("Cannot open this folder ->%s\n",name);
    return -1 ;
    }
  while ((entry = readdir (dir)) != NULL)
    {
      if (strcmp (entry->d_name, ".") == 0
	  || strcmp (entry->d_name, "..") == 0)
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


int printFiles (char *name, uint8_t flag)
{

  DIR *dir;
  char path[1024];
  struct dirent *entry;
  uint8_t Hidden_flag = flag & 0x01;
  uint8_t list_flag = flag & 0x02;

  if (!(dir = opendir (name))){
  printf("Cannot open this folder ->%s\n",name);
    return -1;
}
  while ((entry = readdir (dir)) != NULL)
    {
      if (ISHiddenfile (entry->d_name))
	{
	  if (Hidden_flag)
	    {
	      printf ("%s%s\t", KGRN, entry->d_name);
	      if (list_flag)
		{
		snprintf (path, sizeof (path), "%s/%s", name, entry->d_name);
		chdir(path);
		  listdetials (path);
		}
	      continue;
	    }
	}
      if (entry->d_type == DT_DIR)
	{

	  if (!ISHiddenfile (entry->d_name))
	    {
	      printf ("%s%s\t", KBLU, entry->d_name);
	      if (list_flag){
	      		snprintf (path, sizeof (path), "%s/%s", name, entry->d_name);
		listdetials (path);
	    }
	    }
	}
      else
	{

	  if (!ISHiddenfile (entry->d_name))
	    {
	      printf ("%s%s\t", KWHT, entry->d_name);
	      if (list_flag)
		{
		snprintf (path, sizeof (path), "%s/%s", name, entry->d_name);
		 listdetials (path);
		}
	    }
	}
    }

  closedir (dir);
  printf ("%s\n", KWHT);
}



_Bool ISHiddenfile (char *filename)
{

  if ((strcmp (filename, ".") == 0 || strcmp (filename, "..") == 0
       || filename[0] == '.'))
    return 1;
  else
    return 0;
}
