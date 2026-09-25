#include<stdio.h>
#include<ctype.h>

#define SIZE 1000

int operand[SIZE];
int operandTop=-1;

char operator[SIZE];
int operatorTop=-1;

void pushOperand(int value){
    operand[++operandTop]=value;
}
int popOperand(){
    return operand[operandTop--];
}

void pushOperator(char value){
    operator[++operatorTop]=value;
}
char popOperator(){
    return operator[operatorTop--];
}
char peekOperator(){
    return operator[operatorTop];
}

int precedence(char op){
    if(op=='+'||op=='-') return 1;
    if(op=='*'||op=='/') return 2;
    return 0;
}
int main(){
    char exp[SIZE];
    printf("Enter infix expression");
    fgets(exp,SIZE,stdin);

    int i=0;

    while(exp[i]!='\0'){
        if(isspace(exp[i])){
            i++;
            continue;
        }

        if(isdigit(exp[i])){
            int number=0;
            while(isdigit(exp[i])){
                number=number*10+(exp[i]-'0');
                i++;
            }
            pushOperand(number);
        }
        else{
            if(operatorTop==-1||precedence(peekOperator())<precedence(exp[i])){
                pushOperator(exp[i]);
                i++;
                continue;
            } 
            else{
                while(operatorTop!=-1&&precedence(peekOperator())>=precedence(exp[i])){
                    char op=popOperator();
                    int a=popOperand();
                    int b=popOperand();

                    switch(op){
                        case '+': pushOperand(a+b);
                                  break;
                        case '-': pushOperand(b-a);
                                  break;
                        case '*': pushOperand(a*b);
                                  break;
                        case '/': if(a==0){
                                    printf("Error");
                                    return 0;
                                  }
                                  pushOperand(b/a);
                                  break;
                    }
                }
                pushOperator(exp[i]);
                i++;
            }
        }

    }
    while(operatorTop != -1){
        char op = popOperator();
        int a = popOperand();
        int b = popOperand();

        switch(op){
            case '+': pushOperand(b+a);
                    break;

            case '-': pushOperand(b-a);
                    break;

            case '*': pushOperand(b*a);
                    break;

            case '/': if(a==0){
                        printf("Error");
                        return 0;
                    }
                    pushOperand(b/a);
                    break;
        }
    }
    printf("value of infix expression is %d", popOperand());
    return 0;
}