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
#define Umax 500
#define Nmax 2000
int m, n, u;
int speed_to_data[27] = {0,290,575,813,1082,1351,1620,1889,2158,2427,2696,2965,3234,3503,3772,4041,4310,4579,4848,5117,5386,5655,5924,6093,6360,6611,6800};
double alpha;
struct User{
    int i, w, v0, d;
    double k, b;
    bool operator < (const User &A) const{
        if (k < A.k) return true;
        else if ((w*v0 == A.w*A.v0) && (k < A.k)) return true;
        else if ((w*v0 == A.w*A.v0) && (k == A.k) && (d < A.d)) return true;
        return false;
    }
} user[Umax];
vector<User> vec;
int e[Umax][Nmax], e_c[Umax], e_r[Nmax];
double e_r_k[Nmax], e_c_q[Umax]; 
double v[Umax][Nmax];
vector<int> channel[Nmax];
char intStr[100], path[100];
clock_t clock_start, clock_end;
vector <double> avg_speed[Umax];  
double avg_speed_all, best_speed_all, data_loss, data_all, object_function, penalty_term, score;
int data_sent[Umax];

int Rand(int n)
{
    srand(time(NULL));
    return rand() % n ;
}

inline int read(){  // fast read template, to ignore ',' and get a integer
    int s = 0, w = 1;
    char ch = getchar();
    // while(ch < '0' || ch > '9') { if(ch == '-') w = -1; ch = getchar(); }
    while(ch >= '0' && ch <= '9') s = s * 10 + ch - '0', ch = getchar();
    return s*w;
}

inline void input_path()
{
    // Only need to input test case's number
    int tc_id;
    scanf("%d", &tc_id);
    itoa(tc_id, intStr, 10);
    strcpy(path, "./test_cases/tc");
    strcat(path, intStr);
    strcat(path, ".csv");
    freopen(path, "r", stdin); // <- Here to change the input file's path!
}

inline int Q(double x)
{
    if (x >= 0)
    return speed_to_data[(int)x];
    return 0;
}

inline double G()
{
    double res = 0;
    for (int i = 0; i <= u; i++)
    {
        double q = 0;
        for (int l = 1; l <= u; l++)
        {
            double t = 0;
            for (int j = 1; j <= n; j++)
                t += e[l][j] * e[i][j];
            q += user[l].k * t;
        }
        if (e_c[i] != 0) 
            res += user[i].b * q / e_c[i];
    }
    printf("%lf\n", res);
    return res;
}

inline double F()
{
    double res = 0;
    for (int i = 0; i <= u; i++)
    {
        res += user[i].w * user[i].v0 * (1-user[i].k*user[i].k);
    }
    return res - G();
}

inline void upgrade_e(int i, int j)
{
    if (e[i][j]) return;
    e[i][j] = 1;
    e_c[i] += 1;
    e_r[j] += 1;
    e_r_k[j] += user[i].k;
    // for (int j = 1; j <= n; j++)
    // {
    //     printf("%lf ", e_r_k[j]);
    // }
    // printf("\n");
}

inline void upgrade_v(int i, int j)
{
    for (int i = 1; i <= u; i++)
    {
        v[i][j] = user[i].v0 * (1- (e_r_k[j]-e[i][j]*user[i].k)*user[i].k);
    }
}

inline void upgrade_d(int i, int j)
{
    e_c_q[i] = 0;
    for (int j = 1; j <= n; j++)
    {
        e_c_q[i] += e[i][j] * Q(v[i][j]);
    }
}

inline void recover_e(int i, int j)
{
    if (!e[i][j]) return;
    e[i][j] = 0;
    e_c[i] -= 1;
    e_r[j] -= 1;
    e_r_k[j] -= user[i].k;
    // for (int j = 1; j <= n; j++)
    // {
    //     printf("%lf ", e_r_k[j]);
    // }
    // printf("\n");
}

inline void recover_v(int i, int j)
{
    for (int i = 1; i <= u; i++)
    {
        v[i][j] = user[i].v0 * (1- (e_r_k[j]-e[i][j]*user[i].k)*user[i].k);
    }
}

inline void recover_d(int i, int j)
{
    e_c_q[i] = 0;
    for (int j = 1; j <= n; j++)
    {
        e_c_q[i] += e[i][j] * Q(v[i][j]);
    }
}

int find_column_j_fill_user_i(int i)
{
    int j_ = 0, e_r_k_ = 100;
    for (int jj = 0; jj < vec.size(); jj++)
    {
        int j = vec[jj].i;
        // printf("%d\n", j);
        if (!e[i][j] && e_r_k[j] + user[i].k < 1 && e_r[j] < m) {
            if (e_r_k_ > e_r_k[j] + user[i].k) {
                e_r_k_ = e_r_k[j] + user[i].k;
                j_ = j;
            }
        }
    }
    return j_;
}

int find_in_column_j_replace_user_i(int I, int j)
{
    int k_max = 0, i_k_max = 0;
    for (int i = 1; i <= u; i++)
    {
        if (e[i][j] && user[i].k > k_max) {
            k_max = user[i].k;
            i_k_max = i;
        }
    }
    return i_k_max;
}

void organize_data()// Organizing data
{
    memset(avg_speed, 0, sizeof avg_speed);
    data_all = avg_speed_all = best_speed_all = data_loss = object_function = penalty_term = score = 0;

    for (int j = 1; j <= n; ++j) {
        // Withdraw all speed of the user j.id
        double a = e_r_k[j];
        for (int i = 1; i <= u; i++) {
            if (e[i][j]) {
                double f = 1- user[i].k*(a - user[i].k);
                if (f <= 0) f = 0;
                avg_speed[i].push_back(user[i].v0 * f);
                data_sent[i] += Q(user[i].v0 * f);
            }
        }
    }
    for (int i = 1; i <= u; ++i) {
        // Calculate user i's average speed
        double x = 0;
        for (double j : avg_speed[i]) {
            x += j;
        }
        if (avg_speed[i].size()) {
            avg_speed[i].push_back(x/(double)avg_speed[i].size());
            avg_speed_all += avg_speed[i][avg_speed[i].size()-1] * user[i].w;
        }
        best_speed_all += user[i].v0 * user[i].w;
        // Calculate user i's data loss
        if (data_sent[i] < user[i].d) data_loss += user[i].d-data_sent[i];
        data_all += user[i].d;
    }
    // Calculate the object function and score
    object_function = avg_speed_all / best_speed_all;
    // printf("%lf %lf ")
    penalty_term = data_loss / data_all;
    score = object_function - alpha * penalty_term;
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

    for (int i = 1; i <= u; i++)
    {
        for (int j = 1; j <= n; j++)
        {
            if (e[i][j]) channel[j].push_back(i);
        }
    }

    for (int i = 0; i < m; i++)
    {
        for (int j = 1; j <= n; j++)
        {
           if (channel[j].size() > i) 
            printf("U%d", channel[j][i]);
            else printf("-");
            printf(j == n ? "\n": ",");
        }
    }
   
    for (int i = 1; i <= u; ++i) {
        if (data_sent[i] < user[i].d) printf("%d,", user[i].d-data_sent[i]);
        else printf("%d,", 0);
    }
    printf("%lf\n", penalty_term);
    
    for (int i = 1; i <= u; ++i) {
        if (avg_speed[i].size()) printf("%lf,", avg_speed[i][avg_speed[i].size()-1]);
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
    input_path();
    scanf("%d%*c%d%*c%d%*c", &m, &n, &u);
    scanf("%lf", &alpha);
    printf("%d %d %d %lf\n", m, n, u, alpha);
    for (int i = 0; i < u; ++i) {
        User x;
        scanf("%d%*c%d%*c%d%*c%lf%*c%d%*c", &x.i, &x.v0, &x.d, &x.k, &x.w);
        // x.i = read(), x.v0 = read(), x.d = read(), x.k = read(), x.w = read();
        // printf()
        x.k *= 0.01;
        x.b = x.w * x.v0 * x.k;
        user[x.i] = x;
        vec.push_back(x);
    }
    
    // for (int i = 0; i < vec.size(); i++)
    // {
    //     printf("-- %d\n", vec[i].i);
    // }
    
    //  for (int i = 1; i <= u; i++)
    // {
    //     printf("----- %d %lf %lf\n", user[i].i, user[i].k, user[i].b);
    // }
    // Sorting the v from high to low
    sort(vec.begin(), vec.end());

    int cnt_down = 100;
    while (cnt_down--)
    {
        int cnt_flag = 0;
        int mark_unfinshed[Umax];
        while (cnt_flag != u)
        {
            cnt_flag = 0;
            for (int i = 1; i <= u; i++)
            {
                mark_unfinshed[i] = 0;
                // printf("(%d)\n", i);
                if (e_c_q[i] >= user[i].d) {cnt_flag++; continue;}
                while (e_c_q[i] < user[i].d) {
                    int j = find_column_j_fill_user_i(i);
                    // printf("(%d,%d)\n", i, j);
                    if (j) {
                        upgrade_e(i, j);
                        upgrade_v(i, j);
                        upgrade_d(i, j);
                    }
                    else {
                        int ii = find_in_column_j_replace_user_i(i, j);
                if (ii) {
                    if (1) {
                        recover_e(ii, j);
                        recover_v(ii, j);
                        recover_d(ii, j);
                        upgrade_e(i, j);
                        upgrade_v(i, j);
                        upgrade_d(i, j);
                    }
                }
                   cnt_flag++;      mark_unfinshed[i] = 1; break;
                    }
                }
                // for (int j = 1; j <= n; j++)printf("%lf ", e_r_k[j]);printf("\n\n");
            }
        }

    while (cnt_flag != u)
    {
        cnt_flag = 0;
        int i_flag = 0;
        for (int i = 1; i <= u; i++)
        {
            if (!mark_unfinshed[i]) continue;
            for (int j = 1; j <= n; j++)
            {
                int ii = find_in_column_j_replace_user_i(i, j);
                if (ii) {
                    if (Rand(10) <= 7) {
                        recover_e(ii, j);
                        recover_v(ii, j);
                        recover_d(ii, j);
                        upgrade_e(i, j);
                        upgrade_v(i, j);
                        upgrade_d(i, j);
                        i_flag = 1;
                    }
                    
                }
            }
            if (i_flag) cnt_flag++;
        }
    }
    }
    

    output();
    return 0;
}