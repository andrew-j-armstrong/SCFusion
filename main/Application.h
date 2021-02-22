#ifndef _APPLICATION_H_
#define _APPLICATION_H_

class MyApp : public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();
};

#endif
