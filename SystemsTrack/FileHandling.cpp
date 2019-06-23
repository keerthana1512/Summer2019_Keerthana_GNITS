#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define DISK_SIZE 10000000
#define DISK_NAME "hardDisk.hdd"
struct fileinfo
{
	char name[16];
	int offset;
	int length;
};
void printFileNames()
{
	struct fileinfo File;
	FILE *fptr = fopen(DISK_NAME, "rb+");
	if (fptr == NULL)
		return;
	int no_of_files = 0, free_space = 0;
	if (fread(&no_of_files, sizeof(int), 1, fptr) == -1)
		return;
	if (fread(&free_space, sizeof(int), 1, fptr) == -1)
		return;
	for (int i = 0; i < no_of_files; i++)
	{
		fread(&File, sizeof(struct fileinfo), 1, fptr);
		puts(File.name);
	}
	fclose(fptr);
}
int getFileDetails(char *filename, int *offset, int *length)
{
	struct fileinfo File;
	FILE *fptr = fopen(DISK_NAME, "rb+");
	if (fptr == NULL)
		printf("Error in opening file!\n");
	int no_of_files = 0, free_space = 0;
	int i = fread(&no_of_files, sizeof(int), 1, fptr);
	if (i == -1)
		return -1;
	i = fread(&free_space, sizeof(int), 1, fptr);
	if (i == -1)
		return -1;
	for (int i = 0; i < no_of_files; i++)
	{
		int j = fread(&File, sizeof(struct fileinfo), 1, fptr);
		if (j == -1)
			return -1;
		if (strcmp(File.name, filename) == 0)
		{
			*offset = File.offset;
			*length = File.length;
			return 1;
		}
	}
	fclose(fptr);
	return 0;
}
int getFreeSpace()
{

	FILE *fptr = fopen(DISK_NAME, "rb+");
	if (fptr == NULL)
		printf("Error in opening file!\n");
	int no_of_files = 0, free_space = 0;
	fread(&no_of_files, sizeof(int), 1, fptr);
	fread(&free_space, sizeof(int), 1, fptr);
	fclose(fptr);
	return free_space;
}
int getOffsetToWrite()
{
	struct fileinfo File;
	int i;
	FILE *fptr = fopen(DISK_NAME, "rb+");
	if (fptr == NULL)
		printf("Error in opening file!\n");
	int no_of_files = 0, free_space = 0;
	i = fread(&no_of_files, sizeof(int), 1, fptr);
	if (i == -1)
		return -1;
	i = fread(&free_space, sizeof(int), 1, fptr);
	if (i == -1)
		return -1;
	if (no_of_files != 0)
	{
		fseek(fptr, (no_of_files - 1)*(sizeof(struct fileinfo)), SEEK_CUR);
		i = fread(&File, sizeof(struct fileinfo), 1, fptr);
		if (i == -1)
			return -1;
		fclose(fptr);
		return File.offset + File.length + 1;
	}
	fclose(fptr);
	return 3145728;

}
void addNewFile(char *filename, int length)
{
	int offset = 0, i;
	struct fileinfo File;
	strcpy(File.name, filename);
	File.length = length;
	File.offset = getOffsetToWrite();
	FILE *fptr = fopen(DISK_NAME, "rb+");
	if (fptr == NULL)
		printf("Error in opening file!\n");
	int no_of_files = 0, free_space = 0;
	i = fread(&no_of_files, sizeof(int), 1, fptr);
	if (i == -1)
		return;
	i = fread(&free_space, sizeof(int), 1, fptr);
	if (i == -1)
		return;
	fseek(fptr, (no_of_files)*(sizeof(struct fileinfo)), SEEK_CUR);
	i = fwrite(&File, sizeof(struct fileinfo), 1, fptr);
	if (i == -1)
		return;
	fclose(fptr);
	fptr = fopen(DISK_NAME, "rb+");
	if (fptr == NULL)
		printf("Error in opening file!\n");
	no_of_files++;
	i = fwrite(&no_of_files, sizeof(int), 1, fptr);
	if (i == -1)
		return;
	free_space = free_space - length;
	i = fwrite(&free_space, sizeof(int), 1, fptr);
	if (i == -1)
		return;
	fclose(fptr);
}
int get_file_length(char* file_name)
{
	FILE* fp = fopen(file_name, "r");
	if (fp == NULL) {
		printf("File not found");
		return -1;
	}
	fseek(fp, 0L, SEEK_END);
	int length_of_file = ftell(fp);
	fclose(fp);
	return length_of_file;
}

int copy_to_disk(char* disk_name, char* file_name)
{

	char ch;
	int offset = getOffsetToWrite();
	int free_space = getFreeSpace();
	int i;
	int file_size = get_file_length(file_name);
	FILE* file_pointer = fopen(file_name, "rb");
	if (file_pointer == NULL)
		printf("Error in opening file!\n");
	FILE* disk_write_head = fopen(disk_name, "rb+");
	if (disk_write_head == NULL)
		printf("Error in opening file!\n");
	if (file_size <= free_space)
	{
		fseek(disk_write_head, offset, SEEK_SET);
		while (fread(&ch, 1, 1, file_pointer) == 1)
		{
			//printf("%c", ch);
			i = fwrite(&ch, 1, 1, disk_write_head);
			if (i == -1)
				return -1;
		}
		fclose(disk_write_head);
		fclose(file_pointer);
		addNewFile(file_name, file_size);
		return file_size;
	}
	printf("file is impossible to store\n");
	return -1;
}
void copy_from_disk(char* disk_file, char *dest_file)
{
	int start_offset = 0, length = 0, i;
	if (getFileDetails(disk_file, &start_offset, &length))
	{
		FILE *disk_read_head = fopen(DISK_NAME, "rb");
		if (disk_read_head == NULL)
			printf("Error in opening file!\n");
		FILE *filetowrite = fopen(dest_file, "wb");
		if (filetowrite == NULL)
			printf("unable to write into the file");
		fseek(disk_read_head, start_offset, SEEK_SET);
		while (length--)
		{
			char ch;
			i = fread(&ch, 1, 1, disk_read_head);
			if (i == -1)
				return;
			//printf("%c", ch);
			i = fwrite(&ch, 1, 1, filetowrite);
			if (i == -1)
				return;
		}
		fclose(filetowrite);
	}
	else
	{
		printf("File not found\n");
	}


}
int isEqual(char *str1, char *str2)
{
	for (int i = 0; str1[i] != '\0'; i++)
	{
		if (str1[i] != str2[i])
			return 0;
	}
	return 1;
}int main()
{
	char *command = (char*)malloc(sizeof(char) * 100);
	char disk_name[20];

	printf("Enter disk name : ");
	gets(disk_name);
	while (1)
	{
		printf("\n>>");
		gets(command);
		char command1[20] = "", command2[30] = "", command3[30] = "";
		sscanf(command, "%s %s %s", command1, command2, command3);
		if (isEqual(command1, "format") && isEqual(command2, ""))
		{
			FILE *fp = fopen(DISK_NAME, "rb+");
			int m = 101711872, n = 0;
			fwrite(&n, sizeof(int), 1, fp);
			fwrite(&m, sizeof(int), 1, fp);
			fclose(fp);
		}
		else if (isEqual(command1, "copy_to_disk"))
		{
			copy_to_disk(disk_name, command2);
		}
		else if (isEqual(command1, "copy_from_disk"))
		{
			copy_from_disk(command2, command3);

		}
		else if (isEqual(command, "ls"))
		{
			printFileNames();
		}
		else
		{
			exit(1);
		}
	}

	system("pause");
	return 0;

}
