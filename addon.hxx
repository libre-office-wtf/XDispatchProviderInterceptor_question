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

#ifndef INCLUDED_EXAMPLES_COMPONENTS_ADDONS_PROTOCOLHANDLERADDON_CPP_ADDON_HXX
#define INCLUDED_EXAMPLES_COMPONENTS_ADDONS_PROTOCOLHANDLERADDON_CPP_ADDON_HXX

#include <com/sun/star/lang/XInitialization.hpp>
#include <com/sun/star/lang/XServiceInfo.hpp>
#include <cppuhelper/implbase5.hxx>

#include <com/sun/star/frame/XDispatchProvider.hpp>
#include <com/sun/star/frame/XDispatchProviderInterceptor.hpp>
#include <com/sun/star/frame/XDispatchProviderInterception.hpp>
#include <com/sun/star/bridge/XUnoUrlResolver.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XMultiComponentFactory.hpp>
#include <com/sun/star/frame/XDesktop.hpp>
#include <com/sun/star/frame/XInterceptorInfo.hpp>
#include <com/sun/star/util/URLTransformer.hpp>

#define IMPLEMENTATION_NAME "org.openoffice.Office.addon.example"

namespace com
{
    namespace sun
    {
        namespace star
        {
            namespace frame
            {
                class XFrame;
            }
            namespace awt
            {
                class XToolkit;
            }
            namespace uno
            {
                class XComponentContext;
            }
        }
    }
}

class Addon : public cppu::WeakImplHelper5
    <
    css::frame::XDispatch,
    css::frame::XDispatchProviderInterceptor,
    css::frame::XInterceptorInfo,
    css::lang::XInitialization,
    css::lang::XServiceInfo>
{
private:
    ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > mxContext;
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XFrame > mxFrame;
    ::com::sun::star::uno::Reference< ::com::sun::star::awt::XToolkit > mxToolkit;
    css::uno::Reference<css::frame::XDispatchProviderInterception> _xRegistration;
    /// <summary> Список перехватываемых URL </summary>
    css::uno::Sequence<rtl::OUString> _interceptedURLs;

    /// <summary> Предыдущий DispatchProvider в XDispatchProviderInterceptor </summary>
    css::uno::Reference<css::frame::XDispatchProvider> m_xMaster;

    /// <summary> Следующий DispatchProvider в XDispatchProviderInterceptor </summary>
    css::uno::Reference<css::frame::XDispatchProvider> m_xSlave;

public:
    Addon( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > &rxContext)
        : mxContext( rxContext ) {    }

    // XDispatchProvider
    virtual ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch >
            SAL_CALL queryDispatch( const ::com::sun::star::util::URL& aURL,
                const ::rtl::OUString& sTargetFrameName, sal_Int32 nSearchFlags );
    virtual ::com::sun::star::uno::Sequence < ::com::sun::star::uno::Reference< ::com::sun::star::frame::XDispatch > >
        SAL_CALL queryDispatches(
            const ::com::sun::star::uno::Sequence < ::com::sun::star::frame::DispatchDescriptor >& seqDescriptor );

    // XDispatch
    virtual void SAL_CALL dispatch( const ::com::sun::star::util::URL& aURL,
        const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue >& lArgs );
    virtual void SAL_CALL addStatusListener( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener >& xControl,
        const ::com::sun::star::util::URL& aURL );
    virtual void SAL_CALL removeStatusListener( const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XStatusListener >& xControl,
        const ::com::sun::star::util::URL& aURL );

    // XInitialization
    virtual void SAL_CALL initialize( const ::com::sun::star::uno::Sequence< ::com::sun::star::uno::Any >& aArguments );

    // XServiceInfo
    virtual ::rtl::OUString SAL_CALL getImplementationName(  );
    virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName );
    virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames(  );

    // frame::XInterceptorInfo
    virtual css::uno::Sequence<rtl::OUString> SAL_CALL getInterceptedURLs() override;

    // frame::XDispatchProviderInterceptor
    virtual void SAL_CALL setMasterDispatchProvider(const css::uno::Reference<css::frame::XDispatchProvider>& xNewSupplier) override;
    virtual css::uno::Reference<css::frame::XDispatchProvider> SAL_CALL getMasterDispatchProvider() override;
    virtual void SAL_CALL setSlaveDispatchProvider(const css::uno::Reference<css::frame::XDispatchProvider>& xNewSupplier) override;
    virtual css::uno::Reference<css::frame::XDispatchProvider> SAL_CALL getSlaveDispatchProvider() override;
};

::rtl::OUString Addon_getImplementationName();

sal_Bool SAL_CALL Addon_supportsService( const ::rtl::OUString& ServiceName );

::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL Addon_getSupportedServiceNames(  );

::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface >
SAL_CALL Addon_createInstance( const ::com::sun::star::uno::Reference< ::com::sun::star::uno::XComponentContext > & rContext);

#endif // INCLUDED_EXAMPLES_COMPONENTS_ADDONS_PROTOCOLHANDLERADDON_CPP_ADDON_HXX

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
