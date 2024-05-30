#include <stdlib.h>
#include <stdio.h>
#include <regex.h>

void print_result(int value)
{
    if(value == 0){
        printf("Bulundu \n");
    }else if(value == REG_NOMATCH) {
        printf("Bulunmadı :( \n");
    }else {
        printf("Error mq \n");
    }
}

int main() {
    regex_t regexExp;
    char stringEx[] = "bruuhhadii";
    int value = regcomp(&regexExp, "bruh", 0);
    int value2 = regexec(&regexExp, stringEx, 0, NULL, 0 );
    print_result(value2);

    //creating regular experresion

    char buffer[256];
    return 0;
}