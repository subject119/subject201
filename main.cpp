#include "main.h"
#include "../Classes/AppDelegate.h"
#include "CCEGLView.h"
#include "CCApplication.h"

USING_NS_CC;

ref class CCApplicationFrameworkViewSource sealed : Windows::ApplicationModel::Core::IFrameworkViewSource 
{
public:
	virtual Windows::ApplicationModel::Core::IFrameworkView^ CreateView()
    {
        return cocos2d::getSharedCCApplicationFrameworkView();
    }
};

[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
    // create the application instance
    /*AppDelegate app;
    CCEGLView eglView;
    eglView.init("Gemini",900,640);
    return CCApplication::getInstance()->run();*/

	AppDelegate App;
	auto frameworkViewSource = ref new CCApplicationFrameworkViewSource();
	Windows::ApplicationModel::Core::CoreApplication::Run(frameworkViewSource);
	
    return 0;
}
