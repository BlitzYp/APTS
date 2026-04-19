#include <fstream>
#include <iostream>

#define MAX_NAME_LEN 31
#define DATE_LEN 12
#define MAX_PEOPLE 10001
#define MAX_GROUP_COUNT 366
#define HASH_SIZE 30011

using namespace std;

typedef struct {
    char name[MAX_NAME_LEN];
    char surname[MAX_NAME_LEN];
    char lower_name[MAX_NAME_LEN];
    char lower_surname[MAX_NAME_LEN];
    int day,month,year;
} Person;

typedef struct {
    int index;
    bool free;
} HashEntry;

typedef struct {
    int doy;
    int head;
    int day,month;
} BirthdayGroup;

Person people[MAX_PEOPLE];
BirthdayGroup groups[MAX_GROUP_COUNT];
HashEntry hash_table[HASH_SIZE];
int people_count,group_count;
int next_person[MAX_PEOPLE];
int days_in_month[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};

void str_cpy(const char* src,char* dest)
{
    int i=0;
    for (;*src!='\0';src++) dest[i++]=*src;
    dest[i]='\0';
}

int str_cmp(const char* a,const char* b)
{
    int i;
    for (i=0;*(a+i)!='\0'||*(b+i)!='\0';i++) if (*(a+i)!=*(b+i)) return a[i]-b[i];
    return a[i]-b[i];
}

bool str_eq(const char* a,const char* b)
{
    int i;
    for (i=0;*(a+i)!='\0'||*(b+i)!='\0';i++) if (*(a+i)!=*(b+i)) return false;
    if (a[i]==b[i]) return true;
    return false;
}

void parse_date(const char* in,int& day,int& month,int& year)
{
    day=(int)((in[0]-'0')*10+(in[1]-'0'));
    month=(int)((in[3]-'0')*10+(in[4]-'0'));
    year=(int)((in[6]-'0')*1000+100*(in[7]-'0')+10*(in[8]-'0')+(in[9]-'0'));
}

int day_of_year(int d,int m)
{
    int days=0;
    for (int i=1;i<m;i++) days+=days_in_month[i];
    return days+d;
}

int binary_search_groups(int f)
{
    int l=0,r=group_count-1;
    int res=group_count;
    while (l<=r) {
        int mid=(l+r)/2;
        if (groups[mid].doy>=f) {
            res=mid;
            r=mid-1;
        }
        else l=mid+1;
    }
    return res;
}

bool compare_people(int a,int b)
{
    if (people[a].year!=people[b].year) return people[a].year<people[b].year;
    int r=str_cmp(people[a].lower_surname,people[b].lower_surname);
    if (r!=0) return r<0;
    return str_cmp(people[a].lower_name,people[b].lower_name)<0;
}

void insert_into_group(int idx)
{
    int doy=day_of_year(people[idx].day, people[idx].month);
    int pos=binary_search_groups(doy);

    // Group already exists
    if (pos<group_count && groups[pos].doy==doy) {
        int head=groups[pos].head;
        if (head==-1 || compare_people(idx,head)) {
            next_person[idx]=groups[pos].head;
            groups[pos].head=idx;
            return;
        }

        int curr=head;
        while (next_person[curr]!=-1 && !compare_people(idx,next_person[curr])){
            curr=next_person[curr];
        }
        next_person[idx]=next_person[curr];
        next_person[curr]=idx;
        return;

    }

    // Create a new spce and add a new group
    for (int i=group_count;i>pos;i--) groups[i]=groups[i-1];
    groups[pos].doy=doy;
    groups[pos].head=idx;
    groups[pos].day=people[idx].day;
    groups[pos].month=people[idx].month;
    next_person[idx]=-1;
    group_count++;
}

char to_lower(char c)
{
    if (c>='A' && c<='Z') return c-'A'+'a';
    return c;
}

void make_lowercase(char* dst,const char* src)
{
    int i;
    for (i=0;*(src+i)!='\0';i++) dst[i]=to_lower(src[i]);
    dst[i]='\0';
}

void add_person(const char* name,const char* surname,const char* date)
{
    str_cpy(name,people[people_count].name);
    str_cpy(surname,people[people_count].surname);

    make_lowercase(people[people_count].lower_name, people[people_count].name);
    make_lowercase(people[people_count].lower_surname, people[people_count].surname);

    parse_date(date, people[people_count].day,people[people_count].month, people[people_count].year);
}

unsigned int hash_key(const char* name,const char* surname,int day,int month,int year)
{
    unsigned int h=0;
    for (int i=0;name[i]!='\0';i++) h=h*239u+(unsigned int)name[i];
    h=h*239u+257u;
    for (int i=0;surname[i]!='\0';i++) h=h*239u+(unsigned int)surname[i];
    h=h*239u+(unsigned int)day;
    h=h*239u+(unsigned int)month;
    h=h*239u+(unsigned int)year;
    return h%HASH_SIZE;
}

bool eq_person(int idx,const char* lower_name,const char* lower_surname,int day,int month,int year)
{
    return (str_eq(lower_name,people[idx].lower_name) &&
       str_eq(lower_surname,people[idx].lower_surname) &&
       people[idx].day==day &&
       people[idx].month==month &&
       people[idx].year==year);
}

void insert_hash(int idx)
{
    Person p=people[idx];
    unsigned int pos=hash_key(p.lower_name,p.lower_surname,p.day,p.month,p.year);
    while (!hash_table[pos].free) {
        pos++;
        if (pos==HASH_SIZE) pos=0;
    }
    hash_table[pos].free=false;
    hash_table[pos].index=idx;
}

bool check_exists(const char* lower_name,const char* lower_surname,int day,int month,int year)
{
    unsigned int pos=hash_key(lower_name,lower_surname,day,month,year);
    while (!hash_table[pos].free) {
        if (eq_person(hash_table[pos].index,lower_name,lower_surname,day,month,year)) return true;
        pos++;
        if (pos==HASH_SIZE) pos=0;
    }
    return false;
}

int main(void)
{
    ifstream cin("anniversary.in");
    ofstream cout("anniversary.out");
    people_count=group_count=0;
    char c;
    for (int i=0;i<HASH_SIZE;i++) {
        hash_table[i].free=true;
        hash_table[i].index=-1;
    }
    while (cin>>c) {
        switch (c) {
            case 'P': {
                char name[MAX_NAME_LEN];
                char surname[MAX_NAME_LEN];
                char date[DATE_LEN];
                char lower_name[MAX_NAME_LEN];
                char lower_surname[MAX_NAME_LEN];
                int day,month,year;
                cin>>name>>surname>>date;

                make_lowercase(lower_name,name);
                make_lowercase(lower_surname,surname);

                parse_date(date,day,month,year);

                if (!check_exists(lower_name, lower_surname, day,month,year)) {
                    add_person(name, surname, date);
                    insert_into_group(people_count);
                    insert_hash(people_count);
                    people_count++;
                }
               break;
            }
            case 'D': {
                char date[DATE_LEN];
                int day,month,year;
                cin>>date;
                parse_date(date,day,month,year);
                if (group_count==0) continue;
                int now_doy=day_of_year(day,month);

                int pos=binary_search_groups(now_doy);
                if (pos==group_count) pos=0;

                int group_doy=groups[pos].doy;
                int celebration_year=year;
                if (group_doy<now_doy) celebration_year++;

                // Get resulting date
                day=groups[pos].day;
                month=groups[pos].month;
                if (day<10) cout<<0;
                cout<<day<<'.';
                if (month<10) cout<<0;
                cout<<month<<'.';
                cout<<celebration_year<<'\n';

                // If group has multiple people, collect
                int idx=groups[pos].head;
                while (idx!=-1) {
                    int res_age=celebration_year-people[idx].year;
                    cout<<res_age<<' '<<people[idx].name<<' '<<people[idx].surname<<'\n';
                    idx=next_person[idx];
                }
                break;
            }
           default: break;
        }
    }
    return 0;
}
