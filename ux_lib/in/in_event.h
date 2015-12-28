#ifndef UX_IN_EVENT_H
#define UX_IN_EVENT_H

#include "ux_types_and_defs.h"

/// Base class for ex events.

#define EVENTTYPE_TABLE \
/** A key was pressed down. */ \
ENTRY(ET_KEYDOWN) \
/** A key was released. */ \
ENTRY(ET_KEYUP) \
/** A character key was pressed. */ \
ENTRY(ET_CHAR) \
/** The mouse was moved while all buttons were released. */ \
ENTRY(ET_MOUSEMOVE) \
/** The mousewheel was used. */ \
ENTRY(ET_MOUSEWHEEL) \
/** The left mouse button was released. */ \
ENTRY(ET_LBUTTONUP) \
/** The left mouse button was pressed. */ \
ENTRY(ET_LBUTTONDOWN) \
/** The right mouse button was released. */ \
ENTRY(ET_RBUTTONUP) \
/** The right mouse button was pressed. */ \
ENTRY(ET_RBUTTONDOWN) \
/** A hot key was pressed (used on e.g. smart phones)*/ \
ENTRY(ET_HOTKEYDOWN) \
/** A hot key was released (used on e.g. smart phones)*/ \
ENTRY(ET_HOTKEYUP)

/// List of event type ids.
enum in_event_type_e
{
#define ENTRY_ENUM(a) a,  ///< \ref page_helpermacros
#define ENTRY(a) a,  ///< \ref page_helpermacros
    EVENTTYPE_TABLE
//#ifndef GUILIANI_GUILIANILIB
//    USER_EVENTTYPE_TABLE
//#endif
#undef ENTRY
#undef ENTRY_ENUM
    GUILIANI_INTERNAL_ET = INT_MAX ///< Do not touch this. Required for ensuring consistent enum type size across libraries.
};

/// Definition of all key event types.
#define KEYIDENTIFIER_TABLE \
ENTRY(GK_NONE) \
ENTRY(GK_ACTION) \
ENTRY(GK_DOWN) \
ENTRY(GK_UP) \
ENTRY(GK_LEFT) \
ENTRY(GK_RIGHT) \
ENTRY(GK_PAGEDOWN) \
ENTRY(GK_PAGEUP) \
ENTRY(GK_ESCAPE) \
ENTRY(GK_DELETE) \
ENTRY(GK_BACKSPACE) \
ENTRY(GK_TAB) \
ENTRY(GK_HOME) \
ENTRY(GK_END) \
ENTRY(GK_SELECT_ALL) \
ENTRY(GK_COPY) \
ENTRY(GK_PASTE) \
ENTRY(GK_CUT) \
/** Intermittent event (for multi-tapping on smartphones).*/ \
ENTRY(GK_INTERMIT) \
/* char keys */ \
ENTRY(GK_A) \
ENTRY(GK_B) \
ENTRY(GK_C) \
ENTRY(GK_D) \
ENTRY(GK_E) \
ENTRY(GK_F) \
ENTRY(GK_G) \
ENTRY(GK_H) \
ENTRY(GK_I) \
ENTRY(GK_J) \
ENTRY(GK_K) \
ENTRY(GK_L) \
ENTRY(GK_M) \
ENTRY(GK_N) \
ENTRY(GK_O) \
ENTRY(GK_P) \
ENTRY(GK_Q) \
ENTRY(GK_R) \
ENTRY(GK_S) \
ENTRY(GK_T) \
ENTRY(GK_U) \
ENTRY(GK_V) \
ENTRY(GK_W) \
ENTRY(GK_X) \
ENTRY(GK_Y) \
ENTRY(GK_Z) \
/* number keys */ \
ENTRY(GK_0) \
ENTRY(GK_1) \
ENTRY(GK_2) \
ENTRY(GK_3) \
ENTRY(GK_4) \
ENTRY(GK_5) \
ENTRY(GK_6) \
ENTRY(GK_7) \
ENTRY(GK_8) \
ENTRY(GK_9) \
/* symbol char keys */ \
/** 'SPC' key */ \
ENTRY(GK_SPACE) \
/** '!' key */ \
ENTRY(GK_EXCLAIM) \
/** '"' key - double quote */ \
ENTRY(GK_QUOTEDBL) \
/** '#' key */ \
ENTRY(GK_HASH) \
/** '$' key */ \
ENTRY(GK_DOLLAR) \
/** '%' key */ \
ENTRY(GK_PERCENT) \
/** '&' key */ \
ENTRY(GK_AMPERSAND) \
/** ''' key - single quote */ \
ENTRY(GK_QUOTE) \
/** '(' key */ \
ENTRY(GK_LEFTPAREN) \
/** ')' key */ \
ENTRY(GK_RIGHTPAREN) \
/** '*' key */ \
ENTRY(GK_ASTERISK) \
/** '+' key */ \
ENTRY(GK_PLUS) \
/** ',' key */ \
ENTRY(GK_COMMA) \
/** '-' key */ \
ENTRY(GK_MINUS) \
/** '.' key */ \
ENTRY(GK_PERIOD) \
/** '/' key */ \
ENTRY(GK_SLASH) \
/*Numpad */ \
ENTRY(GK_NUMPAD0) \
ENTRY(GK_NUMPAD1) \
ENTRY(GK_NUMPAD2) \
ENTRY(GK_NUMPAD3) \
ENTRY(GK_NUMPAD4) \
ENTRY(GK_NUMPAD5) \
ENTRY(GK_NUMPAD6) \
ENTRY(GK_NUMPAD7) \
ENTRY(GK_NUMPAD8) \
ENTRY(GK_NUMPAD9) \
ENTRY(GK_MULTIPLY) \
ENTRY(GK_ADD) \
ENTRY(GK_SEPARATOR) \
ENTRY(GK_SUBTRACT) \
ENTRY(GK_DECIMAL) \
ENTRY(GK_DIVIDE) \
/* function keys */ \
ENTRY(GK_F1) \
ENTRY(GK_F2) \
ENTRY(GK_F3) \
ENTRY(GK_F4) \
ENTRY(GK_F5) \
ENTRY(GK_F6) \
ENTRY(GK_F7) \
ENTRY(GK_F8) \
ENTRY(GK_F9) \
ENTRY(GK_F10)

/** @brief Platform independent key identifiers
*/
enum in_event_key_type_e
{
#define ENTRY_ENUM(a) a,
#define ENTRY(a) a,
    KEYIDENTIFIER_TABLE
//#ifndef GUILIANI_GUILIANILIB
//    USER_KEYIDENTIFIER_TABLE
//#endif
#undef ENTRY
#undef ENTRY_ENUM

    GUILIANI_INTERNAL_GK = INT_MAX // Do not touch this. Required for ensuring consistent enum type size across libraries.
};

/**
    ux abstracts user input through the use of an internal platform-independent event system.
    The in_event class is the base class from which all specific events are derived. This ensures
    that every event which is delivered to the event handler has got an event type (EventType_t) and
    a get method (in_event::GetType) to access it, so that the event handler is able to safely
    identify and handle it.
*/
class in_event
{
public:
    /** The modifier keys that are supported by ux, but not necessarily
        by each in_device implementation.
      */
    enum Modifier_t
    {
        GKM_NONE    = 0x00000000, ///< No Modifier.
        GKM_SHIFT   = 0x00000001, ///< Shift key.
        GKM_CONTROL = 0x00000002, ///< Control key.
        GKM_ALT     = 0x00000004, ///< Alt key.
        GKM_META    = 0x00000008  ///< Additional modifier, depends on CGUIInputMedia implementation.
    };

    /** Constructs a in_event object.
        @param eType The type of event.
        @param uiModifiers Logical OR combination of Modifier_t values that
               represent the modifier keys that were pressed when the event
               occurred.
      */
    in_event(in_event_type_e eType, ux_uint uiModifiers)
        : m_eType(eType), m_uiModifiers(uiModifiers)
    {}

    /// Destructor.
    virtual ~in_event() {}

    /** Get the type of the event.
        @return The type of the derived event.
      */
    inline in_event_type_e GetType() const { return m_eType; }

    /** Returns the modifier keys that were pressed when the event occurred.
        @return Logical OR combination of Modifier_t values.
      */
    inline ux_uint GetModifiers() const { return m_uiModifiers; }

private:
    /// Do not allow default construction.
    in_event();

    /// Type of the event.
    in_event_type_e   m_eType;

    /// Modifiers of this event.
    ux_uint m_uiModifiers;
};

/// Represents platform-independent keyboard events.

/**
    Keyboard events expand the functionality of the event base class by adding methods which
    return the identifier (declared in GUIKeyIdentifier_t) and the character of the key which was pressed.
*/
class in_event_keyboard
    : public in_event
{
public:
    /** Constructs a keyboard event.
        @param eType The event type.
        @param uiKey Key 'value', usually the pressed key's character code.
        @param eKeyIdentifier The Guiliani identifier for special keys.
        @param uiModifiers Logical OR combination of in_event::Modifier_t values.
      */
    in_event_keyboard(in_event_type_e eType, ux_uint uiKey, in_event_key_type_e eKeyIdentifier, ux_uint uiModifiers = 0)
        : in_event(eType, uiModifiers), m_uiKey(uiKey), m_eKeyIdentifier(eKeyIdentifier)
    {
    }

    ~in_event_keyboard() {}

    /** Get the character code of the key that was pressed.
        @return the pressed key's character code.
      */
    inline ux_uint GetKeyContent() const { return m_uiKey; }

    /** Get the key that was pressed.
        @return the Guiliani special key identifier.
      */
    inline in_event_key_type_e GetKeyIdentifier() const { return m_eKeyIdentifier; }

private:
    /// Do not allow default construction.
    in_event_keyboard();

    /// The key that was pressed.
    ux_uint m_uiKey;

    /// Identifier for Guiliani special keys.
    in_event_key_type_e m_eKeyIdentifier;
};

/// Represents platform-independent mouse events.

/**
    Mouse events expand the functionality of the event base class by adding several methods for querying
    the mouse status (position, button status). Mouse events may be useful for handling several types
    of pointing devices, including touchpads.
*/
class in_event_mouse
    : public in_event
{
    /// Do not allow default construction.
    in_event_mouse();

    /// The coordinates of the mouse event.
    ux_int m_iX;
    ux_int m_iY;

    /// Indicates whether the left mouse button was pressed.
    ux_bool m_bLeftButtonPressed;
    /// Indicates whether the left mouse button was pressed.
    ux_bool m_bRightButtonPressed;

    // The mousewheel delta.
    ux_value m_vWheelDelta;

public:
    /** Constructs a mouse event.
        @param eType The type of mouse event (movement, click ...).
        @param iX The x position of the mouse pointer when the event occurred.
        @param iY The y position of the mouse pointer when the event occurred.
        @param bLeftButton Whether the left button is pressed down.
        @param bRightButton Whether the right button is pressed down.
        @param uiModifiers Logical OR combination of in_event::Modifier_t values.
        @param vWheelDelta The mousewheel delta.
      */
    in_event_mouse(in_event_type_e eType, ux_int iX=0, ux_int iY=0,
                   ux_bool bLeftButton=false, ux_bool bRightButton=false, ux_uint uiModifiers = 0,
                   ux_value vWheelDelta = 0)
        : in_event(eType, uiModifiers), m_iX(iX), m_iY(iY),
          m_bLeftButtonPressed(bLeftButton), m_bRightButtonPressed(bRightButton),
          m_vWheelDelta(vWheelDelta)
    {
    }

    ~in_event_mouse() {}
    /** Returns the x position of the mouse pointer when the event occurred.
        @return x position relative to Guiliani's root object.
      */
    inline ux_int GetX() const { return m_iX; }
    /** Returns the y position of the mouse pointer when the event occurred.
        @return y position relative to Guiliani's root object.
      */
    inline ux_int GetY() const { return m_iY; }

    /** Returns the wheel delta value when the event occurred.
        A positive value indicates that the wheel was rotated forward,
        away from the user; a negative value indicates that the wheel
        was rotated backward, toward the user.
        @return wheel delta.
      */
    inline ux_value GetWheelDelta() const { return m_vWheelDelta; }

    /** Returns whether the left mouse button was pressed when the event occurred.
        @return The left mouse button state.
      */
    inline ux_bool IsLeftButtonPressed () const { return m_bLeftButtonPressed; }

    /** Returns whether the right mouse button was pressed when the event occurred.
        @return The right mouse button state.
      */
    inline ux_bool IsRightButtonPressed () const { return m_bRightButtonPressed; }
};


#endif
