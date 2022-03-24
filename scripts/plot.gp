reset
set ylabel 'time(sec)'
set style fill solid
set title 'performance comparison'
set term png enhanced font 'Verdana,10'
filePath = 'plot/'
set output filePath.'runtime.png'



plot [0:93][0:2500] filePath.'plot_input' using 0:2 with linespoints linewidth 2 title 'userspace time', \
'' using 0:3 with linespoints linewidth 2 title 'kernel space timer', \
'' using 0:4 with linespoints linewidth 2 title 'system call overhead'