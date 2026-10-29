#define STRINGMANAGER_FREE_NOTHROW

#include <stdlib.h>
#include <string.h>

#include "Tongas2/CMDMGR.h"
#include "Tongas2/ACTOR.h"
#include "Tongas2/ACTORRESOURCES.h"
#include "Tongas2/BONEDMODEL.h"
#include "Tongas2/CAMERA.h"
#include "Tongas2/CPRIMITIVE3.h"
#include "Tongas2/CURVE.h"
#include "Tongas2/DECAL.h"
#include "Tongas2/DRM.h"
#include "Tongas2/EMITTER.h"
#include "Tongas2/FILELOADER.h"
#include "Tongas2/FX.h"
#include "Tongas2/GLOBALDEFS.h"
#include "Tongas2/GUI.h"
#include "Tongas2/GUICHECKBOX.h"
#include "Tongas2/GUICHECKGROUP.h"
#include "Tongas2/GUICONTROL.h"
#include "Tongas2/GUIGENERIC.h"
#include "Tongas2/GUIGROUP.h"
#include "Tongas2/GUISLIDER.h"
#include "Tongas2/GUITEXT.h"
#include "Tongas2/GUITEXTEDIT.h"
#include "Tongas2/GUIVISUAL.h"
#include "Tongas2/KERNELCOMMANDS.h"
#include "Tongas2/LIGHT.h"
#include "Tongas2/LOG.h"
#include "Tongas2/PATH.h"
#include "Tongas2/PATHREPOSITORY.h"
#include "Tongas2/POLYTRAIL.h"
#include "Tongas2/RESOURCE.h"
#include "Tongas2/SCENE.h"
#include "Tongas2/SCREENDECAL.h"
#include "Tongas2/SHADER.h"
#include "Tongas2/SOUND.h"
#include "Tongas2/SPAWNER.h"
#include "Tongas2/TESLA.h"
#include "Tongas2/TEXT.h"
#include "Tongas2/TIMER.h"
#include "Tongas2/TOKENS.h"
#include "Tongas2/TRIGGER.h"
#include "Tongas2/VALUEMANAGER.h"

#define CMDMGR_MAX_CLASSES 0x200

static CMDMGR *CreateCmdMgr();

CMDMGR *(*g_pfnCreateCmdMgr)() = CreateCmdMgr;

int g_classParents[CMDMGR_MAX_CLASSES] = { -1 };

int g_NONEClassId = -1;
int g_CMDBASEClassId = -1;
int KERNELCOMMANDS::s_classId = -1;
int VALUEMANAGER::s_classId = -1;
int GAME::s_classId = -1;
int SCENE::s_classId = -1;
int GUI::s_classId = -1;
int GUICONTROL::s_classId = -1;
int GUITEXT::s_classId = -1;
int GUITEXTEDIT::s_classId = -1;
int GUIVISUAL::s_classId = -1;
int GUIGROUP::s_classId = -1;
int GUICHECKGROUP::s_classId = -1;
int GUICHECKBOX::s_classId = -1;
int GUIGENERIC::s_classId = -1;
int GUISLIDER::s_classId = -1;
int RESOURCES::s_classId = -1;
int RESOURCE::s_classId = -1;
int RESOURCEMASTER::s_classId = -1;
int DRM::s_classId = -1;
int SHADERMASTER::s_classId = -1;
int TEXTMASTER::s_classId = -1;
int CURVEMASTER::s_classId = -1;
int POLYTRAILMASTER::s_classId = -1;
int TESLAMASTER::s_classId = -1;
int EMITTERMASTER::s_classId = -1;
int CPRIMITIVE3MASTER::s_classId = -1;
int FXMASTER::s_classId = -1;
int SOUNDMASTER::s_classId = -1;
int BONEDMODELMASTER::s_classId = -1;
int DECALMASTER::s_classId = -1;
int SCREENDECALMASTER::s_classId = -1;
int GUITEXTMASTER::s_classId = -1;
int GUITEXTEDITMASTER::s_classId = -1;
int GUIVISUALMASTER::s_classId = -1;
int GUIGROUPMASTER::s_classId = -1;
int GUICHECKGROUPMASTER::s_classId = -1;
int GUICHECKBOXMASTER::s_classId = -1;
int GUIGENERICMASTER::s_classId = -1;
int GUISLIDERMASTER::s_classId = -1;
int LIGHTMASTER::s_classId = -1;
int GLOBALDEFSMASTER::s_classId = -1;
int PATHMASTER::s_classId = -1;
int PATHREPOSITORYMASTER::s_classId = -1;
int ACTORRESOURCESMASTER::s_classId = -1;
int ACTOR::s_classId = -1;
int CAMERA::s_classId = -1;
int SPAWNER::s_classId = -1;
int TRIGGER::s_classId = -1;

PROPENTRY *g_classProps[CMDMGR_MAX_CLASSES];
CMDMGR *g_pCmdMgr;
CMDENTRY *g_classCommands[CMDMGR_MAX_CLASSES];
char g_classNames[CMDMGR_MAX_CLASSES][30];
int g_classCount;
// What registers the scriptable classes; units add theirs as they start.
CALLBACKLIST<void (*)()> g_classRegistrars;

// Inline here, so the compiler can expand it where it destroys a result.
inline void SRESULT::FreeValue() throw()
{
    switch (m_type) {
    case SRT_STRING:
    case SRT_UNICODESTRING:
        if (m_value.str) {
            free(m_value.str);
            m_value.str = 0;
        }
        break;
    case SRT_V3:
        if (m_value.v3) {
            delete m_value.v3;
            m_value.v3 = 0;
        }
        break;
    case SRT_V2:
        if (m_value.v2) {
            delete m_value.v2;
            m_value.v2 = 0;
        }
        break;
    case SRT_ANG:
        if (m_value.ang) {
            delete m_value.ang;
            m_value.ang = 0;
        }
        break;
    case SRT_RGB:
        if (m_value.rgb) {
            delete m_value.rgb;
            m_value.rgb = 0;
        }
        break;
    case SRT_TEXTURE:
        if (m_value.tex) {
            delete m_value.tex;
            m_value.tex = 0;
        }
        break;
    }
}

void CMDMGR::RegisterClasses()
{
    g_classRegistrars.Call();
}

// Gives the class the next id, names it and registers its commands and
// properties.
#define REGISTER_CLASS(id, CLASS, name) \
    strncpy(g_classNames[g_classCount], name, 30); \
    id = g_classCount++; \
    CLASS::RegisterCommands(); \
    g_classProps[id] = CLASS::s_properties; \
    HashProperties(CLASS::s_properties);

#define REGISTER(CLASS) REGISTER_CLASS(CLASS::s_classId, CLASS, #CLASS)

void RegisterEngineClasses()
{
    g_classCount = 0;
    REGISTER_CLASS(g_CMDBASEClassId, CMDBASE, "CMDBASE")
    REGISTER(KERNELCOMMANDS)
    REGISTER(VALUEMANAGER)
    REGISTER(GAME)
    REGISTER(SCENE)
    REGISTER(GUI)
    REGISTER(GUICONTROL)
    REGISTER(GUITEXT)
    REGISTER(GUITEXTEDIT)
    REGISTER(GUIVISUAL)
    REGISTER(GUIGROUP)
    REGISTER(GUICHECKGROUP)
    REGISTER(GUICHECKBOX)
    REGISTER(GUIGENERIC)
    REGISTER(GUISLIDER)
    REGISTER(RESOURCES)
    REGISTER(RESOURCE)
    REGISTER(RESOURCEMASTER)
    REGISTER(DRM)
    REGISTER(SHADERMASTER)
    REGISTER(TEXTMASTER)
    REGISTER(CURVEMASTER)
    REGISTER(POLYTRAILMASTER)
    REGISTER(TESLAMASTER)
    REGISTER(EMITTERMASTER)
    REGISTER(CPRIMITIVE3MASTER)
    REGISTER(FXMASTER)
    REGISTER(SOUNDMASTER)
    REGISTER(BONEDMODELMASTER)
    REGISTER(DECALMASTER)
    REGISTER(SCREENDECALMASTER)
    REGISTER(GUITEXTMASTER)
    REGISTER(GUITEXTEDITMASTER)
    REGISTER(GUIVISUALMASTER)
    REGISTER(GUIGROUPMASTER)
    REGISTER(GUICHECKGROUPMASTER)
    REGISTER(GUICHECKBOXMASTER)
    REGISTER(GUIGENERICMASTER)
    REGISTER(GUISLIDERMASTER)
    REGISTER(LIGHTMASTER)
    REGISTER(GLOBALDEFSMASTER)
    REGISTER(PATHMASTER)
    REGISTER(PATHREPOSITORYMASTER)
    REGISTER(ACTORRESOURCESMASTER)
    REGISTER(ACTOR)
    REGISTER(CAMERA)
    REGISTER(SPAWNER)
    REGISTER(TRIGGER)
    if (g_classCount >= CMDMGR_MAX_CLASSES && g_pLog)
        g_pLog->Error("ERROR!! LoadRunnersInformation>FATAL ERROR, max number of supported command runners exceeded");
}

void CMDMGR::Queue(CMDCONTEXT *context)
{
    if (g_pGame)
        context->m_20 = g_pGame->Virtual25();
    if (context->m_time > 0.0f) {
        CMDCONTEXT **link = &m_timed;
        for (CMDCONTEXT *next = m_timed; next; next = next->m_next) {
            if (next->m_time > context->m_time) {
                context->m_next = next;
                *link = context;
                return;
            }
            link = &(*link)->m_next;
        }
        context->m_next = 0;
        *link = context;
    } else {
        CMDCONTEXT **link = &m_framed;
        for (CMDCONTEXT *next = m_framed; next; next = next->m_next) {
            if (next->m_frames > context->m_frames) {
                context->m_next = next;
                *link = context;
                return;
            }
            link = &(*link)->m_next;
        }
        context->m_next = 0;
        *link = context;
    }
}

void ReplaceChar(char *text, char from, char to)
{
    if (text && *text) {
        do {
            if (*text == from)
                *text = to;
        } while (*++text);
    }
}

int HashName(const char *name)
{
    int hash = 0;
    while (*name) {
        hash = (tolower(*name) + hash) * 2;
        name++;
    }
    return hash;
}

void HashCommands(CMDENTRY *commands)
{
    for (; commands->name; commands++)
        commands->hash = HashName(commands->name);
}

CMDENTRY *CMDCONTEXT::FindCommand(const char *name, int classId)
{
    int hash = HashName(name);
    for (; classId != g_NONEClassId; classId = g_classParents[classId]) {
        for (CMDENTRY *command = g_classCommands[classId]; command && command->name; command++) {
            if (command->hash == hash && !_strnicmp(command->name, name, 30)) {
                int args = m_call.tokens->m_count - 1;
                if (args < command->minArgs) {
                    if (g_pLog)
                        g_pLog->Error("Too few parameters: '%s'", name);
                    return 0;
                }
                if (args > command->maxArgs) {
                    if (g_pLog)
                        g_pLog->Error("Too many parameters: '%s'", name);
                    return 0;
                }
                return command;
            }
        }
    }
    return 0;
}

SRESULT CMDCONTEXT::Run(CMDBASE *root)
{
    if (!root) {
        if (g_pLog)
            g_pLog->Error("no context to run command!!");
        return SRESULT();
    }
    char *name = (char *)m_call.tokens->GetToken(0);
    char *dot = strrchr(name, '.');
    if (dot) {
        *dot = 0;
        root = root->FindObject(name, m_call.caller);
        name = dot + 1;
        if (!root) {
            if (g_pLog)
                g_pLog->Error("could not find object to run command %s", name);
            return SRESULT();
        }
    }
    int classId = root->GetClassId();
    if (classId == g_NONEClassId) {
        if (g_pLog)
            g_pLog->Error("scripting object without base type??");
        return SRESULT();
    }
    CMDENTRY *command = FindCommand(name, classId);
    if (command)
        return root->Execute(command, &m_call.tokens);
    PROPENTRY *property = root->FindProperty(name, classId);
    if (property)
        return root->GetProperty(property, atoi(m_call.tokens->GetToken(1)));
    return SRESULT();
}

CMDMGR::CMDMGR()
{
    m_timed = 0;
    m_framed = 0;
    g_classRegistrars.AddHead(RegisterEngineClasses);
    m_loaded = true;
    m_loader = g_pfnCreateFileLoader();
}

static CMDMGR *CreateCmdMgr()
{
    return new CMDMGR;
}

void CMDMGR::LoadMap(const char *name)
{
    m_loaded = false;
    STRING path;
    path.Format("%s%s%s", g_basePath, "data/maps/", name);
    if (g_pLog)
        g_pLog->Trace(path.c_str());
    m_loader->Open((char *)path.c_str());
}

void STRING::Append(const char *text)
{
    int length = strlen(text);
    Reserve(Length() + length + 1);
    strcpy(m_buf + Length(), text);
    m_lengthValid = false;
}

bool CMDMGR::Run(const STRING &command, const RESHANDLE &caller)
{
    TOKENS lines;
    lines.Split(command, ";");
    bool ok = true;
    for (int i = 0; i < lines.m_count; i++) {
        TOKENS tokens;
        bool incomplete;
        if (!tokens.Parse(STRING(lines.GetToken(i)), &incomplete)) {
            if (incomplete && lines.Merge(i, i + 1, ';')) {
                i--;
                continue;
            }
            if (g_pLog)
                g_pLog->Error("Parse error in \"%s\" , command skipped", lines.GetToken(i));
            ok = false;
        } else {
            bool failed;
            {
                STRING line(lines.GetToken(i));
                failed = !Evaluate(line, caller).IsSet();
            }
            if (failed) {
                if (g_pLog)
                    g_pLog->Error("CMDMGR::Run>Error in \"%s\" ", lines.GetToken(i));
                ok = false;
            }
        }
    }
    return ok;
}

SRESULT CMDMGR::Evaluate(const STRING &expression, const RESHANDLE &caller)
{
    TOKENS tokens;
    char c = *expression.c_str();
    if (c == '\0' || c == '#' || c == '/')
        return SRESULT(true);
    if ((c >= '0' && c <= '9') || c == '+' || c == '-')
        return SRESULT((float)atof(expression.c_str()));
    if (c == '@')
        return SRESULT(expression.c_str() + 1);
    if (c == '{' && expression.c_str()[const_cast<STRING &>(expression).Length() - 1] == '}') {
        STRING inner(expression.c_str() + 1);
        ((char *)inner.c_str())[inner.Length() - 1] = 0;
        if (!Run(inner, caller))
            return SRESULT();
        return SRESULT(true);
    }
    if (!tokens.Parse(const_cast<STRING &>(expression), 0))
        return SRESULT();
    if (tokens.m_count <= 0)
        return SRESULT("");
    CMDCONTEXT context(&tokens, caller);
    return context.Run(g_pScriptRoot);
}

// Frees a queued command and its tokens.
static inline void DeleteContext(CMDCONTEXT *context)
{
    if (context->m_call.tokens) {
        delete context->m_call.tokens;
        context->m_call.tokens = 0;
    }
    delete context;
}

void CMDMGR::Clear()
{
    for (CMDCONTEXT *next = m_timed; next;) {
        CMDCONTEXT *context = next;
        m_timed = next->m_next;
        next = next->m_next;
        DeleteContext(context);
    }
    for (CMDCONTEXT *next = m_framed; next;) {
        CMDCONTEXT *context = next;
        m_framed = next->m_next;
        next = next->m_next;
        DeleteContext(context);
    }
}

void CMDMGR::ClearFlagged()
{
    CMDCONTEXT **link = &m_timed;
    for (CMDCONTEXT *next = m_timed; next;) {
        if (next->m_20) {
            CMDCONTEXT *context = next;
            *link = next->m_next;
            next = next->m_next;
            DeleteContext(context);
        } else {
            link = &(*link)->m_next;
            next = next->m_next;
        }
    }
    link = &m_framed;
    for (CMDCONTEXT *next = m_framed; next;) {
        if (next->m_20) {
            CMDCONTEXT *context = next;
            *link = next->m_next;
            next = next->m_next;
            DeleteContext(context);
        } else {
            link = &(*link)->m_next;
            next = next->m_next;
        }
    }
}

bool CMDMGR::LoadScript()
{
    if (m_loaded)
        return true;
    if (!m_loader->IsOpen())
        return false;
    if (m_loader->IsError()) {
        if (g_pLog)
            g_pLog->Error("CMDMGR:LoadScript> Error loading commands script. (status=%d)", m_loader->IsError());
        m_loader->Close();
        return false;
    }
    int line = 0;
    STRING script;
    script.Free();
    while (!m_loader->IsEof()) {
        line++;
        char *text = m_loader->ReadLine();
        if (text) {
            if (strlen(text)) {
                if (!memcmp(text, "end", 3)) {
                    m_loaded = true;
                    free(text);
                    break;
                }
                script.Append(text);
                if (strchr(text, ';')) {
                    if (!Run(script, RESHANDLE()) && g_pLog)
                        g_pLog->Error("CMDMGR:LoadScript> Error in script line %d", line);
                    script.Free();
                }
            }
            free(text);
        }
    }
    m_loader->Close();
    if (!m_loaded)
        m_loaded = true;
    g_pScene->OnScriptLoaded();
    g_StringManager.Check();
    return true;
}

void CMDMGR::Update()
{
    while (m_timed && m_timed->m_time <= g_pTimer->m_time) {
        m_timed->Run(g_pScriptRoot);
        CMDCONTEXT *context = m_timed;
        if (context) {
            m_timed = context->m_next;
            DeleteContext(context);
        }
    }
    while (m_framed && --m_framed->m_frames < 0) {
        m_framed->Run(g_pScriptRoot);
        CMDCONTEXT *context = m_framed;
        if (context) {
            m_framed = context->m_next;
            DeleteContext(context);
        }
    }
}

CMDMGR::~CMDMGR()
{
    Clear();
    if (m_loader) {
        delete m_loader;
        m_loader = 0;
    }
}
