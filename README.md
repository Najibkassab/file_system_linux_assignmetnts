# file_system_linux_assignmetnts(Part of yocto_project_assignments)
#first assignment</br>
# To_build you need to use -lm </br>
ex: gcc -o cfdisk cfdisk.c -lm</br>
# P.S.</br>
1.There are many limitations, one of which is that the GPT and MBR partition names are limited to the most common names. However, there should be a lookup table, but due to time constraints, I must make it this way.</br>
2. There is room for improvement and optimization, and there will be in the future, inshallah.</br>
/*first assignment--------------------------------------------*/</br>
/*Get the Number of Primary & Extended Paritions--------------*/</br>
int CheckNumberofPrimaryPartition(unsigned char *buffer) </br> 
/*Check if there any EBR Left---------------------------------*/</br>
bool CheckNextEBR(unsigned char *buffer) </br>
/*Convert Little-Endian Bytes Address-------------------------*/</br>
int EBRADDRESS(unsigned char *buffer)</br>
/*Parser EBR Header Entries-----------------------------------*/</br>
bool EBRParser(int fd1, int extended_sector, char *argg, int index) </br>
*Check the first three byte of GPT signutre-------------------*/</br>
bool CheckGPT(unsigned char *buffer)</br>
/*Parser GPT Entries Header-----------------------------------*/</br>
void parserGPT(int fd1, char *argg)</br>
/*Get GPT Parition Name---------------------------------------*/</br>
char *GPTPRATNAME(uint64_t partid)</br>
/*Get MBR Parition Name---------------------------------------*/</br>
char *PRATNAME(int partid);

# second assignment</br>
# to build the assignment gcc -o cls cls.c</br>
# to run you shall pass option and directory</br>
# e.g: ./cls -r /home</br>
# the options are -r for recursive</br>
#                 -l for listing information</br>
 #                -a for showing hidden files</br>
# of course you can combien them as you like e.g. -la   -ra -rla</br>
# for directory you can write one or more directories</br>
# P.S if you will not write any directory the command will list the current one.</br>
</br>
/*Functions I used--------------------------------------------*/</br>
1)getopt() to parses the command-line arguments </br>
2)stat() hese functions return information about a file</br>
3)The opendir() function opens a directory stream corresponding to</br>
the directory name, and returns a pointer to the directory</br>
stream. </br>
4)The readdir() function returns a pointer to a dirent structure</br>
representing the next directory entry in the directory stream</br>
pointed to by dirp</br>
5)The closedir() function closes the directory stream associated</br>
with dirp.</br>
