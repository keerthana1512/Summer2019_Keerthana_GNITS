#include<stdlib.h>
#include<string.h>
#include<stdio.h>

#define DISK_SIZE (100*1024*1024)
#define BLOCK_SIZE (16*1024)
#define BLOCKS(size) ((size/BLOCK_SIZE)+(!!(size%BLOCK_SIZE)))
#define START_FILE_OFFSET (2*BLOCK_SIZE+1)
#define META_OFFSET (12+1)
#define CHECKER (0x444E524D)
#define ARRAY_OFFSET (BLOCK_SIZE+1)
#define FILES (32)

struct fileinfo
{
	char name[20];
	int length;
	int start_block;
	int no_of_blocks;
	int indirect_bit;
};

struct metaData
{
	int magicNumber;
	int no_of_files;
	int free_blocks;
	struct fileinfo fileDetails[FILES];
	char bitVector[FILES * 5];
};

int blockRead(int start_block, void *buffer)
{
	FILE *fptr = fopen("hardDisk.hdd", "rb+");
	if (fptr == NULL)
	{
		printf("file not found");
		return -1;
	}
	fseek(fptr, start_block*BLOCK_SIZE, SEEK_SET);
	fread(buffer, BLOCK_SIZE, 1, fptr);
	fclose(fptr);
	return 1;
}
int blockWrite(int start_block, void *buffer)
{
	FILE *fptr = fopen("hardDisk.hdd", "rb+");
	if (fptr == NULL)
	{
		printf("file not found");
		return -1;
	}
	fseek(fptr, start_block*BLOCK_SIZE, SEEK_SET);
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
int find_start_block(int blockcount, char bitvector[])
{

	int i = 1;
	while (i < BLOCKS(DISK_SIZE) - 2)
	{
		if (bitvector[i] == '1')
			i++;
		else
			return i;
	}
	return -1;
}
void addFileDetails(char *filename, struct metaData metadata, int blocks, int start_block, int length, int *arr)
{

	metadata.free_blocks -= blocks;
	strcpy(metadata.fileDetails[metadata.no_of_files + 1].name, filename);
	metadata.fileDetails[metadata.no_of_files + 1].start_block = start_block;
	metadata.fileDetails[metadata.no_of_files + 1].no_of_blocks = blocks;
	metadata.fileDetails[metadata.no_of_files + 1].length = length;
	metadata.magicNumber = CHECKER;
	metadata.no_of_files += 1;
	if (blocks == 1)
	{
		metadata.bitVector[start_block] = '1';
		metadata.fileDetails[metadata.no_of_files + 1].indirect_bit = 0;
	}
	else
	{
		metadata.bitVector[start_block] = '1';
		metadata.fileDetails[metadata.no_of_files + 1].indirect_bit = 1;
		for (int i = 0; i < blocks; i++)
		{
			metadata.bitVector[arr[i]] = '1';
		}
	}
	void *buffer = malloc(sizeof(char)*BLOCK_SIZE);
	memcpy(buffer, &metadata, sizeof(struct metaData));
	blockWrite(0, buffer);
	free(buffer);
}

int copy_to_disk(char *source, char *destination)
{
	int length = fileSize(source);
	int required_blocks = BLOCKS(length);
	struct metaData Metadata;
	int start_block;
	void *buffer = malloc(sizeof(char)*BLOCK_SIZE);
	blockRead(0, buffer);
	memcpy(&Metadata, buffer, sizeof(struct metaData));
	start_block = find_start_block(required_blocks, Metadata.bitVector);
	if (start_block == -1 || required_blocks>Metadata.free_blocks)
	{
		printf("disk size is less\n");
		return -1;
	}
	FILE *fptr = fopen(source, "rb+");
	if (fptr == NULL)
	{
		printf("file not found");
		return -1;
	}
	if (required_blocks == 1)
	{
		int arr[1];
		fread(buffer, length, 1, fptr);
		blockWrite(start_block, buffer);
		fclose(fptr);
		addFileDetails(destination, Metadata, required_blocks, start_block, length, arr);
		return 1;
	}
	int i = start_block + 1;
	int *arr = (int*)malloc(required_blocks*sizeof(int));
	void *number_buffer = malloc(BLOCK_SIZE);
	int temp_length = length;
	for (int j = 0; j < required_blocks;)
	{
		if (Metadata.bitVector[i] == '0')
		{
			Metadata.bitVector[i] = '1';
			arr[j++] = i;
			if (length >= BLOCK_SIZE)
				fread(buffer, BLOCK_SIZE, 1, fptr);
			else
			{
				fread(buffer, length, 1, fptr);
			}
			length -= BLOCK_SIZE;
			blockWrite(i, buffer);

		}
		i++;
	}
	memcpy(number_buffer, arr, sizeof(int)*required_blocks);
	blockWrite(start_block, number_buffer);
	fclose(fptr);
	addFileDetails(destination, Metadata, required_blocks, start_block, temp_length, arr);
	return 1;
}
int copy_from_disk(char *source, char *destination)
{

	int start_block = -1;
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
			start_block = Metadata.fileDetails[i].start_block;
			if (Metadata.fileDetails[i].no_of_blocks == 1)
			{
				blockRead(i, buffer);
				fwrite(buffer, length, 1, fptr);
				fclose(fptr);
				return 1;
			}
			else
			{
				void* number_buffer = malloc(BLOCK_SIZE);
				blockRead(start_block, number_buffer);
				int *arr = (int*)malloc(Metadata.fileDetails[i].no_of_blocks*sizeof(int));
				memcpy(arr, number_buffer, Metadata.fileDetails[i].no_of_blocks*sizeof(int));
				for (int k = 0; k < Metadata.fileDetails[i].no_of_blocks; k++)
				{
					blockRead(arr[k], buffer);
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
	}
	printf("file not found to copy\n");
	return -1;
}
int delete_file(char *filename)
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
			if (Metadata.fileDetails[i].no_of_blocks == 1)
				Metadata.bitVector[i] = '0';
			else
			{
				void* number_buffer = malloc(BLOCK_SIZE);
				blockRead(Metadata.fileDetails[i].start_block, number_buffer);
				int *arr = (int*)malloc(Metadata.fileDetails[i].no_of_blocks*sizeof(int));
				memcpy(arr, number_buffer, Metadata.fileDetails[i].no_of_blocks*sizeof(int));

				for (int j = 0; j < Metadata.fileDetails[i].no_of_blocks; j++)
				{
					Metadata.bitVector[arr[j]] = '0';
				}
			}
			strcpy(Metadata.fileDetails[i].name, Metadata.fileDetails[Metadata.no_of_files].name);
			Metadata.fileDetails[i].length = Metadata.fileDetails[Metadata.no_of_files].length;
			Metadata.fileDetails[i].no_of_blocks = Metadata.fileDetails[Metadata.no_of_files].no_of_blocks;
			Metadata.fileDetails[i].start_block = Metadata.fileDetails[Metadata.no_of_files].start_block;
			Metadata.fileDetails[i].indirect_bit = Metadata.fileDetails[Metadata.no_of_files].indirect_bit;
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
		_strlwr(command1);
		//if (!strcmp(command1, "mount") && !strcmp(command2, ""))
		//{
		//	int no_of_blocks = mount_disk(command2, atoi(command3));
		//	//initialize();
		//}
		if (!strcmp(command1, "copytofs"))
		{
			copy_to_disk(command2, command3);
		}
		else if (!strcmp(command1, "copyfromfs"))
		{
			copy_from_disk(command2, command3);

		}
		else if (!strcmp(command1, "format"))
		{
			format();
		}
		else if (!strcmp(command, "ls"))
		{
			ls();
		}
		else if (!strcmp(command1, "delete"))
		{
			delete_file(command2);
		}
		else if (!strcmp(command1, "debug"))
		{
			debug();
		}
		/*else if (!strcmp(command1, "unmount"))
		{
		unmount();
		}*/
		else
		{
			exit(1);
		}
	}

	system("pause");
	return 0;

}