#include "Plugin.h"
#include "NetInterface.h"
#include "Request.h"
#include "HTTPRequestProtocolContext.h"
#include "HTTPRequestManager.h"

#include <skse/PapyrusNativeFunctions.h>

#include <exception>

namespace SKU { namespace Net { // TODO: Consider writing class with control management (Start, Stop, ..) 
								//   Process flow?
								// TODO: Find a way to dynamically load all needed DLLs (CURL, openssl, ..)

	Interface::Interface() 
		: stopped(false)
	{}

	Interface::~Interface() 
	{
		Stop();
	}

	void Interface::Stop()
	{
		if (stopped == true)
			return;

		HTTP::RequestManager::GetInstance()->Stop();
		stopped = true;

		Plugin::Log(LOGL_VERBOSE, "Net: Stopped.");
	}

	long Interface::HTTPGETRequest(StaticFunctionTag*, BSFixedString url, long timeout)
	{
		if (GetInstance()->stopped == true)
			return Request::sFailed;

		return Interface::HTTPRequest(HTTP::RequestProtocolContext::mGET, std::string(url.data), "", timeout);
	}

	long Interface::HTTPPOSTRequest(StaticFunctionTag*, BSFixedString url, BSFixedString body, long timeout)
	{
		if (GetInstance()->stopped == true)
			return Request::sFailed;

		return Interface::HTTPRequest(HTTP::RequestProtocolContext::mGET, std::string(url.data), std::string(body.data), timeout);
	}

	long Interface::HTTPRequest(HTTP::RequestProtocolContext::Method method, std::string url, std::string body, long timeout)
	{
		if (Plugin::GetInstance()->IsGameReady() == false)	// Note: Just a precaution. Shouldn't happen: If the script was able to call a function
			return Request::sFailed;						// (well, this one in this case) SKSE should've sent a message notification and thus 
															// the method should always return true.

		try
		{
			Request::Ptr request = Request::Create<HTTP::RequestProtocolContext>();

			request->SetTimeout(timeout);
			request->GetProtocolContext<HTTP::RequestProtocolContext>()->Initialize(method, url, body);

			if (true == HTTP::RequestManager::GetInstance()->AddRequest(request, true))
			{
				return request->GetID();
			}
		}
		catch (std::bad_exception)
		{
			Plugin::Log(LOGL_INFO, "Stopping Plugin");
			Plugin::GetInstance()->Stop();
		}
		catch (std::exception) {}

		return Request::sFailed;
	}

	void Interface::OnSKSERegisterPapyrusFunctions(VMClassRegistry *registry)
	{
		registry->RegisterFunction(new NativeFunction2<StaticFunctionTag, long, BSFixedString,					long>("HTTPGETRequest", "SKUNet", HTTPGETRequest, registry));
		registry->RegisterFunction(new NativeFunction3<StaticFunctionTag, long, BSFixedString, BSFixedString,	long>("HTTPPOSTRequest", "SKUNet", HTTPPOSTRequest, registry));

		Plugin::Log(LOGL_VERBOSE, "Net: Registered Papyrus functions.");
	}
}}
