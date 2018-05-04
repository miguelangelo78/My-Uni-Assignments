set style fill solid
set nokey

set datafile separator ","
set format x '%1.2f'
set grid ytics lt 0 lw 1 lc rgb "#bbbbbb"
set grid xtics lt 0 lw 1 lc rgb "#bbbbbb"
set size noratio
set autoscale fix
set samples 5000
set xtics font ", 9"
set zeroaxis linewidth 0.8

set term qt noraise

bind all 'q' 'exit gnuplot'
bind all 'Escape' 'exit gnuplot'
bind Close "exit gnuplot"

round(x) = x - floor(x) < 0.5 ? floor(x) : ceil(x)
round2(x, n) = round(x*10**n)*10.0**(-n)

i = 2
termCtr = 1
plotType = 0

firstrow = system('OutputData\\gnuplot\\bin\\head -1 OutputData\\Output.csv')
firstrow = substr(firstrow, strstrt(firstrow, ",n,") + 3, strlen(firstrow))

do for[i=3:`OutputData\\gnuplot\\bin\\awk -F"," 'NR==1 {print NF; exit}' OutputData\\Output.csv`:3] {
	idx = strstrt(firstrow, ",")
	title_name = substr(firstrow, 0, idx - 1)

	do for[j=0:1] {
		firstrow = substr(firstrow, idx + 1, strlen(firstrow))
		idx = strstrt(firstrow, ",")
	}
	firstrow = substr(firstrow, idx + 1, strlen(firstrow))
		
	set term qt termCtr title title_name
	set title substr(title_name, strstrt(title_name, "-") + 1, strlen(title_name))
	termCtr = termCtr + 1

	stats "OutputData\\Output.csv" using i-1:i name 'stats_' nooutput
	set ytics add (round2(stats_min_y, 2), round2(stats_max_y, 2))
	
	if(plotType == 0 || plotType == 3) {
		set tics format '%h'
		set xlabel "Time (ms)"
		set ylabel "Amplitude x(t)" offset 3,0,0
		set x2label "\nN sample"
		set x2tics 50 font ", 7"
		set xtics 4 add (round2(stats_min_x, 2), round2(stats_max_x, 2))

		plotType = plotType + 1
		plot "OutputData\\Output.csv" using i-1:i with lines linetype 6 linewidth 3, '' using 0:(1/0) axes x2y1
	} else {
		if(plotType == 1 || plotType == 2 || plotType == 4 || plotType == 5) {
			if(plotType == 1) {
				set ylabel "Magnitude x[k]" offset 1,0,0
				plotType = plotType + 1
			} else {
				if(plotType == 2) {
					set ylabel "Magnitude x[k] dB" offset 3,0,0
					plotType = plotType + 1
				} else {
					if(plotType == 4) {
						set ylabel "Magnitude x[k]" offset 3,0,0
						plotType = plotType + 1
					} else {
						set ylabel "Magnitude x[k] dB" offset 3,0,0
						plotType = 0
					}
				}
			}

			set tics format '%h'
			set xlabel "Frequency (Hz)"
			set x2label "\nk (bin)"
			set format x "%1.0f"
			set xrange [:] noextend
			set x2range [:] noextend
			set x2tics 10 font ", 7"
			
			if(strstrt(substr(title_name, 0, strstrt(title_name, "-") - 2), "DSB") > 0) {
				set xtics 1000 nomirror font ", 7" add (round2(stats_min_x, 2), round2(stats_max_x, 2))
				set x2tics 20 font ", 6"
			} else {
				set xtics 500 nomirror font ", 7" add (round2(stats_min_y, 2), round2(stats_max_x, 2))
			}

			plot "OutputData\\Output.csv" using i-1:i with lines linetype 6 linewidth 3, '' using i-2:(1/0) axes x2y1
		}
	}

	set terminal jpeg giant size 1920,1080

	if(strlen(title_name) > 100) {
		set output "OutputData\\".substr(title_name, 0, 100)."__(capped)__.jpg"
	} else {
		set output "OutputData\\".title_name.".jpg"
	}

	replot

	set tics format '\textbf{\scriptsize $%h$}'
	set terminal epslatex color colortext size 28cm,15cm lw 3 font 'phv'
	set title ""
	set output "res/plots/".substr(title_name, 0, strstrt(title_name, "-") - 2).".tex"

	replot
	unset ytics
}