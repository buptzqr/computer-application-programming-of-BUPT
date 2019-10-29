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

/* ���� */
BPlusTree Insert(BPlusTree T, KeyType Key);
/* ���� */
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

/* ���ɽڵ㲢��ʼ�� */
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

/* Ѱ��һ���ֵܽڵ㣬��洢�Ĺؼ���δ�������򷵻�NULL */
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

/* �����ֵܽڵ㣬��ؼ���������M/2 ;û�з���NULL*/
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

/* ��Ҫ��X����Key��ʱ��i��X��Parent��λ�ã�j��KeyҪ�����λ��
��Ҫ��Parent����X�ڵ��ʱ��i��Ҫ�����λ�ã�Key��j��ֵû����
*/
static Position InsertElement(int isKey, Position Parent, Position X, KeyType Key, int i, int j){

	global_stats.nodeNum++;
	int k;
	if (isKey){
		/* ����key */
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
		/* ����ڵ� */

		/* ����Ҷ�ڵ�������� */
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
		/* �����X�Ǹ�����ô�����µĸ������� */
		Parent = MallocNewNode();
		InsertElement(0, Parent, X, Unavailable, 0, Unavailable);
		InsertElement(0, Parent, NewNode, Unavailable, 1, Unavailable);

		return Parent;
	}

	return X;
}

/* �ϲ��ڵ�,X����M/2�ؼ��֣�S�д��ڻ����M/2���ؼ���*/
static Position MergeNode(Position Parent, Position X, Position S, int i){
	int Limit;

	/* S�Ĺؼ�����Ŀ����M/2 */
	if (S->KeyNum > LIMIT_M_2){
		/* ��S���ƶ�һ��Ԫ�ص�X�� */
		MoveElement(S, X, Parent, i, 1);
	}
	else{
		/* ��Xȫ��Ԫ���ƶ���S�У�����Xɾ�� */
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
		/* ɾ��key */
		k = j + 1;
		while (k < Limit){
			X->Key[k - 1] = X->Key[k]; k++;
		}

		X->Key[X->KeyNum - 1] = Unavailable;

		Parent->Key[i] = X->Key[0];

		X->KeyNum--;
	}
	else{
		/* ɾ���ڵ� */

		/* �޸���Ҷ�ڵ������ */
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

/* Src��Dst���������ڵĽڵ㣬i��Src��Parent�е�λ�ã�
��Src��Ԫ���ƶ���Dst�� ,n���ƶ�Ԫ�صĸ���*/
static Position MoveElement(Position Src, Position Dst, Position Parent, int i, int n){
	KeyType TmpKey;
	Position Child;
	int j, SrcInFront;

	SrcInFront = 0;

	if (myCmp(Src->Key[0],Dst->Key[0])<0)
		SrcInFront = 1;

	j = 0;
	/* �ڵ�Src��Dstǰ�� */
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
		/* ����Ҷ�ڵ��������� */
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

	/* ���ҷ�֧ */
	j = 0;
	while (j < T->KeyNum && myCmp(Key,T->Key[j])>=0){
		/* �ظ�ֵ������ */
		if (myCmp(Key,T->Key[j])==0)
			return T;
		j++;
	}
	if (j != 0 && T->Children[0] != NULL) j--;

	/* ��Ҷ */
	if (T->Children[0] == NULL)
		T = InsertElement(1, Parent, T, Key, i, j);
	/* �ڲ��ڵ� */
	else
		T->Children[j] = RecursiveInsert(T->Children[j], Key, j, T);

	/* �����ڵ� */

	Limit = M;

	if (T->KeyNum > Limit){
		/* �� */
		if (Parent == NULL){
			/* ���ѽڵ� */
			T = SplitNode(Parent, T, i);
		}
		else{
			Sibling = FindSibling(Parent, i);
			if (Sibling != NULL){
				/* ��T��һ��Ԫ�أ�Key����Child���ƶ���Sibing�� */
				MoveElement(T, Sibling, Parent, i, 1);
			}
			else{
				/* ���ѽڵ� */
				T = SplitNode(Parent, T, i);
			}
		}

	}

	if (Parent != NULL)
		Parent->Key[i] = T->Key[0];


	return T;
}

/* ���� */
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
		if (Find(head, nodeData)){
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
	printf("�ַ��Ƚϴ���%d\n", global_stats.cmpnum/1000);
	printf("\n");
}