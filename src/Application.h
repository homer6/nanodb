#pragma once


class Application{

	public:
		virtual void setClientFd( int client_fd );

		virtual bool onBeforeListen(){ return true; };
		virtual bool onAfterAccept(){ return true; };
		virtual bool onRead(){ return true; };
		virtual bool onBeforeClose(){ return true; };
		virtual bool onAfterClose(){ return true; };


	private:
		int client_fd = 0;

};