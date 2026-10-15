#include <stdio.h>
#include <string.h>

#define STANDARD 150
#define PREMIUM 250
#define RECLINER 400

int main(){
    char name[50];
    int numberOfTickets=0;
    int ticketCategoryChoice;
    char ticketCategory[20];
    float subTotal=0;
    float discount=0;
    float finalBill=0;

    printf("Enter Customer Name: ");
    scanf("%49[^\n]", name);
    printf("Enter Number of Tickets to Buy: ");
    scanf("%d", &numberOfTickets);
    if(numberOfTickets < 1 || numberOfTickets > 20){
        printf("Number of Tickets should be in between 1 - 20");
        return 1;
    } 
    printf("Enter Choice for Ticket Category: ");
    scanf("%d", &ticketCategoryChoice);

    switch(ticketCategoryChoice){
        case 1:
                strcpy(ticketCategory, "Standard");;
                subTotal += numberOfTickets*STANDARD; 
                break;
        case 2:
                strcpy(ticketCategory, "Premium");;
                subTotal += numberOfTickets*PREMIUM; 
                break;
        case 3:
                strcpy(ticketCategory, "Recliner");;
                subTotal += numberOfTickets*RECLINER;
                break;
        default:
                printf("Ticket Category Selected is Invalid"); 
    }

    if(numberOfTickets >= 5){
        discount += subTotal/10;
    }

    finalBill = subTotal - discount;

    printf("Customer: %s\n", name);
    printf("Ticket Category: %s\n", ticketCategory);
    printf("Tickets: %d\n", numberOfTickets);
    printf("Subtotal: %d\n", subTotal);
    printf("Discount: %.2lf\n", discount);
    printf("Final Bill: %.2lf\n", finalBill);
}