// Triggers: actors that run script commands when something touches them.
// A trigger has up to 16 commands and runs the next one each time it fires;
// it fires when a collision shape of another actor touches it (only the
// first time per contact, with OncePerContact), or all the time with
// AlwaysTouched, after a random wait between WaitTime's two values, and
// only every CommandCounter touches. Scripts can also touch and reset it.
// Placeholder names apart from the class names, properties and commands.
#ifndef TONGAS2_TRIGGER_H
#define TONGAS2_TRIGGER_H

#include "Tongas2/ACTOR.h"
#include "Tongas2/STRING.h"

#define TRIGGER_MAX_COMMANDS 16

// A wait time range, none until set.
struct TRIGGERWAIT : V2
{
    TRIGGERWAIT() : V2(0.0f, 0.0f) {}
    TRIGGERWAIT &operator=(const V2 &other)
    {
        V2::operator=(other);
        return *this;
    }
};

class TRIGGER : public ACTOR
{
public:
    TRIGGER();
    virtual ~TRIGGER();

    static void RegisterCommands();

    virtual int GetClassId() { return s_classId; }
    virtual int GetParentClassId() { return g_classParents[s_classId]; }
    virtual const char *GetClassName() { return g_classNames[s_classId]; }
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (TRIGGER::*)(TOKENS **))command->func)(args);
    }
    virtual int GetFactoryId() { return s_factoryId; }
    virtual int GetParentFactoryId() { return ACTOR::s_factoryId; }
    virtual int GetFactoryFlags() { return g_factoryFlags[s_factoryId]; }
    virtual void SetPosition(const V3 &position);
    virtual void SetScale(const V3 &scale);
    virtual void Update();
    virtual void OnContact(CONTACT *contact);

    // Runs the next command.
    virtual void RunCommand();
    virtual void SetCommandCounter(const int &counter) { m_commandCounter = counter; }
    virtual void SetEnabled(const bool &enabled) { m_enabled = enabled; }
    virtual void SetMaxCommands(const int &max) { m_maxCommands = max; }
    virtual void SetWaitTime(const V2 &time) { m_waitTime = time; }
    virtual void SetOncePerContact(const bool &once) { m_oncePerContact = once; }
    virtual void SetAlwaysTouched(const bool &always) { m_alwaysTouched = always; }
    virtual void SetSendToToucher(const bool &send) { m_sendToToucher = send; }
    virtual void SetCommand(const int &i, const STRING &command);
    virtual SRESULT Touch(CMDCALL *call)
    {
        Touch(call->caller);
        return SRESULT(true);
    }
    virtual SRESULT Reset(CMDCALL *call)
    {
        ResetCounters();
        return SRESULT(true);
    }

    // The toucher touched the trigger.
    void Touch(const RESHANDLE &toucher);
    void ResetCounters()
    {
        m_touches = 0;
        m_commandsRun = 0;
        m_wait = 0.0f;
        m_contact = 0;
    }

    static int s_classId;
    static int s_factoryId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    int m_touches;
    int m_commandsRun;
    // Time left before the trigger can fire again.
    float m_wait;
    // Counts down after each contact; the toucher is still touching while
    // it is not negative.
    int m_contact;
    RESHANDLE m_toucher;
    // The last command set.
    int m_lastCommand;
    int m_commandCounter;
    bool m_enabled;
    int m_maxCommands;
    TRIGGERWAIT m_waitTime;
    bool m_oncePerContact;
    bool m_alwaysTouched;
    bool m_sendToToucher;
    STRING m_commands[TRIGGER_MAX_COMMANDS];
};

#endif
