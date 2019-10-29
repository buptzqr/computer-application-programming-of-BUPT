#include <stdio.h>
#include <string.h>
typedef struct Radix4Node{
	struct Radix4Node* children[4];
	int flag;//为1代表到达字符串的结尾
}Redix4Node,* Redix4ptr;

typedef struct Statistic{
	int mem;
	int cmpnum;
	int nodeNum;
}Statistic;

Statistic global_stats;

void* bupt_malloc(size_t );
Redix4ptr newNode();
Redix4ptr Redix4Insert(Redix4ptr,int);
Redix4ptr Find(Redix4ptr ptr, int key);

Redix4ptr Find(Redix4ptr ptr, int key){
	return ptr->children[key];
}

Redix4ptr newNode(){
	global_stats.nodeNum++;
	Redix4ptr node = (Redix4ptr)bupt_malloc(sizeof(Redix4Node));
	for (int i = 0; i < 4; i++){
		node->children[i] =NULL;
	}
	node->flag = 0;
	return node;
}

Redix4ptr Redix4Insert(Redix4ptr ptr, int key){
	if (!ptr->children[key]){
		ptr->children[key] = newNode();
	}
	return ptr->children[key];
}

void* bupt_malloc(size_t size){
	if (size <= 0) {
		return NULL;
	}
	global_stats.mem += size;
	return malloc(size);
}

int main(){
	global_stats.cmpnum = 0;
	global_stats.mem = 0;
	global_stats.nodeNum = 0;
	FILE *fp;
	Redix4ptr head = newNode();

	if ((fp = fopen("./patterns-127w.txt", "r")) == NULL){
		return -1;
	}
	int flag = 0;
	while (!feof(fp)){
		char strLine[100] = { '\0' };
		fgets(strLine, 100, fp);
		int i = 0;
		Redix4ptr pos = head;
		while (strLine[i] != '\n'&&strLine[i] != '\0'){
			int bit1 = (strLine[i] & 192)>>6;
			pos = Redix4Insert(pos, bit1);
			int bit2 = (strLine[i] & 48) >>4;
			pos = Redix4Insert(pos, bit2);
			int tmp = strLine[i] << 4;
			int bit3 = (strLine[i] &12) >> 2;
			pos = Redix4Insert(pos, bit3);
			int bit4 = strLine[i] &3;
			pos = Redix4Insert(pos, bit4);
			i++;
		}
		pos->flag = 1;
	}
	fclose(fp);
	printf("节点个数:%d\n", global_stats.nodeNum);
	if ((fp = fopen("./words-98w.txt", "r")) == NULL){
		return -1;
	}
	int wordsNum = 0;
	int wordsSuccessNum = 0;
	while (!feof(fp)){
		wordsNum++;
		char strLine[100] = { '\0' };
		fgets(strLine, 100, fp);
		//将字符串转为int
		int i = 0;
		Redix4ptr pos = head;
		int length = 0;
		while (strLine[length] != '\n'&&strLine[length] != '\0'){
			length++;
		}
		while (strLine[i] != '\n'&&strLine[i] != '\0'){
			int bit1 = (strLine[i] & 192) >> 6;
			pos = Find(pos, bit1);
			global_stats.cmpnum++;
			if (!pos)
				break;
			int bit2 = (strLine[i] & 48) >> 4;
			pos = Find(pos, bit2);
			global_stats.cmpnum++;
			if (!pos)
				break;
			int bit3 = (strLine[i] & 12) >> 2;
			pos = Find(pos, bit3);
			global_stats.cmpnum++;
			if (!pos)
				break;
			int bit4 = strLine[i] & 3;
			pos = Find(pos, bit4);
			global_stats.cmpnum++;
			if (!pos)
				break;
			i++;
		}
		if (i == length && pos->flag){
			wordsSuccessNum++;
		}
	}
	fclose(fp);
	printf("树结构占用内存量%d\n", global_stats.mem);
	printf("words总个数%d\n", wordsNum);
	printf("成功检索的word总个数%d\n", wordsSuccessNum);
	printf("字符比较次数 %d\n", global_stats.cmpnum / 1000);
	printf("\n");
}