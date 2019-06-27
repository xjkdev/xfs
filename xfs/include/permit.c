#include"permit.h"
#include<stdio.h>
#include<string.h>
#include"globals.h"
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
    //new_usr.gid=gid_auto_increase++;
    //gid =-1 means usr is not in a group
    new_usr.gid=-1;
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
    //add to group and usr_count++
    g->usr_array[g->usr_count++]=uid;
    return 0;
}


void usr_elem_printf(int16_t uid)
{
    if(uid>=uid_auto_increase)
     {
         perror("uid out of range\n");
         return ;
     }
     usr usr_temp= usr_list[uid];
     //usr_name:usr_passwd:uid:gid

     printf("%-10s:%-10s:%-5d:%-5d\n",usr_temp.usr_name,usr_temp.usr_passwd,usr_temp.uid,usr_temp.gid);
     
}


void group_elem_printf(int16_t gid)
{
    if(gid>=gid_auto_increase)
    {
        perror("gid out of range\n");
        return ;
    }
    group group_temp=group_list[gid];
    printf("%-10s:%-5d:",group_temp.group_name,group_temp.gid);
    for(int i=0;i<group_temp.usr_count-1;++i)
    {
        printf("%-3d,",group_temp.usr_array[i]);
    }
    printf("%-3d\n",group_temp.usr_array[group_temp.usr_count-1]);
    return ;
}

void usr_list_printf()
{
    for(int16_t i=0;i<uid_auto_increase;++i)
    {
        usr_elem_printf(i);
    }
}
void group_list_printf()
{
    for(int16_t i=0;i<gid_auto_increase;++i)
    {
        group_elem_printf(i);
    }
    return ;
}


int32_t login(char * usr_name,char * usr_passwd)
{
    int32_t flag_exsit=0;
    int32_t flag_correct=0;
    usr usr_temp;
    for(int i=0;i<uid_auto_increase;++i)
    {
        usr_temp=usr_list[i];
        if(strcmp(usr_temp.usr_name,usr_name)==0)
        {
            flag_exsit=1;
            break;
        }
    }
    //usr_name does not exsit
    if(flag_exsit==0)
        return 0;
    cur_uid=usr_temp.uid;
    cur_gid=usr_temp.gid;
    return 1;
}

void logout()
{
    cur_gid=-1;
    cur_uid=-1;
    return ;
}