#include <stdio.h>
#include<string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

typedef struct Emp
{
    int ID;
    char name[50];
    char des[50];
    struct Emp *reports[50];
    int repi;
    int reportingManager;

} Emp;

void displayReportingEmp(Emp **reports, int repi, int spaces, int *count)
{
    if (repi == 0)
        return;

    int size = repi;

    

    for (int i = 0; i < size; i++)
    {

        Emp *underEmp = reports[i];

        (*count)++;

        for (int sp = 0; sp < spaces; sp++)
        {
            printf(" ");
        }
        printf("%d - %s - %s\n", underEmp->ID, underEmp->name, underEmp->des);

        displayReportingEmp(underEmp->reports, underEmp->repi, spaces + 3, count);
    }
}

Emp *findUnderEmpID(Emp **reports, int size, int empid)
{
    if (size == 0)
        return NULL;

    for (int i = 0; i < size; i++)
    {
        Emp *currEmp = reports[i];
        if (currEmp->ID == empid)
            return currEmp;

        Emp *found;

        found = findUnderEmpID(currEmp->reports, currEmp->repi, empid);

        if (found != NULL)
            return found;
    }

    return NULL;
}

Emp *findID(Emp **empList, int empi, int empid)
{
    int size = empi;
    for (int i = 0; i < size; i++)
    {
        Emp *ceo = empList[i];
        if (ceo->ID == empid)
        {
            return ceo;
        }

        int underEmpSize = ceo->repi;

        Emp *found = NULL;
        found = findUnderEmpID(ceo->reports, underEmpSize, empid);
        if (found != NULL)
            return found;
    }
    return NULL;
}

void printEmpIDHiearchy(Emp *targetEmp, int *count)
{
    printf("%d %s %s -> %d\n", targetEmp->ID, targetEmp->name, targetEmp->des, targetEmp->reportingManager);

    displayReportingEmp(targetEmp->reports, targetEmp->repi, 3, count);
}

void removeEmp(Emp *targetEmp, Emp *reportingManagerOfTarget)
{

    // find out siblings of targetemp and tranfer the under emps of target to emp sibling who has lowest under emps (repi);

    if (reportingManagerOfTarget->repi == 1)
    {
        // target emp has no siblings so transfer all emps working under target to target reportingManager
        reportingManagerOfTarget->repi = 0;

        int sizeofEmpsUnderTarget = targetEmp->repi;
        for (int i = 0; i < sizeofEmpsUnderTarget; i++)
        {
            reportingManagerOfTarget->reports[reportingManagerOfTarget->repi++] = targetEmp->reports[i];
        }
    }
    else
    {
        // targetEmp has siblings
        // find the siblings who has lowest reporting emps under him
        int minEmps = INT_MAX;
        Emp *sibling;
        int totalReportingEmps = reportingManagerOfTarget->repi;
        for (int i = 0; i < totalReportingEmps; i++)
        {
            if (reportingManagerOfTarget->reports[i] == targetEmp)
                continue;
            if (minEmps > reportingManagerOfTarget->reports[i]->repi)
            {
                minEmps = reportingManagerOfTarget->reports[i]->repi;

                sibling = reportingManagerOfTarget->reports[i];
            }
        }

        // transfer all underemp of target to now its sibling
        for (int i = 0; i < targetEmp->repi; i++)
        {
            sibling->reports[sibling->repi++] = targetEmp->reports[i];
        }

        // remove targetEmp idx from reportingManager reports
        int i = 0;
        while (i < totalReportingEmps)
        {
            if (reportingManagerOfTarget->reports[i] == targetEmp)
            {
                break;
            }
            i++;
        }

        // move idx of targetEMp to last and do reportingManageremp->repi--
        while (i < totalReportingEmps - 1)
        {
            int j = i + 1;

            Emp *temp = reportingManagerOfTarget->reports[i];
            reportingManagerOfTarget->reports[i] = reportingManagerOfTarget->reports[j];
            reportingManagerOfTarget->reports[j] = temp;
            i++;
        }
        reportingManagerOfTarget->repi--;
    }
    printf("Employee removed successfully !\n\n");
}

void displayAll(Emp **empList, int *empi, int *count)
{
    int size = *empi;

    printf("Output\n");
    for (int i = 0; i < size; i++)
    {
        Emp *e = empList[i];

        (*count)++;
        printf("%d - %s - %s", e->ID, e->name, e->des);
        if (e->reportingManager != -1)
            printf("-> %d", e->reportingManager);
        printf("\n");

        // godeep
        int spaces = 3;
        displayReportingEmp(e->reports, e->repi, spaces, count);
    }
}

void addemp(Emp **empList, int *empi)
{
    int idx = *empi;

    Emp *e = (Emp *)malloc(sizeof(Emp));

    e->repi = 0; // emp list as new emp doesnt have a team under him
    // take details of emp

    printf("Enter ID : ");
    scanf("%d", &e->ID);

    printf("\nEnter name : ");
    scanf("%s", e->name);

    while (getchar() != '\n');

    printf("\nEnter Designation : ");
    fgets(e->des,sizeof(e->des),stdin);
    //remove last \n
    int size = strlen(e->des);
    if(e->des[size-1]=='\n')
        e->des[size - 1] = '\0';

    printf("\nEnter the BOSS ID , if not enter -1 : ");
    scanf("%d", &e->reportingManager);

    // now if reportingManager id is -1 then push it in emplist as this list is of emp who are at top of hierachy and doesnt work in someone under
    if (e->reportingManager == -1)
    {
        empList[idx++] = e;
        *empi = idx;
        return;
    }

    Emp *reportingManagerEmp;

    reportingManagerEmp = findID(empList, *empi, e->reportingManager);
    if (reportingManagerEmp != NULL)
    {
        printf("reportingManager found \n");
        reportingManagerEmp->reports[reportingManagerEmp->repi++] = e;
    }

    else if (reportingManagerEmp == NULL)
    {
        empList[idx++] = e;
        *empi = idx;
    }
}

int main()
{
    Emp *empList[50];
    int empi = 0;

    printf("----Employee Hiearchy Management----\n");

    while (true)
    {
        printf("1. Add Employee\n");
        printf("2. Display All\n");
        printf("3. Display Employee Hiearchy\n");
        printf("4. Remove Employee\n");
        int option;
        scanf("%d", &option);

        switch (option)
        {
            case 1:
                addemp(empList, &empi);
                printf("Added Successfully\n\n");
                break;

            case 2:
            {
                int count = 0;
                displayAll(empList, &empi, &count);
                printf("Total Employees : %d\n\n", count);
                break;
            }

            case 3:
            {
                int empid;
                printf("Enter ID of Employee you wish to see Hierarchy : \n");
                scanf("%d", &empid);

                Emp *targetEmp = findID(empList, empi, empid);
                int count = 0;

                if (targetEmp != NULL)
                {
                    // Found the address of targetEmp
                    printEmpIDHiearchy(targetEmp, &count);
                    printf("Total Employees under %d : %d\n\n", empid, count);
                }
                else
                {
                    printf("Enter a valid ID !\n");
                }
                break;
            }

            case 4:
            {
                int targetID;
                printf("Enter ID you wish to delete : ");
                scanf("%d", &targetID);

                // First, find the employee to delete
                Emp *targetEmp = findID(empList, empi, targetID);

                if (targetEmp == NULL)
                {
                    printf("Please enter valid ID\n");
                    continue;
                }

                if (targetEmp->reportingManager == -1)
                {
                    printf("The CEO can't be removed !\n\n");
                    continue;
                }

                int targetreportingManagerID = targetEmp->reportingManager;

                Emp *reportingManagerOfTarget =
                    findID(empList, empi, targetreportingManagerID);

                removeEmp(targetEmp, reportingManagerOfTarget);
                break;
            }

            default:
                printf("Please select a valid option !\n");
                return 0;
        }
    }
}

