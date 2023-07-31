#ifndef SFW_GFX_FILLEDRECT_HPP
#define SFW_GFX_FILLEDRECT_HPP

#include "sfw/Gfx/Render.hpp"

#include "sfw/Gfx/backend.hpp"
#include GFX_BACKEND_SPECIFIC(FilledRect.hpp)

namespace sfw {

using FilledRect = FilledRect_Impl; //!! FilledRect_Impl<FilledRect>;

};

#endif // SFW_GFX_FILLEDRECT_HPP
