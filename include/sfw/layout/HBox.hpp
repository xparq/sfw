#ifndef GUI_HBOXLAYOUT_HPP
#define GUI_HBOXLAYOUT_HPP

#include "sfw/Layout.hpp"

namespace sfw
{

/**
 * Horizontally stacked layout
 */
class HBox: public Layout
{
    void recomputeGeometry() override;
};

} // namespace

#endif // GUI_HBOXLAYOUT_HPP
