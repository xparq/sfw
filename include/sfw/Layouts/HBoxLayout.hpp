#ifndef GUI_HBOXLAYOUT_HPP
#define GUI_HBOXLAYOUT_HPP

#include "sfw/Layout.hpp"

namespace sfw
{

/**
 * Horizontally stacked layout
 */
class HBoxLayout: public Layout
{
public:
private:
    void recomputeGeometry() override;
};

} // namespace

#endif // GUI_HBOXLAYOUT_HPP
