#include <stdio.h>
#include <string.h>

#define MAX(x1,x2) ((x1) > (x2) ? (x1) : (x2))

typedef struct Node{
	int height; //该节点作为子树时的高度
	char* data;   //表示每个节点存贮的数据
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
	//参数检查
	if (T == NULL){
		return -1;
	}

	//找到插入的位置
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
		//树中已存在该数据
		if (myCmp(D,(*T)->data) == 0){
			return -1;
		}
		else
			if (myCmp(D,(*T)->data)>0){//在右子树中插入
				if (Insert(&(*T)->right, D) == -1){
					return -1;
				}

				//插入后，当回溯到该节点进行检查，如果不满足平衡条件，则调整
				//因为是在右子支中插入，如果高度之差等于2，只可能是右子支比左子支高
				if (GetHeight((*T)->right) - GetHeight((*T)->left) == 2){
					if (myCmp(D, (*T)->right->data) >0){
						LeftRotate(T);//RR，左旋
					}
					else{//对应情况RL，先右旋再左旋
						RightRotate(&(*T)->right);
						LeftRotate(T);
					}
				}
			}
			else
				if (myCmp(D ,(*T)->data)<0){//在左子树中插入
					if (Insert(&(*T)->left, D)){
						return -1;
					}

					if (GetHeight((*T)->left) - GetHeight((*T)->right) == 2){
						if (myCmp(D, (*T)->left->data)<0){
							RightRotate(T);//对应情况LL
						}
						else{//LR，先右旋再左旋
							LeftRotate(&(*T)->left);
							RightRotate(T);
						}
					}
				}
	}

	//更新当前节点的高度
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

//空数的高度为0
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

	//旋转以后要更新节点的高度
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
	//旋转以后要更新节点的高度
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
	printf("节点个数:%d\n", global_stats.nodeNum);
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
		//将字符串转为int
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
	printf("树结构占用内存量%d\n", global_stats.mem);
	printf("words总个数%d\n", wordsNum);
	printf("成功检索的word总个数%d\n", wordsSuccessNum);
	printf("字符比较次数%d\n", global_stats.cmpnum / 1000);
	printf("\n");
}