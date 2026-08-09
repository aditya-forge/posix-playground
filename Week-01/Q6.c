#include<stdio.h>
#include<string.h>
int main(){
FILE *fp;
char line[500];
int chars,words,i,inword;
fp=fopen("file.txt","r");
if(fp==NULL){
printf("File not found\n");
return 1;
}
while(fgets(line,sizeof(line),fp)){
chars=0;
words=0;
inword=0;
for(i=0;line[i]!='\0';i++){
if(line[i]!='\n')
chars++;
if(line[i]!=' '&&line[i]!='\n'&&line[i]!='\t'){
if(inword==0){
words++;
inword=1;
}
}
else{
inword=0;
}
}
printf("Characters=%d Words=%d\n",chars,words);
}
fclose(fp);
return 0;
}
