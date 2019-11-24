#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include<stdint.h>

#define NODE_NUM 256
#define SHIFT 1
#define ALL_PAT 2256691

double duration;

unsigned long words = 0;
unsigned long in_words = 0;
unsigned long xx = 0;

typedef struct Statistic{
	long long mem;
	long long cmpnum;
}Statistic;

Statistic global_stats;

struct node
{
	struct node *fail;
	struct node *next[NODE_NUM];
	int count;
	int tag;
};
typedef struct node Node;

Node *q[225669100];  //队列，方便用于bfs构造失败指针
int* count_tag;
int** address;

int head, tail = 0;

void* bupt_malloc(size_t size){
	if (size <= 0){
		return NULL;
	}

	global_stats.mem += size;
	return malloc(size);
}

int index_generate(char c){
	int result = 0;
	uint8_t mask = 1;
	int i;
	int temp;

	for (i = 0; i <= 7; i++){
		temp = c & mask;
		if (temp != 0){
			result += pow(2, i);
		}
		c = c >> SHIFT;
	}

	return result;
}

void insert(char *str, Node *root, int tag){
	Node *p = root;
	int index;

	int* a;
	int* b;
	int len = strlen(str);
	int i = 0;
	while (str[i])
	{
		index = index_generate(str[i]);
		if (p->next[index] == NULL){
			Node *newnode=NULL;
			while (!newnode)
				newnode = (Node *)bupt_malloc(sizeof(Node));
			for (int j = 0; j < NODE_NUM; j++)
				newnode->next[j] = NULL;
			newnode->count = 0;
			newnode->tag = -1;

			p->next[index] = newnode;
		}
		p = p->next[index];
		i++;
	}
	p->count = 1;

	address[tag] = &(count_tag[tag]);
	if (p->tag >= 0){
		address[tag] = address[p->tag];
	}
	p->tag = tag;
}


void build_fail(Node *root){
	int i;
	root->fail = NULL;
	q[head++] = root;

	while (head != tail){
		Node *temp = q[tail++];/*首先让指针指向root*/
		Node *p = NULL;
		for (i = 0; i < NODE_NUM; i++){
			if (temp->next[i] != NULL){/*找到存在的节点，用的是遍历的方法*/
				if (temp == root){
					temp->next[i]->fail = root;/*root所有下一级节点的失败指针全部指向root*/
				}
				else{
					p = temp->fail;/*找到失配点父节点的失败指针,其父节点的字符与该失败指针指向字符相等*/
					while (p != NULL){
						if (p->next[i] != NULL){/*如果p->next[i]有节点，在p->next[i]就是temp->next[i]失败指针位置*/
							temp->next[i]->fail = p->next[i];
							break;
						}
						p = p->fail;/*如果上一个if不成立，则继续向上查找*/
					}
					if (p == NULL){
						temp->next[i]->fail = root;/*如果没有找到，那么将失败指针指向root*/
					}
				}
				q[head++] = temp->next[i];
			}
		}
	}
}


void query(Node *root, char *str){
	int cnt = 0;
	int i = 0;
	int index;

	Node *p = root;
	while (str[i]){
		index = index_generate(str[i]);

		while (p->next[index] == NULL && p != root){
			global_stats.cmpnum++;
			p = p->fail;
		}
		p = p->next[index];

		if (p == NULL)
			p = root;
		Node *temp = p;
		while (temp != root && temp->count != -1)
		{
			global_stats.cmpnum++;
			if (temp->tag >= 0){
				int* temp_address = address[temp->tag];
				*temp_address += temp->count;
			}
			temp = temp->fail;
		}
		i++;
	}
}

int main(){
	count_tag = (int*)bupt_malloc(sizeof(int) * ALL_PAT);
	address = (int**)bupt_malloc(sizeof(int*) * ALL_PAT);

	FILE* f_read = fopen("./string.txt", "r");
	FILE* f_pattern = fopen("./pattern.txt", "r");
	FILE* f_result = fopen("./result.txt", "w");

	int i;
	Node *root = (Node *)bupt_malloc(sizeof(Node));
	for (i = 0; i < NODE_NUM; i++)
		root->next[i] = NULL;
	root->count = 0;
	root->tag = -1;

	int tag = 0;
	while (!feof(f_pattern)){
		char patternLine[100] = { '\0' };
		fgets(patternLine, 100, f_pattern);
		int patternLen = 0;
		while (patternLine[patternLen] != '\n'&&patternLine[patternLen] != '\0'){
			patternLen++;
		}
		patternLine[patternLen] = '\0';
		insert(patternLine, root, tag);
		tag++;
		if (tag % 1000 == 0){
			printf("have insert %d pattern string\n", tag);
		}
	}
	printf("start build\n");
	build_fail(root);
	printf("finish build\n");

	long lSize;
	char * buffer;
	size_t result;
	fseek(f_read, 0, SEEK_END);
	lSize = ftell(f_read);
	rewind(f_read);
	buffer = (char*)bupt_malloc(sizeof(char)*lSize);
	fread(buffer, 1, lSize, f_read);

	query(root, buffer);

	rewind(f_pattern);
	tag = 0;

	while (!feof(f_pattern)){
		char patternLine[100] = { '\0' };
		fgets(patternLine, 100, f_pattern);
		int patternLen = 0;
		while (patternLine[patternLen] != '\n'&&patternLine[patternLen] != '\0'){
			patternLen++;
		}
		patternLine[patternLen] = '\0';
		fprintf(f_result, "%s, %d\n", patternLine, *address[tag]);
		tag++;
	}
	
	fprintf(f_result, "%s", "字符/字节比较次数:");
	fprintf(f_result, "%ld", global_stats.cmpnum / 1000);
	fprintf(f_result, "%c", '\n');
	fprintf(f_result, "%s", "内存开销:");
	fprintf(f_result, "%ld", global_stats.mem / 1000);
	fclose(f_result);
	fclose(f_read);
	fclose(f_pattern);
	return 0;
}