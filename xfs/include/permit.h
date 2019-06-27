#define PASSWDSIZE 30
#define GROUPMEMCOUNT 100
#define USRMAXSIZE 500
#define GROUPMAXSIZE 500 

#include <stdint.h>
// typedef struct usr usr ;
// typedef struct group group;
//all need to be define in main function


//correct return uid else return -1
int32_t creat_usr(char *usr_name,char* passwd);
//correct return gid else return -1
int32_t creat_group(char *group_name);
//add usr to a group 
//false return -1 else return 0
int32_t add_usr_to_group(int16_t gid,int16_t uid);
//printf a usr info by  its uid 
void usr_elem_printf(int16_t uid);
//printf a group info by its gid
void group_elem_printf(int16_t gid);
//printf all usr info by calling usr_elem_printf(...) 
void usr_list_printf();
//printf all group info by calling group_elem_printf(...)
void group_list_printf();
//login
int32_t login(char * usr_name,char * usr_passwd);
//logout
void logout();



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

extern usr usr_list[USRMAXSIZE];
extern group   group_list[GROUPMAXSIZE];
extern int16_t uid_auto_increase;
extern int16_t gid_auto_increase;