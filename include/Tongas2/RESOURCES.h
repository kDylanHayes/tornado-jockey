// The resource system: RESOURCES owns the per-type RESOURCEMANAGERs and two
// global pools, the masters (loaded data) and the copies (instances of a
// master). Placeholder names apart from those the error strings give.
#ifndef TONGAS2_RESOURCES_H
#define TONGAS2_RESOURCES_H

#include "Tongas2/CMDBASE.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/CSTRING.h"
#include "Tongas2/MATHS.h"
#include "Tongas2/SAFEINDEX.h"

// One type of resource (meshes, sounds, ...). Defined in another unit.
class RESOURCEMASTER;

class FILELOADER;

class RESOURCEMANAGER
{
public:
    RESOURCEMANAGER();

    // A new master of the manager's type.
    virtual RESOURCEMASTER *CreateMaster(const char *name);
    // The four-letter type code paths start with.
    virtual const char *GetTypeCode();
    virtual ~RESOURCEMANAGER();
    virtual void Update();
    // Loads the list, then the masters, as long as that takes under half a
    // second a step.
    virtual bool Load();
    // Whether name has this manager's file extension.
    virtual bool IsMyType(const char *name);
    virtual void Virtual06();
    // Opens the list of the manager's masters.
    virtual void Init();

    // Whether name starts with this manager's type code and a ':'.
    bool HasPrefix(const char *name);
    bool RemoveMaster(const RESHANDLE &handle);
    // Deletes the resources not used by level (all of them if all is set).
    void DeleteUnused(int level, bool all);
    void Flush();
    void SetLevel(int level);
    // Whether name starts with this manager's four-letter type code.
    bool HasTypeCode(const char *name);
    // Sets up the masters once the list is loaded.
    void PrepareMasters();
    void Dump(bool verbose, int index);
    void DumpPropsType();
    void FreeAll();
    void DumpTotals(bool verbose);
    // Whether the type's flags allow loading ahead (0x20 set, or 4 clear).
    bool CanPreload();
    // The master with a name, with or without the type code, or 0.
    RESOURCEMASTER *FindMaster(const char *name);
    // Adds a master to the manager.
    void Insert(RESOURCEMASTER *master);
    // A new master with the name, added to the manager.
    RESOURCEMASTER *AddMaster(const char *name);
    // Reads the list: "create <name> [command];" lines up to "end".
    bool IsListLoaded();
    // The master's handle, made if the type allows loading ahead.
    RESHANDLE GetHandle(const char *name);
    RESOURCE *GetResource(const char *name);
    // The master with a name, or 0.
    RESOURCEMASTER *GetMaster(const char *name);

    int m_id;
    bool m_8;
    // The manager's masters, linked through m_next and m_prev.
    RESOURCEMASTER *m_first;
    RESOURCEMASTER *m_last;
    int m_count;
    bool m_18;
    // Reads the list.
    FILELOADER *m_loader;
    bool m_loaded;
    bool m_initialized;
    // The file the manager's types are listed in.
    char m_fileName[0x200];
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
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args);
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

    RESOURCEMASTER *FindMaster(const char *name);
    RESOURCEMASTER *GetMaster(RESHANDLE handle);
    void RegisterMaster(RESOURCEMASTER *master);
    void RemoveMaster(const RESHANDLE &handle);
    void RenameMaster(RESHANDLE &handle, const char *name);
    void RegisterCopy(RESOURCE *copy);
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

    SAFEINDEX<RESOURCEMASTER, 0x2000, 0xfb, 4> m_masters;
    SAFEINDEX<RESOURCE, 0x8000, 0x239, 3> m_copies;
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
