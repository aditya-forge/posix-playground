#include<stdio.h>
#include<ctype.h>
int main(){
char input[100];
int i,isnumber=1;
printf("Enter input: ");
scanf("%s",input);
for(i=0;input[i]!='\0';i++){
if(!isdigit(input[i])){
isnumber=0;
break;
}
}
if(isnumber)
printf("Number\n");
else
printf("String\n");
return 0;
}
