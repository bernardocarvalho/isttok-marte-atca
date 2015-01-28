set terminal png
set title "Last 4 bits variation"
set output "signalError.png"
plot "signalError.data" with histograms
