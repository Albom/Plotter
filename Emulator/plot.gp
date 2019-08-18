set term pngcairo size 700, 700 
set output "out.png"

set palette defined ( 0 "#111175", 1 "#faa5a5")

set size ratio 1

set xrange [0:100]
set yrange [100:0]

set x2label 'x'

set ylabel 'y' rotate by 0


set ytics 10
set x2tics 10
unset xtics

unset colorbox

#set style arrow 1 filled size screen 0.03,15,45 


#plot "out.txt" using 1:2:3:4:5 with vectors arrowstyle 1 notitle 
plot "out.txt" using 1:2:3:4:5 with vectors head filled size screen 0.5,1,1 linecolor palette  notitle

exit gnuplot
