# jacobian result with type a_double

def test_jacobian():
  # A = [ 0 1 2 ]  f(x) = A * x
  #     [ 3 4 5 ]
  A   = numpy.array([ 
    [ 1., 2., 3. ],
    [ 4., 5., 6. ]
  ])
  x   = numpy.array( [ ad(0.), ad(0.), ad(0.) ] )
  a_x = independent(x)
  a_y = numpy.dot(A, a_x)
  f   = adfun(a_x, a_y)
  x   = numpy.array( [ ad(1.), ad(2.), ad(3.) ] )
  J   = f.jacobian(x)
  assert numpy.all( A == J )