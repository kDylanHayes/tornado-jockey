// The behaviors the actors' addBehavior command makes, by name: BLINK,
// GROW, TRAVEL and the rest (BEHAVIORS.h). The class name is from the
// constructor's symbol; the rest are placeholders.
#ifndef TONGAS2_GUIBEHAVIORS_H
#define TONGAS2_GUIBEHAVIORS_H

struct RESHANDLE;
struct CMDCALL;
class BEHAVIOR;

// A behavior's name, the hash of it and what makes it for an actor.
struct BEHAVIORENTRY
{
    const char *name;
    int hash;
    BEHAVIOR *(*create)(const RESHANDLE &owner, CMDCALL *call);
};

class GUIBEHAVIORS
{
public:
    // Hashes the names.
    GUIBEHAVIORS();

    // The behavior with a name, or 0.
    BEHAVIORENTRY *Find(const char *name);
};
extern GUIBEHAVIORS *g_pGuiBehaviors;

#endif
