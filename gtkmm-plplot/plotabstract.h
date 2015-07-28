/*
Copyright (C) 2015 Tom Schoonjans

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GTKMM_PLPLOT_PLOTABSTRACT_H
#define GTKMM_PLPLOT_PLOTABSTRACT_H

#include <sigc++/sigc++.h>
#include <vector>
#include <plstream.h>
#include <cairomm/cairomm.h>
#include <glibmm/ustring.h>

namespace Gtk {
  namespace PLplot {
    /** \class PlotAbstract plotabstract.h <gtkmm-plplot/plotabstract.h>
     *  \brief an abstract base class for all plots
     *
     *  This is the base class from which all other plots classes are derived.
     *  It contains several pure virtual methods that have to be implemented by all derived classes.
     *  This class cannot be instantiated directly, meaning that instances returned by Canvas::get_plot() and
     *  Canvas::add_plot() need to be cast to access methods offered only by the derived class.
     */
     class PlotAbstract : public sigc::trackable {
     protected:
      std::vector<Plot2DData *> plot_data; ///< vector that contains pointers to the Plot2DData datasets
      Glib::ustring axis_title_x; ///< X-axis title
      Glib::ustring axis_title_y; ///< Y-axis title
      Glib::ustring plot_title;   ///< Plot title
      plstream *pls; ///< the \c plstream object that will ensure the connection with PLplot
      double plot_data_range_x[2]; ///< the maximum range covered by the X-values of the datasets
      double plot_data_range_y[2];  ///< the maximum range covered by the X-values of the datasets
      double plotted_range_x[2]; ///< the current range shown on the plot for the X-axis
      double plotted_range_y[2]; ///< the current range shown on the plot for the X-axis
      double cairo_range_x[2]; ///< the current range shown on the plot for the X-axis in Cairo coordinates
      double cairo_range_y[2]; ///< the current range shown on the plot for the X-axis in Cairo coordinates
      int canvas_width; ///< the width of the canvas in Cairo units
      int canvas_height; ///< the height of the canvas in Cairo units
      int plot_offset_x; ///< the offset of the plot with respect to the top left corner of the canvas, measured along the horizontal (X-) axis in Cairo units
      int plot_offset_y; ///< the offset of the plot with respect to the top left corner of the canvas, measured along the vertical (Y-) axis in Cairo units
      int plot_width; ///< the current width of the plot in Cairo units
      int plot_height; ///< the current height of the plot in Cairo units
      bool shown; ///< \c true means the plot is currently visible, \c false means it is not plotted
      Gdk::RGBA background_color; ///< the currently used background color of the plot (default = fully transparent white, meaning that the background will be determined by the canvas)
      double plot_width_norm; ///< the normalized plot width, calculated relative to the canvas width
      double plot_height_norm; ///< the normalized plot height, calculated relative to the canvas height
      double plot_offset_horizontal_norm; ///< the normalized horizontal offset from the canvas top left corner, calculated relative to the canvas width
      double plot_offset_vertical_norm; ///< the normalized vertical offset from the canvas top left corner, calculated relative to the canvas height
      bool region_selectable; ///< \c true indicates that a region on the plot can be selected by dragging a box with the mouse button pressed in when showing, or if double mouse button pressed event zooms out, \c false means not possible. The default is \c true
      Gdk::RGBA axes_color; ///< the currently used color to draw the axes, the box and gridlines. Default is opaque black
      Gdk::RGBA titles_color; ///< the currently used color to draw the axes and plot titles. Default is opaque black
      BoxStyle box_style; ///< the currently used box style to draw the box, axes and grid


      sigc::signal<void, double, double, double, double > _signal_select_region; ///< signal that gets emitted whenever a new region was selected using the mouse pointer in Canvas::on_button_release_event()
      sigc::signal<void> _signal_changed; ///< signal that gets emitted whenever any of the plot parameters, or any of the contained Plot2DData datasets is changed.
      sigc::signal<void, PlotDataAbstract *> _signal_data_added; ///< signal emitted whenever a new PlotDataAbstract dataset is added to the plot
      sigc::signal<void> _signal_data_removed; ///< signal emitted whenever data is removed from the plot.

      virtual void plot_data_modified() = 0; ///< a method that will update the \c _range variables when datasets are added, modified or removed.
      virtual void convert_plplot_to_cairo_coordinates(
        double x_pl, double y_pl,
        double &x_cr, double &y_cr) = 0; ///< method to calculate the Cairo coordinates corresponding to PLplot coordinates, mostly used after draw, which is necessary after Canvas widget resizing.
      virtual void convert_cairo_to_plplot_coordinates(
        double x_cr, double y_cr,
        double &x_pl, double &y_pl) = 0; ///< method to calculate the PLplot coordinates corresponding to Cairo coordinates, mostly used after draw, which is necessary after Canvas widget resizing.

      /** This is a default handler for signal_select_region()
       *
       * This function passes the plot (data) coordinates of the selection box to set_region, in order to set the plotted range corresponding to the selection box.
       * If this behavior is not desired, derive the class and implement your own on_select_region method.
       * \param xmin left X-coordinate
       * \param xmax right X-coordinate
       * \param ymin lower Y-coordinate
       * \param ymax upper Y-coordinate
       * \exception Gtk::PLplot::Exception
       */
      virtual void on_select_region(double xmin, double xmax, double ymin, double ymax);

      /** This is a default handler for signal_changed()
       *
       * This signal is emitted whenever any of the plot properties, or the properties of the Plot2DData datasets therein, is changed.
       * Currently it does nothing but the signal will get caught by Canvas, and will eventually trigger a redrawing of the entire widget.
       */
      virtual void on_changed();

      /** This is a default handler for signal_data_added()
       *
       * This signal is emitted whenever a new PlotDataAbstract dataset is added to the plot.
       * It will call the private method plot_data_modified, which will update \c *_range_* properties, and emit signal_changed.
       * \param new_data a pointer to the newly added PlotDataAbstract dataset, after it has been added to the \c plot_data vector.
       */
      virtual void on_data_added(PlotDataAbstract *new_data);

      /** This is a default handler for signal_data_removed()
       *
       * This signal is emitted whenever a PlotDataAbstract dataset is removed from the plot.
       * It will call the private method plot_data_modified, which will update \c *_range_* properties, and emit signal_changed.
       */
      virtual void on_data_removed();

    public:
      /** Constructor
       *
       * This class provides a single constructor, which takes an existing Plot2DData dataset to construct a plot.
       * Optionally, the constructor takes additional arguments to set the axes and plot titles, as well as normalized coordinates that will determine the position and dimensions of the plot within the canvas. The default corresponds to the plot taking up the full space.
       * \param axis_title_x X-axis title
       * \param axis_title_y Y-axis title
       * \param plot_title plot title
       * \param plot_width_norm the normalized plot width, calculated relative to the canvas width
       * \param plot_height_norm the normalized plot height, calculated relative to the canvas height
       * \param plot_offset_horizontal_norm the normalized horizontal offset from the canvas top left corner, calculated relative to the canvas width
       * \param plot_offset_vertical_norm the normalized vertical offset from the canvas top left corner, calculated relative to the canvas height
       */
      PlotAbstract(const Glib::ustring &axis_title_x,
                   const Glib::ustring &axis_title_y,
                   const Glib::ustring &plot_title,
                   const double plot_width_norm,
                   const double plot_height_norm,
                   const double plot_offset_horizontal_norm,
                   const double plot_offset_vertical_norm);

      /** Copy constructor
       *
       * \param plot plot to be copied
       */
      PlotAbstract(const PlotAbstract &plot);

      /** Destructor
       *
       */
      virtual ~PlotAbstract();

      /** Get a pointer to a dataset included in the plot
       *
       * Throws an exception when \c data_index is invalid.
       * \param data_index index of the dataset in the \c plot_data vector
       * \return a pointer to the Plot2DData in the \c plot_data vector.
       * \exception Gtk::PLplot::Exception
       */
      PlotDataAbstract *get_data(unsigned int data_index);

      /** Method to draw the plot with all of its datasets
       *
       * This method is virtual allowing inheriting classes to implement their own method with the same signature.
       * \param cr the cairo context to draw to.
       * \param width the width of the Canvas widget
       * \param height the height of the Canvas widget
       */
      //void draw_plot(const Cairo::RefPtr<Cairo::Context> &cr, plstream *_pls, int width, int height);
      virtual void draw_plot(const Cairo::RefPtr<Cairo::Context> &cr, const int width, const int height) = 0;
      /** Set the X-axis title
       *
       * \param title the new X-axis title
       */
      void set_axis_title_x(Glib::ustring &title);

      /** Set the Y-axis title
       *
       * \param title the new Y-axis title
       */
      void set_axis_title_y(Glib::ustring &title);

      /** Get the X-axis title
       *
       * \return the current X-axis title
       */
      Glib::ustring get_axis_title_x();

      /** Get the Y-axis title
       *
       * \return the current Y-axis title
       */
      Glib::ustring get_axis_title_y();

      /** Set the plot title
       *
       * \param title the new plot title (may be an empty string)
       */
      void set_plot_title(Glib::ustring &title);

      /** Get the plot title
       *
       * \return the current plot title
       */
      Glib::ustring get_plot_title();

      /** Set the box style
       *
       * \param style the new box style (default is BOX_TICKS_TICK_LABELS)
       */
      void set_box_style(BoxStyle style = BOX_TICKS_TICK_LABELS);

      /** Get the box style
       *
       * \return the currently selected box style
       */
      BoxStyle get_box_style();

      /** Changes the visible plotted region
       *
       * Sets the axes range of the plotted box to the supplied parameters.
       * Throws an exception when the coordinates are outside of \c plot_data_range_x and \c plot_data_range_y.
       * \param xmin left X-coordinate
       * \param xmax right X-coordinate
       * \param ymin lower Y-coordinate
       * \param ymax upper Y-coordinate
       * \exception Gtk::PLplot::Exception
       */
      void set_region(double xmin, double xmax, double ymin, double ymax);

      /** Make the plot visible on the canvas
       *
       */
      void show();

      /** Hide the plot on the canvas
       *
       */
      void hide();

      /** Returns whether or not the plot is showing on the canvas
       *
       * \return \c true if the plot is visible, \c false if not
       */
      bool is_showing() const;

      /** Get the plot background color
       *
       * \return The currently selected plot background color.
       */
      Gdk::RGBA get_background_color();

      /** Set the background color
       *
       * \param color Set a new plot background color.
       */
      void set_background_color(Gdk::RGBA color);

      /** Get the color to draw the axes, the box and gridlines
       *
       * \return The currently selected axes color.
       */
      Gdk::RGBA get_axes_color();

      /** Set the color to draw the axes, the box and gridlines
       *
       * \param color Set a new axes color.
       */
      void set_axes_color(Gdk::RGBA color);

      /** Get the color to draw the plot and axes titles
       *
       * \return The currently selected plot and axes color.
       */
      Gdk::RGBA get_titles_color();

      /** Set the color to draw the plot and axes titles
       *
       * \param color Set a new titles color.
       */
      void set_titles_color(Gdk::RGBA color);

      /** Get whether regions can be selected on the plot by dragging the mouse while the button is clicked in.
       *
       * \return \c true if a region is selectable in the plot, \c false if not
       */
      bool get_region_selectable();

      /** Sets whether regions can be selected on the plot by dragging the mouse while the button is clicked in.
       *
       * \param selectable pass \c true if a region has to be selectable, \c false if not
       */
      void set_region_selectable(bool selectable = true);

      /** signal_select_region is emitted whenever a selection box is dragged across a plot
       *
       * See default handler on_select_region()
       */
      sigc::signal<void, double, double, double, double > signal_select_region() {
        return _signal_select_region;
      }

      /** signal_changed is emitted whenever any of the plot properties or any of the dataset properties has changed.
       *
       * See default handler on_changed()
       */
      sigc::signal<void> signal_changed() {
        return _signal_changed;
      }

      /** signal_data_added is emitted whenever a new dataset is added to the plot
       *
       * See default handler on_data_added()
       */
      sigc::signal<void, PlotDataAbstract *> signal_data_added() {
        return _signal_data_added;
      }

      /** signal_data_removed is emitted whenever a dataset is removed from the plot
      *
      * See default handler on_data_removed()
      */
      sigc::signal<void> signal_data_removed() {
        return _signal_data_removed;
      }



    };
  }
}



#endif