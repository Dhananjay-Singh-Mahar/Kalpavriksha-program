#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

int main() {
	
    int n;
    scanf("%d", &n);
    int temp = n;
    int sumOfDigits = 0;
    while(temp > 0){
        int lastDigit = temp%10;
        sumOfDigits += lastDigit;
        temp /= 10;
    }
    
    printf("%d",sumOfDigits);
…    return 0;
    
}