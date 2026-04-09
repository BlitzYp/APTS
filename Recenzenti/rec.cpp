#include <iostream>
#include <fstream>

#define MAX_NAME_LEN 101
#define MAX_KEYS 51
#define MAX_REVIEWER_COUNT 10001

typedef long long ll;
using namespace std;

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
    int review_count;
    NameEntry names[20001];
    KeyEntry keys[700001];
    HashTable();
    int find_by_name(const char* name);
    int find_by_key(ll key);
    bool insert(const char* name,ll* keys,int n);
    bool delete_reviewer(ll key);
    const char* lookup(ll key);
};

int main(void)
{
    ifstream cin("reviewers.in");
    ofstream cout("reviewers.out");
    HashTable registry;
    char c;
    while (cin>>c) {
        switch (c) {
            case 'I':
                break;
            case 'D':
                break;
            case 'L':
                break;
            default: break;
        }
    }
    return 0;
}
