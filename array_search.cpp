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
	Node* ptr=NULL;
	string patternString="";
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
	FILE *fp;
	char strLine[100];            
	char seachWord[100];
	char* myPool = (char*)bupt_malloc(9700000);
	char* currentWord = myPool;
	if ((fp = fopen("./patterns-127w.txt", "r")) == NULL) //判断文件是否存在及可读
	{
		printf("error!");
		return -1;
	}

	while (!feof(fp))
	{
		fgets(strLine, 1024, fp);  //读取一行
		int i = 0;
		while (strLine[i]!='\0'){
			*currentWord = strLine[i];
			currentWord++;
			i++;
		}
	}
	*currentWord = '\0';
	fclose(fp);//关闭文件
	FILE* fp1;
	if ((fp1 = fopen("./words-98w.txt", "r")) == NULL) //判断文件是否存在及可读
	{
		printf("error!");
		return -1;
	}

	int wordsNum = 0;
	int wordsSuccess = 0;
	while (!feof(fp1)){
		wordsNum++;
		fgets(strLine, 1024, fp1);  //读取一行
		int num=0;
		while (strLine[num] != '\n'){
			num++;
		}
		char* position = myPool;
		while ((*position) != '\0'){
			int patternNum = 0;
			char patternStr[100] = { '\0' };
			while ((*position) != '\n'&&(*position)!='\0'){
				patternStr[patternNum]=(*position);
				patternNum++;
				position++;
			}
			if ((*position) != '\0')
				position++;
			if (num != patternNum)
				continue;
			else{
				int flag = 0;
				for (; flag < num; flag++){
					if (byte_cmp(strLine[flag], patternStr[flag])){
						break;
					}
				}
				if (flag == num){
					printf("%s yes\n", patternStr);
					wordsSuccess++;
					break;
				}
					
			}
		}
	}
	fclose(fp1);//关闭文件
	printf("%d KB\n", global_stats.mem / 1000);
	printf("%d 次\n",global_stats.cmpnum);
	printf("words num:%d\n", wordsNum);
	printf("words sucdess:%d\n", wordsSuccess);
	cin.get();
	return 0;
}