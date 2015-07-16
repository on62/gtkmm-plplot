#include "gtkmm-plplot/canvas.h"
#include "gtkmm-plplot/color.h"
#include <gtkmm/application.h>
#include <glibmm/miscutils.h>
#include <gtkmm/window.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <glib.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/switch.h>
#include <vector>

namespace Test2 {
  class Window : public Gtk::Window {
  private:
    Gtk::PLplot::Canvas canvas;
    Gtk::Grid grid;
    Gtk::Label x_label;
    Gtk::Label y_label;
    Gtk::Switch x_switch;
    Gtk::Switch y_switch;
  public:
    Window(std::vector<PLFLT> &x,
           std::vector<PLFLT> &y1,
           std::vector<PLFLT> &y2,
           std::vector<PLFLT> &y3,
           std::vector<PLFLT> &y4,
           std::string x_title,
           std::string y_title,
           std::string plot_title) :
           canvas(Gtk::PLplot::Plot2D(Gtk::PLplot::Plot2DData(x, y1, Gtk::PLplot::Color::RED), x_title, y_title, plot_title)),
           x_label("X-axis logarithmic"),
           y_label("Y-axis logarithmic") {


        Gtk::PLplot::Plot2D *plot = canvas.get_plot(0);
        plot->add_data(Gtk::PLplot::Plot2DData(x, y2, Gtk::PLplot::Color::BLUE));
        plot->add_data(Gtk::PLplot::Plot2DData(x, y3, Gtk::PLplot::Color::BLUEVIOLET));
        plot->add_data(Gtk::PLplot::Plot2DData(x, y4, Gtk::PLplot::Color::GREEN));

        set_default_size(720, 580);
        Gdk::Geometry geometry;
        geometry.min_aspect = geometry.max_aspect = double(720)/double(580);
        set_geometry_hints(*this, geometry, Gdk::HINT_ASPECT);
        set_title("Gtkmm-PLplot test2");
        canvas.set_hexpand(true);
        canvas.set_vexpand(true);

        x_switch.set_active(plot->get_axis_logarithmic_x());
        y_switch.set_active(plot->get_axis_logarithmic_y());
        x_switch.property_active().signal_changed().connect([this](){
          if (x_switch.get_active()) {
            canvas.get_plot(0)->set_axis_logarithmic_x(true);
          }
          else {
            canvas.get_plot(0)->set_axis_logarithmic_x(false);
          }
        });
        y_switch.property_active().signal_changed().connect([this](){
          if (y_switch.get_active()) {
            canvas.get_plot(0)->set_axis_logarithmic_y(true);
          }
          else {
            canvas.get_plot(0)->set_axis_logarithmic_y(false);
          }
        });
        x_label.set_hexpand(false);
        y_label.set_hexpand(false);
        x_switch.set_hexpand(false);
        y_switch.set_hexpand(false);
        x_label.set_halign(Gtk::ALIGN_END);
        y_label.set_halign(Gtk::ALIGN_END);
        x_switch.set_halign(Gtk::ALIGN_START);
        y_switch.set_halign(Gtk::ALIGN_START);

        grid.attach(x_label, 0, 0, 1, 1);
        grid.attach(y_label, 0, 1, 1, 1);
        grid.attach(x_switch, 1, 0, 1, 1);
        grid.attach(y_switch, 1, 1, 1, 1);
        grid.attach(canvas, 0, 2, 2, 1);
        grid.set_column_spacing(5);

        add(grid);
        set_border_width(10);
        grid.show_all();
    }
    virtual ~Window() {}
  };
}


int main(int argc, char *argv[]) {
  Glib::set_application_name("gtkmm-plplot-test2");
  Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "eu.tomschoonjans.gtkmm-plplot-test2");

  //open our test file
  std::ifstream fs;
  fs.exceptions(std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit);
  try {
	   fs.open(TEST_CSV);
  }
  catch (std::exception &e) {
    std::cerr << "Error opening file " << TEST_CSV << " -> " << e.what() << std::endl;
    return 1;
  }

  std::vector<PLFLT> x;
  std::vector<PLFLT> y1, y2, y3, y4;

  while (1) {
    try {
      fs.clear();
      std::string line;
      std::getline(fs, line);
      gchar **splitted = g_strsplit(line.c_str(), ",", 0);

      x.push_back(g_ascii_strtod(splitted[1], NULL));
      y1.push_back(g_ascii_strtod(splitted[2], NULL));
      y2.push_back(g_ascii_strtod(splitted[3], NULL));
      y3.push_back(g_ascii_strtod(splitted[4], NULL));
      y4.push_back(g_ascii_strtod(splitted[5], NULL));
      g_strfreev(splitted);
    }
    catch (std::exception &e) {
      if (fs.eof()) {
        break;
      }
      std::cerr << "Error parsing " << TEST_CSV << " -> " << e.what() << std::endl;
      return 1;
    }
    catch (...) {
      std::cerr << "Unknown exception occurred" << std::endl;
      return 1;
    }
  }

  //ensure our y-values are greater than 1!
  //the x-values are already...
  std::for_each(std::begin(y1), std::end(y1), [](PLFLT &a) { if (a < 1.0 ) a = 1.0;});
  std::for_each(std::begin(y2), std::end(y2), [](PLFLT &a) { if (a < 1.0 ) a = 1.0;});
  std::for_each(std::begin(y3), std::end(y3), [](PLFLT &a) { if (a < 1.0 ) a = 1.0;});
  std::for_each(std::begin(y4), std::end(y4), [](PLFLT &a) { if (a < 1.0 ) a = 1.0;});

  std::string x_title("Energy (keV)"), y_title("Intensity (counts)"), plot_title("NIST SRM 1155 Stainless steel");
  Test2::Window window(x, y1, y2, y3, y4, x_title, y_title, plot_title);

	return app->run(window);
}