#include "../logreceiver.h"
#include "../setTempTransmitter.h"
#include "timeSel.h"

/*#include <gtkmm/application.h>
#include <gtkmm/window.h>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/label.h>*/
#include <gtkmm.h>


#include <iostream>
#include <iomanip>
#include <sstream>


Gtk::Window* pwndMain    = 0;
Gtk::Label*  plblTime    = 0;
Gtk::Label*  plblInT     = 0;
Gtk::Label*  plblInTset  = 0;
Gtk::Label*  plblOutTS   = 0;
Gtk::Label*  plblOutTN   = 0;

Gtk::Label*  plblPelletsAcc[7];	
Gtk::Label*  plblElPwr   = 0;

Gtk::Label*  plblTempTime = 0;
Gtk::Label*  plblTempConst = 0;
Gtk::Button* pbtnTempTime = 0;
Gtk::Button* pbtnTempConst = 0;

Gtk::Image*  pimgPelletOn = 0;

Gtk::Button* pbtnEnd = 0;
Gtk::Label*  plblCounter = 0;

TimeSel* pdlgTimeSel = 0;


void tToDH(time_t timeo, double& hod, unsigned int& weekday)
{	
	const time_t offset = 3*24*60*60;
	timeo += offset;

	const time_t daymodulo = 60*60*24;
	const time_t weekmodulo = 7*daymodulo;	

	hod = ((double)(timeo % daymodulo))/3600.0;
	weekday = (timeo % weekmodulo)/daymodulo+1;
}




class Updater 
{
public:
	// main thread reads data from network (blocking read), saves new data in variable and sets flag (mutex protected)
	// updates gui when gui timer calls.


	Updater() 
	: stop(false), newData(false), count(0)
	{
		pelletsOldDayOfWeek = 0;
		for (unsigned int ii = 0; ii < 7; ++ii)
		{
			pelletsAcc[ii] = 0;
			electricAcc[ii] = 0;
			temperatureAcc[ii] = 0.0001;
			temperatureNumMeas[ii] = 0.0;
			updateWritePelletsAcc(ii+1, 0, 1.0/0.0, 0);
		}
		updateTimeOfReturn(18.5);
		updateConstantTemp(20.0);
	}
               
	// Called to start the processing on the thread
	void start()
	{
		thread = Glib::Thread::create(sigc::mem_fun(*this, &Updater::run), true);
	}
 

	// When shutting down, we need to stop the thread
	~Updater() 
	{
		{
	     	Glib::Mutex::Lock lock (mutex);
	     	stop = true;
		}
      	if (thread)
        	thread->join(); // Here we block to truly wait for the thread to complete
	}




	bool updateGui() // Called by gui thread for updating labels in gui
	{
		bool doUpdate = false;
		LogReceiver::LoggData localData;
		{
			Glib::Mutex::Lock lock(mutex);
			if (newData) 
			{
				doUpdate = true;
				localData = recData;
				newData = false;
			}
		}

		if (doUpdate)
		{
			std::cout << "UPDATE" << std::endl;
			++count;
			std::ostringstream sss;
			sss << count;
			plblCounter->set_text(sss.str());

			double hourOfDay = 0;
			unsigned int dayOfWeek = 0;
			tToDH((unsigned long)localData.timestamp, hourOfDay, dayOfWeek);

			updateTime(hourOfDay, dayOfWeek);
			updateInT(localData.Tnedervaning+1);
			updateInTset(localData.Tmal+1);
			updateOutT(localData.TuteS, localData.TuteN);
			updatePelletsAcc(dayOfWeek, localData.pelletsCnt, (localData.TuteS+localData.TuteN)*0.5, localData.elforbCnt);
			updateElectricCurrent(hourOfDay, localData.elforbCnt);
			updateStateFlags((unsigned int)localData.controlState);

			updateSetTemp(hourOfDay, dayOfWeek);

		}
		
		return true;
	}


	// ###################################################################
	private:
	static const double tempTimeOffset = .7;
	static const double tempNightFrom = 23-.7;
	static const double tempNightTo = 6-.7-1.5;
	static const double tempDayFrom = 8.5-.7-1.0;
	static const double tempNattsankning = 1.0;
	static const double tempDagsankning = 3.0;
	SetTempTransmitter tempTransmitter;
	bool varyingTemp; // true if varying temp. activated
	bool tempDayLowered; // true when day lowered temperature activated
	bool tempNightLowered; // true when night lowered temperature activated

	void updateSetTemp(double hourOfDay, double dayOfWeek)
	{
		if (!varyingTemp) return; // if constant


		tempNightLowered = (hourOfDay > tempNightFrom || hourOfDay < tempNightTo);
		
		if (dayOfWeek <= 5 && hourOfDay > tempDayFrom && hourOfDay < tempDayFrom+1)
			tempDayLowered = true;

		if (hourOfDay > timeOfReturn-tempTimeOffset)
			tempDayLowered = false;



		double setTemp = constTempTemp;
		if (tempDayLowered) 
			setTemp -= tempDagsankning;
		else if (tempNightLowered) 
			setTemp -= tempNattsankning;

		tempTransmitter.setTemp(setTemp-1);

		updateTimeSelectionLabels();
	}

	void updateTimeSelectionLabels()
	{

		// write time
		int hour = (int)timeOfReturn;
		int minute = (int)( (timeOfReturn-hour)*60);
		std::ostringstream oss;
		if (varyingTemp)
			oss << "<span foreground=\"#" << colFullGreen << "\" size=\"41200\"><b>";
		else
			oss << "<span foreground=\"#" << colDeacGreen << "\" size=\"41200\"><b>";
		oss << "Sänkn. ";
		if (hour < 10) oss << "0";
		oss << hour << ":";
		if (minute < 10) oss << "0";
		oss << minute;

		if (tempDayLowered) 
			oss << " (D)";
		else if (tempNightLowered) 
			oss << " (N)";

		oss << "</b></span>";
		plblTempTime->set_markup(oss.str().c_str());

		// write const
		std::ostringstream osst;
		if (varyingTemp)
			osst << "<span foreground=\"#" << colDeacGreen << "\" size=\"41200\"><b>";
		else
			osst << "<span foreground=\"#" << colFullGreen << "\" size=\"41200\"><b>";		
		osst << "Konst. ";
		osst << "</b></span>";
		osst << "<span foreground=\"#" << colFullGreen << "\" size=\"41200\"><b>";
		osst << std::fixed << std::setprecision(1) << constTempTemp;
		osst << "</b></span>";
		plblTempConst->set_markup(osst.str().c_str());

	}

	public:

	void updateTimeOfReturn(double newTimeOfReturn)
	{
		timeOfReturn = newTimeOfReturn;
		
		varyingTemp = true; // activate dag/natt-sänkning
		tempDayLowered = true; // activate day lower.

		updateTimeSelectionLabels();
	}
	
	double getTimeOfReturn()
	{ return timeOfReturn; };


	void updateConstantTemp(double newConstTempTemp)
	{
		constTempTemp = newConstTempTemp;

		varyingTemp = false; // deactivate varying temp update
		tempNightLowered = false;
		tempDayLowered = false;
		tempTransmitter.setTemp(constTempTemp-1); // transmit temperature

		updateTimeSelectionLabels();
	}

	double getConstTempTemp()
	{ return constTempTemp; };
 
private:
    Glib::Thread * thread;
    Glib::Mutex mutex;
    bool stop;

	LogReceiver::LoggData recData;
	bool newData;

	double timeOfReturn;
	double constTempTemp;

	static const int hej = 323;

	static const char* const colFullGreen;
	static const char* const colDampedGreen;
	static const char* const colDeacGreen;
	static const char* const colFullYellow;

	unsigned int count; 

	void run () 
    {
		LogReceiver logReceiver;

		while(true) 
		{
			{
				Glib::Mutex::Lock lock (mutex);
				if (stop) 
				{
					std::cout << "Stopping!" << std::endl;
					break;
				}
			}

			const LogReceiver::LoggData& newRecData = logReceiver.getDataWait();
			recData.write(std::cout);

			{
				Glib::Mutex::Lock lock(mutex);
				recData = newRecData;
				newData = true;
			}
			

			//std::cout << "test" << std::endl;

			//sleep(1);
		
		}
	}




	// ######################################################################
	void updateTime(double hourOfDay, unsigned int dayOfWeek)
	{
		int hour = (int)hourOfDay;
		int minute = (int)( (hourOfDay-hour)*60);

		std::ostringstream oss;
		oss << "<span foreground=\"#" << colFullGreen << "\" size=\"82400\"><b>";
		switch(dayOfWeek) {
		case 1: oss << "Må "; break;
		case 2: oss << "Ti "; break;
		case 3: oss << "On "; break;
		case 4: oss << "To "; break;
		case 5: oss << "Fr "; break;
		case 6: oss << "Lö "; break;
		case 7: oss << "Sö "; break;
		}

		if (hour < 10) oss << "0";
		oss << hour << ":";
		if (minute < 10) oss << "0";
		oss << minute << "</b></span>";

		plblTime->set_markup(oss.str().c_str());
	}

	// ######################################################################
	void updateInT(double InT)
	{
		std::ostringstream oss;
		oss << "<span foreground=\"#" << colFullGreen << "\" size=\"41200\"><b>";
		oss << std::fixed << std::setprecision(1) << InT;
		oss << "</b></span>";
		plblInT->set_markup(oss.str().c_str());
	}

	// ######################################################################
	void updateInTset(double InTset)
	{
		std::ostringstream oss;
		oss << "<span foreground=\"#" << colFullYellow << "\" size=\"41200\"><b>";
		oss << std::fixed << std::setprecision(1) << InTset;
		oss << "</b></span>";
		plblInTset->set_markup(oss.str().c_str());
	}

	// ######################################################################
	void updateOutT(double outTS, double outTN)
	{
		std::ostringstream oss;
		oss << "<span foreground=\"#" << colFullGreen << "\" size=\"41200\"><b>";
		oss << std::fixed << std::setprecision(1) << outTS;
		oss << "</b></span>";
		plblOutTS->set_markup(oss.str().c_str());

		std::ostringstream ossn;
		ossn << "<span foreground=\"#" << colFullGreen << "\" size=\"41200\"><b>";
		ossn << std::fixed << std::setprecision(1) << outTN;
		ossn << "</b></span>";
		plblOutTN->set_markup(ossn.str().c_str());
	}

	// ############################# pellets and temperature, accumulated electricity
	static const double pelletskgpertick = (0.868-0.128)/7996.0;
	static const double electricKWHpertick = 1.0/1000.0;
	double pelletsAcc[7];
	double electricAcc[7];
	double temperatureAcc[7];
	double temperatureNumMeas[7];
	double pelletsOldDayOfWeek; // init to zero
	double pelletsBasecount;
	double electricBasecount;
	void updatePelletsAcc(unsigned int dayOfWeek, double pelletCount, double outdoorTemp, double electricCount)
	{
		if (dayOfWeek != pelletsOldDayOfWeek)
		{
			pelletsOldDayOfWeek = dayOfWeek;
			pelletsBasecount = pelletCount;
			electricBasecount = electricCount;
			temperatureAcc[dayOfWeek-1] = 0.000001;
			temperatureNumMeas[dayOfWeek-1] = 0;
			for (unsigned int ii = 0; ii < 7; ++ii)
				updateWritePelletsAcc(ii+1, pelletsAcc[ii],
				                      temperatureAcc[ii]/temperatureNumMeas[ii], electricAcc[ii]);

			//std::cout << "NEW DAY" << std::endl;
		}
			
		temperatureAcc[dayOfWeek-1] += outdoorTemp;
		temperatureNumMeas[dayOfWeek-1] += 1;
		pelletsAcc[dayOfWeek-1] = (pelletCount - pelletsBasecount)*pelletskgpertick;
		//std::cout << "ASDF: " << pelletsAcc[dayOfWeek-1] << std::endl;
		electricAcc[dayOfWeek-1] = (electricCount - electricBasecount)*electricKWHpertick;
		updateWritePelletsAcc(dayOfWeek, pelletsAcc[dayOfWeek-1], 
		                      temperatureAcc[dayOfWeek-1]/temperatureNumMeas[dayOfWeek-1], 
		                      electricAcc[dayOfWeek-1], colFullGreen);
	}

	void updateWritePelletsAcc(unsigned int dayOfWeek, double accpellets, double meantemp, double accelectric,
			const char* colcode = colDampedGreen)
	{
		std::ostringstream oss;
		oss << "<span foreground=\"#" << colcode << "\" size=\"30000\"><b>";
		oss << std::fixed << std::setprecision(1) << accpellets << "\n";
		oss << std::fixed << std::setprecision(1) << accelectric << "\n";
		if (meantemp > 1000) // divide by zero
			oss << "----";
		else
			oss << std::fixed << std::setprecision(1) << meantemp;
		oss << "</b></span>";
		plblPelletsAcc[dayOfWeek-1]->set_markup(oss.str().c_str());
	}

	// ######################################################################
	double electricLastCount;
	double electricLastHourOfDay;
	void updateElectricCurrent(double hourOfDay, double electricCount)
	{
		double hours;
		if (hourOfDay < electricLastHourOfDay)
			hours = hourOfDay+24-electricLastHourOfDay;
		else
			hours = hourOfDay-electricLastHourOfDay;

		double KWh = (electricCount-electricLastCount)*electricKWHpertick;

		std::ostringstream oss;
		oss << "<span foreground=\"#" << colFullGreen << "\" size=\"41200\"><b>";
		oss << std::fixed << std::setprecision(1) << KWh/hours;
		oss << "</b></span>";
		plblElPwr->set_markup(oss.str().c_str());

		electricLastCount = electricCount;
		electricLastHourOfDay = hourOfDay;
	}

	// ######################################################################
	void updateStateFlags(unsigned int controlState)
	{
		const bool boilerHold = controlState & 0x01;
		pimgPelletOn->set_visible(!boilerHold);
	}

};


Updater* pTheUpdater = 0;

static void timeSelCallback(double newTime)
{
	pTheUpdater->updateTimeOfReturn(newTime);
}

static void tempSelCallback(double newTemp)
{
	pTheUpdater->updateConstantTemp(newTemp);
}


static void on_btnTempTime_clicked()
{
	pdlgTimeSel->runTimeSel(&timeSelCallback,pTheUpdater->getTimeOfReturn());
}


static void on_btnTempConst_clicked()
{
	pdlgTimeSel->runTempSel(&tempSelCallback,pTheUpdater->getConstTempTemp());
}



static void on_btnEnd_clicked()
{
	std::cout << "QUIT!" << std::endl;
	if(pwndMain)
		pwndMain->hide(); //hide() will cause main::run() to end.
}




const char* const Updater::colFullGreen = "00FF00";
const char* const Updater::colDampedGreen = "00A000";
const char* const Updater::colFullYellow = "FFFF00";
const char* const Updater::colDeacGreen = "000040";







int main(int argc, char* argv[])
{
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create
		(argc, argv, "org.gtkmm.tjohej");

	Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create_from_file
		("dataglade.glade");

	pdlgTimeSel = new TimeSel(builder);

	builder->get_widget("wndMain",    pwndMain);
	builder->get_widget("lblTime",    plblTime);
	builder->get_widget("lblInT",     plblInT);
	builder->get_widget("lblInTset",  plblInTset);
	builder->get_widget("lblOutTS",   plblOutTS);
	builder->get_widget("lblOutTN",   plblOutTN);
	builder->get_widget("lblPelletsAcc1",   plblPelletsAcc[0]);
	builder->get_widget("lblPelletsAcc2",   plblPelletsAcc[1]);
	builder->get_widget("lblPelletsAcc3",   plblPelletsAcc[2]);
	builder->get_widget("lblPelletsAcc4",   plblPelletsAcc[3]);
	builder->get_widget("lblPelletsAcc5",   plblPelletsAcc[4]);
	builder->get_widget("lblPelletsAcc6",   plblPelletsAcc[5]);
	builder->get_widget("lblPelletsAcc7",   plblPelletsAcc[6]);
	builder->get_widget("lblElPwr",     plblElPwr);
	builder->get_widget("lblTempTime",  plblTempTime);
	builder->get_widget("lblTempConst", plblTempConst);
	builder->get_widget("btnTempTime",  pbtnTempTime);
	builder->get_widget("btnTempConst", pbtnTempConst);
	builder->get_widget("imgPelletOn", pimgPelletOn);
	builder->get_widget("btnEnd", pbtnEnd);
	pbtnTempTime->signal_clicked().connect( sigc::ptr_fun(on_btnTempTime_clicked) );
	pbtnTempConst->signal_clicked().connect( sigc::ptr_fun(on_btnTempConst_clicked) );
	pbtnEnd->signal_clicked().connect( sigc::ptr_fun(on_btnEnd_clicked) );
	builder->get_widget("lblCounter", plblCounter);
	plblCounter->set_text("Texten");

	pTheUpdater = new Updater;
	if(!Glib::thread_supported()) Glib::thread_init();

	sigc::slot<bool> labelslot = sigc::mem_fun(*pTheUpdater, &Updater::updateGui);
	sigc::connection labelconn = Glib::signal_timeout().connect(labelslot, 500);

	pTheUpdater->start();

	// TODO: remove updater
	return app->run(*pwndMain);
}

