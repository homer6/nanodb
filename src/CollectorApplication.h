#pragma once

#include "Application.h"

class CollectorApplication : public Application{

	public:
		virtual bool onBeforeListen();
		virtual bool onAfterAccept();
		virtual bool onRead();
		virtual bool onBeforeClose();
		virtual bool onAfterClose();

};