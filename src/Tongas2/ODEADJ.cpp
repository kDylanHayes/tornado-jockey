// G2ODEADJ::Call, which retail compiles in a different object file from the
// destructor that calls it (0x0054d0f0 sits next to 0x0054b0e0 but the two were
// not one translation unit). Keeping the body here is what makes
// G2ODEACTOR::~G2ODEACTOR emit retail's 141-byte SEH frame: when the callee is
// visible, VC7.1 proves the chain cannot throw and drops the frame entirely.
// build/dtor_lab.cpp reproduces the rule: a same-TU callee gives a 78-byte
// destructor with no __ehhandler, an opaque one gives 138-141 bytes with it.

struct G2ODEADJTGT
{
    void Call(void *arg);
};

struct G2ODEADJ
{
    void Call(void *arg);
};

__declspec(noinline) void G2ODEADJ::Call(void *arg)
{
    ((G2ODEADJTGT *)((char *)this + 4))->Call((char *)arg + 8);
}
