// plot_helper.cpp
#include "plot_helper.h"
#include <cstdio>

bool PlotThreeVectors(
        const std::vector<double>& beat,
        const std::vector<double>& meet,
        const std::vector<double>& miss,
        int N_) {

    int expected = 2*N_+1;
    if(beat.size() != expected || meet.size() != expected || miss.size() != expected){
        std::fprintf(stderr, "One or more vectors are not size of 2N+1. Plot function finished.\n");
        return false;
    }
    std::vector<double> x_axis(expected, 0.0);
    for (int i = 0; i < expected; ++i) x_axis[i] = static_cast<double>(i-N_);

    struct Gnu {
        FILE* gp;
        Gnu() : gp(popen("gnuplot -persist", "w")) {}  // Constructor initialization
        ~Gnu() { if (gp) pclose(gp); } // Destructor closes the terminal
    };

    Gnu plot;
    if (!plot.gp) {
        std::fprintf(stderr, "Could not open gnuplot\n");
        return false;
    }

    FILE* gp = plot.gp;
    // Set appearance
    std::fprintf(gp, "set title 'CAAR Comparison: Beat vs Meet vs Miss' font 'Bold,14'\n"); //set title
    std::fprintf(gp, "set xlabel 'Announcement Date Timeline' font ',13'\n"); //set x label
    std::fprintf(gp, "set ylabel 'Percentage' font ',13'\n"); // set y label
    std::fprintf(gp, "set xtics font ',11'\n"); //set tics size (axis numbers)
    std::fprintf(gp, "set ytics font ',11'\n");
    std::fprintf(gp, "set key font ',12'\n");
    std::fprintf(gp, "set key top left\n"); //set legend
    std::fprintf(gp, "set grid\n"); //set grid lines
    // line keywords: lt -> linetype, lc -> line color, lw -> line width
    std::fprintf(gp, "set arrow from 0, graph 0 to 0, graph 1 nohead lw 2 lc rgb 'grey'\n"); //vertical line
    std::fprintf(gp, "set style line 1 lt 1 lc rgb 'blue' lw 2 pt 7\n"); // set plot style for beat
    std::fprintf(gp, "set style line 2 lt 1 lc rgb 'green' lw 2 pt 5\n"); //set plot style for meet
    std::fprintf(gp, "set style line 3 lt 1 lc rgb 'red' lw 2 pt 9\n"); //set plot style for miss
    std::fprintf(gp, "plot '-' using 1:2 with lines linestyle 1 title 'Beat', \
              '-' using 1:2 with lines linestyle 2 title 'Meet', \
              '-' using 1:2 with lines linestyle 3 title 'Miss'\n"); //plot command

    // Load the data to the buffer. lf is double style
    for (size_t i = 0; i < x_axis.size(); ++i) std::fprintf(gp, "%lf %lf\n", x_axis[i], beat[i]);
    std::fprintf(gp, "e\n"); //"e" is the gnuplot command to go to next plot data
    for (size_t i = 0; i < x_axis.size(); ++i) std::fprintf(gp, "%lf %lf\n", x_axis[i], meet[i]);
    std::fprintf(gp, "e\n");
    for (size_t i = 0; i < x_axis.size(); ++i) std::fprintf(gp, "%lf %lf\n", x_axis[i], miss[i]);
    std::fprintf(gp, "e\n");

    // Send buffered output
    std::fflush(gp);
    // Exit GNU terminal
    std::fprintf(gp, "exit\n");
    return true; // SUCCESS
}
//Close GNU plot terminal (via destructor)