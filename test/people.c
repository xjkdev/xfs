#include"stdio.h"
#include"string.h"
typedef struct people
{
    int age;
    char name[10];
}people;

int main( )
{
    people p;
    p.age=10;
    strcpy(p.name,"tony\0");
    people a;
    FILE *f=fopen("people_data","wb+");
    fwrite(&p,sizeof(p),1,f);

    fseek(f,0,0);
    fread(&a,sizeof(a),1,f);
    printf("people age :%d   name: %s\n",p.age,p.name);
    return 0;
}
