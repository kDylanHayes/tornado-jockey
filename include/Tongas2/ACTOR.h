// ACTOR: the base of the game's actors. An actor is a script object (a
// CMDBASE) with a second interface at 0x1c (ACTORLINK), placed in the world
// and carrying up to 16 resources (models, sounds, effects) it makes copies
// of when activated. Its Behavior moves it: by its velocity, attached to
// another actor or one of its bones, following the camera, or along a path.
// Placeholder names apart from the class names, properties and commands.
#ifndef TONGAS2_ACTOR_H
#define TONGAS2_ACTOR_H

#include "Tongas2/ACTORLINK.h"
#include "Tongas2/CMDBASE.h"
#include "Tongas2/CSTRING.h"
#include "Tongas2/MATHS.h"
#include "Tongas2/SAFEINDEX.h"
#include "Tongas2/SRESULT.h"

#include <list>
#include <vector>

struct CONTACT;
class RESOURCE;
class TAG;
class PATHMASTER;
class ACTORRESOURCESMASTER;
class PVSNODE;
class BEHAVIOR;

extern char g_classNames[][30];

// Defined elsewhere: per factory id, the factory's flags.
extern int g_factoryFlags[];
// Defined elsewhere: the id of the factory every actor factory descends from.
extern int g_rootFactoryId;

#define ACTOR_MAX_RESOURCES 16

// A vector that starts at zero.
struct ZEROV3 : V3
{
    ZEROV3() : V3(0.0f, 0.0f, 0.0f) {}
    ZEROV3 &operator=(const V3 &v) { V3::operator=(v); return *this; }
};

// Angles that start at zero.
struct ZEROANG : ANG
{
    ZEROANG() : ANG(0.0f, 0.0f, 0.0f) {}
    ZEROANG &operator=(const ANG &a) { ANG::operator=(a); return *this; }
};

// The corners of a box centered on the origin, a unit cube until set.
struct BOXCORNERS
{
    // Seen as not throwing: making one keeps no unwind state for it.
    BOXCORNERS() throw();

    // Sets the box's full extents.
    void Set(const V3 &size);
    void Set(const float &x, const float &y, const float &z);
    // Sets the box from min to max.
    void Set(const V3 &min, const V3 &max);
    // Moves the box by offset.
    void Move(const V3 &offset);
#ifdef FRUSTUM_MATHS
    // Projects each corner by m.
    void Transform(const MATRIX &m);
#endif
#ifdef HEIGHTMAP_BOXES
    // Defined in HEIGHTMAPDX8.cpp: each corner transformed by m.
    void TransformAffine(const D3DXMATRIX &m);
    // Raises the top corners to height and lowers the bottom ones to it.
    void ExtendHeight(float height);
    // The box around this one and other.
    void Merge(const BOXCORNERS &other);
#endif

    ZEROV3 corners[8];
};

// The behaviors an actor runs each frame, besides its Behavior. Defined
// elsewhere apart from the destructor.
class BEHAVIORLIST
{
public:
    BEHAVIORLIST() {}
    virtual ~BEHAVIORLIST() { Clear(); }
    // Steps the behaviors.
    virtual void Update();

    // Deletes the behaviors.
    void Clear();
    void Add(BEHAVIOR *behavior) { m_list.push_back(behavior); }

    std::vector<BEHAVIOR *> m_list;
};

// The values of Behavior.
enum ACTORBEHAVIOR
{
    ACTORBEHAVIOR_LINEAL,
    ACTORBEHAVIOR_STATIC,
    ACTORBEHAVIOR_ATTACHED,
    ACTORBEHAVIOR_BONEATTACHED,
    ACTORBEHAVIOR_CAMERAATTACHED,
    ACTORBEHAVIOR_PATH,
};

// The values of Activation and Deactivation.
enum ACTORACTIVATION
{
    ACTORACTIVATION_ONSTART,
    ACTORACTIVATION_ALWAYSACTIVE,
    ACTORACTIVATION_USEOCCLUDINGBOX,
    ACTORACTIVATION_MANUALLY,
};

enum ACTORDEACTIVATION
{
    ACTORDEACTIVATION_USEOCCLUDINGBOX,
    ACTORDEACTIVATION_MANUALLY,
};

// The values of PathBehavior.
enum ACTORPATHBEHAVIOR
{
    ACTORPATH_FOLLOW,
    ACTORPATH_RELAXEDFOLLOW,
    ACTORPATH_FLOW,
    ACTORPATH_PATHFINDER,
};

class ACTOR : public CMDBASE, public ACTORLINK
{
public:
    ACTOR();
    virtual ~ACTOR();

    static void RegisterCommands();

    virtual int GetClassId() { return s_classId; }
    virtual int GetParentClassId() { return g_classParents[s_classId]; }
    virtual const char *GetClassName() { return g_classNames[s_classId]; }
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args)
    {
        return (this->*(SRESULT (ACTOR::*)(TOKENS **))command->func)(args);
    }
    // Renames the actor in the scene.
    virtual void SetName(const CSTRING &name);
    virtual int Virtual08();
    // "resN": the copy of resource N.
    virtual void *Virtual09(const char *name);
    // The id the actor factory knows the class by, its parent's and its
    // factory flags.
    virtual int GetFactoryId();
    virtual int GetParentFactoryId() { return g_rootFactoryId; }
    virtual int GetFactoryFlags() { return g_factoryFlags[s_factoryId]; }

    // The property setters.
    virtual void SetPosition(const V3 &position);
    // Turns to look along direction from position.
    virtual void SetDirection(const V3 &direction, const V3 &position);
    virtual void SetAngles(const ANG &angles);
    virtual void SetScale(const V3 &scale);
    virtual void SetIsTemplate(const bool &value);
    virtual void SetFrozen(const bool &frozen);
    virtual void SetComments(const CSTRING &comments);
    virtual void SetColliderColorOverride(const bool &value);
    virtual void SetColliderColor(const RGB &color);
    virtual void SetShowCollider(const bool &show);
    // Attaches the actor to the one with the name, once placed.
    virtual void SetAttachTo(const CSTRING &name);
    // Attaches the actor to a bone of the actor it is attached to.
    virtual void SetAttachBone(const CSTRING &bone);
    virtual void SetAttachBoneResId(const int &id);
    virtual void SetAttachPos(const V3 &position);
    virtual void SetAttachAng(const ANG &angles);
    virtual void SetAttachScale(const V3 &scale);
    virtual void SetAttachOpacity(const float &opacity);
    virtual void SetAttachZSort(const int &zsort);
    virtual void SetActivation(const int &activation);
    virtual void SetDeactivation(const int &deactivation);
    virtual void SetOnActivate(const CSTRING &command);
    virtual void SetTestOnActivate(const bool &test);
    virtual void SetOnActivateRunInEditor(const bool &run);
    virtual void SetBehavior(const int &behavior);
    virtual void SetVel(const V3 &vel);
    virtual void SetAngVel(const V3 &vel);
    virtual void SetAngAceleration(const V3 &aceleration);
    virtual void SetAceleration(const V3 &aceleration);
    virtual void SetPanSounds(const bool &pan);
    virtual void SetMoveWithScreenZ(const bool &move);
    virtual void SetMoveWithScreenY(const bool &move);
    virtual void SetPreviewBehavior(const bool &preview);
    virtual void SetRotateToMove(const bool &rotate);
    virtual void SetForcedCamDist(const float &distance);
    virtual void SetForceInScreen(const bool &force);
    virtual void SetPathName(const CSTRING &name);
    virtual void SetPathBehavior(const int &behavior);
    virtual void SetTestPath(const bool &test);
    virtual void SetVectorUp(const V3 &up);
    virtual void SetVectorForward(const V3 &forward);
    virtual void SetLoop(const bool &loop);
    virtual void SetFlowAngle(const float &angle);
    virtual void SetFlowRadius(const float &radius);
    virtual void SetPathRelativePos(const bool &relative);
    virtual void SetAlsoLineal(const bool &lineal);
    virtual void SetDisplayPath(const bool &display);
    virtual void SetResources(const int &i, const CSTRING &name);
    virtual void SetResourcesPos(const int &i, const V3 &position);
    virtual void SetResourcesAng(const int &i, const ANG &angles);
    virtual void SetResourcesScale(const int &i, const V3 &scale);
    virtual void SetResourcesOpacity(const int &i, const float &opacity);
    virtual void SetResourcesZSort(const int &i, const int &zsort);
    virtual void SetResourcesSpecific(const int &i, const int &specific);
    virtual void SetResourcesBoneAttach(const int &i, const CSTRING &bone);
    virtual void SetOpacity(const float &opacity);
    virtual void SetStartFirstAnim(const bool &start);
    virtual void SetOccludingBox(const V3 &size);
    virtual void SetZSort(const int &zsort);
    virtual void SetAnimSpeed(const float &speed);
    virtual void SetLightMask(const int &mask);
    virtual void SetCheckRenderFrustrum(const bool &check);
    virtual void SetRenderNotifications(const bool &notify);
    // Applies an actor resources template by name.
    virtual void SetActorResourcesTemplate(const CSTRING &name);

    virtual void Virtual85();
    // Copies the actor's properties to target, made by the factory when 0.
    virtual ACTOR *Clone(ACTOR *target);
    virtual void Virtual87();
    // The Activate command.
    virtual SRESULT Activate(CMDCALL *call);
    // Makes the resources' copies and runs OnActivate, or releases them.
    virtual void SetActive(bool active);
    virtual V3 GetPosition();
    virtual ANG GetAngles();
    virtual V3 GetScale();
    virtual void SetRotation(const MATRIX &rotation);
    virtual V3 GetDirection();
    virtual V3 GetUp();
    virtual void Virtual96();
    // Between the actor's space and the world's.
    virtual V3 LocalToWorld(V3 point);
    virtual V3 WorldToLocal(V3 point);
    virtual V3 LocalToWorldDir(V3 direction);
    // Writes the actor out to the log (compiled out of the release build).
    virtual void Dump();
    virtual void Place();
    // Called every frame.
    virtual void Update();
    virtual void Virtual103();
    // Called on activation: attaches to its bone, makes the copies and runs
    // OnActivate.
    virtual void Start();
    // Called on deactivation: releases the copies.
    virtual void Stop();
    // Called once the actor's script has loaded.
    virtual void OnScriptLoaded();
    // The resetResources command.
    virtual SRESULT ResetResources(CMDCALL *call);
    // Attaches resource i's copy to the actor.
    virtual void AttachResource(int i);
    virtual SRESULT ActivateResource(CMDCALL *call);
    virtual SRESULT DeactivateResource(CMDCALL *call);
    virtual SRESULT SetAnimationFrame(CMDCALL *call);
    // Resource i's animation commands.
    virtual void SetResourceFrame(int i, const char *animation, float frame);
    virtual SRESULT PlayAnimation(CMDCALL *call);
    virtual void PlayResourceAnimation(int i, const char *animation, bool loop, float speed);
    virtual SRESULT BlendAnimation(CMDCALL *call);
    virtual void BlendResourceAnimation(int i, const char *animation, float blend, bool loop, float speed);
    virtual SRESULT PauseAnimation(CMDCALL *call);
    // The size of the actor's model.
    virtual V3 GetSize();
    virtual void Virtual119();
    // A contact the collision manager found for the actor.
    virtual void OnContact(CONTACT *contact);
    // Moves the actor along its path.
    virtual void FollowPath();
    virtual void Virtual122();
    // The commands.
    virtual SRESULT Remove(CMDCALL *call);
    virtual SRESULT StopAnimation(CMDCALL *call);
    virtual SRESULT AttachFX(CMDCALL *call);
    virtual SRESULT CreateFX(CMDCALL *call);
    virtual SRESULT CreateTagFX(CMDCALL *call);
    virtual SRESULT Fade(CMDCALL *call);
    virtual SRESULT AddBehavior(CMDCALL *call);
    virtual SRESULT ClearBehaviors(CMDCALL *call);
    virtual SRESULT Deactivate(CMDCALL *call);
    virtual SRESULT SetLight(CMDCALL *call);
    virtual SRESULT SetSpeed(CMDCALL *call);
    virtual SRESULT ChangeResourceMaterialCommand(CMDCALL *call);
    // Takes the resources of an actor resources template.
    virtual void ApplyResourcesTemplate(ACTORRESOURCESMASTER *master);
    virtual void Virtual136();
    virtual void Virtual137();
    virtual SRESULT QueueBehaviorCommand(CMDCALL *call);
    // The actor touched another on the screen.
    virtual void OnCollide(ACTOR *other);

    virtual V2 Link01();
    virtual void Link02(const V2 &position);
    virtual V2 Link03();
    virtual void Link04(const V2 &size);
    virtual V3 Link05();
    virtual void Link06(const V3 &position);
    virtual void Link07(V3 *direction, V3 *up);
    virtual void Link08(const V3 &position, const V3 &direction);
    virtual V3 Link09();
    virtual void Link10(const V3 &scale);
    virtual float Link11();
    virtual void Link12(float opacity);
    virtual void Link13();
    virtual float Link14();

    // The box's top corners.
    void GetBoxTop(V3 *a, V3 *b);
    BOXCORNERS *GetBox();
    // Releases resource i's copy.
    void ReleaseResource(int i);
    // Grows the occluding box to fit the copies.
    void UpdateOccludingBox();
    // Turns to face the way the actor moved.
    void RotateToMove();
    // Steps the actor for a frame.
    void Step();
    // Finds the tag of the bone attached to; whether found.
    bool UpdateAttachTag();
    // Attaches it to the actor with the handle.
    void AttachTo(const RESHANDLE &parent);
    // The copy made by the manager with the id.
    RESOURCE *FindResourceCopy(int managerId);
    // Pauses or resumes resource i's animation.
    void PauseResourceAnimation(int i, bool pause);
    // Resource i's copy, or null.
    RESOURCE *GetCopy(int i);
    // Fades the opacity from from to to over time seconds.
    void StartFade(float from, float to, float time);
    // Changes a material of resource i's model; whether it could.
    bool ChangeResourceMaterial(int i, const char *from, const char *to);
    // Works out whether the actor is in view, for the scene's update.
    void UpdateVisibility();
    // Releases the copies and clears the resource properties.
    void ClearResources();
    // Makes resource i's copy.
    void CreateResource(long i);
    // The rotation and its inverse.
    MATRIX GetMatrix();
    MATRIX GetInverseMatrix();
    // Makes an effect at position, attached to bone of a resource when
    // given as "resource,bone".
    RESHANDLE MakeFX(const char *name, RESHANDLE parent, V3 position, ANG angles, FIXEDSTRING<0x200> bone);

    static int s_classId;
    static int s_factoryId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    ZEROV3 m_position;
    ZEROANG m_angles;
    ZEROV3 m_scale;
    bool m_isTemplate;
    bool m_frozen;
    CSTRING m_comments;
    bool m_colliderColorOverride;
    RGB m_colliderColor;
    bool m_showCollider;
    CSTRING m_attachTo;
    CSTRING m_attachBone;
    int m_attachBoneResId;
    ZEROV3 m_attachPos;
    ZEROANG m_attachAng;
    ZEROV3 m_attachScale;
    float m_attachOpacity;
    int m_attachZSort;
    int m_activation;
    int m_deactivation;
    CSTRING m_onActivate;
    bool m_testOnActivate;
    bool m_onActivateRunInEditor;
    int m_behavior;
    ZEROV3 m_vel;
    ZEROV3 m_angVel;
    ZEROV3 m_angAceleration;
    ZEROV3 m_aceleration;
    bool m_panSounds;
    bool m_moveWithScreenZ;
    bool m_moveWithScreenY;
    bool m_previewBehavior;
    bool m_rotateToMove;
    float m_forcedCamDist;
    bool m_forceInScreen;
    CSTRING m_pathName;
    int m_pathBehavior;
    bool m_testPath;
    ZEROV3 m_vectorUp;
    ZEROV3 m_vectorForward;
    bool m_loop;
    float m_flowAngle;
    float m_flowRadius;
    bool m_pathRelativePos;
    bool m_alsoLineal;
    bool m_displayPath;
    CSTRING m_resources[ACTOR_MAX_RESOURCES];
    ZEROV3 m_resourcesPos[ACTOR_MAX_RESOURCES];
    ZEROANG m_resourcesAng[ACTOR_MAX_RESOURCES];
    ZEROV3 m_resourcesScale[ACTOR_MAX_RESOURCES];
    float m_resourcesOpacity[ACTOR_MAX_RESOURCES];
    int m_resourcesZSort[ACTOR_MAX_RESOURCES];
    int m_resourcesSpecific[ACTOR_MAX_RESOURCES];
    CSTRING m_resourcesBoneAttach[ACTOR_MAX_RESOURCES];
    float m_opacity;
    bool m_startFirstAnim;
    ZEROV3 m_occludingBox;
    int m_zsort;
    float m_animSpeed;
    int m_lightMask;
    bool m_checkRenderFrustrum;
    bool m_renderNotifications;
    CSTRING m_actorResourcesTemplate;
    // Placed in the scene.
    bool m_placed;
    bool m_scriptLoaded;
    // The actor's slot in the scene's list of screen actors, or -1.
    int m_screenIndex;
    bool m_active;
    bool m_9a1;
    // The time the actor last updated at.
    float m_lastTime;
    BOXCORNERS m_box;
    // How many times it was activated.
    int m_activations;
    bool m_anglesChanged;
    // Where the actor was, how it was turned and how fast it went the frame
    // before.
    ZEROV3 m_lastPosition;
    ZEROANG m_lastAngles;
    ZEROV3 m_lastVel;
    // The spawner that made the actor.
    RESHANDLE m_spawner;
    // The actor attached to and the tag of its bone.
    RESHANDLE m_attachParent;
    TAG *m_attachTag;
    ACTORRESOURCESMASTER *m_template;
    // Per resource, its master and the copy made of it.
    RESHANDLE m_masters[ACTOR_MAX_RESOURCES];
    RESHANDLE m_copies[ACTOR_MAX_RESOURCES];
    // One past the last resource set.
    int m_numResources;
    float m_c1c;
    PATHMASTER *m_pathMaster;
    int m_c24;
    // Where along the path it is, now and the frame before.
    float m_pathPos;
    float m_lastPathPos;
    int m_c30;
    ZEROANG m_c34;
    // Where the actor was made.
    ZEROV3 m_origin;
    // The contacts found this frame.
    std::list<CONTACT> m_contacts;
    bool m_collectContacts;
    BEHAVIORLIST m_behaviors;
    // A fade in progress: the time left and the opacity it ends at.
    float m_fadeTime;
    float m_fadeTarget;
    bool m_removeRequested;
    bool m_inView;
    // The scene's visibility node the actor is in, and the actors next to
    // it there.
    PVSNODE *m_node;
    ACTOR *m_next;
    ACTOR *m_prev;
};

#endif
