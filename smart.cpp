// created date : 2012/12/29 17:51:18
// last updated : 2012/12/29 21:00:21
// 
#include "smart.h"


#ifdef TEST
#include <iostream>
#include "hasPtr_I.h"
using namespace std;

class Cls : public nl::hasPtr<Cls>{
public:
    int num;
    ~Cls(){}
    static Ptr build(int n){ return Ptr(new Cls(n)); }
private:
    Cls(int n) : num(n){}
};

// void dump(nl::SmartList<Cls> &list){
//     nl::SmartList<Cls>::Ptr wp;
//     for(int i=0; wp = list.objAt(i); i++ ){
//         cout << (*wp).num << " ";
//     }
//     cout << endl;
// }
void dump(std::list<Cls::Ptr> &list){
    for( std::list<Cls::Ptr>::iterator ite=list.begin(); ite!=list.end(); ++ite ){
        cout << (*ite)->num << " ";
    }
    cout << endl;
}


int main(){
    Cls::Ptr objA = Cls::build(2);

//    nl::SmartList<Cls> list;
//    list.add(Cls::build(0));
//    list.add(Cls::build(1));
//    list.add(objA);
//    list.add(Cls::build(3));
//    dump(list);
//    
//    list.remove(objA.get());
//    list.remove(Cls::build(3).get());
//    dump(list);
    
    std::list<Cls::Ptr> lst2;
    lst2.push_back(Cls::build(0));
    lst2.push_back(Cls::build(1));
    lst2.push_back(objA);
    dump(lst2);
    cout << (find(lst2,objA)?"exist":"not exist") << endl;
    lst2.remove(objA);
    dump(lst2);
    cout << (find(lst2,objA)?"exist":"not exist") << endl;
    
    return 0;
}
#endif
