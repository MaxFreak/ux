#ifndef IN_DEVICE_SDL_H
#define IN_DEVICE_SDL_H

#include <vector>

#include "in_device.h"
#include "in_event.h"
#include "SDL.h"

/// The implementation of the Input Wrapper for Linux.
class in_device_sdl
    : public in_device
{

public:
    /**
    Gets an event from the system.
    @param uiIdleTime Maximum idle time after which the method returns.
    @return Pointer to the event object.
    */
    in_event * GetEvent(ux_uint uiIdleTime);

    void StopIdle();

    in_device_sdl ();
    ~in_device_sdl ();

protected:
    /** Solves a pressed or released SDL keyboard key to GK_-key values.
        @param eSdlKeySym The SDL Keysym to solve value, i.e. SDLK_ value.
        @param uiModifiers Is a modifier key pressed (CTRL, ALT, etc.)
        @param rbIsChar [out] Is set true by this routine in case of the
         input SDL key is a printable character, e.g. alphanumeric.
        @return The GK_-key that is mapped to the SDL-key, or GK_NONE, if
         the key is not mapped to a specific GK_-key.
     */
    in_event_key_type_e GetKeyID(const SDL_keysym& eSdlKeySym,
        ux_uint uiModifiers,
        ux_bool & rbIsChar);

private:
    std::vector<SDL_Event*>    m_EventVector;
    in_event *  m_pAdditionalEvent;
};

#endif
