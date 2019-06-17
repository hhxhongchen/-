#include <stdio.h>
#include <malloc.h>
#include <string>
#include <iostream>

using namespace std;
#define FILEBLOCK 1
#define DATABLOCK 4
#define FILEDESSIZE 100
#define L 512
#define B 32
#define KEEPSIZE  105

std::string TIPSINFO[10] = { "显示文件目录","创建文件","删除文件","打开文件","关闭文件","读取文件","写入文件","清空内容","退出并保存文件"
};

typedef struct
{
	char name[10];
	short int length;
	short int allo[10];
} filedes;

typedef struct
{
	char Buffer[B];
	char* p;
	int id;
	int index;
	filedes dsc;
} OpenFileTable;

char ldisk[L][B];
char map[8] = { 127,64,32,16,8,4,2,1 };
char temp_block[B];
int noofblock;

OpenFileTable openFileTable;
filedes t;
bool write_block(int i, char* p);
bool read_block(int i, char* p);

int getfileheadblock()
{
	read_block(0, temp_block);
	char* p = temp_block;
	for (int i = 0; i < B * FILEBLOCK; i++)
	{
		for (int k = 0; k < 8; k++)
		{
			int tt = *p & map[k];
			if (!tt)
			{
				*p = *p ^ map[k];
				noofblock = 0;
				return i * 8 + k;
			}
		}
		p++;
	}
	return -1;
}

void setfileheadblock() {
	write_block(noofblock, temp_block);
}

void InitFMS() {
	char filename[] = "catalog";
	char* p = filename;
	int length = 0;
	int index = getfileheadblock();
	filedes fd;
	memset(ldisk, 0, L * B);
	memcpy(&fd.name, filename, length + 1);
	fd.length = -1;
	for (int i = 0; i < 10; i++)
	{
		fd.allo[i] = -1;
	}
	write_block(FILEBLOCK + DATABLOCK + index, (char*)& fd);
	noofblock = 0;
	setfileheadblock();
	return;
}

int getfreedatablock() {
	int bIndex = FILEBLOCK;
	while (1)
	{
		read_block(bIndex++, temp_block);
		for (int i = 0; i < B; i++)
		{
			if (temp_block[i] != 0xff)
			{
				for (int k = 0; k < 8; k++)
				{
					if (!(map[k] & temp_block[i]))
					{
						temp_block[i] = map[k] | temp_block[i];
						noofblock = bIndex - 1;
						return KEEPSIZE + (bIndex - 2) * B * 8 + i * 8 + k;
					}
				}
			}
		}
	}
	return 0;
}

bool addfile(int fileIndex) {
	filedes fp;
	read_block(DATABLOCK + FILEBLOCK, (char*)& fp);
	int dataIndex = 0;

	while (dataIndex < 10)
	{
		if (fp.allo[dataIndex] == -1)
		{
			fp.allo[0] = getfreedatablock();
			setfileheadblock();
			temp_block[0] = fileIndex;
			for (int i = 1; i < B; i++)
			{
				temp_block[i] = -1;
			}
			write_block(fp.allo[dataIndex], temp_block);
			write_block(DATABLOCK + FILEBLOCK, (char*)& fp);
			return true;
		}
		read_block(fp.allo[dataIndex++], temp_block);
		for (int i = 0; i < B; i++)
		{
			if (temp_block[i] == -1)
			{
				temp_block[i] = fileIndex;
				write_block(fp.allo[dataIndex - 1], temp_block);
				return true;
			}
		}
	}
	return false;
}

bool create(char* filename) {
	char* p = filename;
	int length = strlen(filename);
	int index = getfileheadblock();
	setfileheadblock();
	p = filename;
	filedes fd;
	memcpy(&fd.name, filename, length + 1);
	fd.length = 0;
	for (int i = 0; i < 10; i++)
	{
		fd.allo[i] = -1;
	}
	fd.allo[0] = getfreedatablock();
	setfileheadblock();
	memset(temp_block, -1, B);
	write_block(fd.allo[0], temp_block);
	write_block(FILEBLOCK + DATABLOCK + index, (char*)& fd);
	addfile(index);
	return true;
}

bool destroy(char* filename)
{
	char temp_block[B];
	filedes fd;
	read_block(DATABLOCK + FILEBLOCK, (char*)& fd);
	for (int i = 0; i < 10; i++)
	{
		if (fd.allo[i] == -1)continue;
		read_block(fd.allo[i], temp_block);
		for (int j = 0; j < B; j++)
		{
			if (temp_block[j] != -1)
			{
				filedes tfd;
				read_block(DATABLOCK + FILEBLOCK + temp_block[j], (char*)& tfd);
				if (memcmp(filename, tfd.name, strlen(filename)) == 0)
				{
					int fileIndex = temp_block[j];
					temp_block[j] = -1;
					write_block(fd.allo[i], temp_block);
					read_block(0, temp_block);
					int a = fileIndex / 8;
					int b = fileIndex % 8;
					temp_block[a] = temp_block[a] & (~map[b]);
					write_block(0, temp_block);
					cout << "删除成功" << endl;
					return true;
				}
			}
		}
	}
	cout << "没有该文件" << endl;
	return false;
}

bool open(char* filename)
{
	filedes fd;
	read_block(DATABLOCK + FILEBLOCK, (char*)& fd);
	for (int i = 0; i < 10; i++)
	{
		if (fd.allo[i] == -1)
		{
			cout << "文件不存在!\n";
			return false;
		}
		read_block(fd.allo[i], temp_block);
		for (int j = 0; j < B; j++)
		{
			if (temp_block[j] != -1)
			{
				filedes temp_fd;
				read_block(DATABLOCK + FILEBLOCK + temp_block[j], (char*)& temp_fd);
				if (memcmp(filename, temp_fd.name, strlen(filename)) == 0)
				{
					openFileTable.id = temp_block[j];
					openFileTable.index = 0;
					if (temp_fd.allo[0] == -1)
					{
						openFileTable.p == NULL;
					}
					read_block(temp_fd.allo[0], openFileTable.Buffer);
					openFileTable.p = openFileTable.Buffer;
					openFileTable.dsc = temp_fd;
					return true;
				}

			}
		}
	}
	return false;
}

bool close(char* index)
{
	if (openFileTable.id == -1)
	{
		cout << "没有打开的文件\n";
		return false;
	}
	openFileTable.id = -1;
	return true;
}

bool read(char* index, char* mem_area, int count)
{
	int left = B - (openFileTable.p - openFileTable.Buffer);
	while (count)
	{
		if (count - left > 0) //当前缓冲区不能满足请求，再取
		{
			count -= left;
			if (openFileTable.p == NULL)
			{
				cout << "越界访问\n";
				return false;
			}
			memcpy(mem_area, openFileTable.p, left);
			openFileTable.index++;
			if (openFileTable.dsc.allo[openFileTable.index] == -1)
			{
				openFileTable.p = NULL;
			}
			read_block(openFileTable.dsc.allo[openFileTable.index], openFileTable.p);
			left = B;
		}
		else
		{
			if (openFileTable.p == NULL)
			{
				cout << "越界访问\n";
				return false;
			}
			memcpy(mem_area, openFileTable.p, count);
			write_block(openFileTable.dsc.allo[openFileTable.index], openFileTable.Buffer);
			openFileTable.p += left;
			count = 0;
			openFileTable.p += count;
		}
	}
	return true;
}


bool write(char* index, char* mem_area, int count) {
	int left = B - (openFileTable.p - openFileTable.Buffer);
	while (count)
	{
		if (count - left > 0) //当前缓冲区不能满足请求，再取
		{
			count -= left;
			if (openFileTable.p == NULL)
			{
				cout << "访问超过范围\n";
				return false;
			}
			memcpy(openFileTable.Buffer, mem_area, left);
			write_block(openFileTable.dsc.allo[openFileTable.index], openFileTable.Buffer);
			openFileTable.index++;
			if (openFileTable.dsc.allo[openFileTable.index] == -1)
			{
				openFileTable.p = NULL;
			}
			openFileTable.p = openFileTable.Buffer;

			left = B;
		}
		else
		{
			if (openFileTable.p == NULL)
			{
				cout << "访问超过范围\n";
				return false;
			}
			memcpy(openFileTable.p, mem_area, count);
			write_block(openFileTable.dsc.allo[openFileTable.index], openFileTable.Buffer);
			openFileTable.p += count;
			openFileTable.dsc.length += count;
			write_block(DATABLOCK + FILEBLOCK + openFileTable.id, (char*)& openFileTable.dsc);
			return true;
		}
	}
	return false;
}

bool directory() {
	filedes fd;
	read_block(FILEBLOCK + DATABLOCK, (char*)& fd);
	cout << "name\tsize\n";
	for (int i = 0; i < 10; i++)
	{
		if (fd.allo[i] != -1)
		{
			read_block(fd.allo[i], temp_block);
			for (int j = 0; j < B; j++)
			{
				if (temp_block[j] != -1)
				{
					filedes temp[B];
					read_block(FILEBLOCK + DATABLOCK + temp_block[j], (char*)& temp);
					cout <<temp->name<<"     ";
					cout <<temp->length<<endl;
				}
			}
		}

	}
	return true;
}


bool read_block(int i, char* p)
{
	memcpy(p, ldisk[i], B);
	return true;
}

bool write_block(int i, char* p)
{
	memcpy(ldisk[i], p, B);
	return true;
}

void savedata() {
	FILE* fp = fopen("data.txt", "w");
	if (fp == NULL) {
		exit(0);
	}
	fwrite(&ldisk[0][0], 1, L * B, fp);
	fclose(fp);
}

void load() {
	FILE* fp = fopen("data.txt", "r");
	if (fp == NULL) {
		exit(0);
	}
	fread(&ldisk[0][0], 1, L * B, fp);
	fclose(fp);
}

void printTips() {
	cout << "输入0-8进行相应操作" << endl << endl;
	for (int i = 0; i < 9; i++) {
		if (i != 8)
		{
			cout << i + 1;
		}
		else
			cout << '0';
		cout << "     ";
		cout << TIPSINFO[i];
		cout << endl;
	}
	cout << endl;
}
int main() {
	InitFMS();
	load();
	printTips();
	int opCounter = 0;
	for (;;) {
		cout << endl;
		cout << "请选择操作：";
		string op = "";
		cin >> op;

		if (!op.compare("0")) {
			savedata();
			return 0;
		}
		else if (!op.compare("2")) {
			cout << "File Name: ";
			char filename1[100];
			cin >> filename1;
			if (!create(filename1)) {
				exit(0);
			}
			getchar();
		}
		else if (!op.compare("3")) {
			cout << "File Name: ";
			char filename[100];
			cin >> filename;
			getchar();
			destroy(filename);
		}
		else if (!op.compare("4")) {
			cout << "File Name: ";
			char filename[100];
			cin >> filename;
			getchar();
			if (!open(filename)) {
				cout << "错误: 请输入正确的文件名！" << endl;
				continue;
			}
			cout << "接下来的指令需为读写指令，请输入:";
			cin >> op;
			if (!op.compare("6")) {
				getchar();
				char temp[B];
				read(NULL, temp, B);
				cout << temp;
				close(filename);
			}
			else if (!op.compare("7")) {
				getchar();
				char str[100];
				cout << "请输入文件内容：\n";
				cin >> str;
				write(NULL, str, strlen(str) + 1);
				close(filename);
				getchar();
			}
		}
		else if (!op.compare("1")) {
			directory();
			cout << "\n";

		}
		else if (!op.compare("8")) {
			InitFMS();
		}
		else {
			cout << "错误: 请输入正确命令" << endl;
		}
	}
	return 0;
}
