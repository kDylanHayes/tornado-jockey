// The base of the game's actors, as far as the actor classes built on it
// use it: ACTOR is a CMDBASE with a second interface at 0x1c (ACTORLINK,
// placeholder name) and is 0xccc bytes. Its code is in another unit; the
// declarations here give its vtable slots and the fields its subclasses
// touch. Placeholder names apart from the class names.
#ifndef TONGAS2_ACTOR_H
#define TONGAS2_ACTOR_H

#include "Tongas2/CMDBASE.h"
#include "Tongas2/MATHS.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/SAFEINDEX.h"
#include "Tongas2/SRESULT.h"

struct CONTACT;

// Defined elsewhere: whether two handles are the same.
bool operator==(const RESHANDLE &a, const RESHANDLE &b);

// What a script command is run with: the tokens and who ran it.
struct CMDCALL
{
    TOKENS *tokens;
    RESHANDLE caller;
};

// Defined elsewhere: per factory id, the factory's flags.
extern int g_factoryFlags[];

// Defined elsewhere: the second interface actors have. ACTOR's overrides
// of it take this pointing at it.
class ACTORLINK
{
public:
    virtual ~ACTORLINK();
    virtual void Link01() = 0;
    virtual void Link02() = 0;
    virtual void Link03() = 0;
    virtual void Link04() = 0;
    virtual void Link05() = 0;
    virtual void Link06() = 0;
    virtual void Link07() = 0;
    virtual void Link08() = 0;
    virtual void Link09() = 0;
    virtual void Link10() = 0;
    virtual void Link11() = 0;
    virtual void Link12() = 0;
    virtual void Link13() = 0;
    virtual void Link14() = 0;

    int m_4;
    int m_8;
};

// Defined elsewhere.
class ACTOR : public CMDBASE, public ACTORLINK
{
public:
    ACTOR();
    virtual ~ACTOR();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args);
    virtual void SetName(const CSTRING &name);
    virtual int Virtual08();
    virtual void *Virtual09(const char *name);
    // The id the actor factory knows the class by, its parent's and its
    // factory flags.
    virtual int GetFactoryId();
    virtual int GetParentFactoryId();
    virtual int GetFactoryFlags();
    virtual void SetPosition(const V3 &position);
    virtual void Virtual23();
    virtual void Virtual24();
    virtual void SetScale(const V3 &scale);
    virtual void Virtual26();
    virtual void Virtual27();
    virtual void Virtual28();
    virtual void Virtual29();
    virtual void Virtual30();
    virtual void Virtual31();
    virtual void Virtual32();
    virtual void Virtual33();
    virtual void Virtual34();
    virtual void Virtual35();
    virtual void Virtual36();
    virtual void Virtual37();
    virtual void Virtual38();
    virtual void Virtual39();
    virtual void Virtual40();
    virtual void Virtual41();
    virtual void Virtual42();
    virtual void Virtual43();
    virtual void Virtual44();
    virtual void Virtual45();
    virtual void Virtual46();
    virtual void Virtual47();
    virtual void Virtual48();
    virtual void Virtual49();
    virtual void Virtual50();
    virtual void Virtual51();
    virtual void Virtual52();
    virtual void Virtual53();
    virtual void Virtual54();
    virtual void Virtual55();
    virtual void Virtual56();
    virtual void Virtual57();
    virtual void Virtual58();
    virtual void Virtual59();
    virtual void Virtual60();
    virtual void Virtual61();
    virtual void Virtual62();
    virtual void Virtual63();
    virtual void Virtual64();
    virtual void Virtual65();
    virtual void Virtual66();
    virtual void Virtual67();
    virtual void Virtual68();
    virtual void Virtual69();
    virtual void Virtual70();
    virtual void Virtual71();
    virtual void Virtual72();
    virtual void Virtual73();
    virtual void Virtual74();
    virtual void Virtual75();
    virtual void Virtual76();
    virtual void Virtual77();
    virtual void Virtual78();
    virtual void Virtual79();
    virtual void Virtual80();
    virtual void Virtual81();
    virtual void Virtual82();
    virtual void Virtual83();
    virtual void Virtual84();
    virtual void Virtual85();
    virtual void Virtual86();
    virtual void Virtual87();
    virtual void Virtual88();
    virtual void Virtual89();
    virtual void Virtual90();
    virtual void Virtual91();
    virtual void Virtual92();
    virtual void Virtual93();
    virtual void Virtual94();
    virtual void Virtual95();
    virtual void Virtual96();
    virtual void Virtual97();
    virtual void Virtual98();
    virtual void Virtual99();
    virtual void Virtual100();
    virtual void Virtual101();
    // Called every frame.
    virtual void Update();
    virtual void Virtual103();
    virtual void Virtual104();
    virtual void Virtual105();
    virtual void Virtual106();
    virtual void Virtual107();
    virtual void Virtual108();
    virtual void Virtual109();
    virtual void Virtual110();
    virtual void Virtual111();
    virtual void Virtual112();
    virtual void Virtual113();
    virtual void Virtual114();
    virtual void Virtual115();
    virtual void Virtual116();
    virtual void Virtual117();
    virtual void Virtual118();
    virtual void Virtual119();
    // A contact the collision manager found for the actor.
    virtual void OnContact(CONTACT *contact);
    virtual void Virtual121();
    virtual void Virtual122();
    virtual void Virtual123();
    virtual void Virtual124();
    virtual void Virtual125();
    virtual void Virtual126();
    virtual void Virtual127();
    virtual void Virtual128();
    virtual void Virtual129();
    virtual void Virtual130();
    virtual void Virtual131();
    virtual void Virtual132();
    virtual void Virtual133();
    virtual void Virtual134();
    virtual void Virtual135();
    virtual void Virtual136();
    virtual void Virtual137();
    virtual void Virtual138();
    // The actor touched another on the screen.
    virtual void OnCollide(ACTOR *other);

    virtual void Link01();
    virtual void Link02();
    virtual void Link03();
    virtual void Link04();
    virtual void Link05();
    virtual void Link06();
    virtual void Link07();
    virtual void Link08();
    virtual void Link09();
    virtual void Link10();
    virtual void Link11();
    virtual void Link12();
    virtual void Link13();
    virtual void Link14();

    static int s_classId;
    static int s_factoryId;

    V3 m_position;
    char m_34[0x84 - 0x34];
    V3 m_scale;
    char m_90[0xccc - 0x90];
};

#endif
