
#ifndef PERMIT_H
#define PERMIT_H
#include <include/xfs.h>
#define PASSWDSIZE 30
#define GROUPMEMCOUNT 100
#define USRMAXSIZE 500
#define GROUPMAXSIZE 500
// typedef struct usr usr ;
// typedef struct group group;
// all need to be define in main function

// add usr to a group
// false return -1 else return 0
// printf a usr info by  its uid
void user_element_print(xuid_t uid);
// printf a group info by its gid
void group_element_print(xgid_t gid);
// printf all usr info by calling user_element_print(...)
void user_list_print();
// printf all group info by calling group_element_print(...)
void group_list_print();

struct usr {
  char usr_name[PASSWDSIZE];
  char usr_passwd[PASSWDSIZE];
  xuid_t uid;
  xgid_t gid;
};

struct group {
  int16_t gid;
  char group_name[PASSWDSIZE];
  // every group can have 100 member
  // usr usr_array[GROUPMEMCOUNT];
  int16_t usr_array[GROUPMAXSIZE];
  // member count
  xsize_t usr_count;
};

#endif
