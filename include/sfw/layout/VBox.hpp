#ifndef GUI_VBOXLAYOUT_HPP
#define GUI_VBOXLAYOUT_HPP

#include "sfw/Layout.hpp"

namespace sfw
{

/**
 * Vertically stacked layout
 */
class VBox: public Layout
{
    void recomputeGeometry() override;
};

} // namespace

#endif // GUI_VBOXLAYOUT_HPP
