#include <stdio.h>
<<<<<<< HEAD
#include <unistd.h>
#include <stdlib.h>

//for clear the screen
void clear_screen() {
    printf("\033[H\033[J");  // ANSI escape code to clear the screen
}

void create_admin(){
    printf("admin_create\n");
}
void admin_acces(){
    printf("login as admin\n");
}
void generel_user(){
    printf("generel_user\n");
}

int main(){
    
    int option;
    while (1)
    {
        printf("\n->");
        scanf("%d", &option);
        switch (option)
        {
        case 1:
            sleep(1);
            clear_screen();
            create_admin();
            break;
        case 2:
            sleep(1);
            clear_screen();
            admin_acces();
            break;
        case 3:
            sleep(1);
            clear_screen();
            generel_user();
            break;
        case 0:
            sleep(1);
            clear_screen();
            printf("program closed!!");
            exit(0);
            //break;
        default:
            sleep(1);
            clear_screen();
            printf("invalid option!!!\n");
            break;
        }
    }
    
=======
int main{
printf("Hello Everyone!");
return 0;
>>>>>>> f25c89ffb6a4126a710bdc620072b17e787da4ea
}