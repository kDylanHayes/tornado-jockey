// A placeholder unit for the model resource code at 0x004fb460.
//
// Discovery scaffold. The unit starts with std::string/vector node helpers,
// then builds model, material and texture objects (the constructor at
// 0x00501a50 initialises the seven string members of the texture object and
// allocates the texture resizer). The class name is descriptive: no
// RegisterClassName string names it. The method names are descriptive too.
// Bodies are empty until the unit is matched; each function's exact target
// size is in config/symbols.txt so a later delink cannot cut the target
// short.
#ifndef TONGAS2_MODEL_H
#define TONGAS2_MODEL_H

class MODEL
{
public:
    MODEL();
    ~MODEL();

    void *Find(const char *name);
    void *Load(const char *name);
    void *Create(const char *name);
};

#endif
