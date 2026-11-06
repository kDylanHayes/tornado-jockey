// A placeholder unit for the object at 0x004f8350: it walks a map's object
// list and handles the special names TARGET, CAMERA, KYBOX, WINGPATH and
// CAMERAPATH, then updates the result over time. The class name is not
// recovered yet, so MAPLOADER is descriptive; the two methods are named from
// what they do, not from strings.
#ifndef TONGAS2_MAPLOADER_H
#define TONGAS2_MAPLOADER_H

class MAPLOADER
{
public:
    void LoadMap(const char *name);
    void Update(float dt);
};

#endif
