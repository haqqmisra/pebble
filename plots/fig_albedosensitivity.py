import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt

solcon = 1360 / 4
Arad   = 218.
Brad   = 1.90
alb    = np.arange( 0.2, 0.5, 0.001 )
temp   = ( solcon * ( 1. - alb ) - Arad ) / Brad + 273.16

plt.plot( alb, temp, color="black" )
plt.xlim( 0.265, 0.29 )
plt.ylim( 286., 290. )
plt.xlabel( "Albedo", fontsize=12 )
plt.ylabel( "Temperature (K)", fontsize=12 )

plt.text( 0.282, 289.75, r"$S_{0}(1 - \alpha) = A + BT$", fontsize=12 )
plt.text( 0.282, 289.50, r"$S_{0} = 340\,$W$\,$m$^{-2}$", fontsize=10 )
plt.text( 0.282, 289.25, r"$A = 218\,$W$\,$m$^{-2}$", fontsize=10 )
plt.text( 0.282, 289.0, r"$B = 1.90\,$W$\,$m$^{-2}\,^{\circ}$C$^{-1}$", fontsize=10 )
plt.plot( ( 0., 0.2731 ), ( 288.5, 288.5 ), color="black", linestyle=(0,(5,10)), linewidth=0.75 )
plt.plot( ( 0., 0.278688 ), ( 287.5, 287.5 ), color="black", linestyle=(0,(5,10)), linewidth=0.75 )
plt.plot( ( 0.2731, 0.2731 ), ( 286.0, 288.5 ), color="black", linestyle=(0,(5,10)), linewidth=0.75 )
plt.plot( ( 0.278688, 0.278688 ), ( 286.0, 287.5 ), color="black", linestyle=(0,(5,10)), linewidth=0.75 )
plt.text( 0.269, 287.95, r"$\Delta T = 1\,$K" )
plt.text( 0.2738, 287.0, r"$\Delta \alpha = 0.0056$" )

plt.savefig( "fig_albedosensitivity.png", bbox_inches='tight' )
plt.savefig( "fig_albedosensitivity.eps", bbox_inches='tight' )
#plt.show()
