#ifndef GUI_VBOXLAYOUT_HPP
#define GUI_VBOXLAYOUT_HPP

#include "sfw/Layout.hpp"

namespace sfw
{

/**
 * Vertically stacked layout
 */
class VBoxLayout: public Layout
{
protected:
    void recomputeGeometry() override;
    void onThemeChanged() override;
};

} // namespace

#endif // GUI_VBOXLAYOUT_HPP
