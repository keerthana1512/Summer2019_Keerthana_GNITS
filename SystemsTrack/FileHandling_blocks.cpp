#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#define DISK_SIZE (100*1024*1024)
#define BLOCK_SIZE (16*1024)
#define BLOCKS(size) ((size/BLOCK_SIZE)+(!!(size%BLOCK_SIZE)))
#define START_FILE_OFFSET (2*BLOCK_SIZE+1)
#define ARRAY_OFFSET (BLOCK_SIZE+1)
#define META_OFFSET (12+1)
#define CHECKER (0x444E524D)
#define FILES (32)
struct fileinfo
{
	char name[20];
	int blockno;
	int block_count;
	int length;
};
struct metaData
{
	unsigned int magicNumber;
	int no_of_files;
	int free_blocks;
	struct fileinfo fileDetails[FILES];
	char bitVector[FILES * 5];
};
int blockRead(int blockNo, void *buffer)
{
	FILE *fptr = fopen("hardDisk.hdd", "rb+");
	if (fptr == NULL)
	{
		printf("file not found");
		return -1;
	}
	fseek(fptr, blockNo*BLOCK_SIZE, SEEK_SET);
	fread(buffer, BLOCK_SIZE, 1, fptr);
	fclose(fptr);
	return 1;
}
int blockWrite(int blockNo, void *buffer)
{
	FILE *fptr = fopen("hardDisk.hdd", "rb+");
	if (fptr == NULL)
	{
		printf("file not found");
		return -1;
	}
	fseek(fptr, blockNo*BLOCK_SIZE, SEEK_SET);
	fwrite(buffer, BLOCK_SIZE, 1, fptr);
	fclose(fptr);
	return 1;
}
void format()
{
	struct metaData Metadata;
	Metadata.magicNumber = CHECKER;
	Metadata.free_blocks = BLOCKS(DISK_SIZE);
	Metadata.no_of_files = 0;

	for (int i = 0; i < FILES * 5; i++)
	{
		Metadata.bitVector[i] = '0';
	}

	void *buffer = malloc(sizeof(char)*BLOCK_SIZE);
	memcpy(buffer, &Metadata, sizeof(struct metaData));
	blockWrite(0, buffer);
	free(buffer);

}
int fileSize(char *filename)
{
	FILE *fptr = fopen(filename, "rb+");
	if (fptr == NULL)
	{
		printf("file not found");
		return -1;
	}
	fseek(fptr, 0L, SEEK_END);
	int size = ftell(fptr);
	fclose(fptr);
	return size;

}
int findStartBlockNo(int blockcount, char bitvector[])
{

	int i = 0;
	int count = 0;
	while (i < BLOCKS(DISK_SIZE) - 2 && count<blockcount)
	{
		if (bitvector[i] == '1')
			count = 0;
		else
			count++;
		i++;
	}
	if (count == blockcount)
		return i - blockcount + 1;
	return -1;
}
void addFileDetails(char *filename, struct metaData metadata, int blocks, int blockno, int length)
{

	metadata.free_blocks -= blocks;

	strcpy(metadata.fileDetails[metadata.no_of_files + 1].name, filename);
	metadata.fileDetails[metadata.no_of_files + 1].blockno = blockno;
	metadata.fileDetails[metadata.no_of_files + 1].block_count = blocks;
	metadata.fileDetails[metadata.no_of_files + 1].length = length;
	metadata.magicNumber = CHECKER;
	metadata.no_of_files += 1;
	for (int i = blockno; i < blocks + blockno; i++)
	{
		metadata.bitVector[i] = '1';
	}
	void *buffer = malloc(sizeof(char)*BLOCK_SIZE);
	memcpy(buffer, &metadata, sizeof(struct metaData));
	blockWrite(0, buffer);


}

int copy_to_disk(char *source, char *destination)
{
	int length = fileSize(source);
	int blocks = BLOCKS(length);
	struct metaData Metadata;
	void *buffer = malloc(sizeof(char)*BLOCK_SIZE);
	blockRead(0, buffer);
	memcpy(&Metadata, buffer, sizeof(struct metaData));
	int blockno = findStartBlockNo(blocks, Metadata.bitVector);
	if (blockno == -1)
	{
		printf("disk size is less\n");
		return -1;
	}
	addFileDetails(destination, Metadata, blocks, blockno, length);
	FILE *fptr = fopen(source, "rb+");
	if (fptr == NULL)
	{
		printf("file not found");
		return -1;
	}
	for (int i = blockno; i <blockno + blocks; i++)
	{

		if (length >= BLOCK_SIZE)
			fread(buffer, BLOCK_SIZE, 1, fptr);
		else
		{
			fread(buffer, length, 1, fptr);
		}
		length -= BLOCK_SIZE;
		blockWrite(i, buffer);
	}
	fclose(fptr);
	return 1;


}
int copy_from_disk(char *source, char *destination)
{

	int blockno = -1;
	struct metaData Metadata;
	void *buffer = malloc(sizeof(char)*BLOCK_SIZE);
	blockRead(0, buffer);
	memcpy(&Metadata, buffer, sizeof(struct metaData));


	for (int i = 1; i <= Metadata.no_of_files; i++)
	{
		if (strcmp(Metadata.fileDetails[i].name, source) == 0)
		{
			FILE *fptr = fopen(destination, "wb+");
			int length = Metadata.fileDetails[i].length;
			int blocks = BLOCKS(length);
			blockno = Metadata.fileDetails[i].blockno;
			for (int i = blockno; i <blockno + blocks; i++)
			{
				blockRead(i, buffer);
				if (length >= BLOCK_SIZE)
					fwrite(buffer, BLOCK_SIZE, 1, fptr);
				else
				{
					fwrite(buffer, length, 1, fptr);
				}
				length -= BLOCK_SIZE;

			}
			fclose(fptr);
			return 1;
		}
	}
	printf("file not found to copy\n");
	return -1;
}
int deleteFile(char *filename)
{
	void *buffer = malloc(sizeof(char)*BLOCK_SIZE);
	blockRead(0, buffer);
	struct metaData Metadata;
	memcpy(&Metadata, buffer, sizeof(struct metaData));

	if (Metadata.no_of_files == 1)

	{
		format();
		return -1;
	}

	for (int i = 1; i <= Metadata.no_of_files; i++)
	{
		if (strcmp(Metadata.fileDetails[i].name, filename) == 0)
		{
			for (int j = Metadata.fileDetails[i].blockno; j < Metadata.fileDetails[i].blockno + Metadata.fileDetails[i].block_count; j++)
				Metadata.bitVector[j] = '0';
			strcpy(Metadata.fileDetails[i].name, Metadata.fileDetails[Metadata.no_of_files].name);
			Metadata.fileDetails[i].length = Metadata.fileDetails[Metadata.no_of_files].length;

			Metadata.fileDetails[i].block_count = Metadata.fileDetails[Metadata.no_of_files].block_count;
			Metadata.fileDetails[i].blockno = Metadata.fileDetails[Metadata.no_of_files].blockno;
			Metadata.no_of_files -= 1;

			memcpy(buffer, &Metadata, sizeof(struct metaData));
			blockWrite(0, buffer);
			free(buffer);
			return 1;
		}
	}
	free(buffer);
	return -1;
}
void ls()
{
	void *buffer = malloc(sizeof(char)*BLOCK_SIZE);
	blockRead(0, buffer);
	struct metaData Metadata;
	memcpy(&Metadata, buffer, sizeof(struct metaData));
	for (int i = 1; i <= Metadata.no_of_files; i++)
	{
		printf("%s\n", Metadata.fileDetails[i].name);
	}
	free(buffer);

}
void debug()
{
	void *buffer = malloc(sizeof(char)*BLOCK_SIZE);
	blockRead(0, buffer);
	struct metaData Metadata;
	memcpy(&Metadata, buffer, sizeof(struct metaData));
	printf("free blocks :%d\n", Metadata.free_blocks);
	printf("no_of_files :%d\n", Metadata.no_of_files);
	printf("magicnumber :%d\n", Metadata.magicNumber);


}
int isEqual(char *str1, char *str2)
{
	for (int i = 0; str1[i] != '\0'; i++)
	{
		if (str1[i] != str2[i])
			return 0;
	}
	return 1;
}
int main()
{
	char *command = (char*)malloc(sizeof(char) * 100);
	char disk_name[20];

	printf("Enter disk name : ");
	gets(disk_name);
	while (1)
	{
		printf(">>");
		gets(command);
		char command1[20] = "", command2[30] = "", command3[30] = "";
		sscanf(command, "%s %s %s", command1, command2, command3);
		if (isEqual(command1, "format") && isEqual(command2, ""))
		{
			format();
		}
		else if (isEqual(command1, "copy_to_disk"))
		{
			copy_to_disk(command2, command3);
		}
		else if (isEqual(command1, "copy_from_disk"))
		{
			copy_from_disk(command2, command3);

		}
		else if (isEqual(command, "ls"))
		{
			ls();
		}
		else if (isEqual(command1, "delete"))
		{
			deleteFile(command2);
		}
		else if (isEqual(command1, "debug"))
		{
			debug();
		}
		else
		{
			exit(1);
		}
	}
