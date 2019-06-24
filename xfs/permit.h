#define PASSWDSIZE 30
#define GROUPMEMCOUNT 100
#define USRMAXSIZE 500
#define GROUPMAXSIZE 500 
#include <stdint.h>
struct usr ;
struct group;
//all need to be define in main function
extern usr usr_list[USRMAXSIZE];
extern group   group_list[GROUPMAXSIZE];
extern int16_t uid_auto_increase;
extern int16_t gid_auto_increase;

//correct return uid else return -1
int32_t creat_usr(char *usr_name,char* passwd);
//correct return gid else return -1
int32_t creat_group(char *group_name);



typedef struct usr 
{
    char usr_name[PASSWDSIZE];
    char usr_passwd[PASSWDSIZE];
    int16_t uid;
    int16_t gid;

}usr;

typedef struct group
{
   int16_t gid;
   char group_name[PASSWDSIZE];
   //every group can have 100 member
   //usr usr_array[GROUPMEMCOUNT];
   int16_t usr_array[GROUPMAXSIZE];
   //member count
   int16_t usr_count;

}group;

