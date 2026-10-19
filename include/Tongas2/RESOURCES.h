// The resource system: RESOURCES owns the per-type RESOURCEMANAGERs and two
// global pools, the masters (loaded data) and the copies (instances of a
// master). Placeholder names apart from those the error strings give.
#ifndef TONGAS2_RESOURCES_H
#define TONGAS2_RESOURCES_H

#include "Tongas2/CMDBASE.h"
#include "Tongas2/CSTRING.h"
#include "Tongas2/MATHS.h"
#include "Tongas2/SAFEINDEX.h"

class RESOURCE
{
public:
    virtual void Virtual00();
    virtual ~RESOURCE();

    int m_4;
    RESHANDLE m_handle;
    CSTRING m_name;
};

class MASTER : public RESOURCE
{
public:
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
    virtual void Virtual25();
    virtual void Virtual26();
    virtual void Virtual27();
    virtual void Virtual28();
    virtual void Virtual29();
    // A copy to use, for kinds of resource that are used through copies.
    virtual RESOURCE *GetCopy();

    CSTRING m_fileName;
};

class COPY : public RESOURCE
{
public:
    int m_18[4];
    MASTER *m_master;
};

// What CreateFromTemplate makes: an instance placed in the world.
class OBJECT : public COPY
{
public:
    // Defined in another unit.
    void SetActive(bool active);

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
    virtual void Virtual25();
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
    virtual void SetPosition(V3 position);
    virtual void Virtual51();
    virtual void SetScale(float x, float y, float z);
    virtual void Virtual53();
    virtual void Virtual54();
    virtual void Virtual55();
    virtual void SetAngles(ANG angles);
    virtual void Virtual57();
    virtual void SetLayer(int layer);
    virtual void Virtual59();
    virtual void SetAlpha(float alpha);
    virtual void Virtual61();
    virtual void Virtual62();
    virtual void Virtual63();
    virtual void SetHidden(bool hidden);
};

// One type of resource (meshes, sounds, ...). Defined in another unit.
class RESOURCEMANAGER
{
public:
    virtual void Virtual00();
    virtual void Virtual01();
    virtual ~RESOURCEMANAGER();
    virtual void Update();
    virtual bool Load();
    // Whether name has this manager's file extension.
    virtual bool IsMyType(const char *name);
    virtual void Virtual06();
    virtual void Init();

    // Deletes the resources not used by level (all of them if all is set).
    void DeleteUnused(int level, bool all);
    void Flush();
    void SetLevel(int level);
    // Whether name starts with this manager's four-letter type code.
    bool HasTypeCode(const char *name);
    void Dump(bool verbose, int index);
    void DumpPropsType();
    void DumpTotals(bool verbose);
    RESHANDLE GetHandle(const char *name);
    RESOURCE *GetResource(const char *name);

    int m_id;
    int m_8[4];
    bool m_18;
};

typedef RESOURCEMANAGER *(*CREATEMANAGER)();

#define RESOURCES_MAX_MANAGERS 128
#define RESOURCES_MAX_GROUPS 31

// Called after loading, or when a level is set.
typedef void (*RESOURCESCALLBACK)();

template <class T>
class CALLBACKLIST
{
public:
    struct NODE
    {
        NODE() : prev(0), next(0), data(0) {}
        NODE *prev;
        NODE *next;
        T data;
    };

    ~CALLBACKLIST()
    {
        NODE *next = m_head;
        while (next) {
            NODE *node = next;
            next = next->next;
            delete node;
        }
    }

    NODE *NewNode(T data)
    {
        NODE *node = new NODE;
        node->data = data;
        return node;
    }

    void AddHead(T data)
    {
        NODE *node = NewNode(data);
        node->next = m_head;
        if (m_head)
            m_head->prev = node;
        m_head = node;
        if (!m_tail)
            m_tail = node;
    }

    void Call()
    {
        for (NODE *node = m_head; node; node = node->next)
            node->data();
    }

    NODE *m_head;
    NODE *m_tail;
};

class RESOURCES : public CMDBASE
{
public:
    RESOURCES();
    virtual ~RESOURCES();

    static CMDBASE *Create();
    static void RegisterCommands();
    static void LoadManagers();
    static void AddMainChannel();

    // Defined inline in the game's resource unit.
    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual void Virtual05();
    virtual int Virtual08();
    virtual void *Virtual09(const char *name);

    virtual void SetSoundVolume(const float &volume);
    virtual void SetMusicVolume(const float &volume);
    virtual SRESULT CmdDumpAll(TOKENS **args);
    virtual SRESULT CmdDump(TOKENS **args);
    virtual SRESULT CmdDumpTotals(TOKENS **args);
    virtual SRESULT CmdDumpPropsType(TOKENS **args);
    virtual SRESULT CmdDeleteUnused(TOKENS **args);
    virtual float GetMusicVolume();
    virtual float GetSoundVolume();
    virtual void Update();
    virtual void SetLevel(int level);
    virtual int ResetGroups();
    virtual int InitGroups();
    virtual int GetDetailBias();
    virtual float GetDetailScale();
    virtual float GetDetailFactor();
    virtual float GetGlobalScale();
    virtual void SetGlobalScale(float scale);
    virtual int GetDetailSteps();
    virtual bool IsLowDetail();
    virtual bool IsDetailed();
    virtual bool IsHighDetail();

    void InitManagers();
    void FreeManagers();
    bool Load();
    void Abort();
    // Finds the manager for a name, from a "CODE:" or "CODE*" prefix or from
    // the file extension, and skips the prefix.
    RESOURCEMANAGER *GetManager(const char *&name);
    RESOURCEMANAGER *FindManager(const char *typeName);
    void DumpAll(bool skipUnlisted);
    SRESULT Dump(TOKENS **args);

    RESHANDLE CreateFromTemplate(const char *name, V3 position, int layer);
    RESOURCE *GetResource(RESHANDLE handle);
    RESOURCE *GetResource(const char *name);
    RESHANDLE GetHandle(const char *name);

    MASTER *FindMaster(const char *name);
    MASTER *GetMaster(RESHANDLE handle);
    void RegisterMaster(MASTER *master);
    void RemoveMaster(const RESHANDLE &handle);
    void RenameMaster(RESHANDLE &handle, const char *name);
    void RegisterCopy(COPY *copy);
    void UnregisterCopy(const RESHANDLE &handle);

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];
    static char s_managerNames[RESOURCES_MAX_MANAGERS][30];
    // Where each manager's global pointer is, to clear it when freed.
    static RESOURCEMANAGER **s_managerSlots[RESOURCES_MAX_MANAGERS];
    static RESOURCEMANAGER *s_managers[RESOURCES_MAX_MANAGERS];
    static int s_numManagers;
    static int s_managerFlags[RESOURCES_MAX_MANAGERS];
    static CALLBACKLIST<RESOURCESCALLBACK> s_onInit;
    static CALLBACKLIST<RESOURCESCALLBACK> s_onCreate;
    static CALLBACKLIST<RESOURCESCALLBACK> s_onSetLevel;
    static CMDBASE *(*s_pfnCreate)();

    SAFEINDEX<MASTER, 0x2000, 0xfb, 4> m_masters;
    SAFEINDEX<COPY, 0x8000, 0x239, 3> m_copies;
    int m_level;
    bool m_loaded;
    bool m_abort;
    float m_soundVolume;
    float m_musicVolume;
    float m_lastMusicVolume;
    float m_lastSoundVolume;
    int m_d4;
    int m_d8;
    CSTRING m_groups[RESOURCES_MAX_GROUPS];
    int m_numChannels;
    CSTRING m_channels[RESOURCES_MAX_GROUPS];
    float m_globalScale;
};

extern RESOURCES *g_pResources;

#endif
