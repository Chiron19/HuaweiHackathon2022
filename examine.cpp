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
    for (int tc_id = 1; tc_id <= 10; ++tc_id) {
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
