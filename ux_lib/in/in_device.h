#ifndef UX_IN_DEVICE_H
#define UX_IN_DEVICE_H

#include "ux_types_and_defs.h"
//#include <cassert>

class CGUI;
class CGUIObject;
class in_event;

/// Base-class for all input devices that are attachable to the GUI
class in_device
{
public:
    /** Retrieves a platform-specific event and creates an appropriate Guiliani event.
        The supplied parameter specifies the time in milliseconds until the next processing of
        animation timers or commands is required. An input media may use this to avoid unnecessary
        event polling. It MUST return from GetEvent after the specified time, though.
        @param uiIdleTime specifies the maximum idle time in ms.
        @return Pointer to a newly generated Guiliani event, or NULL if nothing happened.
    */
    virtual in_event * GetEvent(ux_uint uiIdleTime) = 0;

    /** Forces the input media to quit idling immediately.
        Called by the framework if the input media is requested to stop idling immediately.
        Classes derived from GUIInputMedia should implement StopIdle in such a way, that it interrupts any
        idling/sleeping code in GetEvent().
    */
    virtual void StopIdle() = 0;

protected:
    in_device()
    {
    };

    virtual ~in_device()
    {
    };

private:

    /** Copy-constructor.
        Dummy implementation to avoid unintended use of compiler-generated default    */
    in_device(const in_device & kSource);

    /** Operator= method.
        Dummy implementation to avoid unintended use of compiler-generated default    */
    in_device & operator=(const in_device & kSource);
};

#endif
