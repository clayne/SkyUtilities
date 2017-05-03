#include "RequestManagerBase.h"

#include "Plugin.h"

namespace SKU { namespace Net {
	
	bool RequestManagerBase::AddRequest(Request::Ptr request, bool proccess_immediately)
	{
		try
		{
			pool.Get().insert(request);
		} 
		catch (std::exception e)
		{
			return false;
		}

		if(proccess_immediately)
			Start();

		return true;
	}

	void RequestManagerBase::RemoveRequest(Request::Ptr request)
	{
		try
		{
			pool.Get().erase(request);
		}
		catch (std::exception){}
	}

}}
