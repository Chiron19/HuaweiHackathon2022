#ifndef _GLIBCXX_NO_ASSERT
#include <cassert>
#endif
#include <cctype>
#include <cerrno>
#include <cfloat>
#include <ciso646>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#if __cplusplus >= 201103L
#include <ccomplex>
#include <cfenv>
#include <cinttypes>
#include <cstdalign>
#include <cstdbool>
#include <cstdint>
#include <ctgmath>
#include <cuchar>
#include <cwchar>
#include <cwctype>
#endif

// C++
#include <algorithm>
#include <bitset>
#include <complex>
#include <deque>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <istream>
#include <iterator>
#include <limits>
#include <list>
#include <locale>
#include <map>
#include <memory>
#include <new>
#include <numeric>
#include <ostream>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <typeinfo>
#include <utility>
#include <valarray>
#include <vector>
#include <stdlib.h>
#include <algorithm>
using namespace std;
int m, n, u;
int speed_to_data[27] = {0,290,575,813,1082,1351,1620,1889,2158,2427,2696,2965,3234,3503,3772,4041,4310,4579,4848,5117,5386,5655,5924,6093,6360,6611,6800};
double alpha;
struct userData{
    int id, speed, data, weight;
    double factor;
    // Reload "<" comparing by data-size, init speed, factor, orderly
    bool operator < (const userData &A) const{
        if (data < A.data) return true;
        else if ((data == A.data) && (speed < A.speed)) return true;
        else if ((data == A.data) && (speed == A.speed) && (factor < A.factor)) return true;
        return false;
    }
    bool operator == (const userData &A) const{
        return (id == A.id);
    }
} user[500];
vector <userData> v;    // vector to contain userData, and to sort
int data_sent[500];     // data_sent[i]: data size has been sent by user i
vector <userData> channel[200];     // channel[j]: the j-th column of channel
double channel_factor[200];   // channel_factor[j]: factor of the j-th column of channel
priority_queue<pair<double, int>, vector<pair<double, int> >, greater<> > q;    // small root heap: to pop the channel with less factor
int allocated_channel[500][200];    // allocated_channel[i][j]: if user i has used channel j
vector <double> avg_speed[500];     // avg_speed[i]: speed of user i in each used channel, sum-up and divided by vector size to get avg
int ignore_user[500];
double avg_speed_all, factor_all, best_speed_all, data_loss, data_all, object_function, penalty_term, score, score_h = -1e6;
char intStr[100], path[100];
clock_t clock_start, clock_end;

bool cmp(userData a, userData b)
{
    if (a.factor < b.factor) return true;
    else if ((a.factor == b.factor) && (a.data == b.data)) return true;
        else if ((a.data == b.data) && (a.factor == b.factor) && (a.speed < b.speed)) return true;
        return false;
}

inline int read(){  // fast read template, to ignore ',' and get a integer
    int s = 0, w = 1;
    char ch = getchar();
    // while(ch < '0' || ch > '9') { if(ch == '-') w = -1; ch = getchar(); }
    while(ch >= '0' && ch <= '9') s = s * 10 + ch - '0', ch = getchar();
    return s*w;
}

pair<double, int> allocate_user(userData x, pair<double, int> p)  // to allocate user x into the certain channel
{
    // p.first: channel factor, p.second: channel No.
    double channel_factor = p.first;
    int j = p.second;
    double b = x.factor;
    for (auto & y: channel[j]) {
        double a = channel_factor - y.factor;
        double f0 = (1-y.factor*a), f1 = (1-y.factor*(a+b));
        if (f0 <= 0) f0 = 0;
        if (f1 <= 0) f1 = 0;
        data_sent[y.id] -= speed_to_data[(int)(y.speed * f0)] -
                           speed_to_data[(int)(y.speed * f1)];
    }
    channel[j].push_back(x);
    double f = 1-channel_factor*b;
    if (f <= 0) f = 0;
    data_sent[x.id] += speed_to_data[(int)(x.speed * f)];
    return {channel_factor+b, j};
}

pair<double, int> deallocate_user(userData x, pair<double, int> p)  // to deallocate user x from the certain channel
{
    // p.first: channel factor, p.second: channel No.
    double channel_factor = p.first;
    int j = p.second;
    double b = x.factor;
    for (auto & y: channel[j]) {
        if (y.id != x.id) {
            double a = channel_factor - y.factor;
            double f0 = (1-y.factor*a), f1 = (1-y.factor*(a-b));
            if (f0 <= 0) f0 = 0;
            if (f1 <= 0) f1 = 0;
            data_sent[y.id] += speed_to_data[(int)(y.speed * f1)] - 
                               speed_to_data[(int)(y.speed * f0)];
        }
        else {
            double f = 1-b*(channel_factor-b);
            if (f <= 0) f = 0;
            data_sent[x.id] -= speed_to_data[(int)(x.speed * f)];
        }
    }
    vector <userData> tmp;
    for (auto s: channel[j])
    {
        if (s == x) continue;
        tmp.push_back(s);
    }
    channel[j] = tmp;
    // channel[j].erase(remove(channel[j].begin(), channel[j].end(), x), channel[j].end());
    return {channel_factor-b, j};
}

int Rand(int n)
{
    srand(time(NULL));
    return rand() % n ;
}

void organize_data()// Organizing data
{
    memset(avg_speed, 0, sizeof avg_speed);
    avg_speed_all =  data_loss = object_function = penalty_term = score = 0;

    for (int i = 0; i < n; ++i) {
        // Withdraw all speed of the user j.id
        double a = channel_factor[i];
        for (auto j : channel[i]) {
            double f = 1- j.factor*(a - j.factor);
            if (f <= 0) f = 0;
            avg_speed[j.id].push_back(j.speed * f);
        }
    }

    for (int i = 0; i < u; ++i) {
        // Calculate user i's average speed
        double x = 0;
        for (double j : avg_speed[i]) {
            x += j;
        }
        if (avg_speed[i].size()) {
            avg_speed[i].push_back(x/(double)avg_speed[i].size());
            avg_speed_all += avg_speed[i][avg_speed[i].size()-1] * user[i].weight;
        }
        
        // Calculate user i's data loss
        if (data_sent[i] < user[i].data) 
            data_loss += user[i].data - data_sent[i];
        printf("%d ", data_sent[i]);
    }
    printf("\n");

    // Calculate the object function and score
    object_function = avg_speed_all / best_speed_all;
    penalty_term = 1.0* data_loss / data_all;
    score = object_function - alpha * penalty_term;
    score_h = max(score, score_h);
}

void output()
{
    organize_data();
    // Output
    char output_path[100];
    strcpy(output_path, "./result/");
    strcat(output_path, intStr);
    strcat(output_path, ".csv");
    freopen(output_path, "w", stdout);

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (channel[j].size() > i) 
                printf("U%d", channel[j][i].id+1);
            else 
                printf("-");
            if (j != n-1) printf(","); else printf("\n");
        }
    }
    for (int i = 0; i < u; ++i) {
        if (data_sent[i] < user[i].data) 
            printf("%d,", user[i].data-data_sent[i]);
        else printf("%d,", 0);
    }
    printf("%lf\n", penalty_term);

    for (int i = 0; i < u; ++i) {
        if (avg_speed[i].size() > 0) 
            printf("%lf,", avg_speed[i][avg_speed[i].size()-1]);
        else printf("0,");
    }
    printf("%lf\n", object_function);
    printf("%lf\n", score);
    clock_end = clock();
    printf("%d\n", (int)((clock_end-clock_start)/(double)CLOCKS_PER_SEC*1000));
}

int main() {
    // Initialization and inputs
    clock_start = clock();
    // Only need to input test case's number
    int tc_id;
    scanf("%d", &tc_id);
    itoa(tc_id, intStr, 10);
    strcpy(path, "./test_cases/tc");
    strcat(path, intStr);
    strcat(path, ".csv");
    freopen(path, "r", stdin); // <- Here to change the input file's path!

    scanf("%d%*c%d%*c%d%*c", &m, &n, &u);
    scanf("%lf", &alpha);
    printf("%d %d %d %lf\n", m, n, u, alpha);
    for (int i = 0; i < u; ++i) {
        userData x;
        x.id = read(), x.speed = read(), x.data = read(), x.factor = read(), x.weight = read();
        // printf("%d %d %d %lf %d\n", x.id, x.speed, x.data, x.factor, x.weight);
        // x.speed *= x.weight;
        x.id --;
        x.factor *= 0.01;
        v.push_back(x);
        user[i] = x;
        data_all += user[i].data;
        best_speed_all += user[i].speed*user[i].weight;
    }
    for (int i = 0; i < n; ++i) {
        q.push({0, i});
    }

    // Sorting the v from high to low
    sort(v.rbegin(), v.rend());

    for (int i = 0; i < u; i++)
    {
        userData x = v[i];
        while (data_sent[x.id] < x.data) {
            int target_j = 0, target_sent=0;
            int flag = 0;
            for (int j = 0; j < n; j++)
            {
                if (channel_factor[j] >= 1) {flag++; continue;}
                if (channel[j].size() == 0) {
                    target_sent = speed_to_data[(int)(x.speed)];
                    target_j = j; 
                    break;
                } 
                else {
                    int sent_tmp = speed_to_data[(int)(x.speed*(1-channel_factor[j]*x.factor))];
                    if (sent_tmp > target_sent) {
                        target_sent = sent_tmp;
                        target_j = j;
                    }
                }
            }
            if (flag == n) break;
            for (int j = 0; j < channel[target_j].size(); j++)
            {
                userData y = channel[target_j][j];
                data_sent[y.id] -= speed_to_data[(int)(y.speed*(1-(channel_factor[target_j]-y.factor)*y.factor))];
                data_sent[y.id] += speed_to_data[(int)(y.speed*(1-(channel_factor[target_j]-y.factor+x.factor)*y.factor))];
            }
            channel[target_j].push_back(x);
            channel_factor[target_j] += x.factor;
            data_sent[x.id] += target_sent;
        }
    }
    

    output();
    
    
    return 0;
}