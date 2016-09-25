#include "timeSel.h"
#include <sstream>

#include <iostream>



TimeSel::TimeSel(Glib::RefPtr<Gtk::Builder> builder)
	: builder(builder)
{
	builder->get_widget("wndTimeSel",    pwndTimeSel);
	builder->get_widget("btnTimeSelOK",  pbtnTimeSelOK);
	builder->get_widget("btnTimeSelCancel", pbtnTimeSelCancel);
	builder->get_widget("lblTimeSelTime",plblTimeSelTime);
	
	pbtnTimeSelOK->signal_clicked().connect( sigc::mem_fun(*this,
 		&TimeSel::on_click_OK) );

	pbtnTimeSelCancel->signal_clicked().connect( sigc::mem_fun(*this,
 		&TimeSel::on_click_Cancel) );


	// connect buttons
	Gtk::Button* pbtn = 0;

	builder->get_widget("btnTimeSel0", pbtn);
	pbtn->signal_clicked().connect( sigc::bind<unsigned int>
		(sigc::mem_fun(*this, &TimeSel::figbtnCallback), 0));
	builder->get_widget("btnTimeSel1", pbtn);
	pbtn->signal_clicked().connect( sigc::bind<unsigned int>
		(sigc::mem_fun(*this, &TimeSel::figbtnCallback), 1));
	builder->get_widget("btnTimeSel2", pbtn);
	pbtn->signal_clicked().connect( sigc::bind<unsigned int>
		(sigc::mem_fun(*this, &TimeSel::figbtnCallback), 2));
	builder->get_widget("btnTimeSel3", pbtn);
	pbtn->signal_clicked().connect( sigc::bind<unsigned int>
		(sigc::mem_fun(*this, &TimeSel::figbtnCallback), 3));
	builder->get_widget("btnTimeSel4", pbtn);
	pbtn->signal_clicked().connect( sigc::bind<unsigned int>
		(sigc::mem_fun(*this, &TimeSel::figbtnCallback), 4));
	builder->get_widget("btnTimeSel5", pbtn);
	pbtn->signal_clicked().connect( sigc::bind<unsigned int>
		(sigc::mem_fun(*this, &TimeSel::figbtnCallback), 5));
	builder->get_widget("btnTimeSel6", pbtn);
	pbtn->signal_clicked().connect( sigc::bind<unsigned int>
		(sigc::mem_fun(*this, &TimeSel::figbtnCallback), 6));
	builder->get_widget("btnTimeSel7", pbtn);
	pbtn->signal_clicked().connect( sigc::bind<unsigned int>
		(sigc::mem_fun(*this, &TimeSel::figbtnCallback), 7));
	builder->get_widget("btnTimeSel8", pbtn);
	pbtn->signal_clicked().connect( sigc::bind<unsigned int>
		(sigc::mem_fun(*this, &TimeSel::figbtnCallback), 8));
	builder->get_widget("btnTimeSel9", pbtn);
	pbtn->signal_clicked().connect( sigc::bind<unsigned int>
		(sigc::mem_fun(*this, &TimeSel::figbtnCallback), 9));

}


void TimeSel::runTimeSel(void (*paramcallback)(double), double time)
{
	callback = paramcallback;

	timeSet = true;

	// todo: set numbers from time
	numbers[0] = (unsigned int)(time/10); time -= 10*numbers[0];
	numbers[1] = (unsigned int)(time);    time = (time-numbers[1])*60;
	numbers[2] = (unsigned int)(time/10); time -= 10*numbers[2];
	numbers[3] = (unsigned int)(time);
	setnumber = 4;
	figbtnCallback(numbers[3]);

	pwndTimeSel->show();
}

void TimeSel::runTempSel(void (*paramcallback)(double), double temp)
{
	callback = paramcallback;
	timeSet = false;
	numbers[0] = (unsigned int)(temp/10); temp -= 10*numbers[0];
	numbers[1] = (unsigned int)(temp);    temp = (temp-numbers[1])*10;
	numbers[2] = (unsigned int)(temp);
	numbers[3] = 0;
	setnumber = 3;
	figbtnCallback(numbers[2]);

	pwndTimeSel->show();
}



void TimeSel::on_click_OK()
{
	pwndTimeSel->hide();
	if (timeSet)
	{
		callback(numbers[0]*10.0 + numbers[1] + numbers[2]/6.0 + numbers[3]/60.0);
	}
	else
	{
		callback(numbers[0]*10.0 + numbers[1] + numbers[2]*.1);
	}
}

void TimeSel::on_click_Cancel()
{
	pwndTimeSel->hide();
}


void TimeSel::figbtnCallback(unsigned int btnID)
{
	if (timeSet)
	{
		bool advance = false;
		switch(setnumber)
		{
		case 1: advance = (btnID <= 2); break;
		case 2: advance = (btnID <= 3 || numbers[0] <= 1); break;
		case 3: advance = (btnID <= 5); break;
		case 4: advance = true; break;
		}

		if (advance)
		{
			numbers[setnumber-1] = btnID;
			++setnumber;
		}
		if (setnumber > 4) setnumber = 1;

		std::ostringstream oss;
		for (unsigned int numid = 0; numid < 4; ++numid)
		{
			if (setnumber-1 == numid)
				oss << "<span foreground=\"#00ff00\" size=\"52400\"><b>";
			else
				oss << "<span foreground=\"#00a000\" size=\"52400\"><b>";

			oss << numbers[numid] << "</b></span>";

			if (numid+1 == 2) 
				oss << "<span foreground=\"#00a000\" size=\"52400\"><b>"
				    << ":</b></span>";
		}
		plblTimeSelTime->set_markup(oss.str().c_str());
	}
	else
	{	// tempset
		bool advance = false;
		switch(setnumber)
		{
		case 1: advance = (btnID <= 2 && btnID >= 1); break;
		case 2: advance = (btnID <= 5 || numbers[0] < 2); break;
		case 3: advance = true; break;
		}

		if (advance)
		{
			numbers[setnumber-1] = btnID;
			++setnumber;
		}
		if (setnumber > 3) setnumber = 1;

		std::ostringstream oss;
		for (unsigned int numid = 0; numid < 3; ++numid)
		{
			if (setnumber-1 == numid)
				oss << "<span foreground=\"#00ff00\" size=\"52400\"><b>";
			else
				oss << "<span foreground=\"#00a000\" size=\"52400\"><b>";

			oss << numbers[numid] << "</b></span>";

			if (numid+1 == 2) 
				oss << "<span foreground=\"#00a000\" size=\"52400\"><b>"
				    << ".</b></span>";
		}
		plblTimeSelTime->set_markup(oss.str().c_str());

	}
}


