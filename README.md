# file_system_linux_assignmetnts(Part of yocto_project_assignments)
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
/*Get MBR Parition Name--------------------------------- -----*/</br>
char *PRATNAME(int partid);

