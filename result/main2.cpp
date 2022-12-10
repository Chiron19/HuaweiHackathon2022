#include <bits/stdc++.h>
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
        else if ((data == A.data) && (factor < A.factor)) return true;
        else if ((data == A.data) && (factor == A.factor) && (speed < A.speed)) return true;
        return false;
    }
    bool operator == (const userData &A) const{
        return (id == A.id);
    }
} user[2005];
vector <userData> v;    // vector to contain userData, and to sort
int data_sent[2005];     // data_sent[i]: data size has been sent by user i
vector <userData> channel[505];     // channel[j]: the j-th column of channel
double channel_factor_final[505];   // channel_factor_final[j]: factor of the j-th column of channel
priority_queue<pair<double, int>, vector<pair<double, int> >, greater<> > q;    // small root heap: to pop the channel with less factor
int allocated_channel[2005][505];    // allocated_channel[i][j]: if user i has used channel j
vector <double> avg_speed[2005];     // avg_speed[i]: speed of user i in each used channel, sum-up and divided by vector size to get avg
int ignore_user[2005];
double avg_speed_all, best_speed_all, data_loss, data_all, object_function, penalty_term, score, score_h = -1e6;
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
    while(ch < '0' || ch > '9') { if(ch == '-') w = -1; ch = getchar(); }
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
    avg_speed_all = best_speed_all = data_loss = object_function = penalty_term = score = 0;

    for (int i = 0; i < n; ++i) {
        // Withdraw all speed of the user j.id
        double a = channel_factor_final[i];
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
        if (avg_speed[i].size()) {avg_speed[i][0] = x/(double)avg_speed[i].size();
        avg_speed_all += avg_speed[i][0];}
        best_speed_all += user[i].speed;
        // Calculate user i's data loss
        if (data_sent[i] < user[i].data) data_loss += user[i].data-data_sent[i];
    }
    // Calculate the object function and score
    object_function = avg_speed_all/best_speed_all;
    penalty_term = data_loss/data_all;
    score = object_function - alpha * penalty_term;
    score_h = max(score, score_h);
}

void output()
{
    organize_data();
    // Ready for output: Put the channel allocation into grid m * n
    int grid[m][n];
    memset(grid, -1, sizeof grid);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < channel[i].size(); ++j) {
            grid[j][i] = channel[i][j].id;
        }
    }
    // Output
//    char output_path[100];
//    strcpy(output_path, R"(.\result\)");
//    strcat(output_path, intStr);
//    strcat(output_path, ".csv");
//    freopen(output_path, "w", stdout);
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (grid[i][j] >= 0) printf("U%d", grid[i][j]+1);
            else printf("-");
            if (j != n-1) printf(","); else printf("\n");
        }
    }
    for (int i = 0; i < u; ++i) {
        if (data_sent[i] < user[i].data) printf("%d,", user[i].data-data_sent[i]);
        else printf("%d,", 0);
    }
    printf("%lf\n", penalty_term);
    for (int i = 0; i < u; ++i) {
        if (avg_speed[i].size()) printf("%lf,", avg_speed[i][0]);
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
//    scanf("%d", &tc_id);
//    itoa(tc_id, intStr, 10);
//    strcpy(path, R"(.\test_cases\tc)");
//    strcat(path, intStr);
//    strcat(path, ".csv");
//    freopen(path, "r", stdin); // <- Here to change the input file's path!
    m = read();
    n = read();
    u = read();
    scanf("%lf", &alpha);
    for (int i = 0; i < u; ++i) {
        userData x;
        x.id = read(), x.speed = read(), x.data = read(), x.factor = read(), x.weight = read();
        x.speed *= x.weight;
        x.id --;
        x.factor *= 0.01;
        v.push_back(x);
        user[i] = x;
        data_all += user[i].data;
    }
    for (int i = 0; i < n; ++i) {
        q.push({0, i});
    }

    // Sorting the v from high to low
    sort(v.rbegin(), v.rend());
    if (0) {
        // if (tc_id == 9) {
        //     v.clear();
        //     for (int i = 0; i < u; ++i) {
        //         v.push_back(user[i]);
        //         sort(v.begin(), v.end(), cmp);
        //     }
        //     for (int j = 0; j < n; ++j) {
        //         for (int k = 0; k < v.size(); ++k) {
        //             int i = v[k].id;
        //             if (data_sent[i] < user[i].data) {
        //                 pair<double, int> p = {user[i].factor, j};
        //                 p = allocate_user(user[i], p);
        //                 allocated_channel[i][j] = 1;
        //                 channel_factor_final[j] = p.first;
        //                 break;
        //             }
        //         }
        //     }
        //     for (int j = 0; j < n; ++j) {
        //         for (int k = v.size(); k >=0; --k) {
        //             int i = v[k].id;
        //             if (data_sent[i] < user[i].data) {
        //                 pair<double, int> p = {user[i].factor, j};
        //                 p = allocate_user(user[i], p);
        //                 allocated_channel[i][j] = 1;
        //                 channel_factor_final[j] = p.first;
        //                 break;
        //             }
        //         }
        //     }
        //     for (int j = 0; j < n; ++j) {
        //         q.push({channel_factor_final[j], j});
        //     }
        // }
        bool all_done_flag = false;
        while (!all_done_flag) {
        all_done_flag = true;
        for (auto x : v) {
            vector<pair<double, int> > q_;
            q_.clear();
            all_done_flag &= (data_sent[x.id] >= x.data); // If data-sent is fulfilled, all-done flag AND with true.
            while (data_sent[x.id] < x.data && !q.empty()) {
                pair<double, int> p = q.top(); q.pop();
                if (!allocated_channel[x.id][p.second] && channel[p.second].size() != m) {
                    p = allocate_user(x, p);
                    allocated_channel[x.id][p.second] = 1;
                    q.push(p);
                    channel_factor_final[p.second] = p.first;
                }
                else {
                    q_.push_back(p);
                }
//            printf("%d %d %lf\n", x.id, p.second, p.first);
            }
            for (auto & j : q_) q.push(j);
            if (q_.size() == n) {all_done_flag = true; break;} // If all channels can not fit user x, which means allocation fails, end the loop directly.
        }
    }
    }

    else {


        
    
    
     // Allocating user x in loop, until all done
    bool all_done_flag = false;
    while (!all_done_flag) {
        all_done_flag = true;
        for (auto x : v) {
            // if (ignore_user[x.id]) continue;
            vector<pair<double, int> > q_;
            q_.clear();
            all_done_flag &= (data_sent[x.id] >= x.data); // If data-sent is fulfilled, all-done flag AND with true.
            while (data_sent[x.id] < x.data && !q.empty()) {
                pair<double, int> p = q.top(); q.pop();
                
                    if (!allocated_channel[x.id][p.second]) {
                        int j = p.second;
                        if (channel[j].size() <= m && channel[j].size() >= 1) {
                            int k = Rand(channel[j].size());
                            userData x = user[k];
                            pair<double, int> pp = deallocate_user(x, {channel_factor_final[j], j});
                            allocated_channel[k][j] = 0;
                            channel_factor_final[j] = pp.first;
                            p = pp;
                        }
                        p = allocate_user(x, p);
                        allocated_channel[x.id][p.second] = 1;
                        channel_factor_final[p.second] = p.first;
                        q.push(p);
                    }
                    else {
                        q_.push_back(p);
                    }
//            printf("%d %d %lf\n", x.id, p.second, p.first);
            }
            for (auto & j : q_) q.push(j);
            if (q_.size() == n) {
                all_done_flag = true; break;
                // ignore_user[x.id] = 1;
            } // If all channels can not fit user x, ignore it in the following.
        }
    }
    }
    

    // organize_data();
    // int cnt_ = 0;
    // while (data_loss > 0) {
    //     cnt_ ++;
    //     if (cnt_ > 10) break;
    //     for (int i = 0; i < u; i++)
    //     {
    //         if (data_sent[i] < user[i].data) {
    //             vector <int> J;
    //             for (int k = 0; k < n; k++)
    //             {
    //                 J.push_back(k);
    //             }
                
    //             random_shuffle(J.begin(), J.end(), pointer_to_unary_function<int, int>(Rand));
    //             for (auto j: J)
    //             {
    //                 if (!allocated_channel[i][j]) {
    //                     if (channel[j].size() == m) {
    //                         int k = Rand(m);
    //                         userData x = user[k];
    //                         pair<double, int> p = deallocate_user(x, {channel_factor_final[j], j});
    //                         allocated_channel[k][j] = 0;
    //                         channel_factor_final[j] = p.first;
    //                     }
    //                     userData x = user[i];
    //                     pair<double, int> p = allocate_user(x, {channel_factor_final[j], j});
    //                     allocated_channel[i][j] = 1;
    //                     channel_factor_final[j] = p.first;
    //                 }
    //             }
    //         }
    //     }
    //     organize_data();
    // }
    

    output();
    
    
    return 0;
}
