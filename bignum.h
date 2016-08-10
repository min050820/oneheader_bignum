#ifndef BIGNUM_H_INCLUDED
#define BIGNUM_H_INCLUDED

#include <bitset>
#include <string>
#include <stdint.h>

template<int BITS>
class bignum {
protected:
    //index 0 = LSB
    std::bitset<BITS> num;

public:
    bignum() {}
    bignum(const uintmax_t init) { copy(init); };
    bignum(const bignum& init);
    bignum& copy(uintmax_t init);
    bignum& copy(const bignum& x);

    bignum& add(const bignum& x);
    bignum& sub(const bignum& x);
    bignum& mul(const bignum& x);
    bignum& div(const bignum& x);
    bignum& inc();
    bignum& dec();

    bignum& band(const bignum& x);
    bignum&  bor(const bignum& x);
    bignum& bxor(const bignum& x);
    bignum& bnot();

    bignum& shr(const unsigned int x) { num>>=x; return *this; }
    bignum& shl(const unsigned int x) { num<<=x; return *this; }
    bignum& ror(const unsigned int x);
    bignum& rol(const unsigned int x);

    std::string toString(int base=2);
};

template<int BITS>
bignum<BITS>& bignum<BITS>::copy(uintmax_t x) {
    num=x;
    /*while(x!=0) {
        num[0]=x&1;
        shl(1);
        x>>=1;
    }*/
    return *this;
}

template<int BITS>
bignum<BITS>& bignum<BITS>::add(const bignum<BITS>& x) {
    //bitwise calculating table.
    //bits order : carry, a, b
    //result&1 = x, result&2=carry
    static const int calctable[8] = {
        0, 1, 1, 2, 1, 2, 2, 3
    };
    bool c=false; //carry
    for(int i=0; i<BITS; i++) {
        const bool a=num[i], b=x.num[i];
        num[i]=calctable[(c<<2)|(b<<1)|(a)]&1;
        c=calctable[(c<<2)|(b<<1)|(a)]&2;
    }
    return *this;
}

template<int BITS>
bignum<BITS>& bignum<BITS>::sub(const bignum<BITS>& x) {
    //Use 2's complement.
    bignum<BITS> t;
    t.copy(x);
    t.bnot();
    t.inc();
    return add(x);
}

template<int BITS>
std::string bignum<BITS>::toString(int base) {
    std::string ret;
    for(int i=BITS; i>=0; i--)
        ret.push_back(num[i]?'1':'0');
    return ret;
}

#endif // BIGNUM_H_INCLUDED
using namespace std;

int main() {
    bignum<8> num(0b10);
    num.sub(1);
    cout << num.toString();
    return 0;
}
