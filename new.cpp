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
#define Umax 2005
#define Nmax 1605
struct userData{
    int v0, d, w;
    double k;
    bool operator < (const userData &A) const{
        if (v0 )
    }
}
int m, n, u;
int speed_to_data[27] = {0,290,575,813,1082,1351,1620,1889,2158,2427,2696,2965,3234,3503,3772,4041,4310,4579,4848,5117,5386,5655,5924,6093,6360,6611,6800};
double alpha;

int e[Umax][Nmax], v0[Umax], w[Umax], k[Umax], b[Umax], d[Umax];
vector<pair<int, int> > vec;

void input()
{
    scanf("%d%*c%d%*c%d%*c", &m, &n, &u);
    scanf("%lf", &alpha);
    for (int i = 0; i < u; i++)
    {
        scanf("%*d%*c%d%*c%d%*c%d%*c%d%*c%d%*c", &v0[i], &d[i], &k[i], &w[i]);
    }
}

int main()
{
    input();

    
    
    return 0;
}