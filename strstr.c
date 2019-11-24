#include <stdio.h>
#include <string.h>
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
int main(){
	global_stats.cmpnum = 0;
	global_stats.mem = 0;
	FILE *fp;
	int strLen = 0;
	char* strLine = (char*)bupt_malloc(900 * 1024 * 1024);
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
		int strIdx = 0;
		int occNum = 0;
		while (strLen - strIdx >= patternLen){
			int patternIdx = 0;
			for (; patternIdx < patternLen; patternIdx++)
			{
				if (strLine[strIdx + patternIdx] != patternLine[patternIdx])
					break;
			}
			global_stats.cmpnum += patternIdx;
			if (patternIdx == patternLen){
				occNum++;
			}
			strIdx++;
		}
		fprintf(fp1, "%s", patternLine);
		fprintf(fp1, "%c", ',');
		fprintf(fp1, "%d", occNum);
		fprintf(fp1, "%c", '\n');
	}
	fprintf(fp1, "%s", "字符/字节比较次数:");
	fprintf(fp1, "%d", global_stats.cmpnum/1000);
	fprintf(fp1, "%c", '\n');
	fprintf(fp1, "%s", "内存开销:");
	fprintf(fp1, "%d", global_stats.mem/1000);
	fclose(fp);
	fclose(fp1);	
	return 0;
}