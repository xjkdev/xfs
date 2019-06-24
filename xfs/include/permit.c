#include"permit.h"
#include<stdio.h>
#include<string.h>
//correct return uid else return -1
// int32_t creat_usr(char *usr_name,char* passwd);
// //correct return gid else return -1
// int32_t creat_group(char *group_name);

int32_t creat_usr(char *usr_name,char* passwd)
{
    if((uid_auto_increase)>=USRMAXSIZE)
    {
        return -1;
    }
    usr new_usr;
    new_usr.uid=uid_auto_increase++;
    new_usr.gid=gid_auto_increase++;
    memcpy(new_usr.usr_name,usr_name,strlen(usr_name));
    memcpy(new_usr.usr_passwd,passwd,strlen(passwd));
    usr_list[uid_auto_increase]=new_usr;
    return new_usr.uid;
}


int32_t creat_group(char *group_name)
{
    if((gid_auto_increase)>=GROUPMAXSIZE)
    {
        return -1;
    }
    group new_group;
    new_group.gid=gid_auto_increase++;
    new_group.usr_count=0;
    memcpy(new_group.group_name,group_name,strlen(group_name));
    group_list[gid_auto_increase]=new_group;
    return new_group.gid;
}
//false return -1 else return 0
int32_t add_usr_to_group(int16_t gid,int16_t uid)
{
    if(gid>=GROUPMAXSIZE||uid>=USRMAXSIZE)
        return -1;
    group* g=&group_list[gid];
    usr *u=&usr_list[uid];
    //the group is full size
    if (g->usr_count>=GROUPMEMCOUNT)
        return -1;
    for (int i=0;i<g->usr_count;++i)
    {
        //the group has had this usr already
        if(g->usr_array[i]==uid)
            return -1;
    }
    g->usr_array[g->usr_count++]=uid;
    return 0;
}

