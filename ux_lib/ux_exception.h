//
// Created by Bernd Lappas on 30.11.15.
//

#ifndef UX_UX_EXCEPTION_H
#define UX_UX_EXCEPTION_H

/// This is the base class for ux exceptions.

class ux_exception
{
public:
    /**
        Standard constructor.
    */
    ux_exception();

    /**
        Destructor.
    */
    virtual ~ux_exception();

    /**
        Report the message of the exception.
        @return The message of the exception.
    */
    virtual std::string report() const;
};

#endif //UX_UX_EXCEPTION_H
