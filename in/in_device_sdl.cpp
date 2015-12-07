#include "in_device_sdl.h"

in_device_sdl::in_device_sdl()
{
    SDL_EnableUNICODE(1);
//    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
    SDL_EnableKeyRepeat(100, 10);
    m_pAdditionalEvent = NULL;

    //init SDL timer subsystem
    if (SDL_InitSubSystem(SDL_INIT_TIMER) != 0)
    {
        UX_TRACE_ERROR("CGUIInputSDL::CGUIInputSDL: SDL timer init failed: " + ux_string(SDL_GetError()) + "\n");
    }
}

in_device_sdl::~in_device_sdl()
{
}

void in_device_sdl::StopIdle()
{
    SDL_Event event;
    memset(&event, 0, sizeof(event));
    event.type = SDL_USEREVENT;
    SDL_PushEvent(&event);
}

// Callback function for AddTimer() which pushes an event to the message queue.
Uint32 SendEvent(Uint32 uiTime, void *pVoid)
{
    SDL_PushEvent((SDL_Event *) pVoid);
    return uiTime;
}

in_event *in_device_sdl::GetEvent(ux_uint uiIdleTime)
{
//
//TODO :
//This handler does NOT interpret all possible mouse actions,
//e.g. click while other button pressed
//
    SDL_Event CurrentSDLEvent;
    in_event *pEvent = NULL;

    ux::eC_Time64 kElapsedTime;
    SDL_TimerID pTimerID;
    ux_int iRes = 0;

    if (m_pAdditionalEvent)
    {
        in_event *pEv = m_pAdditionalEvent;
        m_pAdditionalEvent = NULL;
        return pEv;
    }

    // If the GUI wishes to idle, issue a timer to wake us up when the idle time expires and start waiting for events
    if (uiIdleTime > 0)
    {
        // Create timer to push an event to leave SDL_WaitEvent after end of IdleTime
        CurrentSDLEvent.type = SDL_NOEVENT;
        pTimerID = SDL_AddTimer(uiIdleTime, SendEvent, (void *) &CurrentSDLEvent);
        if (pTimerID != NULL)
        {
            // Wait for event
            iRes = SDL_WaitEvent(&CurrentSDLEvent);
            SDL_RemoveTimer(pTimerID);
        }
    }

    // Otherwise, if the GUI does not wish to idle, just peek into the message loop and continue immediately with the GUI.
    if (iRes == 0)
    {
        iRes = SDL_PollEvent(&CurrentSDLEvent);
    }

    if (iRes == 1)
    {
        //skip all consecutive SDL_MOUSEMOTION events and parse only the final one
        if (CurrentSDLEvent.type == SDL_MOUSEMOTION)
        {
            SDL_Event QueueEvent;
            while (SDL_PollEvent(&QueueEvent) != 0)
            {
                if (QueueEvent.type == SDL_MOUSEMOTION)
                {
                    //overwrite earlier mouse motion event with later one
                    memcpy(&CurrentSDLEvent, &QueueEvent, sizeof(SDL_Event));
                }
                else
                {
                    //there is a non-mousemove event - push it onto the queue and abort skipping
                    SDL_Event *pNewEvent = new SDL_Event;
                    memcpy(pNewEvent, &QueueEvent, sizeof(SDL_Event));
                    m_EventVector.push_back(pNewEvent);

                    //now that we've encountered a non-mousemove event we need to flush the queue to avoid accidental reordering!
                    while (SDL_PollEvent(&QueueEvent) != 0)
                    {
                        pNewEvent = new SDL_Event;
                        memcpy(pNewEvent, &QueueEvent, sizeof(SDL_Event));
                        m_EventVector.push_back(pNewEvent);
                    }
                    break;
                }
            }

            // NOTE/TODO:
            // Even though we do NOT have a way to lock the SDL queue again new incoming events while we remove
            // and re-add the non-mousemove events it seems to work as expected. This needs to be investigated.

            //re-append the queue
            for (std::vector<SDL_Event *>::iterator it = m_EventVector.begin(); it != m_EventVector.end(); ++it)
            {
                SDL_PushEvent(*it);
                delete *it;
            }
            m_EventVector.clear();
        }
    }

    // Check modifiers.
    ux_uint uiModifiers = 0;
    ux_ubyte *pubKeyState = SDL_GetKeyState(0);
    if (pubKeyState[SDLK_LSHIFT] || pubKeyState[SDLK_RSHIFT])
    {
        uiModifiers |= in_event::GKM_SHIFT;
    }
    if (pubKeyState[SDLK_LCTRL] || pubKeyState[SDLK_RCTRL])
    {
        uiModifiers |= in_event::GKM_CONTROL;
    }
    if (pubKeyState[SDLK_LALT] || pubKeyState[SDLK_RALT])
    {
        uiModifiers |= in_event::GKM_ALT;
    }
    if (pubKeyState[SDLK_LSUPER] || pubKeyState[SDLK_RSUPER])
    {
        //On PC Linux, the SDL maps the left and right "Windows" keys to "super", not "meta"
        uiModifiers |= in_event::GKM_META;
    }

    if (iRes == 1)
    {
        // check for input events
        switch (CurrentSDLEvent.type)
        {
            // --------------------------------------------------------------------
            //      Mouse Handling
            // --------------------------------------------------------------------

            case SDL_MOUSEBUTTONUP:
            {
                //GUILOG (GUI_TRACE_DEBUG, ux_string("SDL_PollEvent : Type: SDL_MOUSEBUTTONUP")  +  "\n");
                switch (CurrentSDLEvent.button.button)
                {
                    case SDL_BUTTON_LEFT:
                        pEvent = new in_event_mouse(ET_LBUTTONUP, CurrentSDLEvent.button.x, CurrentSDLEvent.button.y,
                                                    true, false, uiModifiers);
                        break;
                    case SDL_BUTTON_RIGHT:
                        pEvent = new in_event_mouse(ET_RBUTTONUP, CurrentSDLEvent.button.x, CurrentSDLEvent.button.y,
                                                    false, true, uiModifiers);
                        break;
                    case SDL_BUTTON_WHEELUP:
                        pEvent = new in_event_mouse(ET_MOUSEWHEEL, CurrentSDLEvent.button.x, CurrentSDLEvent.button.y,
                                                    false, false, uiModifiers, 1.f);
                        break;
                    case SDL_BUTTON_WHEELDOWN:
                        pEvent = new in_event_mouse(ET_MOUSEWHEEL, CurrentSDLEvent.button.x, CurrentSDLEvent.button.y,
                                                    false, false, uiModifiers, -1.f);
                        break;
                    default:
                        break;
                }
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                //GUILOG (GUI_TRACE_DEBUG, ux_string("SDL_PollEvent : Type: SDL_MOUSEBUTTONDOWN")  +  "\n");
                switch (CurrentSDLEvent.button.button)
                {
                    case SDL_BUTTON_LEFT:
                        pEvent = new in_event_mouse(ET_LBUTTONDOWN, CurrentSDLEvent.button.x, CurrentSDLEvent.button.y,
                                                    true, false, uiModifiers);
                        break;
                    case SDL_BUTTON_RIGHT:
                        pEvent = new in_event_mouse(ET_RBUTTONDOWN, CurrentSDLEvent.button.x, CurrentSDLEvent.button.y,
                                                    false, true, uiModifiers);
                        break;
                    default:
                        break;
                }
                break;
            }
            case SDL_MOUSEMOTION:
            {
                //GUILOG (GUI_TRACE_DEBUG, ux_string("SDL_PollEvent : Type: SDL_MOUSEMOTION")  +  "\n");

                switch (CurrentSDLEvent.motion.state)
                {
                    case SDL_BUTTON(SDL_BUTTON_LEFT):
                        pEvent = new in_event_mouse(ET_MOUSEMOVE, CurrentSDLEvent.motion.x, CurrentSDLEvent.motion.y,
                                                    true, false, uiModifiers);
                        break;
                    case SDL_BUTTON(SDL_BUTTON_RIGHT):
                        pEvent = new in_event_mouse(ET_MOUSEMOVE, CurrentSDLEvent.motion.x, CurrentSDLEvent.motion.y,
                                                    false, true, uiModifiers);
                        break;
                    default:
                        pEvent = new in_event_mouse(ET_MOUSEMOVE, CurrentSDLEvent.motion.x, CurrentSDLEvent.motion.y,
                                                    false, false, uiModifiers);
                        break;
                }
                break;
            }
                // -----------------------------------------------------------------
                //      Keyboard Handling
                // -----------------------------------------------------------------
            case SDL_KEYDOWN:
            {
                //GUILOG (GUI_TRACE_DEBUG, ux_string("SDL_PollEvent : Type: SDL_KEYDOWN")  +  "\n");
                ux_bool bIsChar;
                // Get GK_ key.
                in_event_key_type_e eKeyID = GetKeyID(CurrentSDLEvent.key.keysym, uiModifiers, bIsChar);
                // Throw KeyDown event.
                pEvent = new in_event_keyboard(ET_KEYDOWN, CurrentSDLEvent.key.keysym.unicode, eKeyID, uiModifiers);
                if (bIsChar == true)
                {
                    // Pressed key is a printable character, throw a DoChar
                    // event additionally.
                    m_pAdditionalEvent = new in_event_keyboard(ET_CHAR, CurrentSDLEvent.key.keysym.unicode, GK_NONE,
                                                               uiModifiers);
                }
                break;
            }
            case SDL_KEYUP:
            {
                //GUILOG (GUI_TRACE_DEBUG, ux_string("SDL_PollEvent : Type: SDL_KEYUP")  +  "\n");
                ux_bool bIsChar;
                // Get GK_ key.
                in_event_key_type_e eKeyID = GetKeyID(CurrentSDLEvent.key.keysym, uiModifiers, bIsChar);
                // Throw KeyUp event.
                pEvent = new in_event_keyboard(ET_KEYUP, CurrentSDLEvent.key.keysym.unicode, eKeyID, uiModifiers);
                break;
            }
                // -----------------------------------------------------------------
                //      Application Handling
                // -----------------------------------------------------------------
            case SDL_QUIT:
            {
                //GUILOG (GUI_TRACE_DEBUG, ux_string("SDL_PollEvent : Type: SDL_QUIT")  +  "\n");
//                GETGUI.Quit();
                break;
            }
            case SDL_VIDEORESIZE:
            {
                //GUILOG (GUI_TRACE_DEBUG, ux_string("SDL_PollEvent : Type: SDL_VIDEORESIZE: w = ")  + ux_string(CurrentSDLEvent.resize.w) + " h = " + ux_string(CurrentSDLEvent.resize.h) + "\n");
//                GETGUI.SetWidth(eC_FromInt(CurrentSDLEvent.resize.w));
//                GETGUI.SetHeight(eC_FromInt(CurrentSDLEvent.resize.h));
//                GETGFX.set_screen_size(CurrentSDLEvent.resize.w,CurrentSDLEvent.resize.h);
//                GETGUI.InvalidateArea();
                break;
            }
            case SDL_VIDEOEXPOSE://happens e.g. when window is alt-tabbed back into visibility
            {
                //GUILOG (GUI_TRACE_DEBUG, ux_string("SDL_PollEvent : Type: SDL_VIDEOEXPOSE")  +  "\n");
//                GETGUI.InvalidateArea();
                break;
            }
                // unused events begin here
            case SDL_ACTIVEEVENT:
            {
                //GUILOG (GUI_TRACE_DEBUG, ux_string("SDL_PollEvent : Type: SDL_ACTIVEEVENT")  +  "\n");
                break;
            }
            case SDL_SYSWMEVENT:
            {
                //GUILOG (GUI_TRACE_DEBUG, ux_string("SDL_PollEvent : Type: SDL_SYSWMEVENT")  +  "\n");
                break;
            }
            case SDL_NOEVENT:
            {
                //GUILOG (GUI_TRACE_DEBUG, ux_string("SDL_PollEvent : Type: SDL_NOEVENT   ")  +  "\n");
                break;
            }
                //unused events end here
            default:
            {
                //GUILOG (GUI_TRACE_DEBUG, ux_string("SDL_PollEvent : Type = ") + ux_string(CurrentSDLEvent.type) + "\n");
                break;
            }
        }//switch (CurrentSDLEvent.type)
    }//if (SDL_PollEvent(pSDLEvent))

    return pEvent;
}

in_event_key_type_e in_device_sdl::GetKeyID(const SDL_keysym &eSdlKeySym, ux_uint uiModifiers, ux_bool &rbIsChar)
{
    in_event_key_type_e eKey = GK_NONE;
    rbIsChar = true;

    // GK_A ... GK_Z
    if (eSdlKeySym.sym >= SDLK_a && eSdlKeySym.sym <= SDLK_z)
    {
        eKey = static_cast<in_event_key_type_e>(GK_A + eSdlKeySym.sym - SDLK_a);

        if (uiModifiers == in_event::GKM_CONTROL)
        {
            switch (eKey)
            {
                case GK_A:
                    eKey = GK_SELECT_ALL;
                    break;
                case GK_C:
                    eKey = GK_COPY;
                    break;
                case GK_V:
                    eKey = GK_PASTE;
                    break;
                case GK_X:
                    eKey = GK_CUT;
                    break;
                default:
                    break;
            }
        }
    }
        // GK_0 ... GK_9
    else if (eSdlKeySym.sym >= SDLK_0 && eSdlKeySym.sym <= SDLK_9)
    {
        eKey = static_cast<in_event_key_type_e>(GK_0 + eSdlKeySym.sym - SDLK_0);
    }
        // GK_F1 ... GK_F10
    else if (eSdlKeySym.sym >= SDLK_F1 && eSdlKeySym.sym <= SDLK_F10)
    {
        eKey = static_cast<in_event_key_type_e>(GK_F1 + eSdlKeySym.sym - SDLK_F1);
    }
        // GK_SPACE ... GK_DOLLAR
    else if (eSdlKeySym.sym >= SDLK_SPACE && eSdlKeySym.sym <= SDLK_DOLLAR)
    {
        eKey = static_cast<in_event_key_type_e>(
            GK_SPACE + eSdlKeySym.sym - SDLK_SPACE);
    }
        // GK_AMPERSAND ... GK_SLASH
    else if (eSdlKeySym.sym >= SDLK_AMPERSAND && eSdlKeySym.sym <= SDLK_SLASH)
    {
        eKey = static_cast<in_event_key_type_e>(
            GK_AMPERSAND + eSdlKeySym.sym - SDLK_AMPERSAND);
    }
    else
    {
        rbIsChar = false;
        switch (eSdlKeySym.sym)
        {
            case SDLK_ESCAPE:
                eKey = GK_ESCAPE;
                break;
            case SDLK_UP:
                eKey = GK_UP;
                break;
            case SDLK_DOWN:
                eKey = GK_DOWN;
                break;
            case SDLK_LEFT:
                eKey = GK_LEFT;
                break;
            case SDLK_RIGHT:
                eKey = GK_RIGHT;
                break;
            case SDLK_RETURN:
                eKey = GK_ACTION;
                break;
            case SDLK_BACKSPACE:
                eKey = GK_BACKSPACE;
                break;
            case SDLK_PAGEUP:
                eKey = GK_PAGEUP;
                break;
            case SDLK_PAGEDOWN:
                eKey = GK_PAGEDOWN;
                break;
            case SDLK_TAB:
                eKey = GK_TAB;
                break;
            case SDLK_HOME:
                eKey = GK_HOME;
                break;
            case SDLK_END:
                eKey = GK_END;
                break;
            case SDLK_DELETE:
                eKey = GK_DELETE;
                break;
            default:
                rbIsChar = true;
                break;
        }
    }
    return eKey;
}
