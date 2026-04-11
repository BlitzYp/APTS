#include <iostream>
#include <fstream>

#define MAX_NAME_LEN 101
#define MAX_KEYS 50
#define MAX_REVIEWER_COUNT 10000
#define HASH_NAME_COUNT 20011
#define HASH_KEY_COUNT 700001

typedef long long ll;
using namespace std;

bool str_eq(const char* a,const char* b)
{
    int i=0;
    for (;a[i]!='\0'&&b[i]!='\0';i++) if (a[i]!=b[i]) return false;
    return a[i]==b[i];
}

void str_cpy(char* src,char* dest)
{
    int i=0;
    for (;*src!='\0';src++) dest[i++]=*src;
    dest[i]='\0';
}

struct Reviewer {
    char name[MAX_NAME_LEN];
    ll keys[MAX_KEYS];
    int key_count;
    bool active;
};

struct NameEntry {
    int reviewer_id;
    char state; // 0 - free, 1 - taken, 2 - deleted
};

struct KeyEntry {
    int reviewer_id;
    char state; // 0 - free, 1 - taken, 2 - deleted
    ll key;
};

class HashTable {
public:
    Reviewer reviewers[MAX_REVIEWER_COUNT];
    int review_count=0;
    int free_ids[MAX_REVIEWER_COUNT];
    int free_top=0;
    NameEntry names[HASH_NAME_COUNT];
    KeyEntry keys[HASH_KEY_COUNT];
    HashTable() {
        for (int i=0;i<HASH_NAME_COUNT;i++) {
            names[i].state=0;
            names[i].reviewer_id=-1;
        }
        for (int i=0;i<HASH_KEY_COUNT;i++) {
            keys[i].key=0;
            keys[i].reviewer_id=-1;
            keys[i].state=0;
        }
    }

    int hash_name(const char* name) {
        unsigned long long h=0;
        for (int i=0;name[i]!='\0';i++) h=h*131+(unsigned char)name[i];
        return (int)(h%HASH_NAME_COUNT);
    }

    int hash_key(ll key) {
        if (key<0) key*=-1;
        return (int)(key%HASH_KEY_COUNT);
    }

    int find_name_index(const char* name) {
        int h=hash_name(name);
        int i=h;
        while (names[i].state!=0) {
            if (names[i].state==1) {
                int id=names[i].reviewer_id;
                if (id!=-1 && reviewers[id].active && str_eq(name,reviewers[id].name)) return i;
            }
            i++;
            if (i==HASH_NAME_COUNT) i=0;
            if (i==h) break;
        }
        return -1;
    }

    int find_key_index(ll key) {
        int h=hash_key(key);
        int i=h;
        while (keys[i].state!=0) {
            if (keys[i].state==1 && keys[i].key==key) {
                int id=keys[i].reviewer_id;
                if (id!=-1 && reviewers[id].active) return i;
            }
            i++;
            if (i==HASH_KEY_COUNT) i=0;
            if (i==h) break;
        }
        return -1;
    }

    int find_by_name(const char* name) {
        int i=find_name_index(name);
        if (i==-1) return -1;
        return names[i].reviewer_id;
    }

    int find_by_key(ll key) {
        int i=find_key_index(key);
        if (i==-1) return -1;
        return keys[i].reviewer_id;
    }

    const char* lookup(ll key) {
        int id=find_by_key(key);
        if (id==-1) return nullptr;
        return reviewers[id].name;
    }

    bool delete_reviewer(ll key) {
        int id=find_by_key(key);
        if (id==-1) return false;

        // Remove associated keys
        for (int i=0;i<reviewers[id].key_count;i++) {
            ll curr_key=reviewers[id].keys[i];
            int key_index=find_key_index(curr_key);
            if (key_index!=-1) {
                keys[key_index].state=2;
                keys[key_index].reviewer_id=-1;
                keys[key_index].key=0;
            }
        }
        // Remove associated name
        int name_index=find_name_index(reviewers[id].name);
        if (name_index!=-1) {
            names[name_index].state=2;
            names[name_index].reviewer_id=-1;
        }

        reviewers[id].active=false;
        reviewers[id].key_count=0;
        free_ids[free_top++]=id;
        return true;
    }

    int get_insert_hash_index_name(char* name) {
        int i=hash_name(name);
        int start=i;
        int deleted_index=-1;
        while (names[i].state!=0) {
            if (names[i].state==1) {
                int id=names[i].reviewer_id;
                if (id!=-1 && reviewers[id].active && str_eq(name,reviewers[id].name)) return i;
            }
            else if (names[i].state==2 && deleted_index==-1) deleted_index=i;
            i++;
            if (i==HASH_NAME_COUNT) i=0;
            if (i==start) break;
        }
        if (deleted_index!=-1) return deleted_index;
        return i;
    }

    int get_insert_hash_index_key(ll key) {
        int i=hash_key(key);
        int start=i;
        int deleted_index=-1;
        while (keys[i].state!=0) {
            if (keys[i].state==1 && keys[i].key==key) {
                int id=keys[i].reviewer_id;
                if (id!=-1 && reviewers[id].active) return i;
            }
            else if (keys[i].state==2 && deleted_index==-1) deleted_index=i;
            i++;
            if (i==HASH_KEY_COUNT) i=0;
            if (i==start) break;
        }
        if (deleted_index!=-1) return deleted_index;
        return i;
    }

    bool insert(char* name,ll* k,int n) {
        int id=find_by_name(name);
        bool is_new=false;
        if (id==-1) is_new=true;

        // Remove duplicate keys
        ll unique_keys[MAX_KEYS];
        int u_count=0;
        for (int i=0;i<n;i++) {
            bool dup=false;
            for (int j=0;j<u_count;j++) {
                if (unique_keys[j]==k[i]) {
                    dup=true;
                    break;
                }
            }
            if (!dup) unique_keys[u_count++]=k[i];
        }

        int curr_key_count=0,actually_new_count=0;
        ll new_keys[MAX_KEYS]; // actually new keys to add
        if (!is_new) curr_key_count=reviewers[id].key_count;

        for (int i=0;i<u_count;i++) {
            ll key=unique_keys[i];
            int key_owner=find_by_key(key);
            // Key is free to use
            if (key_owner==-1) new_keys[actually_new_count++]=key;
            // Key already is associated with the reviewer
            else if (key_owner==id) continue;
            // Key already is associated with a different reviewer
            else return false;
        }
        if (curr_key_count+actually_new_count>50) return false;

        if (is_new) {
            if (free_top>0) id=free_ids[--free_top];
            else {
                if (review_count>=MAX_REVIEWER_COUNT) return false;
                id=review_count++;
            }
            reviewers[id].key_count=0;
            reviewers[id].active=1;
            str_cpy(name,reviewers[id].name);

            int hash_index=get_insert_hash_index_name(name);
            names[hash_index].state=1;
            names[hash_index].reviewer_id=id;
        }

        for (int i=0;i<actually_new_count;i++) {
            ll key=new_keys[i];
            reviewers[id].keys[reviewers[id].key_count++]=key;

            int hash_index=get_insert_hash_index_key(key);
            keys[hash_index].state=1;
            keys[hash_index].key=key;
            keys[hash_index].reviewer_id=id;
        }
        return true;
    }
};

HashTable registry;

int main(void)
{
    ifstream cin("reviewers.in");
    ofstream cout("reviewers.out");
    char c;
    ll key;
    while (cin>>c) {
        switch (c) {
            case 'I': {
                char name[MAX_NAME_LEN];
                int count;
                ll keys[MAX_KEYS];
                cin>>name>>count;
                for (int i=0;i<count;i++) cin>>keys[i];
                cout<<(registry.insert(name,keys,count)? "ok" : "no") <<'\n';
                break;
            }
            case 'D': {
                cin>>key;
                cout<<(registry.delete_reviewer(key)? "ok" : "no") <<'\n';
                break;
            }
            case 'L': {
                cin>>key;
                const char* res=registry.lookup(key);
                if (res) cout<<res<<'\n';
                else cout<<"no"<<'\n';
                break;
            }
        }
    }
    return 0;
}
