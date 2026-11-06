#include "Tongas2/MESHDX8SD.h"

__declspec(noinline) void MESHDX8SDSETTER::Empty()
{
}

#include "Tongas2/LIGHTDX8.h"
#include "Tongas2/LOG.h"
#include "Tongas2/RENDERER.h"
#include "Tongas2/RESOURCES.h"

// The packed device color of a float color: each component 0..1 becomes
// 0..255, with 0x80 for zero.
int __fastcall PackColor(const V3 &color)
{
    unsigned int r = (unsigned int)(int)(color.x * 127.0f + 128.0f);
    unsigned int g = (unsigned int)(int)(color.y * 127.0f + 128.0f);
    unsigned int b = (unsigned int)(int)(color.z * 127.0f + 128.0f);
    return (int)(((r + 0xff00) * 0x100 + g) * 0x100 + b);
}

LIGHTVALUE UnpackColor(const int *color)
{
    LIGHTVALUE value;
    value.r = (float)color[0] * (1.0f / 255.0f);
    value.g = (float)color[1] * (1.0f / 255.0f);
    value.b = (float)color[2] * (1.0f / 255.0f);
    value.a = 1.0f;
    return value;
}

LIGHTDX8RESOURCE::LIGHTDX8RESOURCE(LIGHTMASTER *master, const RESHANDLE &handle)
    : LIGHT(master, handle), m_direction(V3(0.0f, 0.0f, 0.0f)), m_savedAmbient(0)
{
    memset(&m_light, 0, sizeof(m_light));
}

void LIGHTDX8RESOURCE::Start()
{
    LIGHT::Start();

    LIGHTMASTER *master = (LIGHTMASTER *)m_master;
    LIGHTDX8MANAGER *manager = (LIGHTDX8MANAGER *)master->m_manager;

    switch (master->m_type) {
    case 0:
        m_savedAmbient = manager->m_color;
        manager->m_color = ((m_color.r | 0xffffff00) << 8 | m_color.g & 0xff) << 8 | m_color.b & 0xff;
        break;
    case 1: {
        LIGHTVALUE diffuse;
        LIGHTVALUE specular;
        LIGHTVALUE ambient;
        m_light.type = 3;
        diffuse = UnpackColor(&m_color.r);
        m_light.diffuse = diffuse;
        specular = UnpackColor(&m_specular.r);
        m_light.specular = specular;
        ambient = UnpackColor(&master->m_ambient.r);
        m_light.ambient = ambient;
        break;
    }
    case 2: {
        LIGHTVALUE diffuse;
        LIGHTVALUE specular;
        LIGHTVALUE ambient;
        m_light.type = 1;
        diffuse = UnpackColor(&m_color.r);
        m_light.diffuse = diffuse;
        specular = UnpackColor(&m_specular.r);
        m_light.specular = specular;
        ambient = UnpackColor(&master->m_ambient.r);
        m_light.ambient = ambient;
        m_light.range = master->m_range;
        m_light.attenuation0 = master->m_attenuation0;
        m_light.attenuation1 = master->m_attenuation1;
        m_light.attenuation2 = master->m_attenuation2;
        break;
    }
    default:
        if (g_pLog)
            g_pLog->Error("LIGHTDX8RESOURCE::OnActivate - Light type not supported");
        break;
    }

    RESHANDLE *handle = &m_handle;
    int type = master->m_type;
    manager->m_mask = 0xffffff01;
    switch (type) {
    case 1:
        manager->m_lights.push_back(*handle);
        break;
    case 2:
        manager->m_lights2.push_back(*handle);
        break;
    }

    manager->Invalidate(master->m_lightMask);
}

void LIGHTDX8RESOURCE::Update()
{
    OBJECT::Update();

    LIGHTMASTER *master = (LIGHTMASTER *)m_master;
    LIGHTDX8MANAGER *manager = (LIGHTDX8MANAGER *)master->m_manager;

    if (master->m_type == 0) {
        if (m_color.r != m_baseColor.r || m_color.g != m_baseColor.g || m_color.b != m_baseColor.b) {
            m_baseColor = m_color;
            manager->Invalidate(master->m_lightMask);
            manager->m_color = ((m_color.r | 0xffffff00) << 8 | m_color.g & 0xff) << 8 | m_color.b & 0xff;
            AddCounts();
        }
        return;
    }

    if (m_drawPosition.x != m_lastPosition.x || m_drawPosition.y != m_lastPosition.y ||
        m_drawPosition.z != m_lastPosition.z) {
        m_lastPosition = m_drawPosition;
        AddCounts();
        V3 position = m_position;
        m_light.position = position;
        manager->Invalidate(master->m_lightMask);
    }

    m_c0 = m_position;

    if (m_angles != m_134) {
        m_134 = m_angles;
        AddCounts();
        m_direction = m_angles.GetDirection();
        V3 direction = m_direction;
        m_light.direction = direction;
        manager->Invalidate(master->m_lightMask);
        if (master->m_type == 1)
            g_pRenderer->GetDevice()->SetRenderState(D3DRS_TEXTUREFACTOR, PackColor(m_direction));
    }

    if (m_color.r != m_baseColor.r || m_color.g != m_baseColor.g || m_color.b != m_baseColor.b) {
        m_baseColor = m_color;
        AddCounts();
        m_light.diffuse = UnpackColor(&m_color.r);
        manager->Invalidate(master->m_lightMask);
    }

    if (m_specular.r != m_baseSpecular.r || m_specular.g != m_baseSpecular.g ||
        m_specular.b != m_baseSpecular.b) {
        m_baseSpecular = m_specular;
        AddCounts();
        m_light.specular = UnpackColor(&m_specular.r);
        manager->Invalidate(master->m_lightMask);
    }
}

void LIGHTDX8RESOURCE::Stop()
{
    LIGHTMASTER *master = (LIGHTMASTER *)m_master;
    LIGHTDX8MANAGER *manager = (LIGHTDX8MANAGER *)master->m_manager;

    if (master->m_type == 0)
        manager->m_color = m_savedAmbient;

    LIGHT::Stop();
    manager->RemoveHandle(master->m_type, m_handle);
    manager->Invalidate(master->m_lightMask);
}

RESOURCE *LIGHTDX8MASTER::CreateCopy(const RESHANDLE &handle)
{
    return new LIGHTDX8RESOURCE(this, handle);
}

void LIGHTDX8MASTER::SetType(const int &type)
{
    if (m_type != type) {
        RESOURCE *copy = m_used;
        while (copy) {
            RESOURCE *next = copy->m_next;
            if (copy->m_active) {
                LIGHTMASTER *copyMaster = (LIGHTMASTER *)copy->m_master;
                LIGHTDX8MANAGER *manager = (LIGHTDX8MANAGER *)copyMaster->m_manager;
                manager->RemoveHandle(copyMaster->m_type, copy->m_handle);
                int newType = type;
                manager->m_mask = 0xffffff01;
                switch (newType) {
                case 1:
                    manager->m_lights.push_back(copy->m_handle);
                    break;
                case 2:
                    manager->m_lights2.push_back(copy->m_handle);
                    break;
                }
            }
            copy = next;
        }
    }
    m_type = type;
}

LIGHTDX8MANAGER::LIGHTDX8MANAGER()
{
    m_color = 0xff808080;
}

LIGHTDX8MANAGER::~LIGHTDX8MANAGER()
{
}

RESOURCEMASTER *LIGHTDX8MANAGER::CreateMaster(const char *name)
{
    return new LIGHTDX8MASTER(this, name);
}

const char *LIGHTDX8MANAGER::GetTypeCode()
{
    return "LGHT";
}

void LIGHTDX8MANAGER::Update()
{
    LIGHTMANAGER::Update();
}

void LIGHTDX8MANAGER::Virtual09(int mask)
{
    if (m_mask == mask)
        return;

    m_mask = mask;
    IDirect3DDevice8 *device = g_pRenderer->GetDevice();
    if (mask & 1)
        device->SetRenderState(D3DRS_AMBIENT, m_color);
    else
        device->SetRenderState(D3DRS_AMBIENT, 0);

    int maxLights = g_pRenderer->GetCap(2);
    unsigned int index = 0;
    for (std::vector<RESHANDLE>::iterator it = m_lights.begin(); it != m_lights.end(); ++it) {
        if (index >= (unsigned int)maxLights)
            return;
        LIGHTDX8RESOURCE *light = (LIGHTDX8RESOURCE *)g_pResources->GetResource(*it);
        if (light && (((LIGHTMASTER *)light->m_master)->m_lightMask & mask)) {
            device->SetLight(index, (const D3DLIGHT8 *)&light->m_light);
            device->LightEnable(index, TRUE);
            index++;
        }
    }
    while (index < (unsigned int)maxLights) {
        device->LightEnable(index, FALSE);
        index++;
    }
}

void LIGHTDX8MANAGER::LightVertex(const LIGHTRGB &ambient, const V3I &color, const V3 &normal, int mask,
                                  LIGHTRGB &out)
{
    if (mask & 1) {
        out.r = ((unsigned char *)&m_color)[2];
        out.g = ((unsigned char *)&m_color)[1];
        out.b = m_color & 0xff;
        if (ambient.r != 0xff)
            out.r = out.r * ambient.r / 0xff;
        if (ambient.g != 0xff)
            out.g = out.g * ambient.g / 0xff;
        if (ambient.b != 0xff)
            out.b = out.b * ambient.b / 0xff;
    }

    unsigned int maxLights = (unsigned int)g_pRenderer->GetCap(2);
    for (std::vector<RESHANDLE>::iterator it = m_lights.begin(); it != m_lights.end(); ++it) {
        if (maxLights <= 0)
            break;
        LIGHTDX8RESOURCE *light = (LIGHTDX8RESOURCE *)g_pResources->GetResource(*it);
        if (light && (((LIGHTMASTER *)light->m_master)->m_lightMask & mask)) {
            float dot = -(light->m_direction.x * normal.x + normal.y * light->m_direction.y +
                          normal.z * light->m_direction.z);
            if (dot > 0.0f) {
                unsigned int r = (unsigned int)(light->GetColor().r * dot);
                unsigned int g = (unsigned int)(light->GetColor().g * dot);
                unsigned int b = (unsigned int)(light->GetColor().b * dot);
                if (color.x != 0xff)
                    r = color.x * r / 0xff;
                if (color.y != 0xff)
                    g = color.y * g / 0xff;
                if (color.z != 0xff)
                    b = color.z * b / 0xff;
                out.r += r;
                out.g += g;
                out.b += b;
            }
        }
    }

    if (out.r > 0xff)
        out.r = 0xff;
    if (out.g > 0xff)
        out.g = 0xff;
    if (out.b > 0xff)
        out.b = 0xff;
}

void LIGHTDX8MANAGER::Invalidate(int mask)
{
    if (m_mask & mask)
        m_mask = -1;
}

void LIGHTDX8MANAGER::RemoveHandle(int type, const RESHANDLE &handle)
{
    m_mask = 0xffffff01;
    switch (type) {
    case 1:
        for (std::vector<RESHANDLE>::iterator it = m_lights.begin(); it != m_lights.end(); ++it) {
            if (*it == handle) {
                m_lights.erase(it);
                break;
            }
        }
        break;
    case 2:
        for (std::vector<RESHANDLE>::iterator it = m_lights2.begin(); it != m_lights2.end(); ++it) {
            if (*it == handle) {
                m_lights2.erase(it);
                break;
            }
        }
        break;
    }
}

RESOURCEMANAGER *CreateLIGHTDX8()
{
    return new LIGHTDX8MANAGER;
}
