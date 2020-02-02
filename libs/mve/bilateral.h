/*
 * Implementation of a bilateral filter for images and depth maps.
 *
 * Bilateral filtering smoothes similar regions (similar in color value)
 * but preserves edges (depth/color discontinuities). This is achieved
 * by combining geometric closeness (gaussian smoothing) with photometric
 * closeness (edge preservation).
 *
 * Written by Simon Fuhrmann.
 */

#ifndef MVE_BILATERAL2_HEADER
#define MVE_BILATERAL2_HEADER

#include "math/vector.h"
#include "math/accum.h"
#include "math/functions.h"

#include "defines.h"
#include "image.h"
#include "filter.h"

MVE_NAMESPACE_BEGIN
MVE_IMAGE_NAMESPACE_BEGIN


mve::FloatImage::Ptr
median_filter (mve::FloatImage::ConstPtr in, float size)
{
    if (in == nullptr)
        throw std::invalid_argument("Null image given");

    int const w = in->width();
    int const h = in->height();
    mve::FloatImage::Ptr out = mve::FloatImage::create(w, h, 1);
    out->fill(0);

    median_filter_2d(w, h, size, size, 0, in->get_data().data(), out->get_data().data());

    return out;
}


mve::FloatImage::Ptr
depthmap_bilateral_filter(mve::FloatImage::ConstPtr dm,
    mve::FloatImage::ConstPtr ci, float sigma, int kernel_size)
{
    int const dm_w(dm->width());
    int const dm_h(dm->height());

    int const w = ci->width();
    int const h = ci->height();
    mve::FloatImage::Ptr out = mve::FloatImage::create(w, h, 1);
    out->fill(0);

    float const scale_x = static_cast<float>(dm_w) / static_cast<float>(w);
    float const scale_y = static_cast<float>(dm_h) / static_cast<float>(h);

    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
        {
            math::Accum<float> accum(0.0f);
            for (int ky = -kernel_size; ky <= kernel_size; ++ky)
                for (int kx = -kernel_size; kx <= kernel_size; ++kx)
                {
                    int const ci_x = math::clamp(x + kx, 0, w - 1);
                    int const ci_y = math::clamp(y + ky, 0, h - 1);
                    int const dm_x = math::clamp(scale_x * ci_x, 0.f,
                        static_cast<float>(dm_w) - 1.f);
                    int const dm_y = math::clamp(scale_y * ci_y, 0.f,
                        static_cast<float>(dm_h) - 1.f);

                    if(dm->at(dm_x, dm_y, 0) == 0.0f)
                        continue;

                    float weight(1.0f);
                    /* spatial weight */
                    weight *= math::gaussian_2d((float)kx, (float)ky,
                        sigma, sigma);
                    /* depth value difference weight */
                    for (int c = 0; c < ci->channels(); ++c)
                        weight *= math::gaussian(ci->at(ci_x, ci_y, c)
                            - ci->at(x, y, c), 0.1f);

                    accum.add(dm->at(dm_x, dm_y, 0), weight);
                }
            if (accum.w > 0)
                out->at(x, y, 0) = accum.normalized();
        }
    return out;
}

MVE_IMAGE_NAMESPACE_END
MVE_NAMESPACE_END

#endif /* MVE_BILATERAL2_HEADER */
