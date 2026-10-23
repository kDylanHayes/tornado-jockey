// Particle emitters: an EMITTER copy places copies of another resource
// (the particles) around itself, gives each a speed, a lifetime and the
// size, alpha and colour it is born, lives and dies with, and moves them
// each frame, optionally along a path or pulled by an actor.
// EMITTERMASTER holds what the emitter type's file sets. Placeholder names
// apart from the class names and properties.
#ifndef TONGAS2_EMITTER_H
#define TONGAS2_EMITTER_H

#include "Tongas2/RESOURCE.h"
#include "Tongas2/RESOURCES.h"
#include "Tongas2/TIMER.h"
#include "lib/freeimage/FreeImage.h"

class EMITTER;
class PATHMASTER;

// Defined elsewhere: the folder the game was started from.
extern char g_basePath[];

// A value picked at random between min and max.
struct EMITRANGE
{
    EMITRANGE() { min = 0.0f; max = 0.0f; }
    EMITRANGE(float min_, float max_) : min(min_), max(max_) {}

    __forceinline float Random() const
    {
        if (max > min)
            return (max - min) * (float)rand() * (1.0f / RAND_MAX) + min;
        return min;
    }

    float min, max;
};

// A vector, zero until set.
struct EMITV3 : V3
{
    EMITV3() { x = 0.0f; y = 0.0f; z = 0.0f; }

    EMITV3 &operator=(const V3 &v)
    {
        V3::operator=(v);
        return *this;
    }
};

// A lit pixel of the emitter's picture: where it is (0..1) and its colour.
struct EMITPIXEL
{
    EMITPIXEL() : u(0.0f), v(0.0f) {}

    float u, v;
    V3I color;
};

// Defined elsewhere: a model with bones.
class BONEDMODEL : public OBJECT
{
public:
    virtual void Virtual70();
    virtual void Virtual71();
    virtual void Virtual72();
    virtual void Virtual73();
    virtual void Virtual74();
    virtual void Virtual75();
    virtual void Virtual76();
    virtual void Virtual77();
    virtual void Virtual78();
};

// Defined elsewhere: the managers of models with bones, decals and paths.
extern RESOURCEMANAGER *g_pBONEDMODEL;
extern RESOURCEMANAGER *g_pDECAL;
extern RESOURCEMANAGER *g_pPATH;

class EMITTERMASTER : public RESOURCEMASTER
{
public:
    enum
    {
        DIRECTION_FW,
        DIRECTION_UP
    };
    enum
    {
        BEHAVIOR_FORCE,
        BEHAVIOR_PATHFLOW,
        BEHAVIOR_DRAGFORCE,
        BEHAVIOR_MAGNETICFIELD
    };

    EMITTERMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~EMITTERMASTER();

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args);
    virtual RESOURCE *CreateCopy(const RESHANDLE &handle);

    virtual void SetRate(const EMITRANGE &value);
    virtual void SetForce(const V3 &value);
    virtual void SetEmitterSize(const V3 &value);
    virtual void SetEmiterOffset(const float &value);
    virtual void SetTimeToLive(const EMITRANGE &value);
    virtual void SetAngRange(const EMITRANGE &value);
    virtual void SetAngSpd(const ANG &value);
    virtual void SetLife(const EMITRANGE &value);
    virtual void SetSpeedRange(const EMITRANGE &value);
    virtual void SetAccelRange(const EMITRANGE &value);
    virtual void SetAccelTime(const float &value);
    virtual void SetRotateToMove(const bool &value);
    virtual void SetResource(const CSTRING &value);
    virtual void SetResourcesSpecific(const int &value);
    virtual void SetEmitAttachedResources(const bool &value);
    virtual void SetKillResourceOnDetach(const bool &value);
    virtual void SetMultiplyColor(const V3I &value);
    virtual void SetOneBigEmission(const int &value);
    virtual void SetChangeColorInTime(const bool &value);
    virtual void SetMaxGlobalParticles(const int &value);
    virtual void SetEmitDirection(const int &value);
    virtual void SetInitialEmitTime(const float &value);
    virtual void SetEmittedBehavior(const int &value);
    virtual void SetBirthSize(const EMITRANGE &value);
    virtual void SetBirthAlpha(const EMITRANGE &value);
    virtual void SetBirthYaw(const EMITRANGE &value);
    virtual void SetBirthPitch(const EMITRANGE &value);
    virtual void SetBirthRoll(const EMITRANGE &value);
    virtual void SetBirthColor1(const V3I &value);
    virtual void SetBirthColor2(const V3I &value);
    virtual void SetUseMidlife(const bool &value);
    virtual void SetMidLife(const EMITRANGE &value);
    virtual void SetMidSize(const EMITRANGE &value);
    virtual void SetMidAlpha(const EMITRANGE &value);
    virtual void SetMidDuration(const EMITRANGE &value);
    virtual void SetMidColor1(const V3I &value);
    virtual void SetMidColor2(const V3I &value);
    virtual void SetDeathSize(const EMITRANGE &value);
    virtual void SetDeathAlpha(const EMITRANGE &value);
    virtual void SetDeathColor1(const V3I &value);
    virtual void SetDeathColor2(const V3I &value);
    virtual void SetPathName(const CSTRING &value);
    virtual void SetPathRelativeRadius(const EMITRANGE &value);
    virtual void SetPathAdjustSize(const bool &value);
    virtual void SetPathOpacityRange(const V3 &value);
    virtual void SetPathOpacity(const bool &value);
    virtual void SetPathOffset(const EMITRANGE &value);
    virtual void SetPathScaleFactor(const float &value);
    virtual void SetPathRetimeFactor(const float &value);
    virtual void SetExtForce(const float &value);
    virtual void SetExtDistanceMax(const float &value);
    virtual void SetExtDistanceMin(const float &value);
    virtual void SetAccelerationAir(const float &value);
    virtual void SetExtForceActorName(const FIXEDSTRING<0x200> &value);
    // Loads the picture particles are emitted from.
    virtual void SetPictureFileName(const CSTRING &value);
    virtual void SetUsePictureColor(const bool &value);

    // The picture in data/art/, as 24 or 32 bits a pixel.
    FIBITMAP *LoadImage(const char *name);
    // Reads the lit pixels of the picture.
    void LoadPicture();

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    // How many particles the emitters of the type have alive.
    int m_numParticles;
    // The path particles flow along, once looked up.
    PATHMASTER *m_path;
    int m_numPixels;
    EMITPIXEL *m_pixels;
    bool m_pictureLoaded;
    int m_pictureWidth;
    int m_pictureHeight;
    EMITRANGE m_rate;
    EMITV3 m_force;
    EMITV3 m_emitterSize;
    float m_emiterOffset;
    EMITRANGE m_timeToLive;
    EMITRANGE m_angRange;
    ANG m_angSpd;
    EMITRANGE m_life;
    EMITRANGE m_speedRange;
    EMITRANGE m_accelRange;
    float m_accelTime;
    bool m_rotateToMove;
    CSTRING m_resource;
    int m_resourcesSpecific;
    bool m_emitAttachedResources;
    bool m_killResourceOnDetach;
    V3I m_multiplyColor;
    int m_oneBigEmission;
    bool m_changeColorInTime;
    int m_maxGlobalParticles;
    int m_emitDirection;
    float m_initialEmitTime;
    int m_emittedBehavior;
    EMITRANGE m_birthSize;
    EMITRANGE m_birthAlpha;
    EMITRANGE m_birthYaw;
    EMITRANGE m_birthPitch;
    EMITRANGE m_birthRoll;
    V3I m_birthColor1;
    V3I m_birthColor2;
    bool m_useMidlife;
    EMITRANGE m_midLife;
    EMITRANGE m_midSize;
    EMITRANGE m_midAlpha;
    EMITRANGE m_midDuration;
    V3I m_midColor1;
    V3I m_midColor2;
    EMITRANGE m_deathSize;
    EMITRANGE m_deathAlpha;
    V3I m_deathColor1;
    V3I m_deathColor2;
    CSTRING m_pathName;
    EMITRANGE m_pathRelativeRadius;
    bool m_pathAdjustSize;
    EMITV3 m_pathOpacityRange;
    bool m_pathOpacity;
    EMITRANGE m_pathOffset;
    float m_pathScaleFactor;
    float m_pathRetimeFactor;
    float m_extForce;
    float m_extDistanceMax;
    float m_extDistanceMin;
    float m_accelerationAir;
    FIXEDSTRING<0x200> m_extForceActorName;
    CSTRING m_pictureFileName;
    bool m_usePictureColor;
    RESHANDLE m_resourceHandle;
};

// One particle: a copy of the master's resource, moved by its emitter.
class PARTICLE
{
public:
    PARTICLE(EMITTER *emitter);
    virtual ~PARTICLE();

    // A pixel of the master's picture the emitter has not emitted from on
    // this pass, picked at random.
    int PickPixel(EMITTERMASTER *master, EMITTER *emitter);
    // Moves the particle on by dt seconds.
    void Update(const float &dt);

    float m_age;
    float m_life;
    EMITV3 m_velocity;
    float m_accel;
    // How far the particle is from where it heads, for pictures.
    float m_distance;
    bool m_20;
    float m_midLife;
    float m_midDuration;
    float m_alpha;
    float m_size;
    V3I m_color;
    float m_midAlpha;
    float m_midSize;
    V3I m_midColor;
    float m_deathAlpha;
    float m_deathSize;
    V3I m_deathColor;
    // Where on the path the particle flows.
    float m_pathRadius;
    float m_pathAngle;
    float m_pathOffset;
    RESHANDLE m_resource;
    // Whether the resource is attached to the emitter.
    bool m_attached;
    EMITV3 m_offset;
    EMITTER *m_emitter;
    PARTICLE *m_next;
    PARTICLE *m_prev;
};

class EMITTER : public OBJECT
{
public:
    EMITTER(EMITTERMASTER *master, const RESHANDLE &handle);
    virtual ~EMITTER();

    virtual void Init();
    virtual void Virtual20();
    virtual void Start();
    virtual void Stop();
    // Emits the particles due and moves the others.
    virtual void Update();
    virtual bool IsLoaded() { return true; }
    virtual void Virtual30();
    virtual void Virtual31();
    virtual V3 GetScaleFactor() { return m_scaleFactor; }
    virtual V3 GetSize() { return m_scaleFactor; }
    virtual void SetFrame(int frame) {}

    void StartEmitting();
    void StopEmitting();
    // Works out when the next particle is due.
    void NextEmitTime();
    void Emit();
    void KillParticles();

    EMITTERMASTER *Master() { return (EMITTERMASTER *)m_master; }

    void Reset()
    {
        m_color = V3I();
        m_emitTime = 0.0f;
        m_nextEmit = 0.0f;
        m_emitting = false;
        m_rateScale = 1.0f;
        m_429 = false;
        m_numParticles = 0;
        m_particles = 0;
        m_numEmitted = 0;
    }
    void ClearPixelsUsed()
    {
        if (m_numPixels > 0)
            for (int i = 0; i < m_numPixels; i++)
                m_pixelUsed[i] = false;
    }

    int m_400;
    // How much longer the emitter emits.
    float m_emitTime;
    // When the next particle is due.
    float m_nextEmit;
    bool m_emitting;
    float m_rateScale;
    V3I m_color;
    int m_numParticles;
    PARTICLE *m_particles;
    bool m_428;
    bool m_429;
    float m_42c;
    int m_numEmitted;
    int m_numPixels;
    // The pixels emitted from since emitting started.
    bool *m_pixelUsed;
};

#endif
