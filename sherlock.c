#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* readline();

char* isValid(char* s) {
    int charCount[26]={0};
    for(int i=0; s[i]!='\0'; i++){
        charCount[s[i]-'a']++;
    }
    int min=INT_MAX;
    int max=0;
    int minCount=0;
    int maxCount=0;
    for(int i=0;i<26;i++){
        if(charCount[i]==0) continue;
        min=min>charCount[i]?charCount[i]:min;
        max=max<charCount[i]?charCount[i]:max;
    }
    for(int i=0;i<26;i++){
        if(charCount[i]==0) continue;
        if(charCount[i]==min) minCount++;
        if(charCount[i]==max) maxCount++;
    }
    if(min==max) return "YES";
    if(min==1 && minCount==1){
        for(int i=0; i<26; i++){
            if(charCount[i]==0||charCount[i]==min||charCount[i]==max) continue;
            return "NO";
        }
        return "YES";
    } 
    if(max==min+1 && maxCount==1) return "YES";
      
    return "NO";
}

int main()
{
    FILE* fptr = fopen(getenv("OUTPUT_PATH"), "w");

    char* s = readline();

    char* result = isValid(s);

    fprintf(fptr, "%s\n", result);

    fclose(fptr);

    return 0;
}

char* readline() {
    size_t alloc_length = 1024;
    size_t data_length = 0;

    char* data = malloc(alloc_length);

    while (true) {
        char* cursor = data + data_length;
        char* line = fgets(cursor, alloc_length - data_length, stdin);

        if (!line) {
            break;
        }

        data_length += strlen(cursor);

        if (data_length < alloc_length - 1 || data[data_length - 1] == '\n') {
            break;
        }

        alloc_length <<= 1;

        data = realloc(data, alloc_length);

        if (!data) {
            data = '\0';

            break;
        }
    }

    if (data[data_length - 1] == '\n') {
        data[data_length - 1] = '\0';

        data = realloc(data, data_length);

        if (!data) {
            data = '\0';
        }
    } else {
        data = realloc(data, data_length + 1);

        if (!data) {
            data = '\0';
        } else {
            data[data_length] = '\0';
        }
    }

    return data;
}

