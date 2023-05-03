#ifndef SCATTER_RECORD_H
#define SCATTER_RECORD_H

#include <memory>

#include "ray.h"
#include "materials.h"
#include "pdf.h"

class ScatterRecord
{
public:
    bool is_specular;
    Ray specular_ray;
    Color attenuation;
    std::shared_ptr<PDF> pdf;

    ScatterRecord()
    {
        is_specular = false;
    }
};

#endif // SCATTER_RECORD_H