#ifndef TEST_FSM_HPP
#define TEST_FSM_HPP
#include <stdio.h>
#include "hfsm.hpp"

//{{{ TestHSM
enum Signal
{
  A_SIG,B_SIG,C_SIG,D_SIG,
  E_SIG,F_SIG,G_SIG,H_SIG
};

class TestHSM
{
public:
  TestHSM();
  ~TestHSM() {};
  void next( const TopState<TestHSM>& state )
  {
    state_ = &state;
  }

  Signal getSig() const { return sig_; }

  void dispatch(Signal sig)
  {
    sig_ = sig;
    state_->handler(*this);
  }

  void foo(int i) { foo_ = i; }
  int foo() const { return foo_; }

private:
  const TopState<TestHSM>* state_;
  Signal sig_;
  int foo_;
};
//}}}

//{{{ States
typedef CompState<TestHSM,0>     Top;
typedef CompState<TestHSM,1,Top>  S0;
typedef CompState<TestHSM,2,S0>     S1;
typedef LeafState<TestHSM,3,S1>       S11;
typedef CompState<TestHSM,4,S0>     S2;
typedef CompState<TestHSM,5,S2>       S21;
typedef LeafState<TestHSM,6,S21>        S211;
//}}}

//{{{ handle
template<> template<typename X>
inline void S0::handle(TestHSM& h, const X& x) const //{{{
{
  switch( h.getSig() )
  {
    case E_SIG: { Tran<X,This,S211> t(h); printf("s0-E;"); return; }

    default:
      break;
  }
  return Base::handle(h,x);
} //}}}

template<> template<typename X>
inline void S1::handle(TestHSM& h, const X& x) const //{{{
{
  switch( h.getSig() )
  {
    case A_SIG: { Tran<X,This,S1>   t(h); printf("s1-A;"); return; }
    case B_SIG: { Tran<X,This,S11>  t(h); printf("s1-B;"); return; }
    case C_SIG: { Tran<X,This,S2>   t(h); printf("s1-C;"); return; }
    case D_SIG: { Tran<X,This,S0>   t(h); printf("s1-D;"); return; }
    case F_SIG: { Tran<X,This,S211> t(h); printf("s1-F;"); return; }
    default: break;
  }
  return Base::handle(h,x);
} //}}}

template<> template<typename X>
inline void S11::handle(TestHSM& h, const X& x) const //{{{
{
  switch( h.getSig() )
  {
    case G_SIG: { Tran<X,This,S211> t(h); printf("s11-G;"); return; }
    case H_SIG:
                if(h.foo())
                {
                  printf("s11-H;");
                  h.foo(0); return;
                } break;
    default: break;
  }
  return Base::handle(h,x);
} //}}}

template<> template<typename X>
inline void S2::handle(TestHSM& h, const X& x) const //{{{
{
  switch( h.getSig() )
  {
    case C_SIG: { Tran<X,This,S1>   t(h); printf("s2-C;"); return; }
    case F_SIG: { Tran<X,This,S11>  t(h); printf("s2-F;"); return; }
    default: break;
  }
  return Base::handle(h,x);
} //}}}

template<> template<typename X>
inline void S21::handle(TestHSM& h, const X& x) const //{{{
{
  switch( h.getSig() )
  {
    case B_SIG: { Tran<X,This,S211> t(h); printf("s21-B;"); return; }
    case H_SIG:
                if(!h.foo())
                {
                  Tran<X,This,S21> t(h);
                  printf("s21-H;");
                  h.foo(1);
                  return;
                } break;
    default: break;
  }
  return Base::handle(h,x);
} //}}}

template<> template<typename X>
inline void S211::handle(TestHSM& h, const X& x) const
{
  switch( h.getSig() )
  {
    case D_SIG: { Tran<X,This,S21>  t(h); printf("s211-D;"); return; }
    case G_SIG: { Tran<X,This,S0>   t(h); printf("s211-G;"); return; }

    default: break;
  }
  return Base::handle(h,x);
}
//}}}

//{{{ entry actions
template<> inline void Top::entry(TestHSM&)   { printf("Top-ENTRY;"); }
template<> inline void  S0::entry(TestHSM&)   { printf("s0-ENTRY;"); }
template<> inline void  S1::entry(TestHSM&)   { printf("s1-ENTRY;"); }
template<> inline void S11::entry(TestHSM&)   { printf("s11-ENTRY;"); }
template<> inline void  S2::entry(TestHSM&)   { printf("s2-ENTRY;"); }
template<> inline void S21::entry(TestHSM&)   { printf("s21-ENTRY;"); }
template<> inline void S211::entry(TestHSM&)  { printf("s211-ENTRY;"); }
//}}}

//{{{ exit actions
template<> inline void Top::exit(TestHSM&)    { printf("Top-EXIT;"); }
template<> inline void  S0::exit(TestHSM&)    { printf("s0-EXIT;"); }
template<> inline void  S1::exit(TestHSM&)    { printf("s1-EXIT;"); }
template<> inline void S11::exit(TestHSM&)    { printf("s11-EXIT;"); }
template<> inline void  S2::exit(TestHSM&)    { printf("s2-EXIT;"); }
template<> inline void S21::exit(TestHSM&)    { printf("s21-EXIT;"); }
template<> inline void S211::exit(TestHSM&)   { printf("s211-EXIT;"); }
//}}}

//{{{ init actions (note the reverse ordering!)
template<> inline void S21::init(TestHSM& h)  { Init<S211> i(h);  printf("s21-INIT;"); }
template<> inline void S2::init(TestHSM& h)   { Init<S21> i(h);   printf("s2-INIT;"); }
template<> inline void S1::init(TestHSM& h)   { Init<S11> i(h);   printf("s1-INIT;"); }
template<> inline void S0::init(TestHSM& h)   { Init<S1> i(h);    printf("s0-INIT;"); }
template<> inline void Top::init(TestHSM& h)  { Init<S0> i(h);    printf("Top-INIT;"); }
//}}}

TestHSM::TestHSM()
{
  foo_ = 0;
  Top::init( *this );
}


#endif
