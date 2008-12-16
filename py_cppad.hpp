#ifndef PY_CPPAD_HPP
#define PY_CPPAD_HPP

#define PY_ARRAY_UNIQUE_SYMBOL PyArrayHandle

#include "num_util.h"
#include "cppad/cppad.hpp"

using namespace std;
namespace b = boost;
namespace bp = boost::python;
namespace bpn = boost::python::numeric;
namespace nu = num_util;



namespace{

	class ADFun_double {
		private:
			CppAD::ADFun<double> f_;
		public:
			ADFun_double(bpn::array& x_array, bpn::array& y_array);
			bpn::array Forward(int p, bpn::array& xp);
	};

	template<class Tdouble>
	class vec {
			size_t       length_;
			Tdouble  *pointer_;
			Tdouble **handle_;
			bool    allocated_;
		public:
			typedef Tdouble value_type;
			vec(bpn::array& py_array);
			vec(size_t length);
			vec(const vec<Tdouble>& in_vec);
			vec(void);
			~vec(void);
			void operator=(const vec<Tdouble>& in_vec);
			size_t size(void) const;
			void resize(size_t length);
			Tdouble& operator[](size_t i);
			const Tdouble& operator[](size_t i) const;
	};

	/* =================================== */
	/* CLASS SPECIALIZATION OF vec<double> */
	/* =================================== */

	template<>
	const double& vec<double>::operator[](size_t i) const{
		assert( i < length_ );
		return pointer_[i];
	}
	
	template<>
	vec<double>::vec(bpn::array& py_array)
	{	// get array info
		int* dims_ptr = PyArray_DIMS(py_array.ptr());
		int ndim      = PyArray_NDIM(py_array.ptr());
		int length    = dims_ptr[0];

		// check array info
		assert( ndim == 1 );
		assert( length >= 0 );

		// set private data
		length_    = static_cast<size_t>( length );
		pointer_   = static_cast<double*>(
			PyArray_DATA(py_array.ptr())
		);
		allocated_ = false;
		return;
	}
	
	template<>
	vec<double>::vec(size_t length){
		length_    = length;
		pointer_   = CPPAD_TRACK_NEW_VEC(length, pointer_);
		allocated_ = true;
		return;
	}
	template<>
	vec<double>::vec(const vec<double>& vec)
	{	length_    = vec.length_;
		pointer_   = CPPAD_TRACK_NEW_VEC(length_, pointer_);
		allocated_ = true;
		for(size_t i = 0; i < length_; i++)
			pointer_[i] = vec[i];
	}
	template<>
	vec<double>::vec(void){
		length_    = 0;
		pointer_   = 0;
		allocated_ = false;
	}
	
	template<>
	vec<double>::~vec(void){
		if( allocated_ )
			CPPAD_TRACK_DEL_VEC(pointer_);
	}

	template<>
	void vec<double>::operator=(const vec<double>& vec){
		assert( length_ == vec.length_ );
		for(size_t i = 0; i < length_; i++){
			pointer_[i] = vec.pointer_[i];
		}
	}

	template<>
	size_t vec<double>::size(void) const{
		return length_;
	}

	template<>
	void vec<double>::resize(size_t length){
		if( allocated_ ){
			CPPAD_TRACK_DEL_VEC(pointer_);
		}
		pointer_   = CPPAD_TRACK_NEW_VEC(length, pointer_);
		length_    = length;
		allocated_ = true;
	}

	template<>
	double& vec<double>::operator[](size_t i){
		assert( i < length_ );
		return pointer_[i];
	}

	/* =================================== */
	/* CLASS vec<Tdouble>                  */
	/* =================================== */

	template<class Tdouble>
	vec<Tdouble>::vec(bpn::array& py_array){
		// get array info
		int* dims_ptr = PyArray_DIMS(py_array.ptr());
		int ndim      = PyArray_NDIM(py_array.ptr());
		int length    = dims_ptr[0];

		// check array info
		assert( ndim == 1 );
		assert( length >= 0 );

		// pointer to object
		bp::object *obj_ptr = static_cast<bp::object*>(
			PyArray_DATA(py_array.ptr())
		);

		// set private data
		length_  = static_cast<size_t>(length);
		pointer_ = 0;
		handle_  = CPPAD_TRACK_NEW_VEC(length_, handle_);
		for(size_t i = 0; i < length_; i++) handle_[i] =
			& bp::extract<CppAD::AD<double>&>(obj_ptr[i])();
		return;
	}

	template<class Tdouble>
	vec<Tdouble>::vec(size_t length){
		length_  = length;
		pointer_ = CPPAD_TRACK_NEW_VEC(length, pointer_);
		handle_  = CPPAD_TRACK_NEW_VEC(length, handle_);
		for(size_t i = 0; i < length_; i++)
			handle_[i] = pointer_ + i;
		return;
	}

	template<class Tdouble>
	vec<Tdouble>::vec(const vec<Tdouble>& in_vec)	{
		length_   = in_vec.length_;
		pointer_  = CPPAD_TRACK_NEW_VEC(length_, pointer_);
		handle_   = CPPAD_TRACK_NEW_VEC(length_, handle_);
		for(size_t i = 0; i < length_; i++)
		{	handle_[i]  = pointer_ + i;
			pointer_[i] = in_vec[i];
		}
	}

	template<class Tdouble>
	vec<Tdouble>::vec(void){
		length_  = 0;
		pointer_ = 0;
		handle_  = 0;
	}

	template<class Tdouble>
	vec<Tdouble>::~vec(void){
		if( handle_ != 0 )
			CPPAD_TRACK_DEL_VEC(handle_);
		if( pointer_ != 0 )
			CPPAD_TRACK_DEL_VEC(pointer_);
	}

	template<class Tdouble>
	void vec<Tdouble>::operator=(const vec<Tdouble>& in_vec)	{
		assert( length_ == in_vec.length_ );
		for(size_t i = 0; i < length_; i++)
			*handle_[i] = *(in_vec.handle_[i]);
		return;
	}

	template<class Tdouble>
	size_t vec<Tdouble>::size(void) const{
		return length_;
	}

	template<class Tdouble>
	void vec<Tdouble>::resize(size_t length)
	{
		if( handle_ != 0 )
			CPPAD_TRACK_DEL_VEC(handle_);
		if( pointer_ != 0 )
			CPPAD_TRACK_DEL_VEC(pointer_);
		pointer_   = CPPAD_TRACK_NEW_VEC(length, pointer_);
		handle_    = CPPAD_TRACK_NEW_VEC(length, handle_);
		length_    = length;
		for(size_t i = 0; i < length_; i++)
			handle_[i]  = pointer_ + i;
	}

	template<class Tdouble>
	Tdouble& vec<Tdouble>::operator[](size_t i){
		assert( i < length_ );
		return *handle_[i];
	}

	template<class Tdouble>
	const Tdouble& vec<Tdouble>::operator[](size_t i) const{
		assert( i < length_ );
		return *handle_[i];
	}

	/* functions that make the wrapping a little more convenient */
	bpn::array vector2array(const vec<double>& in_vec);

	/* general functions */
	void Independent(bpn::array& x_array);

	/* atomic (aka elementary) operations */
	CppAD::AD<double>	(*cos_AD_double) 		( const CppAD::AD<double> & ) = &CppAD::cos;
	CppAD::AD<double>	(*sin_AD_double) 		( const CppAD::AD<double> & ) = &CppAD::sin;

	typedef CppAD::AD<double> AD_double;
	typedef CppAD::AD<CppAD::AD<double> > AD_AD_double;
	typedef vec<double> double_vec;
	typedef vec<AD_double> AD_double_vec;
	typedef vec<AD_AD_double> AD_Ad_double_vec;

	
}

BOOST_PYTHON_MODULE(_cppad)
{
	using namespace boost::python;
	import_array(); 										/* some kind of hack to get numpy working */
	bpn::array::set_module_and_type("numpy", "ndarray");	/* some kind of hack to get numpy working */
	
	scope().attr("__doc__") =" CppAD: docstring \n\
							   next line";

	def("Independent", &Independent);


	class_<AD_double>("AD_double", init<double>())
		.def(boost::python::self_ns::str(self))
		
		.def(self+self)
		.def(self-self)
		.def(self*self)
		.def(self/self)

		.def(-self)
		.def(+self)
		.def(self += double() )
		.def(self -= double() )
		.def(self *= double() )
		.def(self /= double() )

		.def(self += self )
		.def(self -= self )
		.def(self *= self )
		.def(self /= self )

		.def("cos", cos_AD_double  )
		.def("sin", sin_AD_double  )
	;

	class_<ADFun_double>("ADFun_double", init< bpn::array& , bpn::array& >())
		.def("Forward", &ADFun_double::Forward)
	;
}

#endif
