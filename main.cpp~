#include <gtkmm/application.h>
#include <iostream>
#include <streambuf>
#include <fstream>
#include "simulador.h"

using namespace std;

int main(int argc, char *argv[])
{
	Glib::RefPtr<Gtk::Application> aplicacao = Gtk::Application::create(argc, argv, "");

	Simulador sim(2);

	return aplicacao->run(sim);
}
