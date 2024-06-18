#ifndef SFW_GFX_FILLEDRECT_HPP
#define SFW_GFX_FILLEDRECT_HPP

#include "sfw/gfx/Render.hpp"

#include "sfw/gfx/backend.hpp"
#include GFX_BACKEND_SPECIFIC(FilledRect.hpp)

namespace sfw {

using FilledRect = FilledRect_Impl; //!! FilledRect_Impl<FilledRect>;

};

#endif // SFW_GFX_FILLEDRECT_HPP
