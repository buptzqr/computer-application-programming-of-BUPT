#include <iostream>
#include <fstream>
#include <string>
using namespace std;

struct Statistic{
	int mem;
	int cmpnum;
};
Statistic global_stats;

struct Node{
	Node* ptr = NULL;
	char* str = NULL;
};

void* bupt_malloc(size_t size){
	if (size <= 0) {
		return NULL;
	}
	global_stats.mem += size;
	return malloc(size);
}

int byte_cmp(char a, char b)
{
	global_stats.cmpnum++;
	return a - b;
}


int main(){
	Node* hashTable[4999];
	for (int i = 0; i < 4999; i++){
		hashTable[i] = (Node*)bupt_malloc(sizeof(Node));
		hashTable[i]->ptr = NULL;
		hashTable[i]->str = NULL;
	}
	FILE *fp;
	if ((fp = fopen("./patterns-127w.txt", "r")) == NULL){
		printf("error!");
		return -1;
	}

	while (!feof(fp)){
		char strLine[100] = { '\0' };
		fgets(strLine, 1024, fp);
		//将字符串转为int
		int i = 0;
		unsigned long long str2int = 0;
		while (strLine[i] != '\n'&&strLine[i] != '\0'){
			str2int = str2int * 131 + (int)(strLine[i]);
			i++;
		}
		int index = str2int % 4999;
		Node* position = hashTable[index];
		while (position->ptr){
			position = position->ptr;
		}
		Node* node = (Node*)bupt_malloc(sizeof(Node));
		node->ptr = NULL;
		node->str = NULL;
		node->str = (char*)bupt_malloc(i+1);
		for (int j = 0; j < i; j++){
			node->str[j] = strLine[j];
		}
		node->str[i] = '\0';
		position->ptr = node;

	}
	fclose(fp);

	int wordsNum = 0;
	int wordsSuccess = 0;
	FILE * fp1;
	if ((fp1 = fopen("./words-98w.txt", "r")) == NULL) //判断文件是否存在及可读
	{
		printf("error!");
		return -1;
	}
	while (!feof(fp1)){
		wordsNum++;
		char strLine[100] = { '\0' };
		fgets(strLine, 1024, fp1);
		//将字符串转为int
		int i = 0;
		unsigned long long str2int = 0;
		while (strLine[i] != '\n'&&strLine[i] != '\0'){
			str2int = str2int * 131 + (int)(strLine[i]);
			i++;
		}
		if (strLine[i] == '\n')
			strLine[i] = '\0';
		int index = str2int % 4999;
		if (!hashTable[index]->ptr){
			continue;;
		}
		else{
			Node* pos = hashTable[index]->ptr;
			while (pos){
				int flag = 0;
				if (strlen(strLine) == strlen(pos->str)){
					for (flag = 0; flag < i; flag++){
						if (byte_cmp(strLine[flag], *((pos->str) + flag))){
							pos = pos->ptr;
							break;
						}
					}
					if (flag == i){
						printf("%s yes\n", strLine);
						wordsSuccess++;
						break;
					}
				}
				else{
					pos = pos->ptr;
				}
			}
			
		}
	}
	fclose(fp1);//关闭文件
	printf("%d KB\n", global_stats.mem / 1000);
	printf("%d 次\n", global_stats.cmpnum);
	printf("words num:%d\n", wordsNum);
	printf("words sucdess:%d\n", wordsSuccess);
	cin.get();
	return 0;

}