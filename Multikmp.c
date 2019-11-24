#include <stdio.h>
#include <string.h>
#include <stdlib.h>
typedef struct Statistic{
	int mem;
	long long cmpnum;
}Statistic;

Statistic global_stats;
void* bupt_malloc(size_t size){
	if (size <= 0) {
		return NULL;
	}
	global_stats.mem += size;
	return malloc(size);
}
void getnext(char *p, int *next)
{
	int len = strlen(p);
	int k = -1;
	int j = 0;
	next[0] = -1;
	while (j < len - 1)
	{
		if (k == -1 || p[k] == p[j])
		{
			k++;
			j++;
			next[j] = k;
		}
		else{
			k = next[k];
		}
	}
}
int kmp(char *s, char *p, int *next)
{
	int i = 0;
	int j = 0;
	int sLen = strlen(s);
	int pLen = strlen(p);
	int occNum = 0;
	while (i < sLen){
		if (j >= pLen){
			occNum++;
			j = 0;
		}
		if (j == -1 || s[i] == p[j]){
			i++;
			j++;
			global_stats.cmpnum++;
		}
		else{     
			j = next[j];
		}
	}
	if (j >= pLen){
		occNum++;
	}
	return occNum;
}
int main(){
	global_stats.cmpnum = 0;
	global_stats.mem = 0;
	FILE *fp;
	int strLen = 0;
	char* strLine = (char*)bupt_malloc(900*1024*1024);
	if ((fp = fopen("./string.txt", "r")) == NULL){
		return -1;
	}
	while (!feof(fp)){
		fgets(strLine, 900 * 1024 * 1024, fp);
		while (strLine[strLen] != '\n'&&strLine[strLen] != '\0'){
			strLen++;
		}
		strLine[strLen] = '\0';
	}
	fclose(fp);
	if ((fp = fopen("./pattern_bf_kmp.txt", "r")) == NULL){
		return -1;
	}
	FILE* fp1 = fopen("./result.txt", "w");
	while (!feof(fp)){
		char patternLine[100] = { '\0' };
		fgets(patternLine, 100, fp);
		int patternLen = 0;
		while (patternLine[patternLen] != '\n'&&patternLine[patternLen] != '\0'){
			patternLen++;
		}
		patternLine[patternLen] = '\0';
		int *patternNext = (int*)bupt_malloc(sizeof(int)*patternLen);
		getnext(patternLine, patternNext);
		int occNum = kmp(strLine, patternLine, patternNext);
		fprintf(fp1, "%s", patternLine);
		fprintf(fp1, "%c", ',');
		fprintf(fp1, "%d", occNum);
		fprintf(fp1, "%c", '\n');
	}
	fprintf(fp1, "%s", "字符/字节比较次数:");
	fprintf(fp1, "%d", global_stats.cmpnum / 1000);
	fprintf(fp1, "%c", '\n');
	fprintf(fp1, "%s", "内存开销:");
	fprintf(fp1, "%d", global_stats.mem / 1000);
	fclose(fp);
	fclose(fp1);
	/*int next[50], n;
	char s[100] = "受力过程同图5非团截面机构受  O二一刀书炭=机构子刀{V机构256";
	char p[50] = "机构";
	printf("\n实现如下:");
	printf("\n s[] =%s:", s);
	printf("\n p[] =%s:", p);
	getnext(p, next);
	n = kmp(s, p, next);
	printf("\n匹配的位置为 %d", n);*/
	return 0;

}

