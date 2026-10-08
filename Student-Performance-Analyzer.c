#include <stdio.h>
#include <string.h>

struct Student{
    char name[50];
    int rollNo;
    float marks[3];
    float totalMarks;
    float averageMarks;
    char grade;
};

void inputStudentDetails(struct Student student[],int numberOfStudents){
    for(int studentIndex=0; studentIndex < numberOfStudents; studentIndex++){
            printf("Enter Name of student\n");
            getchar();
            fgets(student[studentIndex].name, 50, stdin);
            printf("Enter rollNo of student %d: \n",studentIndex+1);
            scanf("%d", &student[studentIndex].rollNo);
            printf("Enter the marks obatined in three subject\n");
            scanf("%f %f %f", &student[studentIndex].marks[0],
                              &student[studentIndex].marks[1],
                              &student[studentIndex].marks[2]);
            
            
            student[studentIndex].totalMarks= student[studentIndex].marks[0]+
                                              student[studentIndex].marks[1]+
                                              student[studentIndex].marks[2];
            
            student[studentIndex].averageMarks= student[studentIndex].totalMarks/3;

            if (student[studentIndex].averageMarks >= 85) {
                student[studentIndex].grade = 'A';
            }
            else if (student[studentIndex].averageMarks >= 70) {            
                student[studentIndex].grade = 'B';
            }
            else if (student[studentIndex].averageMarks >= 50) {            
                student[studentIndex].grade = 'C';
            }
            else if (student[studentIndex].averageMarks >= 35) {
                student[studentIndex].grade = 'D';
            }
            else {
                student[studentIndex].grade = 'F';
            }
                
    }
}

void displayStudentPerformance(struct Student student[],int studentIndex){
        
        switch(student[studentIndex].grade){
            case 'A':
                        printf("Performance: * * * * *\n");
                        break;
            case 'B':
                       
                        printf("Performance: * * * *\n");
                        break;
            case 'C':
                        
                        printf("Performance: * * *\n");
                        break;
            case 'D':
                        
                        printf("Performance: * *\n");
                        break;
            default:
                        break;
        }
}

void printStudentDetails(struct Student student[], int numberOfStudents){
    for(int studentIndex=0; studentIndex < numberOfStudents; studentIndex++){
        printf("Roll: %d\n", student[studentIndex].rollNo);
        printf("Name: %s\n", student[studentIndex].name);
        printf("Total: %.2f \n", student[studentIndex].totalMarks);
        printf("Average: %.2f \n", student[studentIndex].averageMarks);
        printf("Grade: %c \n", student[studentIndex].grade);
        displayStudentPerformance(student,studentIndex);
        printf("\n");
    }
}

void printRollNumberRecursively(int studentIndex,int numberOfStudents,struct Student student[]){
    if(studentIndex>=numberOfStudents){
        return;
    }
    printf("%d\n", student[studentIndex].rollNo);
    printRollNumberRecursively(studentIndex+1,numberOfStudents,student);
}

int main(){
    printf("Enter number of students\n");
    int numberOfStudents;
    scanf("%d", &numberOfStudents);
    if(numberOfStudents<1 || numberOfStudents>100){
        printf("Invalid number of student");
        return 1;
    }

    struct Student student[numberOfStudents];
    printf("Enter Student Details\n\n");

    inputStudentDetails(student, numberOfStudents);

    printf("Printing Student Details\n");
    printStudentDetails(student, numberOfStudents);

    printf("Printing Roll NUmbers Recursively\n");
    printRollNumberRecursively(0, numberOfStudents, student);

    return 0;
}