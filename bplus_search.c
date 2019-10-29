#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#define M (4)
#define LIMIT_M_2 (M % 2 ? (M + 1)/2 : M/2)

typedef char* KeyType;
typedef struct BPlusNode{
	int KeyNum;
	KeyType Key[M + 1];
	struct BPlusNode* Children[M + 1];
	struct BPlusNode* Next;
} BPlusNode,*BPlusTree, *Position;

typedef struct Statistic{
	int mem;
	int cmpnum;
	int nodeNum;
}Statistic;

Statistic global_stats;

/* 插入 */
BPlusTree Insert(BPlusTree T, KeyType Key);
/* 查找 */
int Find(BPlusTree T, KeyType Key);

int myCmp(char* a, char* b);

void* bupt_malloc(size_t size);

Position RemoveElement(int isKey, Position Parent, Position X, int i, int j);

Position MoveElement(Position Src, Position Dst, Position Parent, int i, int n);

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


void* bupt_malloc(size_t size){
	if (size <= 0) {
		return NULL;
	}
	global_stats.mem += size;
	return malloc(size);
}

static KeyType Unavailable = INT_MIN;

/* 生成节点并初始化 */
static BPlusTree MallocNewNode(){
	BPlusTree NewNode;
	int i;
	NewNode = bupt_malloc(sizeof(struct BPlusNode));
	if (NewNode == NULL)
		exit(EXIT_FAILURE);


	i = 0;
	while (i < M + 1){
		NewNode->Key[i] = NULL;
		NewNode->Children[i] = NULL;
		i++;
	}
	NewNode->Next = NULL;
	NewNode->KeyNum = 0;

	return NewNode;
}

static Position FindMostLeft(Position P){
	Position Tmp;

	Tmp = P;

	while (Tmp != NULL && Tmp->Children[0] != NULL) {
		Tmp = Tmp->Children[0];
	}
	return Tmp;
}

static Position FindMostRight(Position P){
	Position Tmp;

	Tmp = P;

	while (Tmp != NULL && Tmp->Children[Tmp->KeyNum - 1] != NULL) {
		Tmp = Tmp->Children[Tmp->KeyNum - 1];
	}
	return Tmp;
}

/* 寻找一个兄弟节点，其存储的关键字未满，否则返回NULL */
static Position FindSibling(Position Parent, int i){
	Position Sibling;
	int Limit;

	Limit = M;

	Sibling = NULL;
	if (i == 0){
		if (Parent->Children[1]->KeyNum < Limit)
			Sibling = Parent->Children[1];
	}
	else if (Parent->Children[i - 1]->KeyNum < Limit)
		Sibling = Parent->Children[i - 1];
	else if (i + 1 < Parent->KeyNum && Parent->Children[i + 1]->KeyNum < Limit){
		Sibling = Parent->Children[i + 1];
	}

	return Sibling;
}

/* 查找兄弟节点，其关键字数大于M/2 ;没有返回NULL*/
static Position FindSiblingKeyNum_M_2(Position Parent, int i, int *j){
	int Limit;
	Position Sibling;
	Sibling = NULL;

	Limit = LIMIT_M_2;

	if (i == 0){
		if (Parent->Children[1]->KeyNum > Limit){
			Sibling = Parent->Children[1];
			*j = 1;
		}
	}
	else{
		if (Parent->Children[i - 1]->KeyNum > Limit){
			Sibling = Parent->Children[i - 1];
			*j = i - 1;
		}
		else if (i + 1 < Parent->KeyNum && Parent->Children[i + 1]->KeyNum > Limit){
			Sibling = Parent->Children[i + 1];
			*j = i + 1;
		}

	}

	return Sibling;
}

/* 当要对X插入Key的时候，i是X在Parent的位置，j是Key要插入的位置
当要对Parent插入X节点的时候，i是要插入的位置，Key和j的值没有用
*/
static Position InsertElement(int isKey, Position Parent, Position X, KeyType Key, int i, int j){

	global_stats.nodeNum++;
	int k;
	if (isKey){
		/* 插入key */
		if (X->KeyNum == 0)
			X->Key[0] = Key;
		else{
			k = X->KeyNum - 1;
			while (k >= j){
				X->Key[k + 1] = X->Key[k]; k--;
			}

			X->Key[j] = Key;

		}
		if (Parent != NULL)
			Parent->Key[i] = X->Key[0];
		X->KeyNum++;

	}
	else{
		/* 插入节点 */

		/* 对树叶节点进行连接 */
		if (X->Children[0] == NULL){
			if (i > 0)
				Parent->Children[i - 1]->Next = X;
			X->Next = Parent->Children[i];
		}

		k = Parent->KeyNum - 1;
		while (k >= i){
			Parent->Children[k + 1] = Parent->Children[k];
			Parent->Key[k + 1] = Parent->Key[k];
			k--;
		}
		Parent->Key[i] = X->Key[0];
		Parent->Children[i] = X;

		Parent->KeyNum++;

	}
	return X;
}

static BPlusTree SplitNode(Position Parent, Position X, int i){
	int j, k, Limit;
	Position NewNode;

	NewNode = MallocNewNode();

	k = 0;
	j = X->KeyNum / 2;
	Limit = X->KeyNum;
	while (j < Limit){
		if (X->Children[0] != NULL){
			NewNode->Children[k] = X->Children[j];
			X->Children[j] = NULL;
		}
		NewNode->Key[k] = X->Key[j];
		X->Key[j] = NULL;
		NewNode->KeyNum++; X->KeyNum--;
		j++; k++;
	}

	if (Parent != NULL)
		InsertElement(0, Parent, NewNode, Unavailable, i + 1, Unavailable);
	else{
		/* 如果是X是根，那么创建新的根并返回 */
		Parent = MallocNewNode();
		InsertElement(0, Parent, X, Unavailable, 0, Unavailable);
		InsertElement(0, Parent, NewNode, Unavailable, 1, Unavailable);

		return Parent;
	}

	return X;
}

/* 合并节点,X少于M/2关键字，S有大于或等于M/2个关键字*/
static Position MergeNode(Position Parent, Position X, Position S, int i){
	int Limit;

	/* S的关键字数目大于M/2 */
	if (S->KeyNum > LIMIT_M_2){
		/* 从S中移动一个元素到X中 */
		MoveElement(S, X, Parent, i, 1);
	}
	else{
		/* 将X全部元素移动到S中，并把X删除 */
		Limit = X->KeyNum;
		MoveElement(X, S, Parent, i, Limit);
		RemoveElement(0, Parent, X, i, Unavailable);

		free(X);
		X = NULL;
	}

	return Parent;
}

static Position RemoveElement(int isKey, Position Parent, Position X, int i, int j){

	int k, Limit;

	if (isKey){
		Limit = X->KeyNum;
		/* 删除key */
		k = j + 1;
		while (k < Limit){
			X->Key[k - 1] = X->Key[k]; k++;
		}

		X->Key[X->KeyNum - 1] = Unavailable;

		Parent->Key[i] = X->Key[0];

		X->KeyNum--;
	}
	else{
		/* 删除节点 */

		/* 修改树叶节点的链接 */
		if (X->Children[0] == NULL && i > 0){
			Parent->Children[i - 1]->Next = Parent->Children[i + 1];
		}
		Limit = Parent->KeyNum;
		k = i + 1;
		while (k < Limit){
			Parent->Children[k - 1] = Parent->Children[k];
			Parent->Key[k - 1] = Parent->Key[k];
			k++;
		}

		Parent->Children[Parent->KeyNum - 1] = NULL;
		Parent->Key[Parent->KeyNum - 1] = Unavailable;

		Parent->KeyNum--;

	}
	return X;
}

/* Src和Dst是两个相邻的节点，i是Src在Parent中的位置；
将Src的元素移动到Dst中 ,n是移动元素的个数*/
static Position MoveElement(Position Src, Position Dst, Position Parent, int i, int n){
	KeyType TmpKey;
	Position Child;
	int j, SrcInFront;

	SrcInFront = 0;

	if (myCmp(Src->Key[0],Dst->Key[0])<0)
		SrcInFront = 1;

	j = 0;
	/* 节点Src在Dst前面 */
	if (SrcInFront){
		if (Src->Children[0] != NULL){
			while (j < n) {
				Child = Src->Children[Src->KeyNum - 1];
				RemoveElement(0, Src, Child, Src->KeyNum - 1, Unavailable);
				InsertElement(0, Dst, Child, Unavailable, 0, Unavailable);
				j++;
			}
		}
		else{
			while (j < n) {
				TmpKey = Src->Key[Src->KeyNum - 1];
				RemoveElement(1, Parent, Src, i, Src->KeyNum - 1);
				InsertElement(1, Parent, Dst, TmpKey, i + 1, 0);
				j++;
			}

		}

		Parent->Key[i + 1] = Dst->Key[0];
		/* 将树叶节点重新连接 */
		if (Src->KeyNum > 0)
			FindMostRight(Src)->Next = FindMostLeft(Dst);

	}
	else{
		if (Src->Children[0] != NULL){
			while (j < n) {
				Child = Src->Children[0];
				RemoveElement(0, Src, Child, 0, Unavailable);
				InsertElement(0, Dst, Child, Unavailable, Dst->KeyNum, Unavailable);
				j++;
			}

		}
		else{
			while (j < n) {
				TmpKey = Src->Key[0];
				RemoveElement(1, Parent, Src, i, 0);
				InsertElement(1, Parent, Dst, TmpKey, i - 1, Dst->KeyNum);
				j++;
			}

		}

		Parent->Key[i] = Src->Key[0];
		if (Src->KeyNum > 0)
			FindMostRight(Dst)->Next = FindMostLeft(Src);

	}

	return Parent;
}


static BPlusTree RecursiveInsert(BPlusTree T, KeyType Key, int i, BPlusTree Parent){
	int j, Limit;
	Position Sibling;

	/* 查找分支 */
	j = 0;
	while (j < T->KeyNum && myCmp(Key,T->Key[j])>=0){
		/* 重复值不插入 */
		if (myCmp(Key,T->Key[j])==0)
			return T;
		j++;
	}
	if (j != 0 && T->Children[0] != NULL) j--;

	/* 树叶 */
	if (T->Children[0] == NULL)
		T = InsertElement(1, Parent, T, Key, i, j);
	/* 内部节点 */
	else
		T->Children[j] = RecursiveInsert(T->Children[j], Key, j, T);

	/* 调整节点 */

	Limit = M;

	if (T->KeyNum > Limit){
		/* 根 */
		if (Parent == NULL){
			/* 分裂节点 */
			T = SplitNode(Parent, T, i);
		}
		else{
			Sibling = FindSibling(Parent, i);
			if (Sibling != NULL){
				/* 将T的一个元素（Key或者Child）移动的Sibing中 */
				MoveElement(T, Sibling, Parent, i, 1);
			}
			else{
				/* 分裂节点 */
				T = SplitNode(Parent, T, i);
			}
		}

	}

	if (Parent != NULL)
		Parent->Key[i] = T->Key[0];


	return T;
}

/* 插入 */
extern BPlusTree Insert(BPlusTree T, KeyType Key){
	return RecursiveInsert(T, Key, 0, NULL);
}

int Find(BPlusTree T, KeyType Key){
	int j = 0;
	while (j < T->KeyNum && myCmp(Key,T->Key[j])>=0){
		if (myCmp(Key, T->Key[j]) == 0)
			return 1;
		j++;
	}

	if (j == 0)
		return 0;
	else if (T->Children[0] == NULL)
		return 0;
	else{
		Find(T->Children[j-1],Key);
	}

}


int main(){
	global_stats.cmpnum = 0;
	global_stats.mem = 0;
	global_stats.nodeNum = 0;
	FILE *fp;
	BPlusTree head = MallocNewNode();
	global_stats.nodeNum++;
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
		char* nodeData = (char*)bupt_malloc(i + 1);
		memcpy(nodeData, strLine, i + 1);

		head=Insert(head, nodeData);
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
		if (Find(head, nodeData)){
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
	printf("字符比较次数%d\n", global_stats.cmpnum/1000);
	printf("\n");
}