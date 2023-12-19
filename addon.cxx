/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*************************************************************************
 *
 *  The Contents of this file are made available subject to the terms of
 *  the BSD license.
 *
 *  Copyright 2000, 2010 Oracle and/or its affiliates.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of Sun Microsystems, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 *  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 *  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 *  USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *************************************************************************/

#include <addon.hxx>
#include <com/sun/star/beans/PropertyValue.hpp>
#include <com/sun/star/frame/XFrame.hpp>
#include <com/sun/star/frame/XController.hpp>
#include <com/sun/star/awt/Toolkit.hpp>
#include <com/sun/star/awt/XWindowPeer.hpp>
#include <com/sun/star/awt/WindowAttribute.hpp>
#include <com/sun/star/awt/XMessageBox.hpp>
#include <com/sun/star/uno/XComponentContext.hpp>
#include <cppuhelper/supportsservice.hxx>
#include <rtl/ustring.hxx>

#include <Debug.h>

using rtl::OUString;
using namespace com::sun::star::uno;
using namespace com::sun::star::frame;
using namespace com::sun::star::awt;
using com::sun::star::beans::PropertyValue;
using com::sun::star::util::URL;

// This is the service name an Add-On has to implement
#define SERVICE_NAME "com.sun.star.frame.ProtocolHandler"


/**
  * Show a message box with the UNO based toolkit
  */
static void ShowMessageBox( const Reference< XToolkit >& rToolkit, const Reference< XFrame >& rFrame, const OUString& aTitle, const OUString& aMsgText )
{
    if ( rFrame.is() && rToolkit.is() )
    {
        // describe window properties.
        WindowDescriptor                aDescriptor;
        aDescriptor.Type              = WindowClass_MODALTOP;
        aDescriptor.WindowServiceName = "infobox";
        aDescriptor.ParentIndex       = -1;
        aDescriptor.Parent            = Reference< XWindowPeer >( rFrame->getContainerWindow(), UNO_QUERY );
        aDescriptor.Bounds            = Rectangle(0,0,300,200);
        aDescriptor.WindowAttributes  = WindowAttribute::BORDER |
WindowAttribute::MOVEABLE |
WindowAttribute::CLOSEABLE;

        Reference< XWindowPeer > xPeer = rToolkit->createWindow( aDescriptor );
        if ( xPeer.is() )
        {
            Reference< XMessageBox > xMsgBox( xPeer, UNO_QUERY );
            if ( xMsgBox.is() )
            {
                xMsgBox->setCaptionText( aTitle );
                xMsgBox->setMessageText( aMsgText );
                xMsgBox->execute();
            }
        }
    }
}

/**
  * Called by the Office framework.
  * One-time initialization. We have to store the context information
  * given, like the frame we are bound to, into our members.
  */
void SAL_CALL Addon::initialize( const Sequence< Any >& aArguments )
{

    Reference < XFrame > xFrame;
    if ( aArguments.getLength() )
    {
        aArguments[0] >>= xFrame;
        mxFrame = xFrame;
    }
    _xRegistration = css::uno::Reference<css::frame::XDispatchProviderInterception>(mxFrame, css::uno::UNO_QUERY);
    _xRegistration->registerDispatchProviderInterceptor(this);
    // Create the toolkit to have access to it later
    mxToolkit = Reference< XToolkit >( Toolkit::create(mxContext), UNO_QUERY_THROW );
}

/**
  * Called by the Office framework.
  * We are ask to query the given URL and return a dispatch object if the URL
  * contains an Add-On command.
  */
Reference< XDispatch > SAL_CALL Addon::queryDispatch( const URL& aURL, const ::rtl::OUString& sTargetFrameName, sal_Int32 SearchFlags )
{
    //HERE i`m expecting that all URLs will be written by DEBUG_TOOLS::debug_log !!!
    DEBUG_TOOLS::debug_log(rtl::OUStringToOString(aURL.Complete,    RTL_TEXTENCODING_UTF8).getStr(),
                           rtl::OUStringToOString(sTargetFrameName, RTL_TEXTENCODING_UTF8).getStr());

    if ( aURL.Protocol.equalsAscii("org.openoffice.Office.addon.example:") )
    {
        if ( aURL.Path.equalsAscii( "Function1" ) )
            return this;
        else if ( aURL.Path.equalsAscii( "Function2" ) )
            return this;
        else if ( aURL.Path.equalsAscii( "Help" ) )
            return this;
    }
    else if (getSlaveDispatchProvider().is())
    {
        return getSlaveDispatchProvider()->queryDispatch(aURL, sTargetFrameName, SearchFlags);
    }
    return css::uno::Reference<css::frame::XDispatch>();
}

/**
  * Called by the Office framework.
  * We are ask to execute the given Add-On command URL.
  */
void SAL_CALL Addon::dispatch( const URL& aURL, const Sequence < PropertyValue >& lArgs )
{
    if ( aURL.Protocol.equalsAscii("org.openoffice.Office.addon.example:") )
    {
        if ( aURL.Path.equalsAscii( "Function1" ) )
        {
            ShowMessageBox( mxToolkit, mxFrame,
                            OUString( "SDK Add-On example" ),
                            OUString( "Function 1 activated" ) );
        }
        else if ( aURL.Path.equalsAscii( "Function2" ) )
        {
            ShowMessageBox( mxToolkit, mxFrame,
                            OUString( "SDK Add-On example" ),
                            OUString( "Function 2 activated" ) );
        }
        else if ( aURL.Path.equalsAscii( "Help" ) )
        {
            // Show info box
            ShowMessageBox( mxToolkit, mxFrame,
                            OUString( "About SDK Add-On example" ),
                            OUString( "This is the SDK Add-On example" ) );
    }
    }
}

/**
  * Called by the Office framework.
  * We are ask to query the given sequence of URLs and return dispatch objects if the URLs
  * contain Add-On commands.
  */
Sequence < Reference< XDispatch > > SAL_CALL Addon::queryDispatches( const Sequence < DispatchDescriptor >& seqDescripts )
{
    //sal_Int32 nCount = seqDescripts.getLength();
    //Sequence < Reference < XDispatch > > lDispatcher( nCount );
    //
    //for( sal_Int32 i=0; i<nCount; ++i )
    //    lDispatcher[i] = queryDispatch( seqDescripts[i].FeatureURL, seqDescripts[i].FrameName, //seqDescripts[i].SearchFlags );
    //
    //return lDispatcher;
    return getSlaveDispatchProvider()->queryDispatches(seqDescripts);
}

/**
  * Called by the Office framework.
  * We are ask to query the given sequence of URLs and return dispatch objects if the URLs
  * contain Add-On commands.
  */
void SAL_CALL Addon::addStatusListener( const Reference< XStatusListener >& xControl, const URL& aURL )
{
}

/**
  * Called by the Office framework.
  * We are ask to query the given sequence of URLs and return dispatch objects if the URLs
  * contain Add-On commands.
  */
void SAL_CALL Addon::removeStatusListener( const Reference< XStatusListener >& xControl, const URL& aURL )
{
}

// Helper functions for the implementation of UNO component interfaces.
OUString Addon_getImplementationName()
{
    return OUString ( IMPLEMENTATION_NAME );
}

Sequence< ::rtl::OUString > SAL_CALL Addon_getSupportedServiceNames()
{
    Sequence < ::rtl::OUString > aRet(1);
    ::rtl::OUString* pArray = aRet.getArray();
    pArray[0] =  OUString ( SERVICE_NAME );
    return aRet;
}

Reference< XInterface > SAL_CALL Addon_createInstance( const Reference< XComponentContext > & rContext)
{
    return (cppu::OWeakObject*) new Addon( rContext );
}

// Implementation of the recommended/mandatory interfaces of a UNO component.
// XServiceInfo
::rtl::OUString SAL_CALL Addon::getImplementationName(  )
{
    return Addon_getImplementationName();
}

sal_Bool SAL_CALL Addon::supportsService( const ::rtl::OUString& rServiceName )
{
    return cppu::supportsService(this, rServiceName);
}

Sequence< ::rtl::OUString > SAL_CALL Addon::getSupportedServiceNames(  )
{
    return Addon_getSupportedServiceNames();
}

// XDispatchProviderInterceptor
css::uno::Sequence<rtl::OUString> Addon::getInterceptedURLs() { return _interceptedURLs; }

void Addon::setMasterDispatchProvider(const css::uno::Reference<css::frame::XDispatchProvider>& xNewSupplier)
{
    m_xMaster = xNewSupplier;
}

css::uno::Reference<css::frame::XDispatchProvider> Addon::getMasterDispatchProvider()
{
    return m_xMaster;
}

void Addon::setSlaveDispatchProvider(const css::uno::Reference<css::frame::XDispatchProvider>& xNewSupplier)
{
    m_xSlave = xNewSupplier;
}

css::uno::Reference<css::frame::XDispatchProvider> Addon::getSlaveDispatchProvider()
{
    return m_xSlave;
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
