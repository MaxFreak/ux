//
// Created by Bernd Lappas on 30.11.15.
//

#include <string>

#include "ux_exception.h"

ux_exception::ux_exception(void)
{
}

ux_exception::~ux_exception()
{
}

std::string ux_exception::report(void) const
{
    return std::string("A ux_exception is thrown: ");
}
