//
// Created by Bernd Lappas on 29.11.15.
//

#ifndef UX_GP_WRAP_OGL_H
#define UX_GP_WRAP_OGL_H

#include "gp_wrap_gl.h"

class gp_wrap_ogl : public gp_wrap_gl
{
public:
    virtual ux_bool Refresh(const refresh_type eRefreshCallType);

protected:
    /** Constructor
        Protected because of the singleton pattern.
    */
    gp_wrap_ogl();

    /** Destructor
        Protected because of the singleton pattern.
    */
    virtual ~gp_wrap_ogl();

    /** init
        Intended to be executed in
        user specific OS implementations of the OGLwrapper.
        @return True if OK, False otherwise
    */
    ux_bool init();

    /** de_init
        Intended to be executed in
        user specific OS implementations of the OGLwrapper.
    */
    void de_init();

    ux_bool m_bFBOSupported;    ///< Indicates if FrameBufferObjects are supported on this machine

//    virtual CGUIBitmapPlane* CreateGfxDependentBitmap(
//        GUIBitmapPlaneID_t uiNewBitmapID,
//        const CGUIObject& rkParentObject,
//        const ux_value &width, const ux_value &vHeight);

private:
    /** Copy-constructor.
        Dummy implementation to avoid unintended use of compiler-generated default
    */
    gp_wrap_ogl(const gp_wrap_ogl &kSource);

    /** Operator= method.
        Dummy implementation to avoid unintended use of compiler-generated default
    */
    gp_wrap_ogl &operator=(const gp_wrap_ogl &kSource);
};


#endif //UX_GP_WRAP_OGL_H
