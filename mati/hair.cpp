#include <iostream>
#include <fstream>
#include <climits>

using namespace std;
typedef long long ll;
const ll INF=LLONG_MAX;

ll min(ll a,ll b)
{
    if (a<b) return a;
    return b;
}

ll max(ll a,ll b)
{
    if (a>b) return a;
    return b;
}

template <typename T>
class Vector {
public:
    T* data;
    int len;
    int cap;
    Vector(): len(0), cap(1024) { data=new T[cap]; }
    void extend() {
        cap*=2;
        T* new_data=new T[cap];
        for (int i=0;i<len;i++) new_data[i]=data[i];
        delete[] data;
        data=new_data;
    }
    void push_back(const T& v) {
        if (len==cap) extend();
        data[len++]=v;
    }
    T& operator[] (int i) {
        return data[i];
    }
    int size() {
        return len;
    }
    ~Vector() {
        delete[] data;
    };
};

template <typename T>
class Queue {
public:
    Vector<T> data;
    int head;
    Queue(): head(0) {}
    void push(const T& v) {
        data.push_back(v);
    }
    void pop() {
        head++;
    }
    T& front() {
        return data[head];
    }
    bool empty() {
        return (data.size()==head);
    }
};

template <typename T>
class MinHeap {
public:
    Vector<T> data;
    int leftChild(int i) { return 2*i+1; }
    int rightChild(int i) { return 2*i+2; }
    int parent(int i) { return (i-1)/2; }
    MinHeap() {}
    void swap(T& a,T& b) {
        T t=a;
        a=b;
        b=t;
    }
    T& top() {
        return data[0];
    }
    void heapify(int i) {
        int curr_min=i;
        int lc=leftChild(i),rc=rightChild(i);
        if (lc<data.size()&&data[curr_min]>data[lc]) curr_min=lc;
        if (rc<data.size()&&data[curr_min]>data[rc]) curr_min=rc;
        if (curr_min!=i) {
            swap(data[curr_min],data[i]);
            heapify(curr_min);
        }
    }
    void push(T v) {
        data.push_back(v);
        int i=data.size()-1;
        while (i!=0&&data[parent(i)]>data[i]) {
            swap(data[parent(i)],data[i]);
            i=parent(i);
        }
    }
    void pop() {
        data[0]=data[data.size()-1];
        data.len--;
        if (data.size()>0) heapify(0);
    }
    bool empty() {
        return data.size()==0;
    }
};

class Client {
public:
    int id;
    ll arrival;
    ll durr;
    Client(): id(0), arrival(0), durr(0) {}
    Client(int id,ll arrival,ll durr) {
        this->id=id;
        this->arrival=arrival;
        this->durr=durr;
    }
};

typedef struct Barber {
    int id;
    ll free_from;
    bool busy;
    ll get_next_break(ll t) {
        ll block=t/100;
        ll digit=block%10;
        if (digit==id) return t;
        ll shift=(id-digit+10)%10;
        if (shift==0) shift=10;
        ll target=block+shift;
        return target*100;
    }

    bool can_start(ll t,ll durr) {
        ll finish=t+durr-1;
        return finish<get_next_break(t);
    }

    ll earliest_start(ll t,ll durr) {
        if (durr>900) return INF;
        t=max(t,this->free_from);
        while (true) {
            if (this->can_start(t,durr)) return t;
            ll next_break=this->get_next_break(t);
            if (next_break==t) t=(t/100)*100+100;
            else t=next_break+100;
            t=max(t,this->free_from);
        }
    }

} Barber;

class Event {
public:
    ll finish_time;
    int barber;
    int client;
    Event(): finish_time(0), barber(0), client(0) {}
    Event(ll finish_time,int barber,int client) {
        this->finish_time=finish_time;
        this->barber=barber;
        this->client=client;
    }
    bool operator >(const Event& comp) const {
        if (finish_time!=comp.finish_time) return finish_time>comp.finish_time;
        return barber>comp.barber;
    }
    bool operator <(const Event& comp) const {
        if (finish_time!=comp.finish_time) return finish_time<comp.finish_time;
        return barber<comp.barber;
    }
};

Queue<Client> q;
//priority_queue<Event,vector<Event>,greater<Event>> pq; // Min-Heap
MinHeap<Event> pq;
Barber barbers[11];
int n;
Vector<Client> arrivals;

int get_barber(ll t,Client& c)
{
    int res=0;
    ll best_idle=-1;
    for (int i=1;i<=n;i++) {
        Barber& b=barbers[i];
        if (b.busy) continue;
        if (b.free_from<=t && b.can_start(t,c.durr)) {
            ll idle=t-b.free_from;
            if (res==0 || idle>best_idle || (i<res && best_idle==idle)) {
                best_idle=idle;
                res=i;
            }
        }
    }
    return res;
}

void assign_clients(ll t)
{
    while (!q.empty()) {
        Client c=q.front();
        int b=get_barber(t,c);
        if (b==0) break;
        ll finish_time=t+c.durr-1;
        barbers[b].busy=true;
        pq.push(Event(finish_time,b,c.id));
        q.pop();
    }
}

void process_clients(ll t,ofstream& cout)
{
    while (!pq.empty() && pq.top().finish_time<t) {
        auto e=pq.top();
        cout<<e.finish_time<<' '<<e.barber<<' '<<e.client<<'\n';
        barbers[e.barber].busy=false;
        barbers[e.barber].free_from=e.finish_time+1;
        pq.pop();
    }
}

ll next_possible_time(ll current_time)
{
    if (q.empty()) return INF;
    Client c=q.front();
    ll ans=INF;
    for (int i=1;i<=n;i++) {
        if (barbers[i].busy) continue;
        ans=min(ans,barbers[i].earliest_start(current_time, c.durr));
    }
    return ans;
}

int main(void)
{
    ifstream cin("hair.in");
    ofstream cout("hair.out");
    ll t,client,durr,curr_time=0;
    cin>>n;
    for (int i=1;i<=n;i++) {
        barbers[i].id=i;
        barbers[i].free_from=0;
        barbers[i].busy=false;
    }

    while (true) {
        cin>>t;
        if (t==0) break;
        cin>>client>>durr;
        arrivals.push_back(Client(client,t,durr));
    }

    int idx=0;
    while (idx<arrivals.size() || !q.empty() || !pq.empty()) {
        ll next_arrival=idx<arrivals.size() ? arrivals[idx].arrival : INF;
        ll next_finished_ready=pq.empty() ? INF : pq.top().finish_time+1;
        ll next_start_time=next_possible_time(curr_time);

        ll next_time=min(next_arrival,min(next_finished_ready,next_start_time));
        curr_time=next_time;
        process_clients(curr_time,cout);

        while (idx<arrivals.size() && arrivals[idx].arrival==curr_time) {
            q.push(arrivals[idx]);
            idx++;
        }
        assign_clients(curr_time);
    }
    return 0;
}
