#ifndef BIGNUM_H_INCLUDED
#define BIGNUM_H_INCLUDED

#include <bitset>
#include <string>
#include <cstddef> //std::size_t
#include <stdint.h>

template<int BITS>
class bignum {
protected:
    //[0] = LSB
    std::bitset<BITS> num;

public:
    bignum() {}
    bignum(const uintmax_t init) { copy(init); }
    bignum(const bignum& init) { copy(init); }
    bignum& copy(const uintmax_t init);
    bignum& copy(const bignum& x);
    void swap(bignum<BITS>& other);

    bignum& add(const bignum& x);
    bignum& sub(const bignum& x);
    bignum mul(bignum x);
    bignum div(bignum x);
    bignum mod(bignum x);
    bignum& inc();
    bignum& dec();

    bignum& band(const bignum& x) { num&=x.num; return *this; }
    bignum&  bor(const bignum& x) { num|=x.num; return *this; }
    bignum& bxor(const bignum& x) { num^=x.num; return *this; }
    bignum& bnot() { num=~num; return *this; }

    bignum& shr(const std::size_t x) { num>>=x; return *this; }
    bignum& shl(const std::size_t x) { num<<=x; return *this; }
    bignum& ror(const std::size_t x);
    bignum& rol(const std::size_t x);

    bool eq(const bignum& x) const { return num==x.num; }
    bool gre(const bignum& x) const;
    bool les(const bignum& x) const;
    bool geq(const bignum& x) const { return !les(x); }
    bool leq(const bignum& x) const { return !gre(x); }
    bool empty() { return !num.any(); }

    std::string to_string(const int base=2);
    operator intmax_t();
    operator uintmax_t();
    
    bignum& operator +=(const bignum& x) { add(x); return *this; }
    bignum& operator -=(const bignum& x) { sub(x); return *this; }
    bignum operator *(const bignum& x) { return mul(x); }
    bignum operator /(const bignum& x) { return div(x); }
    bignum operator %(const bignum& x) { return mod(x); }
    bignum& operator ++() { inc(); return *this; }
    bignum& operator --() { dec(); return *this; }
    /* 
     * Should there be post-increment?? Is it dont by compiler?
     * And if so, will having one help performance/memory-footprint/etc?
     */
    bignum& operator &=(const bignum& x) { band(x); return *this; }
    bignum& operator |=(const bignum& x) { bor(x); return *this; }
    bignum& operator ^=(const bignum& x) { bxor(x); return *this; }
    bignum& operator ~=() { bnot(); return *this; }
    bignum& operator >>=(const std::size_t x) { bshr(x); return *this; }
    bignum& operator <<=(const std::size_t x) { bshl(x); return *this; }
    bool operator ==(const bignum& x) { return eq(x); }
    bool operator >(const bignum& x) { return gre(x); }
    bool operator <(const bignum& x) { return les(x); }
    bool operator <=(const bingum& x) { return geq(x); }
    bool operator >=(const bignum& x) { return leq(x); }
};

template<int BITS>
bignum<BITS>& bignum<BITS>::copy(const uintmax_t x) {
    num=x;
    return *this;
}

template<int BITS>
bignum<BITS>& bignum<BITS>::copy(const bignum<BITS>& x) {
    num=x.num;
    return *this;
}

template<int BITS>
void bignum<BITS>::swap(bignum<BITS>& other) {
    //Optimization needed
    bitset<BITS> tmp = num;
    num = other.num;
    other.num = tmp;
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
    add(t);
    return *this;
}

template<int BITS>
bignum<BITS> bignum<BITS>::mul(bignum<BITS> x) {
    bignum<BITS> now(*this), ret;
    while(!x.empty()) {
        if(x.num[0])
            ret.add(now);
        now.shl(1);
        x.shr(1);
    }
    return ret;
}

/* Not done!!!
template<int BITS>
bignum<BITS> bignum<BITS>::div(bignum<BITS> x) {
    bignum<BITS> quo, rem;
    for(int i=0; i<BITS; i++) {
        rem.shl(1);
        rem.num[i]=num[i];
        if(rem.geq(x)) {
            rem.sub(x);
            quo.num[i]=true;
        }
    }
    return quo;
}

template<int BITS>
bignum<BITS> bignum<BITS>::mod(bignum<BITS> x) {
    bignum<BITS> quo, rem;
    for(int i=0; i<BITS; i++) {
        rem.shl(1);
        rem.num[i]=num[i];
        if(rem.geq(x)) {
            rem.sub(x);
            quo.num[i]=true;
        }
    }
    return rem;
}*/

template<int BITS>
bignum<BITS>& bignum<BITS>::inc() {
    //Thinks there was a carry. It allows to quit when the carry is cleared.
    for(int i=0; i<BITS && (i==0 ? true : !num[i-1]); i++)
        num[i]=!num[i];
    return *this;
}

template<int BITS>
bignum<BITS>& bignum<BITS>::dec() {
    //Same as inc() :)
    for(int i=0; i<BITS && (i==0 ? true : num[i-1]); i++)
        num[i]=!num[i];
    return *this;
}

template<int BITS>
bool bignum<BITS>::gre(const bignum<BITS>& x) const {
    for(int i=BITS-1; i>=0; i--)
        if(num[i] && !x.num[i])
            return true;
        else if(!num[i] && x.num[i])
            return false;
    return false;
}

template<int BITS>
bool bignum<BITS>::les(const bignum<BITS>& x) const {
    for(int i=BITS-1; i>=0; i--)
        if(!num[i] && x.num[i])
            return true;
        else if(num[i] && !x.num[i])
            return false;
    return false;
}

template<int BITS>
std::string bignum<BITS>::to_string(const int base) {
    static_assert(base==2); //For now, it only works for base 2
    return num.to_string();
}

//Proxy function for std::swap replacement
template<int BITS>
void swap(bitset<BITS>& x, bitset<BITS>& y) { x.swap(y); }

#endif // BIGNUM_H_INCLUDED
