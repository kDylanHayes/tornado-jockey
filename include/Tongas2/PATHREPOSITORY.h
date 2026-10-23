// Path repositories: PATHREPOSITORYMASTER loads a file of paths, each a
// name and a list of points, into PATH masters and keeps its own copy of
// every point. Placeholder names apart from the class name and property.
#ifndef TONGAS2_PATHREPOSITORY_H
#define TONGAS2_PATHREPOSITORY_H

#include <string.h>
#include <string>
#include <vector>

#include "Tongas2/CSTRING.h"
#include "Tongas2/PATH.h"

// A point of a stored path and the step from it to the point's end.
struct PATHREPOPOINT
{
    PATHV3 pos;
    PATHV3 step;
};

// An array that grows to fit the index it is asked for.
template <class T>
class GROWARRAY
{
public:
    GROWARRAY()
    {
        m_size = 0;
        m_data = 0;
        m_capacity = 0;
    }
    virtual ~GROWARRAY()
    {
        delete[] m_data;
        m_data = 0;
        m_capacity = 0;
        m_size = 0;
    }

    // Makes room for count elements; the old ones are copied over as bytes.
    void Reserve(unsigned int count)
    {
        if (count > m_capacity) {
            T *data = new T[count];
            if (m_data) {
                if (m_size > 0)
                    memcpy(data, m_data, m_size * sizeof(T));
                delete[] m_data;
            }
            m_data = data;
            m_capacity = count;
        }
    }
    void SetSize(unsigned int size)
    {
        Reserve(size);
        if (size <= m_capacity)
            m_size = size;
        else
            m_size = m_capacity;
    }
    T &operator[](unsigned int i)
    {
        if (i + 1 > m_size)
            SetSize(i + 1);
        return m_data[i];
    }

    std::vector<int> m_4;
    T *m_data;
    unsigned int m_size;
    unsigned int m_capacity;
};

class PATHREPOSITORYMASTER : public RESOURCEMASTER
{
public:
    PATHREPOSITORYMASTER(RESOURCEMANAGER *manager, const char *name);
    virtual ~PATHREPOSITORYMASTER();

    static void RegisterCommands();

    virtual int GetClassId();
    virtual int GetParentClassId();
    virtual const char *GetClassName();
    virtual SRESULT Execute(CMDENTRY *command, TOKENS **args);

    // Loads the paths in data/art/name into PATH masters, making the ones
    // that don't exist yet.
    virtual void SetFileName(const CSTRING &name);

    // Empties every path and the names.
    void Clear();

    static int s_classId;
    static CMDENTRY s_commands[];
    static PROPENTRY s_properties[];

    CSTRING m_file;
    GROWARRAY<std::vector<PATHREPOPOINT> > m_paths;
    std::vector<std::string> m_names;
};

#endif
