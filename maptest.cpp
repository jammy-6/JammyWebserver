#include <bits/stdc++.h>
using namespace std;

int main(){
    map<int,int> mm;
    mm[3] = 9;
    mm[4] = 10;
    
    for(auto &p : mm){
        mm.erase(p.first);
        std::cout<<p.second<<std::endl;
    }
}