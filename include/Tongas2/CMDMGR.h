// The script command manager: registers the scriptable classes, runs
// script commands, evaluates expressions, loads the map's command script and
// keeps the queue of commands waiting for a time or a number of frames.
// Placeholder names apart from the class name and the names its log
// messages give.
#ifndef TONGAS2_CMDMGR_H
#define TONGAS2_CMDMGR_H

#include "Tongas2/SAFEINDEX.h"
#include "Tongas2/SRESULT.h"
#include "Tongas2/STRING.h"

class CMDBASE;
class CMDCONTEXT;
class FILELOADER;

class CMDMGR
{
public:
    CMDMGR();
    virtual ~CMDMGR();

    // Calls the class registrars.
    void RegisterClasses();
    // Adds a command to the queue: by its time if it has one, otherwise by
    // its frame count.
    void Queue(CMDCONTEXT *context);
    // Opens the command script of the map with the name.
    void LoadMap(const char *name);
    // Runs a ';'-separated list of commands on behalf of the resource with
    // the handle; false if any of them failed.
    bool Run(const STRING &command, const RESHANDLE &caller);
    // The value of a script expression, for the resource with the handle.
    SRESULT Evaluate(const STRING &expression, const RESHANDLE &caller);
    // Drops the queued commands.
    void Clear();
    // Drops the queued commands m_20 is set for.
    void ClearFlagged();
    // Runs the map's command script, once.
    bool LoadScript();
    // Runs the queued commands that are due.
    void Update();

    FILELOADER *m_loader;
    // The map's command script has been run.
    bool m_loaded;
    // The commands waiting for a time, earliest first.
    CMDCONTEXT *m_timed;
    // The commands waiting for frames, fewest first.
    CMDCONTEXT *m_framed;
};

extern CMDMGR *g_pCmdMgr;

// Makes the command manager.
extern CMDMGR *(*g_pfnCreateCmdMgr)();

// The object scripts run commands on.
extern CMDBASE *g_pScriptRoot;

// Replaces each from in text with to.
void ReplaceChar(char *text, char from, char to);

#endif
