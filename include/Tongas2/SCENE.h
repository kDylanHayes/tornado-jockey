// The scene at 0x6d15e0: every actor in the world, by handle and by name,
// the actors on the screen, the potentially visible set they are sorted
// into, and the actor factories that make actors by type. Placeholder
// names apart from the class name and the names its commands, properties
// and messages give.
#ifndef TONGAS2_SCENE_H
#define TONGAS2_SCENE_H

#include "Tongas2/ACTOR.h"
#include "Tongas2/CMDBASE.h"
#include "Tongas2/FIXEDSTRING.h"
#include "Tongas2/SAFEINDEX.h"
#include "Tongas2/SRESULT.h"
#include "Tongas2/STRING.h"

class ACTOR;
class WTACTOR;
class OCTREE;
class ONENODE;

extern char g_classNames[][30];

// The actor factories, by factory id: the function that makes the actor,
// its class id, its type name and its flags. g_classFactoryIds gives a
// class's factory id.
#define MAX_ACTOR_FACTORIES 0x80
extern int g_factoryClassIds[MAX_ACTOR_FACTORIES];
extern int g_classFactoryIds[0x200];
extern ACTOR *(*g_factoryCreators[MAX_ACTOR_FACTORIES])();
extern char g_factoryNames[MAX_ACTOR_FACTORIES][30];
extern int g_factoryFlags[MAX_ACTOR_FACTORIES];
extern int g_numFactories;

// The factory id of ACTOR's parent: none.
extern int g_rootFactoryId;

// The kinds of potentially visible set.
enum PVSTYPE
{
    PVS_NONE,
    PVS_OCTREE,
    PVS_ONENODE,
};

class SCENE : public CMDBASE
{
public:
    SCENE();
    virtual ~SCENE();

    static void RegisterCommands();
    // Registers the engine's actor factories. The scene's constructor adds
    // it to the start-up functions.
    static void LoadActorsInformation();

    virtual int GetClassId() { return s_classId; }
    virtual int GetParentClassId() { return g_classParents[s_classId]; }
    virtual const char *GetClassName() { return g_classNames[s_classId]; }
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    { return (this->*(SRESULT (SCENE::*)(TOKENS **))command->func)(args); }
    virtual int Virtual08();
    virtual void *Virtual09(const char *name);
    // The FogColor property's setter.
    virtual void SetFogColor(const RGB &color);
    // The testCmd command: whether its argument is above 0.
    virtual SRESULT TestCmd(CMDCALL *call);
    // The cols command: shows or hides the collision shapes' helper models.
    virtual SRESULT ToggleCollisions(CMDCALL *call);
    virtual SRESULT Virtual22(CMDCALL *call);
    // Called before and after the actors are stepped.
    virtual void BeginUpdate() {}
    virtual void EndUpdate() {}
    // Whether an actor that is out of view should still be stepped.
    virtual bool CanUpdate(ACTOR *actor);
    // The CreatePVS command: the type of set and its arguments.
    virtual SRESULT CreatePVS(CMDCALL *call);
    // The DumpPVS command.
    virtual SRESULT DumpPVS(CMDCALL *call);

    // Runs the actor start-up functions; false when it has already run.
    bool Init();
    // Steps the actors for a frame.
    void Update();
    // Writes the potentially visible set out to the log.
    void PVSDump();
    // Loads a map's command script.
    void LoadMap(const char *name);
    // Puts an actor on the screen list; its slot, or -1.
    int AddScreenActor(ACTOR *actor);
    void RemoveScreenActor(ACTOR *actor);
    // Shows or hides the collision shapes' helper models.
    void ToggleHelpers();
    void DestroyPVS();
    // Makes the potentially visible set of the type with the name.
    void CreatePVS(const char *type, const char *args);
    void PVSAddMember(ACTOR *actor);
    // Tells each actor the map's command script has run.
    void OnScriptLoaded();
    // Calls Dump on the active actors (-1) or the screen actors.
    void DumpActors(int which);
    // The active actor of the class nearest a point on the screen.
    ACTOR *PickActor(float x, float y, bool activeOnly, int classId);
    // Removes all the actors and empties the lists and the set.
    void Reset();
    // Removes all the actors.
    void RemoveAll();
    WTACTOR *GetActor(RESHANDLE handle);
    // The actor with a name, or 0.
    WTACTOR *FindActor(const char *name);
    // Removes the actor with a handle; while the actors are being stepped,
    // once they all have been.
    void Remove(RESHANDLE handle);
    // Sends the actors a spawner made a msg command.
    void MessageSpawned(RESHANDLE spawner, STRING message);
    void RenameActor(RESHANDLE handle, const char *name);
    // Makes an actor of the factory with the id, or of the type with the
    // name, and adds it.
    ACTOR *CreateActor(int factoryId);
    ACTOR *CreateActor(const char *type);

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    RGB m_fogColor;
    bool m_initialized;
    FIXEDSTRING<0x200> m_29;
    bool m_229;
    bool m_22a;
    // The actors are being stepped: removals wait in m_pendingRemoves.
    bool m_updating;
    RESHANDLE m_pendingRemoves[5000];
    int m_numPendingRemoves;
    SAFEINDEX<ACTOR, 5000, 0xfb, 1> m_actors;
    ACTOR *m_screenActors[1500];
    int m_freeScreenSlots[1500];
    int m_numFreeScreenSlots;
    int m_numScreenSlots;
    char m_11bc8[0x11c4c - 0x11bc8];
    ONENODE *m_oneNode;
    OCTREE *m_octree;
    int m_pvsType;
};
extern SCENE *g_pScene;

// Makes the scene; the game points it at its own.
extern SCENE *(*g_pfnCreateScene)();

#endif
