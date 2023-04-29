#ifndef SFW_SELECTION_HPP
#define SFW_SELECTION_HPP

#include <cstddef> // size_t
#include <algorithm> // using std::min, std::max, std::swap;

namespace sfw
{

/*****************************************************************************
  Generalized text selection abstraction, independent from the internals
  of the text editor using it. The editor should hook up various selection
  methods in its event handlers and operations (most importantly calling
  `follow(pos)` in its cursor updating proc.) to achieve the desired
  selection behavior.

  (This implementation is optimized and tested for volatile selections, but
  can easily support persistent modes, too... I guess... Never tried. ;)
  But added at least a persistence flag, and the update proc. does take it
  into account. However, the owner (editor) must behave somewhat differently
  with a pers. selection, so it's not just a matter of flipping a flag.)

  Rules (-- defined for volatile (non-persistent) selections):

  * A selection represents a sequence of cells (of some actual text) stored
    elsewhere. The sequence is defined by a lower and an upper bound, both
    unsigned, and implicitly matching the position ordering of the tracked
    text (so letf-to-right or right-to-left, or even vertical scripts, is
    not our concern here).

    The selection itself doesn't know or care about the text it tracks,
    it doesn't even know its (cell) type.

  * It also doesn't know the actual length of the sequence; it just
    accepts whatever position it is given on updates.

  * Selections are directional. Not in the left/right sense, but temporally:
    An anchor position is remembered when the selection is started, and then
    the current "active end" (or "head") of it is what future actions will
    affect (by default).

    The anchor and active positions are always at the opposite ends of
    a (valid) selection, unambiguously (but indirectly) defining it.

    Both the anchor and active end can be read and (re)set directly at any
    time. (So, if getters/setters are provided, that's for convenience,
    not consistency.)

  * A selection can be in these mutually exclusive operative states:
    - invalid (or cancelled): not representing any selection
    - valid/started (or active): representing a selection, reacting to
      (following up) changes
    - valid/stopped: representing a selection, but not following changes

    These states can be queried and set directly at any time (via dedicated
    getters/setters).

    Care must be taken to not just set a selection as valid without also
    setting its anchor/active positions properly.

  * The intended way to update the selection is:
    a) setting it to one of the operative sates as needed (e.g. on the
       state of the Shift key), and
    b) every time the cursor moves, telling it the new position.

    This way, the updates can be basically automatic: nothing would
    happen if not desired, and it would faithfully follow the cursor
    in the active mode. It could also be paused and resumed to support
    persistent selections etc.

    For mouse support:

        1. The click-in action should be recognized by the owner, to
        2. start a new selection, setting both the anchor and the active
           pos. to the click position.
        3. Then, until the button is released, keep sending the currently
          "hit" text position to the selection on mouse-move events.
        4. Stop the selection (or just stop sending it updates)
           on button release.

        Note: what should happen then (i.e. when to cancel the selection),
        is up to the owner.

  * The current selection, if valid, can be queried, and returned as its
    [lower, upper] bounds. (I.e. the anchor & active pos., but swapped if
    necessay.)

    (Note: this is typically done e.g. on clipboard actions, or for marking
    up the selected portion of the actual text for drawing.)

    If the anchor pos == lower and the active pos == upper, the selection
    is said to be "normalized" (but the selection itself doesn't care about
    this).

  * If the selection is valid, it can also be empty or non-empty.
    An invalid selection is always empty.

  * An empty selection is represented by lower = upper in these responses
    (and can be queried explicitly, too).

  NOTE: It's generally *not enough* to just query if the selection if empty,
        and then decide that there's "no selection"... Disregarding the op.
        state could lead to "easy to get, hard to rid" inconsistencies.

        The operations are NOT atomic.
 *****************************************************************************/
struct TextSelection
{
    size_t anchor_pos = 0;
    size_t active_pos = 0;
    enum { CANCELLED, ACTIVE, STOPPED } state = CANCELLED;
    bool persistent = false;

    //------------------------------------------------------------------------
    // Queries
    //------------------------------------------------------------------------
    operator bool()  const { return valid() && !empty(); }
    bool   valid()   const { return state != CANCELLED; }
    size_t empty()   const { return !valid() || active_pos == anchor_pos; }
    bool   active()  const { return state == ACTIVE; }
    bool   stopped() const { return state == STOPPED; }
    size_t lower()   const { return valid() ? std::min(anchor_pos, active_pos) : 0; }
    size_t upper()   const { return valid() ? std::max(anchor_pos, active_pos) : 0; }
    size_t length()  const { return valid() ? upper() - lower() : 0; }

    //------------------------------------------------------------------------
    // Convenience setters to assist the text editing actions
    //------------------------------------------------------------------------
    void set(size_t head) { active_pos = head; }

    void move(int delta); // delta can be negative! Underflow will be prevented.
    void set_from_to(size_t anchor, size_t head) { anchor_pos = anchor; set(head); }
    void set_empty(size_t pos) { set_from_to(pos, pos); }
    void set_span(size_t anchor, int offset); // offset can be negative! Underflow will be prevented.
    //!!??void set_length(size_t length); // Move the head only, even if < anchor -- is there a use-case for this?
    void reverse() { std::swap(active_pos, anchor_pos); }
    void normalize() { if (active_pos < anchor_pos) reverse(); }
    void replace(size_t length) { normalize(); set_span(anchor_pos, (int)length); }

    //------------------------------------------------------------------------
    // Operations
    //------------------------------------------------------------------------
    void start(size_t pos, bool persistent = false);
    void stop()   { if (active())  state = STOPPED; }
    void resume() { if (stopped()) state = ACTIVE; }
    void cancel() { state = CANCELLED; }
    void reset()  { cancel(); set_empty(0); }

    void follow(size_t pos);
};


//----------------------------------------------------------------------------
inline void TextSelection::move(int delta)
{
    if (int(active_pos) + delta < 0) active_pos = 0; else active_pos += delta;
}

//----------------------------------------------------------------------------
inline void TextSelection::set_span(size_t anchor, int offset)
{
    anchor_pos = anchor;
    if (int(anchor_pos) + offset < 0) active_pos = 0; else active_pos += offset;
}

//----------------------------------------------------------------------------
inline void TextSelection::start(size_t pos, bool persist)
{
    persistent = persist; set_empty(pos); state = ACTIVE;
}

//----------------------------------------------------------------------------
inline void TextSelection::follow(size_t pos)
{
    if (active()) set(pos);
    else if (!persistent) cancel();
}

} // namespace

#endif // SFW_SELECTION_HPP
