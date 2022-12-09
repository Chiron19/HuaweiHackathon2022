#include <bits/stdc++.h>
#include <stdlib.h>
using namespace std;

inline double read(){  // fast read template, to ignore ',' and get a integer
    int s = 0, w = 1, m = 0, n = 1;
    char ch = getchar();
    if (ch == EOF) return 1919810;
    while(ch < '0' || ch > '9') { if(ch == '-') w = -1; ch = getchar(); }
    while(ch >= '0' && ch <= '9') s = s * 10 + ch - '0', ch = getchar();
    if (ch == '.') {
        ch = getchar();
        while(ch >= '0' && ch <= '9') m = m * 10 + ch - '0', n *= 10, ch = getchar();
        return s*w+(double)m/n;
    }
    else return s*w;
}

int main() {
    double tot = 0;
    int cnt_valid = 0;
    for (int tc_id = 1; tc_id <= 13; ++tc_id) {
        char intStr[100], path[100];
        itoa(tc_id, intStr, 10);
        strcpy(path, R"(.\result\)");
        strcat(path, intStr);
        strcat(path, ".csv");
        freopen(path, "r", stdin); // <- Here to change the input file's path!
        vector <double> v;
        while (1) {
            double a = read();
            // printf("%lf\n", a);
            if (a == 1919810) {
                if (v[v.size()-2] > 0.0) cnt_valid++;
                printf("%-3d %13.6lf\n", tc_id, v[v.size()-2]);
                tot += v[v.size()-2];
                break;
            }
            else v.push_back(a);
        }
    }
    printf("-----------------------\n");
    printf("%-3d %13.6lf\n", cnt_valid, tot);
    system("pause");
    return 0;
}
