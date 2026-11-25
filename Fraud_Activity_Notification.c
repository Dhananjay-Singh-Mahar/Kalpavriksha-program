#include<stdio.h>
#include<stdlib.h>

int activityNotifications(int expenditure_count, int* expenditure, int d) {
    
    int count = 0;
    int freq[201] = {0};

    for(int i = 0; i < d; i++) {
        freq[expenditure[i]]++;
    }
    
    for(int i = d; i < expenditure_count; i++) {
        int twiceMedian = 0;
        int cumulativeCount = 0;
      
        for(int j = 0; j <= 200; j++) {
            cumulativeCount += freq[j];

            if(d % 2 == 1) {
                if(cumulativeCount >= d/2 + 1) {
                    twiceMedian = 2 * j;
                    break;
                }
            }
            else {
                if(cumulativeCount >= d/2 && cumulativeCount - freq[j] < d/2) {
                    twiceMedian = j;
                }
                if(cumulativeCount >= d/2 + 1 && cumulativeCount - freq[j] < d/2 + 1) {
                    twiceMedian += j;
                    break;
                }
            }
        }
        if(expenditure[i] >= twiceMedian) {
            count++;
        }
        freq[expenditure[i - d]]--;
        freq[expenditure[i]]++;
    }
    return count;
}

int main(){
    int expenditure_size, trailing_days;

    scanf("%d  %d", &expenditure_size, &trailing_days);
    int *exp = malloc( expenditure_size* sizeof(int));
    if (exp ==NULL) return 1;

    for (int i=0; i<expenditure_size; i++){
            scanf("%d", &exp[i]);
    }
    int result = activityNotifications(expenditure_size, exp, trailing_days);
    printf("%d", result);

    free(exp);
    return 0;
}