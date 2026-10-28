#include <stdio.h>
#include <stdlib.h>

void generateRandomMatrix(int size, int arr[][size]){
    for(int i=0; i < size; i++){
        for(int j=0; j < size; j++){
            *(*(arr + i) + j) = rand()%255;
        }
    }
}


void rotateMatrix(int size, int arr[][size]) {
    for (int i = 0; i < size; i++) {
        for (int j = i + 1; j < size; j++) {
            int temp = *(*(arr + i) + j);
            *(*(arr + i) + j) = *(*(arr + j) + i);
            *(*(arr + j) + i) = temp;
        }
    }
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size / 2; j++) {
            int temp = *(*(arr + i) + j);
            *(*(arr + i) + j) = *(*(arr + i) + size - 1 - j);
            *(*(arr + i) + size - 1 - j) = temp;
        }
    }
}


void printMatrix(int size, int arr[][size]){
    for(int i=0; i < size; i++){
        for(int j=0; j < size; j++){
            printf("%3d ", *(*(arr + i) + j));
        }
        printf("\n");
    }
}

void smoothingFilter(int size, int arr[][size]){
    int copy[size][size];
    for(int i=0; i < size; i++){
        for(int j=0; j < size; j++){
            *(*(copy + i) + j)=*(*(arr + i) + j);
        }
    }
    for(int i=0; i < size; i++){
        for(int j=0; j < size; j++){
           int sum = 0;
           int count = 0;
           for (int x = i - 1; x <= i + 1; x++){
               for (int y = j - 1; y <= j + 1; y++){
                    if (x >= 0 && x < size &&
                        y >= 0 && y < size){
                        sum += *(*(arr + x) + y);
                        count++;
                    }
                }
            }
            *(*(arr + i) + j) = sum/count;
        }
    }
}

int main(){
    int size;
    printf("Enter Size of Matrix: ");
    scanf("%d", &size);
    int arr[size][size];
    generateRandomMatrix(size,arr);
    printf("Random Matrix: \n");
    printMatrix(size,arr);
    rotateMatrix(size,arr);
    printf("Rotated Matrix: \n");
    printMatrix(size,arr);
    smoothingFilter(size,arr);
    printf("Smmothing Filter applied: \n");
    printMatrix(size,arr);
}

