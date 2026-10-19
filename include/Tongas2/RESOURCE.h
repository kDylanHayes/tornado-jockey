// The base classes of everything the resource system holds: RESOURCEMASTER
// (a loaded resource, which hands out copies of itself), RESOURCE (one copy)
// and OBJECT (a copy placed in the world). Placeholder names apart from the
// class names the command tables register.
#ifndef TONGAS2_RESOURCE_H
#define TONGAS2_RESOURCE_H

#include "Tongas2/CMDBASE.h"
#include "Tongas2/CSTRING.h"
#include "Tongas2/FIXEDSTRING.h"
#include "Tongas2/MATHS.h"
#include "Tongas2/SAFEINDEX.h"

class RESOURCE;
class RESOURCEMANAGER;
class OBJECT;

// Placeholder for the object at 0x6d09d4; only slot 25 is used here.
class GAME
{
public:
    virtual void Virtual00();
    virtual void Virtual01();
    virtual void Virtual02();
    virtual void Virtual03();
    virtual void Virtual04();
    virtual void Virtual05();
    virtual void Virtual06();
    virtual void Virtual07();
    virtual void Virtual08();
    virtual void Virtual09();
    virtual void Virtual10();
    virtual void Virtual11();
    virtual void Virtual12();
    virtual void Virtual13();
    virtual void Virtual14();
    virtual void Virtual15();
    virtual void Virtual16();
    virtual void Virtual17();
    virtual void Virtual18();
    virtual void Virtual19();
    virtual void Virtual20();
    virtual void Virtual21();
    virtual void Virtual22();
    virtual void Virtual23();
    virtual void Virtual24();
    virtual bool Virtual25();
};
extern GAME *g_pGame;

// Defined in another unit; each name is 30 characters, indexed by class id.
extern char g_classNames[][30];

class RESOURCEMASTER : public CMDBASE
{
public:
    RESOURCEMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~RESOURCEMASTER();

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args);
    // Takes the path ("CODE:name"), and sets the name without its type
    // code and the file name, with each '.' escaped as "#.".
    virtual void SetName(const CSTRING &path);
    // Defined elsewhere.
    virtual int Virtual08();
    virtual void *Virtual09(const char *name);
    virtual void Virtual13();
    virtual void Virtual14();

    // Masters of most types have no copies.
    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);
    virtual void Virtual20() {}
    // How many copies CreateCopies makes at a time.
    virtual int GetCopiesPerBatch() { return 1; }
    virtual void SetGroupMask(const int &mask) { m_groupMask = mask; }
    virtual void SetMinCopies(const int &count);
    virtual void SetDescription(const int &description) {}
    virtual void SetPreview(const int &preview) {}
    // Calls Update on the active copies.
    virtual void UpdateCopies();
    // Loads, then creates the copies asked for.
    virtual bool Load();
    // Deletes the free copies (and frees the used ones first if all is set).
    virtual void DeleteCopies(bool all);
    // Deletes the free copies once there are enough of them.
    virtual void TrimCopies();
    // A free copy, now in use.
    virtual RESOURCE *GetCopy();
    // Called once before the first copies are made.
    virtual void Prepare() {}
    virtual void ReleaseAll();
    virtual void ResetCopies();

    void RequestCopy();
    void CreateCopies();
    void ReleaseCopy(RESOURCE *copy);
    void Release(bool all);
    void FreeUsed();

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    CSTRING m_fileName;
    int m_requested;
    int m_numCopies;
    int m_numUsed;
    bool m_loaded;
    bool m_prepared;
    int m_30;
    int m_level;
    int m_38;
    int m_3c;
    RESOURCE *m_used;
    RESOURCE *m_free;
    bool m_48;
    int m_groupMask;
    int m_minCopies;
    CSTRING m_description;
    bool m_preview;
    RESOURCEMANAGER *m_manager;
};

class RESOURCE : public CMDBASE
{
public:
    RESOURCE(RESOURCEMASTER *master, const RESHANDLE &handle);
    // Defined elsewhere.
    virtual ~RESOURCE();

    static void RegisterCommands();

    // Defined elsewhere.
    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args);
    virtual int Virtual08();
    virtual void *Virtual09(const char *name);

    // Defined elsewhere.
    virtual void Init();
    virtual void Virtual20();
    virtual void Virtual21();
    virtual void Virtual22();
    virtual void Virtual23();
    virtual void Update();
    virtual bool IsLoaded() = 0;
    // The id of the master's manager.
    virtual int GetManagerId();
    virtual void UpdateAngles();
    virtual void Virtual28();

    static int s_classId;
    static CMDENTRY s_commands[];

    bool m_active;
    RESOURCE *m_next;
    RESOURCE *m_prev;
    RESOURCEMASTER *m_master;
    bool m_initialized;
};

class WTACTOR;

// Defined elsewhere: finds the actor with a handle.
class ACTORS
{
public:
    WTACTOR *GetActor(RESHANDLE handle);
};
extern ACTORS *g_pActors;

// Defined elsewhere: the bone or point of an actor an object is attached to.
class TAG
{
public:
    virtual ~TAG();
    virtual V3 GetPosition();
    virtual ANG GetAngles();
};

// Defined elsewhere: an actor in the world. Only what objects use of it.
class WTACTOR : public RESOURCE
{
public:
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
    // The tag with a name, or 0.
    virtual TAG *GetTag(const char *name);
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
    virtual V3 GetWorldPosition();
    virtual ANG GetWorldAngles();
    virtual V3 GetWorldScale();
    virtual void Virtual93();
    virtual void Virtual94();
    virtual void Virtual95();
    virtual void Virtual96();
    virtual V3 LocalToWorld(V3 point);

    char m_30[0x174 - 0x30];
    float m_fadeDistance;
    char m_178[0x970 - 0x178];
    float m_alpha;
    char m_974[0x984 - 0x974];
    int m_layer;
    int m_988;
    int m_visibility;
    char m_990[0x9a0 - 0x990];
    bool m_placed;
};

// Defined elsewhere.
extern WTACTOR *g_pCamera;

class OBJECT : public RESOURCE
{
public:
    // Defined elsewhere.
    OBJECT(RESOURCEMASTER *master, const RESHANDLE &handle);
    void SetActive(bool active);
    // Stops following the parent.
    void Detach();

    // Defined elsewhere.
    virtual void Init();
    virtual void Virtual20();
    // Follows the parent and the tag attached to, and works out where to
    // draw the object.
    virtual void Update();
    virtual bool IsLoaded() = 0;
    // Brings the euler angles up to date (only for the log).
    virtual void UpdateAngles();
    // Defined elsewhere.
    virtual void Virtual28();
    virtual void SetFlag54(const bool &value);
    virtual void Virtual30();
    virtual void Virtual31();
    virtual void Virtual32() = 0;
    // Takes the parent's placement, offset by the object's own.
    virtual void FollowParent(WTACTOR *parent);
    // Defined elsewhere.
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
    virtual void SetPosition(V3 position);
    virtual void Virtual51();
    virtual void SetScale(float x, float y, float z);
    virtual void SetScaleV(V3 scale);
    virtual void Virtual54();
    virtual void SetRotation(float x, float y, float z);
    virtual void SetAngles(ANG angles);
    virtual void Virtual57();
    virtual void SetLayer(int layer);
    virtual void Virtual59();
    virtual void SetAlpha(float alpha);
    virtual void Virtual61();
    virtual void SetVisibility(int value);
    virtual void Virtual63();
    virtual void SetHidden(bool hidden);
    // Between world space and the object's space.
    virtual V3 WorldToLocal(const V3 &point);
    virtual V3 LocalToWorld(const V3 &point);
    // Defined elsewhere.
    virtual void Virtual67();
    virtual void Virtual68();
    virtual void Virtual69();

    // The inverse of the object's rotation.
    MATRIX GetWorldMatrix();
    void AttachTo(WTACTOR *actor, const char *tag);

    RESHANDLE m_parent;
    V3 m_offset;
    V3 m_scaleFactor;
    bool m_54;
    ANG m_relAngles;
    bool m_a8;
    int m_ac;
    float m_alphaFactor;
    V3 m_position;
    V3 m_c0;
    V3 m_scale;
    V3 m_d8;
    ANG m_angles;
    ANG m_134;
    V3 m_184;
    V3 m_190;
    int m_19c;
    int m_1a0;
    int m_layer;
    int m_lastLayer;
    int m_1ac;
    float m_fadeDistance;
    bool m_1b4;
    V3 m_drawPosition;
    V3 m_drawScale;
    V3 m_lastPosition;
    V3 m_lastScale;
    float m_1e8;
    TAG *m_tag;
    FIXEDSTRING<0x200> m_tagName;
    RESHANDLE m_attached;
    bool m_3fc;
};

#endif
