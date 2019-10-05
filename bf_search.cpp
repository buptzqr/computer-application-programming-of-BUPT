#include <iostream>
#include <fstream>
#include <string>
using namespace std;
#define SHIFT 3  
#define MASK 0x7 
#define MAXBIT 18225426

struct Statistic{
	int mem;
	int cmpnum;
};
Statistic global_stats;

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

char *init_bitarray(int size){
	char *tmp;

	tmp = (char*)bupt_malloc(size / 8 + 1);
	memset(tmp, 0, (size / 8 + 1)); //initial to 0  

	return tmp;
}

void add_to_bitarray(char *bitarr, unsigned int num){   
	bitarr[num >> SHIFT] |= (1 << (num & MASK));  
}

int is_in_bitarray(char *bitarr, int num){
	global_stats.cmpnum++;
	return bitarr[num >> SHIFT] & (1 << (num & MASK));
}

unsigned int RSHash(char* str, unsigned int len);
unsigned int JSHash(char* str, unsigned int len);
unsigned int PJWHash(char* str, unsigned int len);
unsigned int ELFHash(char* str, unsigned int len);
unsigned int BKDRHash(char* str, unsigned int len);
unsigned int SDBMHash(char* str, unsigned int len);
unsigned int DJBHash(char* str, unsigned int len);
unsigned int DEKHash(char* str, unsigned int len);
unsigned int BPHash(char* str, unsigned int len);
unsigned int FNVHash(char* str, unsigned int len);
int main(){
	char* hashTable = init_bitarray(MAXBIT);
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
		while (strLine[i] != '\n'&&strLine[i]!='\0'){
			i++;
		}
		unsigned int id1;
		id1= RSHash(strLine, i) % MAXBIT;
		add_to_bitarray(hashTable, id1);
		id1 = JSHash(strLine, i) % MAXBIT;
		add_to_bitarray(hashTable, id1);
		id1 = PJWHash(strLine, i) % MAXBIT;
		add_to_bitarray(hashTable, id1);
		id1 = ELFHash(strLine, i) % MAXBIT;
		add_to_bitarray(hashTable, id1);
		id1 = BKDRHash(strLine, i) % MAXBIT;
		add_to_bitarray(hashTable, id1);
		id1 = SDBMHash(strLine, i) % MAXBIT;
		add_to_bitarray(hashTable, id1);
		id1 = DJBHash(strLine, i) % MAXBIT;
		add_to_bitarray(hashTable, id1);
		id1 = DEKHash(strLine, i) % MAXBIT;
		add_to_bitarray(hashTable, id1);
		id1 = BPHash(strLine, i) % MAXBIT;
		add_to_bitarray(hashTable, id1);
		id1 = FNVHash(strLine, i) % MAXBIT;
		add_to_bitarray(hashTable, id1);
		
	}
	fclose(fp);

	int wordsNum = 0;
	int wordsSuccess = 0;
	if ((fp = fopen("./words-98w.txt", "r")) == NULL) //判断文件是否存在及可读
	{
		printf("error!");
		return -1;
	}
	while (!feof(fp)){
		wordsNum++;
		char strLine[100] = { '\0' };
		fgets(strLine, 1024, fp);
		int i = 0;
		while (strLine[i] != '\n'&&strLine[i] != '\0'){
			i++;
		}
		int flag = 0;
		unsigned int id1 = RSHash(strLine, i) % MAXBIT;
		if ( is_in_bitarray(hashTable,id1))
			flag++;
		id1 = JSHash(strLine, i) % MAXBIT;
		if (is_in_bitarray(hashTable, id1))
			flag++;
		id1 = PJWHash(strLine, i) % MAXBIT;
		if (is_in_bitarray(hashTable, id1))
			flag++;
		id1 = ELFHash(strLine, i) % MAXBIT;
		if (is_in_bitarray(hashTable, id1))
			flag++;
		id1 = BKDRHash(strLine, i) % MAXBIT;
		if (is_in_bitarray(hashTable, id1))
			flag++;
		id1 = SDBMHash(strLine, i) % MAXBIT;
		if (is_in_bitarray(hashTable, id1))
			flag++;
		id1 = DJBHash(strLine, i) % MAXBIT;
		if (is_in_bitarray(hashTable, id1))
			flag++;
		id1 = DEKHash(strLine, i) % MAXBIT;
		if (is_in_bitarray(hashTable, id1))
			flag++;
		id1 = BPHash(strLine, i) % MAXBIT;
		if (is_in_bitarray(hashTable, id1))
			flag++;
		id1 = FNVHash(strLine, i) % MAXBIT;
		if (is_in_bitarray(hashTable, id1))
			flag++;
		if (flag == 10){
			printf("%s yes\n", strLine);
			wordsSuccess++;
		}
		
	}
	fclose(fp);//关闭文件
	printf("%d KB\n", global_stats.mem / 1000);
	printf("%d 次\n", global_stats.cmpnum);
	printf("words num:%d\n", wordsNum);
	printf("words sucdess:%d\n", wordsSuccess);
	cin.get();
	return 0;
}
unsigned int RSHash(char* str, unsigned int len) {
	unsigned int b = 378551;
	unsigned int a = 63689;
	unsigned int hash = 0;
	unsigned int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash = hash*a + (*str);
		a = a*b;
	}
	return hash;
}
/* End Of RS Hash Function */


unsigned int JSHash(char* str, unsigned int len)
{
	unsigned int hash = 1315423911;
	unsigned int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash ^= ((hash << 5) + (*str) + (hash >> 2));
	}
	return hash;
}
/* End Of JS Hash Function */


unsigned int PJWHash(char* str, unsigned int len)
{
	const unsigned int BitsInUnsignedInt = (unsigned int)(sizeof(unsigned int) * 8);
	const unsigned int ThreeQuarters = (unsigned int)((BitsInUnsignedInt * 3) / 4);
	const unsigned int OneEighth = (unsigned int)(BitsInUnsignedInt / 8);
	const unsigned int HighBits = (unsigned int)(0xFFFFFFFF) << (BitsInUnsignedInt - OneEighth);
	unsigned int hash = 0;
	unsigned int test = 0;
	unsigned int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash = (hash << OneEighth) + (*str);
		if ((test = hash & HighBits) != 0) {
			hash = ((hash ^ (test >> ThreeQuarters)) & (~HighBits));
		}
	}

	return hash;
}
/* End Of  P. J. Weinberger Hash Function */


unsigned int ELFHash(char* str, unsigned int len)
{
	unsigned int hash = 0;
	unsigned int x = 0;
	unsigned int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash = (hash << 4) + (*str);
		if ((x = hash & 0xF0000000L) != 0) {
			hash ^= (x >> 24);
		}
		hash &= ~x;
	}
	return hash;
}
/* End Of ELF Hash Function */


unsigned int BKDRHash(char* str, unsigned int len)
{
	unsigned int seed = 131; /* 31 131 1313 13131 131313 etc.. */
	unsigned int hash = 0;
	unsigned int i = 0;

	for (i = 0; i < len; str++, i++)
	{
		hash = (hash * seed) + (*str);
	}

	return hash;
}
/* End Of BKDR Hash Function */


unsigned int SDBMHash(char* str, unsigned int len)
{
	unsigned int hash = 0;
	unsigned int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash = (*str) + (hash << 6) + (hash << 16) - hash;
	}

	return hash;
}
/* End Of SDBM Hash Function */


unsigned int DJBHash(char* str, unsigned int len)
{
	unsigned int hash = 5381;
	unsigned int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash = ((hash << 5) + hash) + (*str);
	}

	return hash;
}
/* End Of DJB Hash Function */


unsigned int DEKHash(char* str, unsigned int len)
{
	unsigned int hash = len;
	unsigned int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash = ((hash << 5) ^ (hash >> 27)) ^ (*str);
	}
	return hash;
}
/* End Of DEK Hash Function */


unsigned int BPHash(char* str, unsigned int len)
{
	unsigned int hash = 0;
	unsigned int i = 0;
	for (i = 0; i < len; str++, i++) {
		hash = hash << 7 ^ (*str);
	}

	return hash;
}
/* End Of BP Hash Function */


unsigned int FNVHash(char* str, unsigned int len)
{
	const unsigned int fnv_prime = 0x811C9DC5;
	unsigned int hash = 0;
	unsigned int i = 0;

	for (i = 0; i < len; str++, i++) {
		hash *= fnv_prime;
		hash ^= (*str);
	}

	return hash;
}
/* End Of FNV Hash Function */


