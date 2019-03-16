
#ifndef _DISPATCH_H
#define _DISPATCH_H
#include <stdint.h>

#include <vector>
#include <math.h>
using namespace std;

#define RAD2DEG 180/M_PI
#define DEG2RAD M_PI/180

struct Point3D
{
    union {
        float pos[3];
        struct {
            float x;
            float y;
            float z;
        };
    };
    Point3D()
    {
        pos[0] = pos[1] = pos[2] = 0;
    }

    Point3D(float *array)
    {
        pos[0] = array[0];
        pos[1] = array[1];
        pos[2] = array[2];
    }

    Point3D(float x, float y, float z)
    {
        pos[0] = x;
        pos[1] = y;
        pos[2] = z;
    }
};

struct Point3DA:Point3D
{
    union {
        float angle[3];
        struct {
            float pitch;
            float roll;
            float yaw;
        };
    };
    Point3DA():Point3D()
    {
        angle[0] = angle[1] = angle[2] = 0;
    }

    Point3DA(Point3D& pt)
    {
        x = pt.x; y = pt.y; z = pt.z;
        angle[0] = angle[1] = angle[2] = 0;
    }
};

struct VeloObjBase:Point3DA
{
    /// size
    union {
        float size[3];
        struct {
            float length;
            float width;
            float height;
        };
    };

    /// range
    union {
        float dist[8];
        struct {
            float xmax;
            float xmin;
            float ymax;
            float ymin;
            float xaymax;
            float xaymin;
            float xsymax;
            float xsymin;
        };
    };
    /// color
    //    unsigned char color[4];
    /// points
    int num;

    VeloObjBase()
    {
        pos[0] = pos[1] = pos[2] = 0;
        dist[0] = dist[1] = dist[2] = dist[3] = 0;
        dist[4] = dist[5] = dist[6] = dist[7] = 0;
    }

    bool InSide(Point3D& pt)
    {
        bool ins = false;
        float x1 = pt.x - x;
        float y1 = pt.y - y;
        float xay = x1 + y1;
        float xsy = x1 - y1;
        if((dist[0] >= x1)&&(x1 >= dist[1]) &&
                (dist[2] >= y1)&&(y1 >= dist[3]) &&
                (dist[4] >= xay)&&(xay >= dist[5]) &&
                (dist[6] >= xsy)&&(xsy >= dist[7]))
        {
            ins = true;
        }
        return ins;
    }
};


struct VeloObjTrack:VeloObjBase
{
    /// speed
    union {
        float spd[3];
        struct {
            float sx;
            float sy;
            float sz;
        };
    };

    /// global position
    union {
        float gpos[3];
        struct {
            float gx;
            float gy;
            float gz;
        };
    };

    /// classification
    unsigned char type;
    unsigned char motion;
    unsigned short age;

    VeloObjTrack(): VeloObjBase()
    {
        type = 0;
        age = 0;
    }
};

struct VeloObj:VeloObjTrack
{
    int id;
    short sid;
    short tid;

    VeloObj(): VeloObjTrack()
    {
        tid = 0;
    }

    void SimpleCopy(const VeloObj& obj)
    {
        pos[0] = obj.pos[0];
        pos[1] = obj.pos[1];
        pos[2] = obj.pos[2];

        dist[0] = obj.dist[0];
        dist[1] = obj.dist[1];
        dist[2] = obj.dist[2];
        dist[3] = obj.dist[3];
        dist[4] = obj.dist[4];
        dist[5] = obj.dist[5];
        dist[6] = obj.dist[6];
        dist[7] = obj.dist[7];

        type = obj.type;
        age = obj.age;
        tid = obj.tid;

        gpos[0] = obj.gpos[0];
        gpos[1] = obj.gpos[1];
        gpos[2] = obj.gpos[2];
    }
};

struct VeloPoint:Point3D
{
    union
    {
        struct
        {
            unsigned char ring;
            unsigned char  intensity;
            unsigned short range;
        };
        int label32;
    };
};

struct VeloBlockObj:VeloObj
{
    unsigned char on_road;
    unsigned short ptnum;
    vector<int> blks;
    vector<VeloPoint> pts;
    VeloBlockObj(): VeloObj()
    {

    }
};

#endif // _DISPATCH_H