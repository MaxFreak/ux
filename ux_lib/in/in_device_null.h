#ifndef IN_DEVICE_NULL_H
#define IN_DEVICE_NULL_H

#include "in_device.h"

/// This is the Null Input-Wrapper class.

/**
    It implements a null Input-Wrapper for Guiliani testing.
*/
class in_device_null:
    public in_device
{
public:
    inline in_event * GetEvent(ux_uint uiIdleTime) {return NULL;}

    virtual void StopIdle() {};

private:
    /** Constructor. */
    in_device_null()
    {
        UX_TRACE_WARNING("----------------------\n");
        UX_TRACE_WARNING("|  in_device_null chosen  |\n");
        UX_TRACE_WARNING("----------------------\n");
    }

    /** Copy-constructor.
        Dummy implementation to avoid unintended use of compiler-generated default    */
    in_device_null(const in_device_null & kSource);

    /** Operator= method.
        Dummy implementation to avoid unintended use of compiler-generated default    */
    in_device_null & operator=(const in_device_null & kSource);
};

#endif
