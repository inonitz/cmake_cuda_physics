import matplotlib.pyplot as plt
import matplotlib.cm as cm
import numpy as np
import scipy as sc


def log_psi(
        n  : np.float64, 
        l  : np.float64, 
        m  : np.float64, 
        a0 : np.float64, 
        R_array     : np.ndarray, 
        theta_array : np.ndarray
):
    if len(R_array) != len(theta_array):
        return IndexError

    sfactor_arr = (2 / n * a0) * R_array
    partition0 = (2 * l + 1) / (np.pi * n**4 * a0**3)
    partition1 = sc.special.factorial((n - l).item()    ) / ( sc.special.factorial((n + l + 1).item())**3 )
    partition2 = sc.special.factorial((l - m + 1).item()) / ( sc.special.factorial((l + m + 1).item()) )
    constant_factor = partition0 * partition1 * partition2
    partition3_buf = sc.special.factorial((2 * sfactor_arr - 1))
    partition4_buf = sc.special.eval_genlaguerre(n - l - 1, 2 * l + 1, sfactor_arr)**2
    partition5_buf = sc.special.lpmv(m, l, np.cos(theta_array))**2

    out = constant_factor * partition3_buf.ravel() * partition4_buf.ravel() * partition5_buf.ravel()
    out = out.reshape(partition3_buf.shape)
    print(out.shape)
    print(out)
    return out



a0 = 1 # Bohr radius in atomic units (a.u.)
r_max = 10 # extent of the plot, this will be the maximum and minimum x and z
n_sample = 1000 # the number of samples.
x = np.linspace(-r_max, r_max, n_sample) # generate array of x
z = np.linspace(-r_max, r_max, n_sample) # generate array of z
X, Z = np.meshgrid(x,z) # create a meshgrid of x-z coordinate system
R = np.sqrt(X**2 + Z**2) # calculate the radius
polar = np.arctan2(X,Z) # calculate the polar angle from arctan2
azimuth = 0 # set the azimuth angle to zero
print(x.shape, z.shape, X.shape, Z.shape, R.shape, polar.shape)
print("Radius Buffer: {}".format(R))
print("Theta Buffer: {}".format(polar))
# print(180 * polar / np.pi)


# Quantum number
n, l, m = 3,2,0
# Factorials for Normalization factor
F1 = sc.special.factorial(n - l - 1, exact=True)
F2 = sc.special.factorial(n + l, exact=True)

# Multiplication factors for radial solution
F3 = np.exp(-R/(n*a0))
F4 = ((2*R)/(n))**l

# Initiate associated Laguerre polynomial and spherical harmonics
# as complex numpy array with zeros
L_func = np.zeros(n_sample, dtype=complex)
Y_func = np.zeros(n_sample, dtype=complex)

# Calculate Laguerre polynomial and spherical harmonics
L_func = sc.special.eval_genlaguerre(n - l - 1, 2*l + 1, (2*R)/(n*a0)) + 1j*0
# Y_func = sc.special.sph_harm_y(l, m, polar, azimuth)
Y_func = [ sc.special.sph_harm_y(l, m, thetai, azimuth) for thetai in polar ]

# Wave function
psi = np.zeros((n_sample,n_sample), dtype=complex) # initiate wave function as complex numpy array with zeros
N_fact = np.sqrt(((2/(n*a0))**3)*(F1/(2*n*(F2**3)))) # normalization factor
Radial = F3*F4*L_func # radial solution
Angular = Y_func # angular solution
psi = N_fact*Radial*Angular # the wave function (complex-valued)
# psi = (N_fact * Radial)**2
# prob_density = np.conjugate(psi)*psi # the probability density
# prob_density = (
#     N_fact * 
#     Radial * 
#     [ sc.special.sph_harm_y(l, m, thetai, 2 * np.pi * 1 / m) for thetai in polar ]
# ) ** 2


# LOOK HERE 
prob_density = (N_fact * Radial)**2
# Constant N
AngularSquared = sc.special.factorial(l - m, exact=True) / sc.special.factorial(l + m, exact=True)
AngularSquared *= (2 * l + 1) / (4 * np.pi)
LegendrePoly = np.array(
    [ sc.special.lpmv(m, l, np.cos(thetai))**2 for thetai in polar ]
)
AngularSquared = AngularSquared * LegendrePoly
print("N_fact: {} = {} * {}\n".format(N_fact, ((2/(n*a0))**3), (F1/(2*n*(F2**3))) ))
print("Radial: {}\n == {} * {} * {}\n".format(Radial, F3, F4, L_func))
print("AngularSquared: {}\n".format(AngularSquared))


prob_density = prob_density * AngularSquared
print("prob_density Buffer: {}".format(prob_density))

# prob_density = log_psi(np.float64(n), np.float64(l), np.float64(m), np.float64(a0), R, polar)
# prob_density = [ np.exp(log_arg) for log_arg in prob_density]


fig = plt.figure() # initiate matplotlib
plt.matplotlib.rc('text', usetex = False) # enable Latex in plot
plt.rcParams.update({
    'text.color': "blue", 
    'axes.labelcolor': "blue",
    'xtick.color':"blue",
    'ytick.color':"blue"
}) # Change the text and label colors

plt.pcolormesh(X,Z, np.abs(prob_density), cmap = 'plasma') # type: ignore # plot heatmap, np.abs() is used to eliminate the imaginary part
plt.colorbar(label="prob density", orientation="vertical") # show the colorbar
strtitle ="$|psi|^2$ for $n,l,m$ = " + str(n) + "," + str(l) + "," + str(m) # The string for the title
plt.title(strtitle) # String for the plot title
plt.xlabel('Factor to Bohr radius') # xlabel of the plot
plt.ylabel('Factor to Bohr radius') # ylabel of the plot

plt.grid(False) # type: ignore # disable the grid

# fname = "Probability density for nlm " + str(n) + str(l) + str(m) + ".png" # string for the file name
# plt.savefig(fname, dpi=200) # type: ignore # save figure
plt.show() # type: ignore # show the plot