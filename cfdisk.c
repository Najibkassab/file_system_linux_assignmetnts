/*Inlcude------------------------------------------------------*/
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <inttypes.h>
#include <stdbool.h>
#include <math.h>
/*Functions Prototype-------------------------------------------*/
char *PRATNAME(int partid);
int CheckNumberofPrimaryPartition(unsigned char *buffer);
bool CheckNextEBR(unsigned char *buffer);
int EBRADDRESS(unsigned char *buffer);
bool EBRParser(unsigned char *buffer, int fd1, int extended_sector,char * argg,int index);
bool CheckGPT(unsigned char *buffer);
void parserGPT(unsigned char *buffer ,int fd1, char *argg);
char *GPTPRATNAME(uint64_t partid);
int NPP=0;
int indexE=0;
/*Global Variables----------------------------------------------*/
typedef struct {
    uint8_t status;
    uint8_t first_chs[3];
    uint8_t partitation_type;
    uint8_t last_chs[3];
    uint32_t starting_sector;
    uint32_t sector_count;
} PartiationEntry;

typedef struct {
    uint64_t GUID_PARTIION[2];
    uint64_t unique_GUID_PARTIION[2];
    uint64_t First_LBA;
    uint64_t last_LBA;
} GPTPartiationEntry;
typedef struct {
    uint8_t  signature[8];
    uint32_t Revision;
    uint32_t Headersize;
    uint32_t HeaderCRC;
    uint32_t Reserved;
    uint64_t PrimaryLBA;
    uint64_t BackupLBA;
    uint64_t FirstusableLBA;
    uint64_t LastusableLBA;
    uint64_t uniqueGUID1;
    uint64_t uniqueGUID2;
    uint64_t FirstEntryLBA;
    uint32_t NumberofEntries;
    uint32_t sizeofEntry;
    uint32_t EntryCRC;
} GPTEntry;
/*main function-------------------------------------------------*/
int main(int argc, char **argv)
{
int extended_sector=0;

    if (argc < 2)
	printf("Please Enter the name of disk partition you want to read\n");
    else {
    off_t seeksize=0;
	unsigned char buf[1024];
	unsigned char buf1[1024];
	PartiationEntry *partition_Lentry_ptr;
	int fd = open(argv[1], O_RDONLY);
	if (read(fd, buf, 1024) != -1) {
	    int NumberofPartition = 0;
	    if ((buf[510] == 0x55) && (buf[511] == 0xAA)) {
		if(!CheckGPT(&buf[512])){	
		printf("This is a MBR partiation table\n");
		PartiationEntry *partition_entry_ptr = (PartiationEntry *) & buf[446];
		NPP=CheckNumberofPrimaryPartition(&buf[450]);			//NPP=Number of Primary or extended partiions
		printf("%-10s|%-10s|%-10s|%-10s|%-10s|%-10s|%-10s|%-10s\n",
		       "Device", "Boot", "Start", "End", "Sectors", "Size", "ID", "type");

		for (int i = 0; i < NPP; i++) {
		    printf
			("%s%d |%-10c|%-10u|%-10u|%-10u|%uG\t |%-10x|%-10s\n",
			 argv[1], i + 1,
			 partition_entry_ptr[i].status == 0x80 ? '*' : ' ',
			 partition_entry_ptr[i].starting_sector,
			 partition_entry_ptr[i].starting_sector +
			 partition_entry_ptr[i].sector_count - 1,
			 partition_entry_ptr[i].sector_count,
			 (uint32_t) round((((uint64_t) partition_entry_ptr[i].sector_count * 512) / (1024 * 1024 * 1024))),
			 partition_entry_ptr[i].partitation_type, PRATNAME(partition_entry_ptr[i].partitation_type));
			 if(partition_entry_ptr[i].partitation_type==0x05){ 
			  extended_sector= partition_entry_ptr[i].starting_sector;
			  indexE=i;
	
		}
	}
	if(indexE!=0) 
		EBRParser(&buf1[partition_entry_ptr[indexE].starting_sector], fd, extended_sector,argv[1] ,NPP+1);

} else parserGPT(&buf[512],fd,argv[1]);
		}else{
		 printf("Not a MBR\n");
		 } 
	   	 }else{
	       printf("ERROR in reading: %s", argv[1]);}
	}
 }
/*----------------------------------------------------------------------*/
char *PRATNAME(int partid)
{
    char *PRAT;
    switch (partid) {
   
    default:
    case 0x83:
	PRAT = "LINUX";
	break;
    case 0x00:
	PRAT = "Free";
	break;
    case 0x05:
	PRAT = "Extended";
	break;
    case 0x01:
	PRAT = "Logical";
	break;
    case 0x0C:
	PRAT = "W95 FAT32 (LBA)";
	break;
    case 0x0E:
	PRAT = "W95 FAT16(LBA)";
	break;

    case 0x0F:
	PRAT = "Primiary FAT16";
	break;
    }
    return PRAT;
}

/*------------------------------------------------------------*/
int CheckNumberofPrimaryPartition(unsigned char *buffer){
int number=0;
while(buffer !=0 && number<4){
buffer+=16;
number++;
}
return number;

}
/*------------------------------------------------------------*/
bool EBRParser(unsigned char *buffer, int fd1, int extended_sector,char * argg , int index){
buffer+=16;
bool IsThereNextEBR=0;
unsigned char Pbuffer[512];
double NextEBRADD=0;
PartiationEntry *partition_Lentry_ptr;
int ind=0;
do{
//ind++;
//printf("i=%d\n",ind);
	   lseek(fd1, 0, SEEK_SET); 					//get back to the beginning
	   lseek(fd1, (off_t)extended_sector*512 , SEEK_SET);
if (read(fd1, Pbuffer, 512) != -1) {
 //	printf("last two bytes are 0x%x 0x%x\n", Pbuffer[510], Pbuffer[511]);
	if ((Pbuffer[510] == 0x55) && (Pbuffer[511] == 0xAA)) { 
 	       partition_Lentry_ptr= (PartiationEntry *) & Pbuffer[446];
		    printf
			("%s%d |%-10c|%-10u|%-10u|%-10u|%uG\t |%-10x|%-10s\n",
			 argg, index++,
			 partition_Lentry_ptr->status == 0x80 ? '*' : ' ',
			 partition_Lentry_ptr->starting_sector+extended_sector,
			 partition_Lentry_ptr->starting_sector + partition_Lentry_ptr->sector_count + extended_sector -1,
			 partition_Lentry_ptr->sector_count,
			 (uint32_t) (((uint64_t) partition_Lentry_ptr-> sector_count * 512) / (1024 * 1024 * 1024)),
			 partition_Lentry_ptr->partitation_type, PRATNAME(partition_Lentry_ptr->partitation_type));
		
	NextEBRADD=EBRADDRESS(&Pbuffer[470]);
	extended_sector+=partition_Lentry_ptr->sector_count+2048;   //todo check this
//	printf("Extended sector =%d\n"	,extended_sector);		
	}else
	printf("EBR signutrues not found\n"); 			  
}else
 printf("ERROR while reading EBR\n"); 
}while(CheckNextEBR(&Pbuffer[462]));


  }


/*------------------------------------------------------------*/

int EBRADDRESS(unsigned char *buffer){
for(int i=0;i<4;i++){
//printf("%d\t",buffer[i]);
}
int Address =       ((buffer[0]>>24)&0x000000ff) | 	    // move byte 3 to byte 0
                    ((buffer[1]>>8)&0x000ff0000) |    // move byte 1 to byte 2
                    ((buffer[2]<<8)&0x0000ff00)  | 	   // move byte 2 to byte 1
                    ((buffer[3])&0xff000000);  // byte 0 to byte 3
  //                  printf("address = %d\n",Address);
return Address;
}
/*------------------------------------------------------------*/
bool CheckNextEBR(unsigned char *buffer){
int j=0;
for(int i=0; i<4;i++){
//printf("buffer=%d\t",buffer[i]);
if(buffer[i]!=0) j++; 
	}
if(j>0) 
return 1;
else return 0;

}


bool CheckGPT(unsigned char *buffer){
if(buffer[0]==0x45 && buffer[1] ==0x46 && buffer[2] ==0x49)
return 1;
else
return 0;

}

/*

typedef struct {
    uint128_t Parition_table_GUID1;
    uint128_t Parition_table_GUID2;
    uint128_t unique_GUID1;
    uint128_t unique_GUID2;
    uint64_t First_LBA;
    uint64_t last_LBA;
    uint64_t flags;
} GPTPartiationEntry;




*/
void parserGPT(unsigned char *buffer ,int fd1, char *argg){
unsigned char pbuffer[128];
int it=0;
GPTEntry *partition_GPT_ptr = (GPTEntry *) & buffer[0];
printf("%-10s|%-10s|%-10s|%-10s|%-10s |%-10s\n", "Device", "Start", "End", "Sectors", "Size", "type");
while(1){
lseek(fd1, 0, SEEK_SET); 					//get back to the beginning
lseek(fd1, (off_t)1024+128*it , SEEK_SET);
if (read(fd1, pbuffer, 128) != -1) {
    GPTPartiationEntry *partition_GPT_entry_ptr= (GPTPartiationEntry *) & pbuffer[0];
    if(partition_GPT_entry_ptr->First_LBA==0) break;
    else{
if(it ==0 || it==1){
		    printf
			("%s%d |%-10lu|%-10lu|%-10lu|%-3dM       |%-15s\n",
			 argg,it+1,
			 partition_GPT_entry_ptr->First_LBA,
			 partition_GPT_entry_ptr->last_LBA,
			 (partition_GPT_entry_ptr->last_LBA)-(partition_GPT_entry_ptr->First_LBA)+1,
			 (uint32_t) ceil((((uint64_t) ((partition_GPT_entry_ptr->last_LBA+1)-(partition_GPT_entry_ptr->First_LBA)) * 512) / (1024 * 1024))),
			 GPTPRATNAME((uint64_t)(partition_GPT_entry_ptr->GUID_PARTIION[0]))					 
);
}
else{		    printf
			("%s%d |%-10lu|%-10lu|%-10lu|%.2fG    |%-10s\n",
			 argg,it+1,
			 partition_GPT_entry_ptr->First_LBA,
			 partition_GPT_entry_ptr->last_LBA,
			 partition_GPT_entry_ptr->last_LBA-(partition_GPT_entry_ptr->First_LBA)+1,
			 (float) (((partition_GPT_entry_ptr->last_LBA)-(partition_GPT_entry_ptr->First_LBA)) * 512) / (1024 * 1024 * 1024),
			 GPTPRATNAME((partition_GPT_entry_ptr->GUID_PARTIION[0])));
}
it++;			 
}
}else{
printf("ERROR while reading GPT Entry header\n");
     }
   }
 }

char *GPTPRATNAME(uint64_t partid)
{
    char *PRAT;
    switch (partid) {
   
 
    case 0x6e6f644921686148:
	PRAT = "BIOS boot";
	break;
    case 0x11d2f81fc12a7328:
	PRAT = "EFI file system";
	break;
    case 0x33445E9B2A0A0DBE:
	PRAT = "Basic Data partition";
	break;
    default:    
    case 0x477284830fc63daf:
	PRAT = "Linux filesystem data";
	break;
    }
    return PRAT;
}