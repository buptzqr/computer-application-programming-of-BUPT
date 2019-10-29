#include <stdio.h>
#include <string.h>

#define MAX(x1,x2) ((x1) > (x2) ? (x1) : (x2))

typedef struct Node{
	int height; //�ýڵ���Ϊ����ʱ�ĸ߶�
	char* data;   //��ʾÿ���ڵ����������
	struct Node* left;
	struct Node* right;
}Node, *AVLtree;

typedef struct Statistic{
	int mem;
	int cmpnum;
	int nodeNum;
}Statistic;

Statistic global_stats;

int Insert(AVLtree* T, char* D);
int Find(AVLtree T, int x);
int GetHeight(AVLtree T);
void LeftRotate(AVLtree* T);
void RightRotate(AVLtree* T);
int FindMin(AVLtree T);
void* bupt_malloc(size_t size);
int byte_cmp(char a, char b);
int myCmp(char* a, char* b);

int myCmp(char* a, char* b){
	global_stats.cmpnum += strlen(b);
	if (strlen(a) > strlen(b))
		return 1;
	else if (strlen(a) < strlen(b))
		return -1;
	else{
		return strcmp(a, b);
	}
}


int Insert(AVLtree* T, char* D ){
	//�������
	if (T == NULL){
		return -1;
	}

	//�ҵ������λ��
	if (*T == NULL){
		*T = (Node*)bupt_malloc(sizeof(Node));
		(*T)->data = D;
		(*T)->height = 1;
		(*T)->left = NULL;
		(*T)->right = NULL;
		global_stats.nodeNum++;
		return 0;
	}
	else{
		//�����Ѵ��ڸ�����
		if (myCmp(D,(*T)->data) == 0){
			return -1;
		}
		else
			if (myCmp(D,(*T)->data)>0){//���������в���
				if (Insert(&(*T)->right, D) == -1){
					return -1;
				}

				//����󣬵����ݵ��ýڵ���м�飬���������ƽ�������������
				//��Ϊ��������֧�в��룬����߶�֮�����2��ֻ����������֧������֧��
				if (GetHeight((*T)->right) - GetHeight((*T)->left) == 2){
					if (myCmp(D, (*T)->right->data) >0){
						LeftRotate(T);//RR������
					}
					else{//��Ӧ���RL��������������
						RightRotate(&(*T)->right);
						LeftRotate(T);
					}
				}
			}
			else
				if (myCmp(D ,(*T)->data)<0){//���������в���
					if (Insert(&(*T)->left, D)){
						return -1;
					}

					if (GetHeight((*T)->left) - GetHeight((*T)->right) == 2){
						if (myCmp(D, (*T)->left->data)<0){
							RightRotate(T);//��Ӧ���LL
						}
						else{//LR��������������
							LeftRotate(&(*T)->left);
							RightRotate(T);
						}
					}
				}
	}

	//���µ�ǰ�ڵ�ĸ߶�
	(*T)->height = MAX(GetHeight((*T)->left), GetHeight((*T)->right)) + 1;
	return 0;
}

int Find(AVLtree T, char* x){
	while (T != NULL){
		if (myCmp(T->data,x)==0){
			return 0;
		}
		else
			if (myCmp(T->data, x) >0){
				T = T->left;
			}
			else{
				T = T->right;
			}
	}
	return -1;
}

//�����ĸ߶�Ϊ0
int GetHeight(AVLtree T){
	if (T == NULL){
		return 0;
	}
	else{
		return T->height;
	}
}

void LeftRotate(AVLtree* T){
	Node *P, *R;
	P = *T;
	R = P->right;
	P->right = R->left;
	R->left = P;
	*T = R;

	//��ת�Ժ�Ҫ���½ڵ�ĸ߶�
	P->height = MAX(GetHeight(P->left), GetHeight(P->right)) + 1;
	R->height = MAX(GetHeight(R->left), GetHeight(R->right)) + 1;
}

void RightRotate(AVLtree* T){
	Node *P, *L;
	P = *T;
	L = P->left;
	P->left = L->right;
	L->right = P;
	*T = L;
	//��ת�Ժ�Ҫ���½ڵ�ĸ߶�
	P->height = MAX(GetHeight(P->left), GetHeight(P->right)) + 1;
	L->height = MAX(GetHeight(L->left), GetHeight(L->right)) + 1;
}

int FindMin(AVLtree T){
	if (T == NULL){
		return -1;
	}

	while (T->left != NULL){
		T = T->left;
	}
	return T->data;
}

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
	global_stats.cmpnum = 0;
	global_stats.mem = 0;
	global_stats.nodeNum = 0;
	FILE *fp;
	AVLtree head = (AVLtree)bupt_malloc(sizeof(Node));
	if ((fp = fopen("./patterns-127w.txt", "r")) == NULL){
		return -1;
	}
	int flag = 0;
	while (!feof(fp)){
		char strLine[100] = { '\0' };
		fgets(strLine, 100, fp);
		int i = 0;
		while (strLine[i] != '\n'&&strLine[i] != '\0'){
			i++;
		}
		strLine[i] = '\0';
		char* nodeData = (char*)bupt_malloc(i+1);
		memcpy(nodeData, strLine, i+1);
		if (!flag){
			global_stats.nodeNum++;
			head->data = nodeData;
			head->left = NULL;
			head->right = NULL;
			head->height = 0;
			flag = 1;
			continue;
		}

		Insert(&head, nodeData);
	}
			
	fclose(fp);
	printf("�ڵ����:%d\n", global_stats.nodeNum);
	if ((fp = fopen("./words-98w.txt", "r")) == NULL){
		return -1;
	}
	FILE* fp1 = fopen("./result.txt", "w");
	int wordsNum = 0;
	int wordsSuccessNum = 0;
	while (!feof(fp)){
		wordsNum++;
		char strLine[100] = { '\0' };
		fgets(strLine, 100, fp);
		//���ַ���תΪint
		int i = 0;
		while (strLine[i] != '\n'&&strLine[i] != '\0'){
			i++;
		}
		strLine[i] = '\0';
		char* nodeData = (char*)bupt_malloc(i + 1);
		memcpy(nodeData, strLine, i + 1);
		AVLtree temp = head;
		if (!Find(temp, nodeData)){
			wordsSuccessNum++;
			fputs(nodeData, fp1);
			fputs("\n", fp1);

		}
	}
	fclose(fp);
	fclose(fp1);
	printf("���ṹռ���ڴ���%d\n", global_stats.mem);
	printf("words�ܸ���%d\n", wordsNum);
	printf("�ɹ�������word�ܸ���%d\n", wordsSuccessNum);
	printf("�ַ��Ƚϴ���%d\n", global_stats.cmpnum / 1000);
	printf("\n");
}