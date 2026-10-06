#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "users.txt"

struct User{
    int id;
    char name[50];
    int age;
};

void createFile(){
    FILE *file=fopen(FILE_NAME,"a");
    if(file==NULL){
        printf("Error: Creating File");
        return;
    }
    fclose(file);
}

void createUser(){
    FILE *file=fopen(FILE_NAME,"a");
    if(file==NULL){
        printf("Error: Creating File");
        return;
    }
    int n=0;
    printf("Enter no of records to be enter");
    scanf("%d",&n);
    struct User user[n];
    for(int i=0;i<n;i++){
        printf("Enter user id");
        scanf("%d",&user[i].id);
        getchar();
        printf("Enter user name");
        fgets(user[i].name,50,stdin);
        user[i].name[strcspn(user[i].name, "\n")] = '\0';
        printf("Enter user age");
        scanf("%d",&user[i].age);

        fprintf(file, "%d|%s|%d\n", user[i].id,user[i].name,user[i].age);
    }
    
    fclose(file);
    printf("Record added successfully");
}

void readUser(){
    FILE *file=fopen(FILE_NAME,"r");
    if(file==NULL){
        printf("Error: Creating File");
        return;
    }
    struct User user;
    while(fscanf(file,"%d|%49[^|]|%d\n",&user.id,user.name,&user.age)==3){
        printf("ID: %d\n",user.id);
        printf("Name: %s\n",user.name);
        printf("Age: %d\n",user.age);
    }
    fclose(file);
    printf("Records read successfully");
}

void updateUser(){
    FILE *file=fopen(FILE_NAME,"r");
    FILE *temp=fopen("temp.txt","w");
    if(file==NULL){
        printf("Error: Creating File");
        return;
    }
    if(temp==NULL){
        printf("Error: Creating File");
        return;
    }
    struct User user;
    int found=0;
    int id;
    printf("Enter ID whose data to be modified");
    scanf("%d",&id);
    while(fscanf(file,"%d|%49[^|]|%d",&user.id,user.name,&user.age)==3){
        if(user.id==id){
            found=1;
            getchar();
            printf("Enter new name: ");
            fgets(user.name, 50, stdin);
            user.name[strcspn(user.name, "\n")] = '\0';
            printf("Enter new age");
            scanf("%d",&user.age);
        }
        fprintf(temp,"%d|%s|%d\n",user.id,user.name,user.age);
    }
    fclose(file);
    fclose(temp);
    remove(FILE_NAME);
    rename("temp.txt",FILE_NAME);
}

void deleteUser(){
     FILE *file=fopen(FILE_NAME,"r");
    FILE *temp=fopen("temp.txt","w");
    if(file==NULL){
        printf("Error: Creating File");
        return;
    }
    if(temp==NULL){
        printf("Error: Creating File");
        return;
    }
    struct User user;
    int found=0;
    int id;
    printf("Enter ID whose data to be delete");
    scanf("%d",&id);
    while(fscanf(file,"%d|%49[^|]|%d",&user.id,user.name,&user.age)==3){
        if(user.id==id){
            found=1;
            continue;
        }
        fprintf(temp,"%d|%s|%d\n",user.id,user.name,user.age);
    }
    if(found==0) printf("Record not found");
    else printf("Record Deleted Duccessfully\n");
    fclose(file);
    fclose(temp);
    remove(FILE_NAME);
    rename("temp.txt",FILE_NAME);
}
int main(){
    createFile();
    //createUser();
    deleteUser();
    readUser();
}