# $begin runge_kutta_4_correct.py$$ $newlinech #$$
# $spell
#	runge_kutta
#	numpy
#	pycppad
#	def
#	dt
#	yi
#	yf
# $$
# $index ODE solution, correct$$
# $index runge_kutta_4, correct$$
# $index example, runge_kutta_4$$
#
# $section runge_kutta_4 A Correctness Example and Test$$
#
# $head Discussion$$
# Define $latex y : \B{R} \rightarrow \B{R}^n$$ by
# $latex \[
#	y_j (t) =  t^{j+1}
# \] $$ 
# It follows that the derivative of $latex y(t)$$ satisfies the 
# $cref/runge_kutta_4/$$ ODE equation where 
# $latex y(0) = 0 $$ and $latex f(t, y)$$ is given by
# $latex \[
# f(t , y)_j = y_j '(t) = \left\{ \begin{array}{ll}
#  1                 & {\; \rm if \;} j = 0      \\
#  (j+1) y_{j-1} (t) & {\; \rm otherwise }
# \end{array} \right.
# \] $$
#
# $head Source Code$$
#
# $code
# $verbatim%example/runge_kutta_4_correct.py%0%# BEGIN CODE%# END CODE%1%$$
# $$
# $end
# BEGIN CODE
from pycppad import *
def pycppad_test_runge_kutta_4_correct() :
	def fun(t , y) :
		n        = y.size
		f        = numpy.zeros(n)
		f[0]     = 1.
		index    = numpy.array( range(n-1) ) + 1
		f[index] = (index + 1) * y[index-1] 
		return f
	n  = 5              # size of y(t) (order of method plus 1)
	ti = 0.             # initial time
	dt = 2.             # a very large time step size to test correctness
	yi = numpy.zeros(n) # initial value for y(t); i.e., y(0)

	# take one 4-th order Runge-Kutta integration step of size dt 
	yf = runge_kutta_4(fun, ti, yi, dt)

	# check the results
	t_jp = 1.                                # t^0 at t = dt
	for j in range(n-1) :
		t_jp = t_jp * dt                    # t^(j+1) at t = dt
		assert abs( yf[j] - t_jp ) < 1e-10  # check yf[j] = t^(j+1)
# END CODE
