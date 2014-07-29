#include "FacebookAgent.h"
#include "AgentManager.h"
#include "PluginJniHelper.h"

namespace cocos2d{namespace plugin{

extern "C" {
JNIEXPORT void JNICALL Java_org_cocos2dx_plugin_UserFacebook_nativeRequestCallback(JNIEnv*  env, jobject thiz, jint ret, jstring msg, jobject response, jint cbIndex)
{
	std::string stdMsg = PluginJniHelper::jstring2string(msg);
	FacebookAgent::FBInfo stdResponse = PluginJniHelper::JSONObject2Map(response);
	FacebookAgent::FBCallback callback = FacebookAgent::getInstance()->getRequestCallback(cbIndex);
	callback(ret, stdMsg, stdResponse);
}
}

static FacebookAgent* s_sharedFacebookAgent = nullptr;

FacebookAgent* FacebookAgent::getInstance()
{
	if(nullptr == s_sharedFacebookAgent)
	{
		s_sharedFacebookAgent = new FacebookAgent();
	}
	return s_sharedFacebookAgent;
}

void FacebookAgent::destroyInstance()
{
	if(s_sharedFacebookAgent)
	{
		delete s_sharedFacebookAgent;
		s_sharedFacebookAgent = nullptr;
	}
}

FacebookAgent::FacebookAgent()
{
	agentManager = AgentManager::getInstance();
}

FacebookAgent::~FacebookAgent()
{
	AgentManager::destroyInstance();
}

void FacebookAgent::login(FBCallback cb)
{
	agentManager->getUserPlugin()->login(cb);
}

void FacebookAgent::logout(FBCallback cb)
{
	agentManager->getUserPlugin()->logout(cb);
}

std::string FacebookAgent::getAccessToken()
{
	return agentManager->getUserPlugin()->callStringFuncWithParam("getAccessToken", NULL);
}

void FacebookAgent::share(FBInfo& info, FBCallback cb)
{
	agentManager->getSharePlugin()->share(info, cb);
}

void FacebookAgent::dialog(FBInfo& info, FBCallback cb)
{
	auto sharePlugin = agentManager->getSharePlugin();
	sharePlugin->setCallback(cb);
	PluginParam params(info);
	sharePlugin->callFuncWithParam("dialog", &params, NULL);
}

void FacebookAgent::request(std::string &path, int method, FBInfo &params, FBCallback cb)
{
	requestCallbacks.push_back(cb);

	PluginParam _path(path.c_str());
	PluginParam _method(method);
	PluginParam _params(params);
	PluginParam _cbIndex((int)(requestCallbacks.size() - 1));

	agentManager->getUserPlugin()->callFuncWithParam("request", &_path, &_method, &_params, &_cbIndex, NULL);
}

FacebookAgent::FBCallback FacebookAgent::getRequestCallback(int index)
{
	return requestCallbacks[index];
}
}}
