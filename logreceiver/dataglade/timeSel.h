#ifndef TIMESEL_H
#define TIMESEL_H

#include <gtkmm.h>

class TimeSel
{
public:
	TimeSel(Glib::RefPtr<Gtk::Builder> builder);

	void runTimeSel(void (*callback)(double), double time);
	void runTempSel(void (*callback)(double), double temp);

private:
	Glib::RefPtr<Gtk::Builder> builder;
	Gtk::Window* pwndTimeSel;
	Gtk::Button* pbtnTimeSelOK;
	Gtk::Button* pbtnTimeSelCancel;
	Gtk::Label*  plblTimeSelTime;

	void on_click_OK();       // ok click
	void on_click_Cancel();
	void (*callback)(double); // callback when closing

	bool timeSet; // else temperature set
	unsigned int numbers[4];
	unsigned int setnumber;



	void figbtnCallback(unsigned int btnID);

};



#endif
